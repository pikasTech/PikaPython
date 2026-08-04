#!/usr/bin/env python3
# SPEC: PJ2026-050109 network-ready v0.1; Python 3 stub normalizer.
import argparse
import ast
import json
import math
import re
import sys
from pathlib import Path


SCHEMA = "pika.binding/v1"
IDENTIFIER = re.compile(r"^[A-Za-z_][A-Za-z0-9_]*$")
VALUE_KINDS = {
    "None": "PIKA_BINDING_VALUE_NONE",
    "bool": "PIKA_BINDING_VALUE_BOOLEAN",
    "int": "PIKA_BINDING_VALUE_INTEGER",
    "float": "PIKA_BINDING_VALUE_FLOAT",
    "str": "PIKA_BINDING_VALUE_STRING",
    "bytes": "PIKA_BINDING_VALUE_BYTES",
    "bytearray": "PIKA_BINDING_VALUE_BYTEARRAY",
    "list": "PIKA_BINDING_VALUE_LIST",
    "tuple": "PIKA_BINDING_VALUE_TUPLE",
    "dict": "PIKA_BINDING_VALUE_DICT",
    "Any": "PIKA_BINDING_VALUE_ANY",
}
CONTAINER_TYPES = ("list", "tuple", "dict")


class StubError(Exception):
    def __init__(self, node, message):
        super().__init__(message)
        self.line = getattr(node, "lineno", 1)
        self.column = getattr(node, "col_offset", 0) + 1


def fail(node, message):
    raise StubError(node, message)


def simple_annotation_name(node, class_names=()):
    if isinstance(node, ast.Name):
        if node.id in VALUE_KINDS or node.id in class_names:
            return node.id
    if isinstance(node, ast.Constant) and node.value is None:
        return "None"
    if (
        isinstance(node, ast.Attribute)
        and isinstance(node.value, ast.Name)
        and node.value.id == "typing"
        and node.attr == "Any"
    ):
        return "Any"
    fail(node, "unsupported type annotation")


def annotation_name(node, class_names=()):
    if not isinstance(node, ast.Subscript):
        return simple_annotation_name(node, class_names)
    if not isinstance(node.value, ast.Name):
        fail(node, "container annotation must use a built-in type")
    container = node.value.id
    if container not in CONTAINER_TYPES:
        fail(node, "unsupported generic type annotation")
    arguments = (
        list(node.slice.elts)
        if isinstance(node.slice, ast.Tuple)
        else [node.slice]
    )
    if container == "list" and len(arguments) != 1:
        fail(node, "list annotation requires one element type")
    if container == "dict" and len(arguments) != 2:
        fail(node, "dict annotation requires key and value types")
    if container == "tuple" and not arguments:
        fail(node, "tuple annotation requires an element type")
    normalized = []
    for index, argument in enumerate(arguments):
        if (
            container == "tuple"
            and index == 1
            and len(arguments) == 2
            and isinstance(argument, ast.Constant)
            and argument.value is Ellipsis
        ):
            normalized.append("...")
            continue
        value = simple_annotation_name(argument, class_names)
        if value in class_names:
            fail(
                argument,
                "native object container annotations are not supported",
            )
        if value in CONTAINER_TYPES or value == "bytearray":
            fail(argument, "nested mutable container annotation is not supported")
        normalized.append(value)
    return "%s[%s]" % (container, ",".join(normalized))


def binding_type_name(type_name):
    return type_name.split("[", 1)[0]


def binding_value_kind(type_name):
    return VALUE_KINDS.get(
        binding_type_name(type_name),
        "PIKA_BINDING_VALUE_OBJECT",
    )


def literal_value(node):
    sign = 1
    value_node = node
    if isinstance(node, ast.UnaryOp) and isinstance(
        node.op, (ast.UAdd, ast.USub)
    ):
        sign = -1 if isinstance(node.op, ast.USub) else 1
        value_node = node.operand
    if not isinstance(value_node, ast.Constant):
        fail(node, "default and constant values must be literals")
    value = value_node.value
    if sign != 1:
        if type(value) not in (int, float):
            fail(node, "sign is only valid for numeric literals")
        value *= sign
    if type(value) not in (type(None), bool, int, float, str, bytes):
        fail(node, "unsupported literal value")
    if type(value) is int and not (-(1 << 63) <= value < (1 << 63)):
        fail(node, "integer literal is outside signed 64-bit range")
    if type(value) is float and not math.isfinite(value):
        fail(node, "float literal must be finite")
    if type(value) is bytes:
        return {"hex": value.hex()}
    return value


