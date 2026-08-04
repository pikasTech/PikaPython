# SPEC: PJ2026-050110 PikaPython CLI v0.21; visible default product source.
from copy import deepcopy

from . import builder
from .configuration import (
    config_lock,
    initialize,
    load_config,
    parse_dependency,
    validate_config,
    write_config,
)
from .errors import PackageError
from .installer import install


def initialize_project():
    value = initialize()
    return {
        "config": "pikapython.yaml",
        "dependencies": value["dependencies"],
        "packages": deepcopy(value["packages"]),
    }


def add_dependency(dependency):
    name, version = parse_dependency(dependency)
    with config_lock():
        value = load_config()
        current = {
            parse_dependency(item)[0]: item
            for item in value["dependencies"]
        }
        if name in current:
            raise PackageError(
                "duplicate_dependency",
                "dependency already exists: %s" % name,
                'Use "pikapython-cli remove %s" before changing its version.' % name,
            )
        value["dependencies"].append(dependency)
        value["dependencies"].sort(key=lambda item: parse_dependency(item)[0])
        write_config(value)
    return {"name": name, "version": version, "dependency": dependency}


def remove_dependency(name):
    with config_lock():
        value = load_config()
        kept = []
        removed = None
        for dependency in value["dependencies"]:
            dependency_name, version = parse_dependency(dependency)
            if dependency_name == name:
                removed = {"name": name, "version": version}
            else:
                kept.append(dependency)
        if removed is None:
            raise PackageError(
                "dependency_not_found",
                "dependency does not exist: %s" % name,
            )
        value["dependencies"] = kept
        write_config(value)
    return removed


def list_dependencies():
    value = load_config()
    return {
        "dependencies": [
            {
                "name": parse_dependency(item)[0],
                "version": parse_dependency(item)[1],
                "specification": item,
            }
            for item in value["dependencies"]
        ]
    }


def install_dependencies():
    return install(load_config())


def build_project(arguments):
    return builder.build(
        load_config(),
        requested_target=arguments.target,
        output_text=arguments.output,
        prebuild_only=arguments.prebuild_only,
    )


def show_config():
    return {"config": deepcopy(load_config())}


def validate_project_config():
    value = load_config()
    return {
        "config": "pikapython.yaml",
        "version": value["version"],
        "projectKind": value.get("projectKind", "pikapython"),
        "dependencyCount": len(value["dependencies"]),
        "targetCount": len(value.get("targets", {})),
    }


def set_config(key, value_text):
    if key not in {
        "projectKind",
        "packages.sourceUrl",
        "packages.ref",
    }:
        raise PackageError(
            "unsupported_config_key",
            "unsupported config key: %s" % key,
            "Use projectKind, packages.sourceUrl or packages.ref.",
        )
    if not value_text:
        raise PackageError("invalid_config", "%s must not be empty" % key)
    with config_lock():
        value = load_config()
        if key == "projectKind":
            value[key] = value_text
        else:
            field = key.split(".", 1)[1]
            packages = value.setdefault("packages", {})
            packages[field] = value_text
        validate_config(value)
        write_config(value)
    return {"key": key, "value": value_text}


def set_capability(config_file, profile):
    with config_lock():
        value = load_config()
        value["capability"] = {
            "configFile": config_file,
            "profile": profile,
        }
        validate_config(value)
        write_config(value)
    return {"configFile": config_file, "profile": profile}


def set_python_modules(module_format, include):
    selection = sorted(include)
    with config_lock():
        value = load_config()
        value["pythonModules"] = {
            "format": module_format,
            "include": selection,
        }
        validate_config(value)
        write_config(value)
    return {"format": module_format, "include": selection}


def unset_config(key):
    if key not in {"packages.ref"}:
        raise PackageError(
            "unsupported_config_key",
            "config key cannot be unset: %s" % key,
            "Only packages.ref is optional.",
        )
    field = key.split(".", 1)[1]
    with config_lock():
        value = load_config()
        packages = value.get("packages", {})
        if field not in packages:
            raise PackageError(
                "config_key_not_found",
                "config key is not set: %s" % key,
            )
        del packages[field]
        if not packages:
            del value["packages"]
        write_config(value)
    return {"key": key}


def _target_descriptor(arguments, existing=None):
    descriptor = {} if existing is None else dict(existing)
    fields = {
        "adapter": arguments.adapter,
        "projectFile": arguments.project_file,
        "targetName": arguments.target_name,
        "toolExecutable": arguments.tool_executable,
        "outputDirectory": arguments.output_directory,
        "buildDirectory": arguments.build_directory,
        "arguments": arguments.argument,
    }
    for field, item in fields.items():
        if item is not None:
            descriptor[field] = item
    return descriptor


def add_target(arguments):
    with config_lock():
        value = load_config()
        targets = value.setdefault("targets", {})
        if arguments.id in targets:
            raise PackageError(
                "duplicate_target",
                "target already exists: %s" % arguments.id,
            )
        descriptor = _target_descriptor(arguments)
        targets[arguments.id] = descriptor
        if arguments.default:
            value["defaultTarget"] = arguments.id
        validate_config(value)
        write_config(value)
    return {
        "id": arguments.id,
        "target": descriptor,
        "default": value.get("defaultTarget") == arguments.id,
    }


def update_target(arguments):
    with config_lock():
        value = load_config()
        targets = value.get("targets", {})
        if arguments.id not in targets:
            raise PackageError(
                "target_not_found",
                "target does not exist: %s" % arguments.id,
            )
        descriptor = _target_descriptor(arguments, targets[arguments.id])
        targets[arguments.id] = descriptor
        if arguments.default:
            value["defaultTarget"] = arguments.id
        validate_config(value)
        write_config(value)
    return {
        "id": arguments.id,
        "target": descriptor,
        "default": value.get("defaultTarget") == arguments.id,
    }


def remove_target(target_id):
    with config_lock():
        value = load_config()
        targets = value.get("targets", {})
        if target_id not in targets:
            raise PackageError(
                "target_not_found",
                "target does not exist: %s" % target_id,
            )
        del targets[target_id]
        if value.get("defaultTarget") == target_id:
            del value["defaultTarget"]
        if not targets:
            value.pop("targets", None)
        write_config(value)
    return {"id": target_id}


def list_targets():
    value = load_config()
    default_target = value.get("defaultTarget")
    return {
        "targets": [
            {
                "id": target_id,
                "default": target_id == default_target,
                "target": descriptor,
            }
            for target_id, descriptor in value.get("targets", {}).items()
        ]
    }


def show_target(target_id):
    value = load_config()
    targets = value.get("targets", {})
    if target_id not in targets:
        raise PackageError(
            "target_not_found",
            "target does not exist: %s" % target_id,
        )
    return {
        "id": target_id,
        "default": value.get("defaultTarget") == target_id,
        "target": targets[target_id],
    }
