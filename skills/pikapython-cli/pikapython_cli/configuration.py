# SPEC: PJ2026-050110 PikaPython CLI v0.25; configurable boot entry.
import os
import re
import tempfile
from contextlib import contextmanager
from pathlib import Path

import yaml

from .errors import PackageError


CONFIG_NAME = "pikapython.yaml"
DEFAULT_PACKAGE_SOURCE = "https://github.com/pikasTech/PikaPython.git"
DEFAULT_PACKAGE_REF = "v2"
PACKAGE_NAME = re.compile(r"^[a-z][a-z0-9]*(?:-[a-z0-9]+)*$")
TARGET_NAME = re.compile(r"^[a-z0-9][a-z0-9-]*$")
VERSION = re.compile(r"^[0-9]+(?:\.[0-9]+){2}(?:[-+][A-Za-z0-9.-]+)?$")
TOP_LEVEL_FIELDS = {
    "version",
    "projectKind",
    "dependencies",
    "bootEntry",
    "packages",
    "capability",
    "pythonModules",
    "defaultTarget",
    "targets",
}
PROJECT_KINDS = {"pikapython", "external"}
PACKAGE_FIELDS = {"sourceUrl", "ref"}
CAPABILITY_FIELDS = {"configFile", "profile"}
PYTHON_MODULE_FIELDS = {"format", "include"}
PYTHON_MODULE_FORMATS = {"program", "source"}
PYTHON_IMPORT_NAME = re.compile(
    r"^[A-Za-z_][A-Za-z0-9_]*(?:\.[A-Za-z_][A-Za-z0-9_]*)*$"
)
TARGET_FIELDS = {
    "adapter",
    "projectFile",
    "targetName",
    "toolExecutable",
    "outputDirectory",
    "buildDirectory",
    "arguments",
}
TARGET_ADAPTERS = {"cmake", "iar", "keil", "make"}
CAPABILITY_PREBUILD_ARGUMENT = "PIKA_CAPABILITY_PREBUILD_DIR"
FORBIDDEN_BUILD_ARGUMENTS = {
    "PIKA_REPL_ENABLE",
    "PIKA_REPL_AUTO_INDENT_ENABLE",
    "PIKA_FROZEN_PROGRAM_ENABLE",
    "PIKA_FROZEN_SOURCE_ENABLE",
    "PIKA_GC_ENABLE",
    "PIKA_SYNTAX_LEVEL",
}


def default_config():
    return {
        "version": 1,
        "dependencies": ["pikapython-kernel==2.0.0"],
        "bootEntry": "main.py",
        "packages": {
            "sourceUrl": DEFAULT_PACKAGE_SOURCE,
            "ref": DEFAULT_PACKAGE_REF,
        },
    }


def parse_dependency(value):
    if not isinstance(value, str) or value.count("==") != 1:
        raise PackageError(
            "invalid_dependency",
            "dependency must use <name>==<version>",
            "Use a value such as pikapython-kernel==2.0.0.",
        )
    name, version = value.split("==", 1)
    if PACKAGE_NAME.fullmatch(name) is None or VERSION.fullmatch(version) is None:
        raise PackageError(
            "invalid_dependency",
            "dependency name or version is invalid: %s" % value,
            "Use lowercase package names and a three-part version.",
        )
    return name, version


def _require_string(value, field):
    if not isinstance(value, str) or not value.strip():
        raise PackageError(
            "invalid_config",
            "%s must be a non-empty string" % field,
        )
    return value


def _validate_packages(value):
    if not isinstance(value, dict):
        raise PackageError("invalid_config", "packages must be an object")
    unknown = set(value).difference(PACKAGE_FIELDS)
    if unknown:
        raise PackageError(
            "unknown_config_field",
            "unknown packages field: %s" % sorted(unknown)[0],
        )
    if "sourceUrl" in value:
        _require_string(value["sourceUrl"], "packages.sourceUrl")
    if "ref" in value:
        _require_string(value["ref"], "packages.ref")


def _validate_relative_path(value, field):
    text = _require_string(value, field)
    path = Path(text)
    if path.is_absolute() or ".." in path.parts or "\\" in text:
        raise PackageError(
            "invalid_config",
            "%s must be a portable project-relative path" % field,
        )
    return text


def _validate_capability(value):
    if not isinstance(value, dict):
        raise PackageError("invalid_config", "capability must be an object")
    unknown = set(value).difference(CAPABILITY_FIELDS)
    if unknown:
        raise PackageError(
            "unknown_config_field",
            "unknown capability field: %s" % sorted(unknown)[0],
        )
    if set(value) != CAPABILITY_FIELDS:
        raise PackageError(
            "invalid_config",
            "capability.configFile and capability.profile are both required",
        )
    _validate_relative_path(value["configFile"], "capability.configFile")
    _require_string(value["profile"], "capability.profile")