def literal_type(value):
    if isinstance(value, dict):
        return "bytes"
    if value is None:
        return "None"
    if type(value) is bool:
        return "bool"
    if type(value) is int:
        return "int"
    if type(value) is float:
        return "float"
    return "str"


def validate_literal_type(node, annotation, value):
    actual = literal_type(value)
    if annotation != "Any" and annotation != actual:
        fail(
            node,
            "literal type %s does not match annotation %s"
            % (actual, annotation),
        )


def normalize_constant(node):
    if not isinstance(node.target, ast.Name) or node.simple != 1:
        fail(node, "constant name must be a simple identifier")
    if node.value is None:
        fail(node, "constant requires a literal value")
    annotation = annotation_name(node.annotation)
    if annotation == "Any":
        fail(node, "constant annotation must be concrete")
    value = literal_value(node.value)
    validate_literal_type(node.value, annotation, value)
    return {
        "name": node.target.id,
        "type": annotation,
        "value": value,
    }


def is_ellipsis_body(body):
    return (
        len(body) == 1
        and isinstance(body[0], ast.Expr)
        and isinstance(body[0].value, ast.Constant)
        and body[0].value.value is Ellipsis
    )


def normalize_function(node, method, class_names):
    if node.decorator_list:
        fail(node.decorator_list[0], "decorators are not supported")
    arguments = node.args
    if (
        arguments.posonlyargs
        or arguments.kwonlyargs
        or arguments.vararg is not None
        or arguments.kwarg is not None
        or any(value is not None for value in arguments.kw_defaults)
    ):
        fail(node, "positional-only, keyword-only, and variadic parameters are not supported")
    if not is_ellipsis_body(node.body):
        fail(node, "stub body must be an ellipsis")
    positional = list(arguments.args)
    default_start = len(positional) - len(arguments.defaults)
    if method:
        if not positional or positional[0].arg != "self":
            fail(node, "instance method must start with self")
        if positional[0].annotation is not None:
            fail(positional[0], "self must not have a type annotation")
        if default_start == 0:
            fail(positional[0], "self cannot have a default value")
        positional = positional[1:]
    parameters = []
    parameter_offset = 1 if method else 0
    for index, argument in enumerate(positional):
        if argument.annotation is None:
            fail(argument, "parameter requires a type annotation")
        annotation = annotation_name(
            argument.annotation, class_names
        )
        if annotation in class_names:
            fail(
                argument.annotation,
                "native object parameters are not supported",
            )
        parameter = {"name": argument.arg, "type": annotation}
        original_index = index + parameter_offset
        if original_index >= default_start:
            default = literal_value(
                arguments.defaults[original_index - default_start]
            )
            validate_literal_type(
                arguments.defaults[original_index - default_start],
                annotation,
                default,
            )
            parameter["default"] = default
        parameters.append(parameter)
    if node.returns is None:
        fail(node, "return type annotation is required")
    returns = annotation_name(node.returns, class_names)
    if method and node.name == "__init__" and returns != "None":
        fail(node, "__init__ must return None")
    return {
        "name": node.name,
        "parameters": parameters,
        "requiredParameterCount": sum(
            1 for parameter in parameters if "default" not in parameter
        ),
        "returns": returns,
        "deinitializer": (
            method
            and node.name == "deinit"
            and not parameters
            and returns == "None"
        ),
    }


def normalize_class(node, class_names):
    if node.bases or node.keywords or node.decorator_list:
        fail(node, "class inheritance and decorators are not supported")
    constructor = None
    methods = []
    constants = []
    names = set()
    for item in node.body:
        if isinstance(item, ast.AnnAssign):
            value = normalize_constant(item)
            if value["name"] in names:
                fail(item, "duplicate class member")
            names.add(value["name"])
            constants.append(value)
            continue
        if isinstance(item, ast.FunctionDef):
            if item.name in names:
                fail(item, "duplicate class member")
            names.add(item.name)
            value = normalize_function(item, True, class_names)
            if item.name == "__init__":
                constructor = value
            else:
                methods.append(value)
            continue
        fail(item, "class body may only contain constants and methods")
    if constructor is None:
        fail(node, "C class requires __init__")
    return {
        "name": node.name,
        "constants": constants,
        "constructor": constructor,
        "methods": methods,
    }


