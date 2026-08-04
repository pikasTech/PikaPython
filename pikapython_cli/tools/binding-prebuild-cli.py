#!/usr/bin/env python3
# SPEC: PJ2026-050109 network-ready v0.1; independent binding prebuild.
import argparse
import hashlib
import json
import os
import re
import shutil
import subprocess
import sys
import tempfile
import uuid
from pathlib import Path


SCHEMA = "pika.binding-prebuild/v1"
IDENTIFIER = re.compile(r"^[A-Za-z_][A-Za-z0-9_]*$")
ROOT = Path.cwd().resolve()
GENERATOR = Path(__file__).with_name("binding_stub.py")
TYPE_CAPABILITIES = {
    "None": "value.none",
    "bool": "value.bool",
    "int": "value.int",
    "float": "value.float",
    "str": "value.string",
    "bytes": "value.bytes",
    "bytearray": "container.bytearray",
    "list": "container.list",
    "tuple": "container.tuple",
    "dict": "container.dict",
    "Any": None,
}


class PrebuildError(Exception):
    def __init__(self, code, message):
        super().__init__(message)
        self.code = code


def emit(ok, data=None, code=None, error=None):
    value = {"ok": ok, "operation": "binding.prebuild"}
    if data is not None:
        value["data"] = data
    if code is not None:
        value["code"] = code
    if error is not None:
        value["error"] = error
    print(json.dumps(value, ensure_ascii=True, indent=2))
    return 0 if ok else 1


def project_relative(path):
    try:
        return path.resolve().relative_to(ROOT).as_posix()
    except ValueError as error:
        raise PrebuildError(
            "input_outside_project",
            "binding input must be inside the project",
        ) from error


def annotation_types(annotation):
    base, separator, remainder = annotation.partition("[")
    values = [base]
    if separator:
        if not remainder.endswith("]"):
            raise PrebuildError(
                "invalid_descriptor",
                "invalid normalized type annotation",
            )
        values.extend(
            item
            for item in remainder[:-1].split(",")
            if item and item != "..."
        )
    return values


def annotation_capabilities(annotation, class_names):
    required = set()
    for type_name in annotation_types(annotation):
        if type_name in class_names:
            continue
        if type_name not in TYPE_CAPABILITIES:
            raise PrebuildError(
                "invalid_descriptor",
                "unknown normalized type annotation",
            )
        capability = TYPE_CAPABILITIES[type_name]
        if capability is not None:
            required.add(capability)
    return required


def callable_capabilities(value, class_names):
    required = set()
    if value["parameters"]:
        required.add("call.keyword")
    for parameter in value["parameters"]:
        required.update(
            annotation_capabilities(
                parameter["type"], class_names
            )
        )
        if parameter["type"] == "bytearray":
            required.add("binding.mutable-buffer")
        if "default" in parameter:
            required.add("call.defaults")
    return_type = annotation_types(value["returns"])[0]
    required.update(
        annotation_capabilities(value["returns"], class_names)
    )
    if return_type in class_names:
        required.add("binding.object-result")
    if return_type in (
        "list",
        "tuple",
        "dict",
    ):
        required.add("binding.container-result")
    return required


def required_capabilities(descriptor):
    required = set()
    class_names = {
        value["name"] for value in descriptor["classes"]
    }
    if descriptor["functions"] or descriptor["classes"]:
        required.add("binding.c")
    if descriptor["constants"] or any(
        value["constants"] for value in descriptor["classes"]
    ):
        required.add("binding.constants")
    for constant in descriptor["constants"]:
        required.update(
            annotation_capabilities(
                constant["type"], class_names
            )
        )
    for function in descriptor["functions"]:
        required.update(
            callable_capabilities(function, class_names)
        )
    for binding_class in descriptor["classes"]:
        required.add("binding.c-object")
        for constant in binding_class["constants"]:
            required.update(
                annotation_capabilities(
                    constant["type"], class_names
                )
            )
        required.update(
            callable_capabilities(
                binding_class["constructor"], class_names
            )
        )
        for method in binding_class["methods"]:
            required.update(
                callable_capabilities(method, class_names)
            )
    return required


