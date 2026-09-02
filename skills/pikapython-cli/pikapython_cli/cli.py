# SPEC: PJ2026-050110 PikaPython CLI v0.21; visible default product source.
import argparse
import json
import sys

import yaml

from . import commands
from .errors import PackageError
from .identity import get_identity, get_project_ref


class PackageArgumentParser(argparse.ArgumentParser):
    def error(self, message):
        raise PackageError("invalid_arguments", message)


def _add_target_fields(parser, adapter_required):
    parser.add_argument("--id", required=True)
    parser.add_argument("--adapter", required=adapter_required)
    parser.add_argument("--project-file")
    parser.add_argument("--target-name")
    parser.add_argument("--tool-executable")
    parser.add_argument("--output-directory")
    parser.add_argument("--build-directory")
    parser.add_argument("--argument", action="append")
    parser.add_argument("--default", action="store_true")


def create_parser():
    parser = PackageArgumentParser(
        prog="pikapython-cli",
        description="Manage PikaPython packages and generated target sources.",
    )
    parser.add_argument(
        "--json",
        action="store_true",
        help="emit JSON instead of human-readable text",
    )
    parser.add_argument(
        "--version",
        action="version",
        version=json.dumps(get_identity(), ensure_ascii=True, sort_keys=True),
    )
    actions = parser.add_subparsers(dest="action", required=True)
    actions.add_parser("init", help="create pikapython.yaml")

    add = actions.add_parser("add", help="add a dependency")
    add.add_argument("dependency")
    remove = actions.add_parser("remove", help="remove a dependency")
    remove.add_argument("name")
    actions.add_parser("list", help="list dependencies")
    actions.add_parser("install", help="install configured dependencies")

    build = actions.add_parser("build", help="prebuild and optionally compile")
    build.add_argument("--prebuild-only", action="store_true")
    build.add_argument("--target")
    build.add_argument("--output")

    config = actions.add_parser("config", help="manage project configuration")
    config_actions = config.add_subparsers(dest="config_action", required=True)
    config_actions.add_parser("show")
    config_actions.add_parser("validate")
    capability = config_actions.add_parser("capability")
    capability.add_argument("--config-file", required=True)
    capability.add_argument("--profile", required=True)
    python_modules = config_actions.add_parser("python-modules")
    python_modules.add_argument("--format", required=True)
    python_modules.add_argument(
        "--include", action="append", required=True
    )
    config_set = config_actions.add_parser("set")
    config_set.add_argument("key")
    config_set.add_argument("value")
    config_unset = config_actions.add_parser("unset")
    config_unset.add_argument("key")

    target = actions.add_parser("target", help="manage compiler targets")
    target_actions = target.add_subparsers(dest="target_action", required=True)
    target_add = target_actions.add_parser("add")
    _add_target_fields(target_add, True)
    target_update = target_actions.add_parser("update")
    _add_target_fields(target_update, False)
    target_remove = target_actions.add_parser("remove")
    target_remove.add_argument("--id", required=True)
    target_show = target_actions.add_parser("show")
    target_show.add_argument("--id", required=True)
    target_actions.add_parser("list")
    return parser


def _dependency_lines(data):
    dependencies = data["dependencies"]
    if not dependencies:
        return ["No dependencies configured."]
    return ["Dependencies:"] + [
        "  - %s" % item["specification"] for item in dependencies
    ]


def _target_lines(data):
    targets = data["targets"]
    if not targets:
        return ["No targets configured."]
    lines = ["Targets:"]
    for item in targets:
        suffix = " (default)" if item["default"] else ""
        lines.append(
            "  - %s: %s%s"
            % (item["id"], item["target"]["adapter"], suffix)
        )
    return lines