def validate_import(node):
    if isinstance(node, ast.ImportFrom):
        if (
            node.module == "typing"
            and node.level == 0
            and len(node.names) == 1
            and node.names[0].name == "Any"
            and node.names[0].asname is None
        ):
            return
    if isinstance(node, ast.Import):
        if (
            len(node.names) == 1
            and node.names[0].name == "typing"
            and node.names[0].asname is None
        ):
            return
    fail(node, "only typing.Any imports are supported")


def normalize(module_name, tree):
    functions = []
    classes = []
    constants = []
    names = set()
    class_names = {
        node.name
        for node in tree.body
        if isinstance(node, ast.ClassDef)
    }
    for node in tree.body:
        if isinstance(node, (ast.Import, ast.ImportFrom)):
            validate_import(node)
            continue
        if isinstance(node, ast.AnnAssign):
            value = normalize_constant(node)
            if value["name"] in names:
                fail(node, "duplicate module member")
            names.add(value["name"])
            constants.append(value)
            continue
        if isinstance(node, ast.FunctionDef):
            if node.name in names:
                fail(node, "duplicate module member")
            names.add(node.name)
            functions.append(
                normalize_function(node, False, class_names)
            )
            continue
        if isinstance(node, ast.ClassDef):
            if node.name in names:
                fail(node, "duplicate module member")
            names.add(node.name)
            classes.append(normalize_class(node, class_names))
            continue
        fail(node, "module may only contain imports, functions, and classes")
    return {
        "schema": SCHEMA,
        "module": module_name,
        "constants": constants,
        "functions": functions,
        "classes": classes,
    }


def c_name(*parts):
    return "_".join(parts)


def callback_name(module_name, class_name, function_name):
    if function_name == "__init__":
        value = "init"
    elif class_name is not None and function_name == "init":
        value = "method_init"
    else:
        value = function_name
    parts = ["pika", "binding", module_name]
    if class_name:
        parts.append(class_name)
    parts.append(value)
    return c_name(*parts)


def parameter_array_name(module_name, class_name, function_name):
    if function_name == "__init__":
        value = "init"
    elif class_name is not None and function_name == "init":
        value = "method_init"
    else:
        value = function_name
    parts = [module_name]
    if class_name:
        parts.append(class_name)
    parts.extend([value, "parameters"])
    return c_name(*parts)


def parameter_name_array_name(module_name, class_name, function_name):
    return parameter_array_name(
        module_name, class_name, function_name
    ) + "_names"


def default_array_name(module_name, class_name, function_name):
    return parameter_array_name(
        module_name, class_name, function_name
    ) + "_defaults"


def render_buffer(lines, name, type_name, value):
    if type_name not in ("str", "bytes"):
        return None
    data = (
        value.encode("utf-8")
        if type_name == "str"
        else bytes.fromhex(value["hex"])
    )
    if not data:
        return None
    lines.append("static const uint8_t %s[] = {" % name)
    for offset in range(0, len(data), 12):
        lines.append(
            "    %s,"
            % ", ".join(
                "0x%02xu" % byte
                for byte in data[offset : offset + 12]
            )
        )
    lines.append("};")
    lines.append("")
    return name


def render_value_fields(lines, indent, type_name, value, data_name):
    lines.append(
        "%s.kind = %s," % (indent, binding_value_kind(type_name))
    )
    if type_name == "bool":
        initializer = ".boolean = %su" % (1 if value else 0)
    elif type_name == "int":
        if value == -(1 << 63):
            integer = "(-INT64_C(9223372036854775807) - INT64_C(1))"
        elif value < 0:
            integer = "-INT64_C(%d)" % -value
        else:
            integer = "INT64_C(%d)" % value
        initializer = ".integer = %s" % integer
    elif type_name == "float":
        initializer = ".floating = %s" % repr(value)
    elif type_name in ("str", "bytes"):
        length = (
            len(value.encode("utf-8"))
            if type_name == "str"
            else len(bytes.fromhex(value["hex"]))
        )
        initializer = ".buffer = { %s, %du }" % (
            data_name if data_name is not None else "NULL",
            length,
        )
    else:
        initializer = ".integer = 0"
    lines.append("%s.as = { %s }," % (indent, initializer))