def _validate_boot_entry(value):
    text = _validate_relative_path(value, "bootEntry")
    path = Path(text)
    if path.parent != Path(".") or path.suffix != ".py":
        raise PackageError(
            "invalid_boot_entry",
            "bootEntry must name a Python file in the project root",
        )


def _validate_python_modules(value):
    if not isinstance(value, dict):
        raise PackageError("invalid_config", "pythonModules must be an object")
    unknown = set(value).difference(PYTHON_MODULE_FIELDS)
    if unknown:
        raise PackageError(
            "unknown_config_field",
            "unknown pythonModules field: %s" % sorted(unknown)[0],
        )
    if set(value) != PYTHON_MODULE_FIELDS:
        raise PackageError(
            "invalid_config",
            "pythonModules.format and pythonModules.include are both required",
        )
    module_format = value["format"]
    if module_format not in PYTHON_MODULE_FORMATS:
        raise PackageError(
            "invalid_python_module_format",
            "pythonModules.format must be program or source",
        )
    include = value["include"]
    if not isinstance(include, list) or not include:
        raise PackageError(
            "invalid_python_module_selection",
            "pythonModules.include must be a non-empty list",
        )
    seen = set()
    for module in include:
        if not isinstance(module, str) or (
            module != "*" and PYTHON_IMPORT_NAME.fullmatch(module) is None
        ):
            raise PackageError(
                "invalid_python_module",
                "invalid Python import name: %s" % module,
            )
        if module == "__main__":
            raise PackageError(
                "invalid_python_module_selection",
                "__main__ is included implicitly",
            )
        if module in seen:
            raise PackageError(
                "duplicate_python_module",
                "duplicate Python module: %s" % module,
            )
        seen.add(module)
    if "*" in seen and len(seen) != 1:
        raise PackageError(
            "invalid_python_module_selection",
            '"*" cannot be combined with named modules',
        )


def _validate_target(target_id, value):
    if TARGET_NAME.fullmatch(target_id) is None:
        raise PackageError(
            "invalid_target",
            "invalid target id: %s" % target_id,
        )
    if not isinstance(value, dict):
        raise PackageError(
            "invalid_target",
            "target %s must be an object" % target_id,
        )
    unknown = set(value).difference(TARGET_FIELDS)
    if unknown:
        raise PackageError(
            "unknown_config_field",
            "unknown target field: %s" % sorted(unknown)[0],
        )
    adapter = _require_string(value.get("adapter"), "targets.%s.adapter" % target_id)
    if adapter not in TARGET_ADAPTERS:
        raise PackageError(
            "unsupported_adapter",
            "unsupported target adapter: %s" % adapter,
        )
    for field in (
        "projectFile",
        "targetName",
        "toolExecutable",
        "outputDirectory",
        "buildDirectory",
    ):
        if field in value:
            _require_string(value[field], "targets.%s.%s" % (target_id, field))
    if adapter in {"cmake", "keil"}:
        for field in ("projectFile", "targetName", "buildDirectory"):
            if field not in value:
                raise PackageError(
                    "invalid_target",
                    "%s target %s requires %s"
                    % (adapter, target_id, field),
                )
        _validate_relative_path(
            value["projectFile"], "targets.%s.projectFile" % target_id
        )
        _validate_relative_path(
            value["buildDirectory"],
            "targets.%s.buildDirectory" % target_id,
        )
        if adapter == "keil" and not value["projectFile"].lower().endswith(
            ".uvprojx"
        ):
            raise PackageError(
                "invalid_target",
                "Keil target %s requires a .uvprojx projectFile" % target_id,
            )
    arguments = value.get("arguments")
    if arguments is not None and (
        not isinstance(arguments, list)
        or not all(isinstance(item, str) for item in arguments)
    ):
        raise PackageError(
            "invalid_target",
            "targets.%s.arguments must be a string list" % target_id,
        )
    for argument in arguments or []:
        names = re.findall(r"PIKA_[A-Z0-9_]+", argument)
        for name in names:
            forbidden = name in FORBIDDEN_BUILD_ARGUMENTS or (
                name.startswith("PIKA_CAPABILITY_")
                and name != CAPABILITY_PREBUILD_ARGUMENT
            )
            if forbidden:
                raise PackageError(
                    "capability_argument_forbidden",
                    (
                        "target %s must not set capability build argument: %s"
                        % (target_id, name)
                    ),
                    (
                        "Use pikapython-cli config capability and pass only "
                        "PIKA_CAPABILITY_PREBUILD_DIR={generated}."
                    ),
                )