def human_lines(operation, data):
    if operation == "project.init":
        return [
            "Initialized pikapython.yaml.",
            "Default dependency: %s" % data["dependencies"][0],
            "Package source: %s" % data["packages"]["sourceUrl"],
            "Package ref: %s" % data["packages"]["ref"],
        ]
    if operation == "dependency.add":
        return ["Added dependency: %s" % data["dependency"]]
    if operation == "dependency.remove":
        return ["Removed dependency: %s" % data["name"]]
    if operation == "dependency.list":
        return _dependency_lines(data)
    if operation == "dependency.install":
        return [
            "Installed %d package(s)." % len(data["packages"]),
            "Files: %d" % data["fileCount"],
            "Manifest: %s" % data["manifest"],
        ]
    if operation == "project.build":
        if data.get("prebuildSkipped"):
            lines = [
                "Prebuild validation completed.",
                "Generated: skipped for external project",
            ]
        else:
            lines = [
                "Prebuild completed.",
                "Generated: %s" % data["generatedDirectory"],
                "Program Image modules: %d" % data["programModuleCount"],
                "Frozen Source modules: %d" % data["sourceModuleCount"],
                "Skipped modules: %d" % data["skippedModuleCount"],
            ]
        for skipped in data.get("skippedModules", []):
            lines.append(
                "  - skipped %s: %s"
                % (skipped["name"], skipped["reason"])
            )
        integration = data.get("integration")
        if integration is not None:
            added = sum(
                len(group["added"]) for group in integration["groups"]
            )
            removed = sum(
                len(group["removed"]) for group in integration["groups"]
            )
            lines.extend([
                "Keil project: %s" % integration["projectFile"],
                "Project sync: %s"
                % ("updated" if integration["projectChanged"] else "unchanged"),
                "Managed sources: +%d -%d" % (added, removed),
            ])
        if data["built"]:
            adapter = data["toolchain"]["adapter"]
            lines.extend([
                "Build completed.",
                "Target: %s" % data["target"],
                "Adapter: %s" % adapter,
                "Toolchain target: %s"
                % data["toolchain"]["targetName"],
            ])
            if data["toolchain"].get("logFile"):
                lines.extend([
                    "Errors: %d" % data["toolchain"]["errors"],
                    "Warnings: %d" % data["toolchain"]["warnings"],
                    "Build log: %s" % data["toolchain"]["logFile"],
                ])
        elif data.get("reason") == "requested":
            lines.extend(["Compiler: skipped", "Reason: prebuild-only was requested"])
        elif data.get("warning") == "target_config_missing":
            lines.extend([
                "Build: skipped",
                "Warning: no target configuration found.",
                "Hint: %s" % data["hint"],
            ])
        return lines
    if operation == "config.show":
        return [
            yaml.safe_dump(
                data["config"],
                allow_unicode=True,
                default_flow_style=False,
                sort_keys=False,
            ).rstrip()
        ]
    if operation == "config.validate":
        return [
            "Configuration is valid.",
            "Project kind: %s" % data["projectKind"],
            "Dependencies: %d" % data["dependencyCount"],
            "Targets: %d" % data["targetCount"],
        ]
    if operation == "config.capability":
        return [
            "Configured capability profile: %s" % data["profile"],
            "Capability config: %s" % data["configFile"],
        ]
    if operation == "config.python-modules":
        return [
            "Configured Python modules: %s" % data["format"],
            "Included: %s" % ", ".join(data["include"]),
        ]
    if operation == "config.set":
        return ["Set %s." % data["key"]]
    if operation == "config.unset":
        return ["Unset %s." % data["key"]]
    if operation == "target.add":
        return ["Added target: %s" % data["id"]]
    if operation == "target.update":
        return ["Updated target: %s" % data["id"]]
    if operation == "target.remove":
        return ["Removed target: %s" % data["id"]]
    if operation == "target.show":
        suffix = " (default)" if data["default"] else ""
        return [
            "Target: %s%s" % (data["id"], suffix),
            yaml.safe_dump(
                data["target"],
                allow_unicode=True,
                default_flow_style=False,
                sort_keys=False,
            ).rstrip(),
        ]
    if operation == "target.list":
        return _target_lines(data)
    return ["Completed: %s" % operation]