def render_parameter_names(
    lines, module_name, class_name, function
):
    if not function["parameters"]:
        return "NULL"
    name = parameter_name_array_name(
        module_name, class_name, function["name"]
    )
    lines.append("static const char* const %s[] = {" % name)
    for parameter in function["parameters"]:
        lines.append('    "%s",' % parameter["name"])
    lines.append("};")
    lines.append("")
    return name


def default_literal_type(parameter):
    type_name = parameter["type"]
    value = parameter["default"]
    if type_name != "Any":
        return type_name
    if value is None:
        return "None"
    if isinstance(value, bool):
        return "bool"
    if isinstance(value, int):
        return "int"
    if isinstance(value, float):
        return "float"
    if isinstance(value, str):
        return "str"
    if isinstance(value, dict) and set(value) == {"hex"}:
        return "bytes"
    raise ValueError("unsupported Any default literal")


def render_defaults(lines, module_name, class_name, function):
    defaults = [
        parameter
        for parameter in function["parameters"]
        if "default" in parameter
    ]
    if not defaults:
        return "NULL"
    name = default_array_name(
        module_name, class_name, function["name"]
    )
    data_names = []
    for index, parameter in enumerate(defaults):
        literal_type = default_literal_type(parameter)
        data_names.append(
            render_buffer(
                lines,
                "%s_%d_data" % (name, index),
                literal_type,
                parameter["default"],
            )
        )
    lines.append("static const PikaBindingValue %s[] = {" % name)
    for parameter, data_name in zip(defaults, data_names):
        literal_type = default_literal_type(parameter)
        lines.append("    {")
        render_value_fields(
            lines,
            "        ",
            literal_type,
            parameter["default"],
            data_name,
        )
        lines.append("    },")
    lines.append("};")
    lines.append("")
    return name


def render_parameters(lines, module_name, class_name, function):
    if not function["parameters"]:
        return "NULL"
    name = parameter_array_name(module_name, class_name, function["name"])
    lines.append(
        "static const PikaBindingValueKind %s[] = {" % name
    )
    for parameter in function["parameters"]:
        lines.append("    %s," % binding_value_kind(parameter["type"]))
    lines.append("};")
    lines.append("")
    return name


def render_callable(
    lines,
    module_name,
    class_name,
    function,
    kind,
    class_indices,
):
    parameters = render_parameters(
        lines, module_name, class_name, function
    )
    parameter_names = render_parameter_names(
        lines, module_name, class_name, function
    )
    defaults = render_defaults(
        lines, module_name, class_name, function
    )
    return [
        "    {",
        "        .record_size = sizeof(PikaBindingCallable),",
        '        .name = "%s",' % function["name"],
        "        .kind = %s," % kind,
        "        .parameters = %s," % parameters,
        "        .parameter_count = %du," % len(function["parameters"]),
        "        .result_kind = %s," % binding_value_kind(function["returns"]),
        "        .callback = %s," % callback_name(
            module_name, class_name, function["name"]
        ),
        "        .parameter_names = %s," % parameter_names,
        "        .defaults = %s," % defaults,
        "        .required_parameter_count = %du,"
        % function["requiredParameterCount"],
        "        .result_class_index = %s,"
        % (
            "%du" % class_indices[function["returns"]]
            if function["returns"] in class_indices
            else "UINT16_MAX"
        ),
        "        .flags = %s,"
        % (
            "PIKA_BINDING_CALLABLE_DEINITIALIZER"
            if function.get("deinitializer", False)
            else "0u"
        ),
        "    },",
    ]


def constant_array_name(module_name, class_name):
    return c_name(
        module_name,
        class_name if class_name else "module",
        "constants",
    )