def validate_config(value):
    if not isinstance(value, dict):
        raise PackageError("invalid_config", "configuration must be an object")
    unknown = set(value).difference(TOP_LEVEL_FIELDS)
    if unknown:
        raise PackageError(
            "unknown_config_field",
            "unknown top-level field: %s" % sorted(unknown)[0],
        )
    if value.get("version") != 1 or isinstance(value.get("version"), bool):
        raise PackageError(
            "unsupported_config_version",
            "pikapython.yaml version must be 1",
        )
    project_kind = value.get("projectKind", "pikapython")
    if project_kind not in PROJECT_KINDS:
        raise PackageError(
            "invalid_project_kind",
            "projectKind must be pikapython or external",
        )
    dependencies = value.get("dependencies")
    if not isinstance(dependencies, list):
        raise PackageError(
            "invalid_config",
            "dependencies must be a list",
        )
    names = []
    for dependency in dependencies:
        name, _ = parse_dependency(dependency)
        if name in names:
            raise PackageError(
                "duplicate_dependency",
                "duplicate dependency: %s" % name,
            )
        names.append(name)
    if project_kind == "external":
        if dependencies:
            raise PackageError(
                "external_project_dependencies_unsupported",
                "external projects must not configure package dependencies",
            )
        unsupported = sorted(
            set(value).intersection(
                {"bootEntry", "packages", "capability", "pythonModules"}
            )
        )
        if unsupported:
            raise PackageError(
                "external_project_field_unsupported",
                "external projects must not configure %s" % unsupported[0],
            )
    if "packages" in value:
        _validate_packages(value["packages"])
    if "bootEntry" in value:
        _validate_boot_entry(value["bootEntry"])
    if "capability" in value:
        _validate_capability(value["capability"])
    if "pythonModules" in value:
        _validate_python_modules(value["pythonModules"])
    targets = value.get("targets", {})
    if not isinstance(targets, dict):
        raise PackageError("invalid_config", "targets must be an object")
    for target_id, descriptor in targets.items():
        if not isinstance(target_id, str):
            raise PackageError("invalid_target", "target id must be a string")
        _validate_target(target_id, descriptor)
    default_target = value.get("defaultTarget")
    if default_target is not None:
        _require_string(default_target, "defaultTarget")
        if default_target not in targets:
            raise PackageError(
                "unknown_default_target",
                "defaultTarget does not exist: %s" % default_target,
            )
    return value


def config_path(project=None):
    root = Path.cwd() if project is None else Path(project)
    return root.resolve() / CONFIG_NAME


def load_config(project=None):
    path = config_path(project)
    try:
        value = yaml.safe_load(path.read_text(encoding="utf-8"))
    except FileNotFoundError as error:
        raise PackageError(
            "config_not_found",
            "%s was not found" % CONFIG_NAME,
            'Run "pikapython-cli init" in the project directory.',
        ) from error
    except (OSError, UnicodeError, yaml.YAMLError) as error:
        raise PackageError(
            "config_read_failed",
            "cannot read %s: %s" % (CONFIG_NAME, error),
        ) from error
    return validate_config(value)


def dump_config(value):
    validate_config(value)
    return yaml.safe_dump(
        value,
        allow_unicode=True,
        default_flow_style=False,
        sort_keys=False,
    )


@contextmanager
def config_lock(project=None):
    path = config_path(project)
    state = path.parent / ".pikapython"
    state.mkdir(parents=True, exist_ok=True)
    lock_path = state / "config.lock"
    handle = lock_path.open("a+", encoding="utf-8")
    try:
        try:
            import fcntl

            fcntl.flock(handle.fileno(), fcntl.LOCK_EX)
        except ImportError:
            pass
        yield
    finally:
        try:
            import fcntl

            fcntl.flock(handle.fileno(), fcntl.LOCK_UN)
        except ImportError:
            pass
        handle.close()


def write_config(value, project=None):
    path = config_path(project)
    path.parent.mkdir(parents=True, exist_ok=True)
    content = dump_config(value)
    temporary = None
    try:
        descriptor, temporary_name = tempfile.mkstemp(
            prefix=".pikapython-",
            suffix=".yaml",
            dir=str(path.parent),
        )
        temporary = Path(temporary_name)
        with os.fdopen(descriptor, "w", encoding="utf-8", newline="\n") as handle:
            handle.write(content)
            handle.flush()
            os.fsync(handle.fileno())
        os.replace(temporary, path)
    except OSError as error:
        raise PackageError(
            "config_write_failed",
            "cannot write %s: %s" % (CONFIG_NAME, error),
        ) from error
    finally:
        if temporary is not None and temporary.exists():
            temporary.unlink()


def initialize(project=None):
    path = config_path(project)
    if path.exists():
        raise PackageError(
            "config_exists",
            "%s already exists" % CONFIG_NAME,
        )
    value = default_config()
    try:
        descriptor = os.open(path, os.O_WRONLY | os.O_CREAT | os.O_EXCL, 0o644)
        with os.fdopen(descriptor, "w", encoding="utf-8", newline="\n") as handle:
            handle.write(dump_config(value))
            handle.flush()
            os.fsync(handle.fileno())
    except FileExistsError as error:
        raise PackageError(
            "config_exists",
            "%s already exists" % CONFIG_NAME,
        ) from error
    except OSError as error:
        raise PackageError(
            "config_write_failed",
            "cannot create %s: %s" % (CONFIG_NAME, error),
        ) from error
    return value