def emit_success(operation, data, json_output):
    payload = {
        "ok": True,
        "operation": operation,
        "data": data,
        "cli": get_identity(),
        "projectRef": get_project_ref(),
    }
    if json_output:
        print(json.dumps(payload, ensure_ascii=True, indent=2, sort_keys=True))
    else:
        lines = human_lines(operation, data)
        lines.append("CLI source: %s" % (payload["cli"]["sourceCommit"] or "unknown"))
        lines.append("CLI package: %s" % payload["cli"]["packageVersion"])
        if payload["projectRef"] is not None:
            lines.append("Project package ref: %s" % payload["projectRef"])
        print("\n".join(lines))
    return 0


def emit_error(error, json_output):
    payload = {
        "ok": False,
        "operation": "error",
        "code": error.code,
        "error": str(error),
        "cli": get_identity(),
        "projectRef": get_project_ref(),
    }
    if error.stage is not None:
        payload["stage"] = error.stage
    if error.module is not None:
        payload["module"] = error.module
    if error.hint is not None:
        payload["hint"] = error.hint
    if json_output:
        print(json.dumps(payload, ensure_ascii=True, indent=2, sort_keys=True))
    else:
        print("Error [%s]: %s" % (error.code, error), file=sys.stderr)
        if error.hint is not None:
            print("Hint: %s" % error.hint, file=sys.stderr)
        if error.module is not None:
            print("Module: %s" % error.module, file=sys.stderr)
        print(
            "CLI source: %s"
            % (payload["cli"]["sourceCommit"] or "unknown"),
            file=sys.stderr,
        )
        if payload["projectRef"] is not None:
            print(
                "Project package ref: %s" % payload["projectRef"],
                file=sys.stderr,
            )
    return 1


def dispatch(arguments):
    if arguments.action == "init":
        return "project.init", commands.initialize_project()
    if arguments.action == "add":
        return "dependency.add", commands.add_dependency(arguments.dependency)
    if arguments.action == "remove":
        return "dependency.remove", commands.remove_dependency(arguments.name)
    if arguments.action == "list":
        return "dependency.list", commands.list_dependencies()
    if arguments.action == "install":
        return "dependency.install", commands.install_dependencies()
    if arguments.action == "build":
        return "project.build", commands.build_project(arguments)
    if arguments.action == "config":
        if arguments.config_action == "show":
            return "config.show", commands.show_config()
        if arguments.config_action == "validate":
            return "config.validate", commands.validate_project_config()
        if arguments.config_action == "capability":
            return "config.capability", commands.set_capability(
                arguments.config_file, arguments.profile
            )
        if arguments.config_action == "python-modules":
            return "config.python-modules", commands.set_python_modules(
                arguments.format, arguments.include
            )
        if arguments.config_action == "set":
            return "config.set", commands.set_config(arguments.key, arguments.value)
        return "config.unset", commands.unset_config(arguments.key)
    if arguments.action == "target":
        operation = "target.%s" % arguments.target_action
        if arguments.target_action == "add":
            return operation, commands.add_target(arguments)
        if arguments.target_action == "update":
            return operation, commands.update_target(arguments)
        if arguments.target_action == "remove":
            return operation, commands.remove_target(arguments.id)
        if arguments.target_action == "show":
            return operation, commands.show_target(arguments.id)
        return operation, commands.list_targets()
    raise PackageError(
        "not_implemented",
        "%s is not implemented yet" % arguments.action,
        stage=arguments.action,
    )


def main(argv=None):
    raw_arguments = list(sys.argv[1:] if argv is None else argv)
    json_count = raw_arguments.count("--json")
    json_output = json_count == 1
    if json_count > 1:
        return emit_error(
            PackageError("duplicate_option", "--json was provided more than once"),
            True,
        )
    parse_arguments = [item for item in raw_arguments if item != "--json"]
    try:
        arguments = create_parser().parse_args(parse_arguments)
        operation, data = dispatch(arguments)
        return emit_success(operation, data, json_output)
    except PackageError as error:
        return emit_error(error, json_output)


if __name__ == "__main__":
    raise SystemExit(main())