def run_generator(module, source, stage):
    descriptor = stage / ("%s.json" % module)
    generated_source = stage / ("%s_binding.c" % module)
    generated_header = stage / ("%s_binding.h" % module)
    result = subprocess.run(
        [
            sys.executable,
            str(GENERATOR),
            "--module",
            module,
            "--input",
            str(source),
            "--output-manifest",
            str(descriptor),
            "--output-source",
            str(generated_source),
            "--output-header",
            str(generated_header),
        ],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
        check=False,
    )
    if result.returncode != 0:
        message = result.stderr.strip() or "binding generator failed"
        code = message.split(":", 1)[0]
        raise PrebuildError(code, message)
    try:
        value = json.loads(descriptor.read_text(encoding="utf-8"))
    except (OSError, UnicodeError, json.JSONDecodeError) as error:
        raise PrebuildError(
            "invalid_descriptor",
            "generated descriptor is not readable",
        ) from error
    return value


def write_manifest(stage, module, source, capabilities, required):
    descriptor = "%s.json" % module
    generated_source = "%s_binding.c" % module
    generated_header = "%s_binding.h" % module
    value = {
        "schema": SCHEMA,
        "module": module,
        "input": {
            "path": project_relative(source),
            "sha256": hashlib.sha256(source.read_bytes()).hexdigest(),
        },
        "capabilities": capabilities,
        "requiredCapabilities": sorted(required),
        "outputs": {
            "descriptor": descriptor,
            "publicHeader": generated_header,
            "source": generated_source,
        },
        "sources": [generated_source],
        "publicHeaders": [generated_header],
        "includeDirectories": ["."],
    }
    (stage / "binding-prebuild.json").write_text(
        json.dumps(value, ensure_ascii=True, indent=2, sort_keys=True) + "\n",
        encoding="utf-8",
    )
    return value


def publish(stage, output):
    backup = output.with_name(
        ".%s.backup-%s" % (output.name, uuid.uuid4().hex)
    )
    moved_previous = False
    try:
        if output.exists():
            if output.is_symlink() or not output.is_dir():
                raise PrebuildError(
                    "invalid_output",
                    "binding prebuild output must be a directory",
                )
            os.replace(output, backup)
            moved_previous = True
        os.replace(stage, output)
    except Exception:
        if moved_previous and backup.exists() and not output.exists():
            os.replace(backup, output)
        raise
    if backup.exists():
        shutil.rmtree(backup)


def prebuild(args):
    if not IDENTIFIER.fullmatch(args.module):
        raise PrebuildError("invalid_module", "invalid binding module name")
    source = Path(args.input).resolve()
    if not source.is_file():
        raise PrebuildError("input_not_found", "binding input does not exist")
    project_relative(source)
    output = Path(args.output).resolve()
    output.parent.mkdir(parents=True, exist_ok=True)
    capabilities = sorted(set(args.capability))
    temporary = Path(
        tempfile.mkdtemp(prefix=".binding-prebuild-", dir=str(output.parent))
    )
    stage = temporary / "publish"
    stage.mkdir()
    try:
        descriptor = run_generator(args.module, source, stage)
        required = required_capabilities(descriptor)
        missing = sorted(required.difference(capabilities))
        if missing:
            raise PrebuildError(
                "missing_capability",
                "missing required capabilities: %s" % ", ".join(missing),
            )
        manifest = write_manifest(
            stage,
            args.module,
            source,
            capabilities,
            required,
        )
        publish(stage, output)
    finally:
        if temporary.exists():
            shutil.rmtree(temporary)
    return emit(
        True,
        {
            "manifest": "binding-prebuild.json",
            "module": manifest["module"],
            "outputs": manifest["outputs"],
        },
    )


def parse_arguments():
    parser = argparse.ArgumentParser(prog="binding-prebuild-cli.py")
    actions = parser.add_subparsers(dest="action", required=True)
    command = actions.add_parser("prebuild")
    command.add_argument("--module", required=True)
    command.add_argument("--input", required=True)
    command.add_argument("--output", required=True)
    command.add_argument("--capability", action="append", default=[])
    return parser.parse_args()


def main():
    args = parse_arguments()
    try:
        return prebuild(args)
    except PrebuildError as error:
        return emit(False, code=error.code, error=str(error))
    except (OSError, UnicodeError) as error:
        return emit(False, code="prebuild_io_error", error=str(error))


if __name__ == "__main__":
    raise SystemExit(main())