def render_constants(lines, module_name, class_name, constants):
    if not constants:
        return "NULL"
    name = constant_array_name(module_name, class_name)
    data_names = []
    for index, constant in enumerate(constants):
        data_names.append(
            render_buffer(
                lines,
                "%s_%d_data" % (name, index),
                constant["type"],
                constant["value"],
            )
        )
    lines.append("static const PikaBindingConstant %s[] = {" % name)
    for constant, data_name in zip(constants, data_names):
        lines.extend(
            [
                "    {",
                "        .record_size = sizeof(PikaBindingConstant),",
                '        .name = "%s",' % constant["name"],
                "        .value = {",
            ]
        )
        render_value_fields(
            lines,
            "            ",
            constant["type"],
            constant["value"],
            data_name,
        )
        lines.extend(["        },", "    },"])
    lines.append("};")
    lines.append("")
    return name


def header_guard(module_name):
    return "PIKA_GENERATED_%s_BINDING_H" % module_name.upper()


def render_header(manifest):
    module_name = manifest["module"]
    guard = header_guard(module_name)
    lines = [
        "/* Generated from a Python 3 stub. */",
        "#ifndef %s" % guard,
        "#define %s" % guard,
        "",
        '#include "pika_binding.h"',
        "",
        "#ifdef __cplusplus",
        'extern "C" {',
        "#endif",
        "",
    ]
    for function in manifest["functions"]:
        lines.append(
            "PikaStatus %s(const PikaBindingCall* call,"
            % callback_name(module_name, None, function["name"])
        )
        lines.append("                    PikaBindingValue* result);")
    for value in manifest["classes"]:
        constructor = value["constructor"]
        lines.append(
            "PikaStatus %s(const PikaBindingCall* call,"
            % callback_name(module_name, value["name"], "__init__")
        )
        lines.append("                    PikaBindingValue* result);")
        for method in value["methods"]:
            lines.append(
                "PikaStatus %s(const PikaBindingCall* call,"
                % callback_name(module_name, value["name"], method["name"])
            )
            lines.append("                    PikaBindingValue* result);")
        lines.append(
            "void %s(void* module_context, void* instance);"
            % c_name(
                "pika", "binding", module_name, value["name"], "destroy"
            )
        )
    lines.extend(
        [
            "",
            "const PikaBindingModule* %s(void);"
            % c_name("pika", "binding", "module", module_name),
            "",
            "#ifdef __cplusplus",
            "}",
            "#endif",
            "",
            "#endif",
            "",
        ]
    )
    return "\n".join(lines)


def render_source(manifest, header_name):
    module_name = manifest["module"]
    class_indices = {
        value["name"]: index
        for index, value in enumerate(manifest["classes"])
    }
    class_constant_arrays = {}
    lines = [
        "/* Generated from a Python 3 stub. */",
        '#include "%s"' % header_name,
        "",
        "#include <stddef.h>",
        "#include <stdint.h>",
        "",
    ]
    module_constants = render_constants(
        lines, module_name, None, manifest["constants"]
    )
    function_entries = []
    for function in manifest["functions"]:
        function_entries.extend(
            render_callable(
                lines,
                module_name,
                None,
                function,
                "PIKA_BINDING_CALL_FUNCTION",
                class_indices,
            )
        )
    if function_entries:
        lines.append(
            "static const PikaBindingCallable %s_functions[] = {"
            % module_name
        )
        lines.extend(function_entries)
        lines.append("};")
        lines.append("")
    for value in manifest["classes"]:
        constructor = value["constructor"]
        class_constants = render_constants(
            lines, module_name, value["name"], value["constants"]
        )
        class_constant_arrays[value["name"]] = class_constants
        constructor_entry = render_callable(
            lines,
            module_name,
            value["name"],
            constructor,
            "PIKA_BINDING_CALL_CONSTRUCTOR",
            class_indices,
        )
        lines.append(
            "static const PikaBindingCallable %s_%s_constructor ="
            % (module_name, value["name"])
        )
        for entry in constructor_entry[:-1]:
            lines.append(
                entry[4:] if entry.startswith("    ") else entry
            )
        lines.append("};")
        lines.append("")
        method_entries = []
        for method in value["methods"]:
            method_entries.extend(
                render_callable(
                    lines,
                    module_name,
                    value["name"],
                    method,
                    "PIKA_BINDING_CALL_METHOD",
                    class_indices,
                )
            )
        if method_entries:
            lines.append(
                "static const PikaBindingCallable %s_%s_methods[] = {"
                % (module_name, value["name"])
            )
            lines.extend(method_entries)
            lines.append("};")
            lines.append("")
    if manifest["classes"]:
        lines.append(
            "static const PikaBindingClass %s_classes[] = {"
            % module_name
        )
        for value in manifest["classes"]:
            methods = (
                "%s_%s_methods" % (module_name, value["name"])
                if value["methods"]
                else "NULL"
            )
            lines.extend(
                [
                    "    {",
                    "        .record_size = sizeof(PikaBindingClass),",
                    '        .name = "%s",' % value["name"],
                    "        .constructor = &%s_%s_constructor,"
                    % (module_name, value["name"]),
                    "        .methods = %s," % methods,
                    "        .method_count = %du," % len(value["methods"]),
                    "        .destroy = %s,"
                    % c_name(
                        "pika",
                        "binding",
                        module_name,
                        value["name"],
                        "destroy",
                    ),
                    "        .constants = %s,"
                    % class_constant_arrays[value["name"]],
                    "        .constant_count = %du,"
                    % len(value["constants"]),
                    "    },",
                ]
            )
        lines.append("};")
        lines.append("")
    lines.extend(
        [
            "static PikaBindingModule %s_module = {" % module_name,
            "    .schema_version = PIKA_BINDING_SCHEMA_VERSION,",
            "    .record_size = sizeof(PikaBindingModule),",
            '    .name = "%s",' % module_name,
            "    .functions = %s,"
            % (
                "%s_functions" % module_name
                if manifest["functions"]
                else "NULL"
            ),
            "    .function_count = %du," % len(manifest["functions"]),
            "    .classes = %s,"
            % (
                "%s_classes" % module_name
                if manifest["classes"]
                else "NULL"
            ),
            "    .class_count = %du," % len(manifest["classes"]),
            "    .context = NULL,",
            "    .constants = %s," % module_constants,
            "    .constant_count = %du," % len(manifest["constants"]),
            "};",
            "",
            "const PikaBindingModule* %s(void) {"
            % c_name("pika", "binding", "module", module_name),
            "    return &%s_module;" % module_name,
            "}",
            "",
        ]
    )
    return "\n".join(lines)


def write_text(path, value):
    if path is None:
        return
    destination = Path(path)
    destination.parent.mkdir(parents=True, exist_ok=True)
    destination.write_text(value, encoding="utf-8")


def parse_arguments():
    parser = argparse.ArgumentParser()
    parser.add_argument("--module", required=True)
    parser.add_argument("--input", required=True)
    parser.add_argument("--output-manifest")
    parser.add_argument("--output-source")
    parser.add_argument("--output-header")
    return parser.parse_args()


def main():
    args = parse_arguments()
    if not IDENTIFIER.fullmatch(args.module):
        print("unsupported_stub:1:1: invalid module name", file=sys.stderr)
        return 2
    try:
        text = Path(args.input).read_text(encoding="utf-8")
        tree = ast.parse(text, filename=args.input, mode="exec")
        manifest = normalize(args.module, tree)
    except (OSError, UnicodeError, SyntaxError) as error:
        line = getattr(error, "lineno", 1) or 1
        column = getattr(error, "offset", 1) or 1
        print(
            "unsupported_stub:%d:%d: %s" % (line, column, error),
            file=sys.stderr,
        )
        return 2
    except StubError as error:
        print(
            "unsupported_stub:%d:%d: %s"
            % (error.line, error.column, error),
            file=sys.stderr,
        )
        return 2
    write_text(
        args.output_manifest,
        json.dumps(manifest, ensure_ascii=True, indent=2) + "\n",
    )
    if args.output_header:
        write_text(args.output_header, render_header(manifest))
    if args.output_source:
        header_name = (
            Path(args.output_header).name
            if args.output_header
            else "%s_binding.h" % args.module
        )
        write_text(
            args.output_source,
            render_source(manifest, header_name),
        )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
