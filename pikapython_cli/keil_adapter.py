import io
import json
import os
import re
import shutil
import subprocess
import tempfile
import xml.etree.ElementTree as ET
from pathlib import Path, PurePosixPath

from .errors import PackageError


MANAGED_GROUPS = (
    ("PikaPython Kernel", "kernel"),
    ("PikaPython Packages", "packages"),
    ("PikaPython Generated", "generated"),
)
BUILD_SUMMARY = re.compile(
    r"(?P<errors>[0-9]+)\s+Error\(s\),\s*"
    r"(?P<warnings>[0-9]+)\s+Warning\(s\)",
    re.IGNORECASE,
)
SOURCE_MANIFEST_SCHEMA = "pika.generated-sources/v1"


def _bounded_tail(value, maximum=4000):
    text = value.strip()
    return text if len(text) <= maximum else text[-maximum:]


def _project_file(project, descriptor):
    path = (project / descriptor["projectFile"]).resolve()
    if path.suffix.lower() != ".uvprojx" or not path.is_file():
        raise PackageError(
            "project_file_not_found",
            "Keil project file was not found: %s"
            % descriptor["projectFile"],
            "Set target.projectFile to an existing .uvprojx file.",
            stage="configure",
        )
    return path


def _read_project(path):
    try:
        content = path.read_bytes()
        tree = ET.ElementTree(ET.fromstring(content))
    except (OSError, ET.ParseError) as error:
        raise PackageError(
            "project_file_invalid",
            "Keil project XML is invalid: %s" % error,
            "Open the .uvprojx in Keil and repair it before building.",
            stage="configure",
        ) from error
    return content, tree


def _select_target(tree, target_name):
    for target in tree.findall("./Targets/Target"):
        if (target.findtext("TargetName") or "") == target_name:
            return target
    raise PackageError(
        "target_not_found",
        "Keil target does not exist: %s" % target_name,
        "Use the exact TargetName stored in the .uvprojx file.",
        stage="configure",
    )


def _relative_path(project_directory, path):
    relative = os.path.relpath(str(path), str(project_directory))
    return relative.replace("/", "\\")


def _path_key(value):
    return os.path.normcase(value.replace("/", "\\"))


def _manifest_path(value, field):
    if not isinstance(value, str) or not value:
        raise PackageError(
            "source_manifest_invalid",
            "%s must contain non-empty relative paths" % field,
            stage="configure",
        )
    path = PurePosixPath(value)
    if path.is_absolute() or ".." in path.parts or chr(92) in value:
        raise PackageError(
            "source_manifest_invalid",
            "%s contains a non-portable path: %s" % (field, value),
            stage="configure",
        )
    return path


def _project_manifest_path(project, generated, path):
    parts = path.parts
    if not parts:
        raise PackageError(
            "source_manifest_invalid",
            "source manifest contains an empty path",
            stage="configure",
        )
    if parts[0] in {"pikapython-kernel", "pikapython-packages"}:
        return project.joinpath(*parts)
    if parts[0] == "pikapython-generated":
        return generated.joinpath(*parts[1:])
    if len(parts) == 1:
        return generated / parts[0]
    raise PackageError(
        "source_manifest_invalid",
        "source manifest path is outside managed roots: %s"
        % path.as_posix(),
        stage="configure",
    )


def _source_layout(project, generated):
    manifest_path = generated / "source-manifest.json"
    try:
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    except (OSError, UnicodeError, json.JSONDecodeError) as error:
        raise PackageError(
            "source_manifest_invalid",
            "cannot read generated source manifest: %s" % error,
            stage="configure",
        ) from error
    if (
        not isinstance(manifest, dict)
        or manifest.get("schema") != SOURCE_MANIFEST_SCHEMA
        or not isinstance(manifest.get("sources"), list)
        or not isinstance(manifest.get("includeDirectories"), list)
    ):
        raise PackageError(
            "source_manifest_invalid",
            "generated source manifest is unsupported",
            stage="configure",
        )
    groups = {"kernel": [], "packages": [], "generated": []}
    seen_sources = set()
    for value in manifest["sources"]:
        path = _manifest_path(value, "sources")
        source = _project_manifest_path(project, generated, path)
        if path.suffix != ".c" or not source.is_file():
            raise PackageError(
                "source_manifest_invalid",
                "source manifest entry is not an existing C file: %s"
                % path.as_posix(),
                stage="configure",
            )
        key = str(source)
        if key in seen_sources:
            raise PackageError(
                "source_manifest_invalid",
                "source manifest contains a duplicate source: %s"
                % path.as_posix(),
                stage="configure",
            )
        seen_sources.add(key)
        if path.parts[0] == "pikapython-kernel":
            groups["kernel"].append(source)
        elif path.parts[0] == "pikapython-packages":
            groups["packages"].append(source)
        else:
            groups["generated"].append(source)
    directories = []
    seen_directories = set()
    for value in manifest["includeDirectories"]:
        path = _manifest_path(value, "includeDirectories")
        directory = _project_manifest_path(project, generated, path)
        if not directory.is_dir():
            raise PackageError(
                "source_manifest_invalid",
                "include manifest entry is not an existing directory: %s"
                % path.as_posix(),
                stage="configure",
            )
        key = str(directory)
        if key not in seen_directories:
            seen_directories.add(key)
            directories.append(directory)
    for paths in groups.values():
        paths.sort()
    return groups, directories


def _groups_node(target):
    groups = target.find("Groups")
    if groups is None:
        groups = ET.SubElement(target, "Groups")
    return groups


def _group_node(groups, name):
    matches = [
        group
        for group in groups.findall("Group")
        if (group.findtext("GroupName") or "") == name
    ]
    if len(matches) > 1:
        raise PackageError(
            "project_group_conflict",
            "Keil project contains duplicate managed group: %s" % name,
            "Remove the duplicate group and retry.",
            stage="configure",
        )
    if matches:
        return matches[0]
    group = ET.SubElement(groups, "Group")
    ET.SubElement(group, "GroupName").text = name
    ET.SubElement(group, "Files")
    return group


def _file_entry(path):
    entry = ET.Element("File")
    ET.SubElement(entry, "FileName").text = Path(path).name
    ET.SubElement(entry, "FileType").text = "1"
    ET.SubElement(entry, "FilePath").text = path
    return entry


def _sync_group(group, paths):
    files = group.find("Files")
    if files is None:
        files = ET.SubElement(group, "Files")
    existing = []
    for entry in files.findall("File"):
        path = entry.findtext("FilePath") or ""
        if path:
            existing.append(path)
    old = {_path_key(path): path for path in existing}
    new = {_path_key(path): path for path in paths}
    added = [new[key] for key in sorted(set(new).difference(old))]
    removed = [old[key] for key in sorted(set(old).difference(new))]
    unchanged = [new[key] for key in sorted(set(old).intersection(new))]
    for child in list(files):
        files.remove(child)
    for path in paths:
        files.append(_file_entry(path))
    return {
        "added": added,
        "removed": removed,
        "unchanged": unchanged,
    }


def _include_node(target):
    node = target.find(
        "./TargetOption/TargetArmAds/Cads/VariousControls/IncludePath"
    )
    if node is None:
        raise PackageError(
            "project_structure_unsupported",
            "Keil target does not contain a C include-path field",
            "Use a complete MDK .uvprojx target with TargetArmAds/Cads settings.",
            stage="configure",
        )
    return node


def _sync_includes(target, paths):
    node = _include_node(target)
    existing = [item for item in (node.text or "").split(";") if item]
    keys = {_path_key(item) for item in existing}
    added = []
    unchanged = []
    for path in paths:
        key = _path_key(path)
        if key in keys:
            unchanged.append(path)
            continue
        existing.append(path)
        keys.add(key)
        added.append(path)
    node.text = ";".join(existing)
    return {"added": added, "unchanged": unchanged}


def _serialize(tree):
    ET.indent(tree, space="  ")
    output = io.BytesIO()
    tree.write(output, encoding="utf-8", xml_declaration=True)
    return output.getvalue() + b"\n"


def _atomic_write(path, content):
    temporary = None
    try:
        descriptor, name = tempfile.mkstemp(
            prefix=".%s-" % path.name,
            suffix=".tmp",
            dir=str(path.parent),
        )
        temporary = Path(name)
        with os.fdopen(descriptor, "wb") as handle:
            handle.write(content)
            handle.flush()
            os.fsync(handle.fileno())
        os.replace(temporary, path)
    except OSError as error:
        raise PackageError(
            "project_write_failed",
            "cannot update Keil project: %s" % error,
            stage="configure",
        ) from error
    finally:
        if temporary is not None and temporary.exists():
            temporary.unlink()


def sync_project(project, descriptor, generated):
    project_file = _project_file(project, descriptor)
    original, tree = _read_project(project_file)
    target = _select_target(tree, descriptor["targetName"])
    source_groups, include_directories = _source_layout(project, generated)
    groups = _groups_node(target)
    group_results = []
    for name, source_key in MANAGED_GROUPS:
        relative_sources = [
            _relative_path(project_file.parent, path)
            for path in source_groups[source_key]
        ]
        group_results.append({
            "name": name,
            **_sync_group(_group_node(groups, name), relative_sources),
        })
    include_paths = [
        _relative_path(project_file.parent, path)
        for path in include_directories
    ]
    include_result = _sync_includes(target, include_paths)
    updated = _serialize(tree)
    changed = updated != original
    if changed:
        _atomic_write(project_file, updated)
    return {
        "adapter": "keil",
        "projectFile": descriptor["projectFile"],
        "targetName": descriptor["targetName"],
        "projectChanged": changed,
        "groups": group_results,
        "includePaths": include_result,
    }


def _resolve_executable(descriptor):
    configured = descriptor.get("toolExecutable")
    if configured:
        path = Path(configured).expanduser()
        if path.is_file():
            return str(path.resolve())
        executable = shutil.which(configured)
        if executable is not None:
            return executable
        raise PackageError(
            "toolchain_not_found",
            "Keil UV4 executable was not found: %s" % configured,
            "Install Keil MDK or set target.toolExecutable to UV4.exe.",
            stage="compile",
        )
    for name in ("UV4.exe", "UV4"):
        executable = shutil.which(name)
        if executable is not None:
            return executable
    if os.name == "nt":
        system_drive = os.environ.get("SystemDrive", "C:")
        for relative in (
            "Keil_v5/UV4/UV4.exe",
            "Keil/UV4/UV4.exe",
        ):
            candidate = Path(system_drive + os.sep) / Path(relative)
            if candidate.is_file():
                return str(candidate)
    raise PackageError(
        "toolchain_not_found",
        "Keil UV4 executable was not found",
        "Install Keil MDK or set target.toolExecutable to UV4.exe.",
        stage="compile",
    )


def compile_project(project, descriptor):
    project_file = _project_file(project, descriptor)
    executable = _resolve_executable(descriptor)
    build_directory = (project / descriptor["buildDirectory"]).resolve()
    build_directory.mkdir(parents=True, exist_ok=True)
    log_path = build_directory / "keil-build.log"
    if log_path.exists():
        log_path.unlink()
    command = [
        executable,
        "-b",
        str(project_file),
        "-t",
        descriptor["targetName"],
        "-o",
        str(log_path),
        *descriptor.get("arguments", []),
    ]
    try:
        completed = subprocess.run(
            command,
            cwd=project_file.parent,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            check=False,
            text=True,
        )
    except OSError as error:
        raise PackageError(
            "toolchain_failed",
            "cannot start Keil UV4: %s" % error,
            "Check target.toolExecutable and the Keil installation.",
            stage="compile",
        ) from error
    try:
        log_text = log_path.read_bytes().decode("utf-8", errors="replace")
    except OSError as error:
        raise PackageError(
            "toolchain_failed",
            "Keil build log was not produced: %s" % error,
            "Inspect the UV4 installation and target project.",
            stage="compile",
        ) from error
    summaries = list(BUILD_SUMMARY.finditer(log_text))
    if not summaries:
        raise PackageError(
            "toolchain_failed",
            "Keil build log does not contain an error summary",
            "Inspect the full build log: %s" % log_path,
            stage="compile",
        )
    summary = summaries[-1]
    errors = int(summary.group("errors"))
    warnings = int(summary.group("warnings"))
    if completed.returncode != 0 or errors != 0:
        detail = _bounded_tail(log_text)
        raise PackageError(
            "toolchain_failed",
            "Keil compile failed with %d error(s)%s"
            % (errors, ": %s" % detail if detail else ""),
            "Inspect the full build log: %s" % log_path,
            stage="compile",
        )
    return {
        "adapter": "keil",
        "targetName": descriptor["targetName"],
        "buildDirectory": descriptor["buildDirectory"],
        "command": command,
        "exitCode": completed.returncode,
        "errors": errors,
        "warnings": warnings,
        "stdoutTail": _bounded_tail(completed.stdout),
        "stderrTail": _bounded_tail(completed.stderr),
        "logFile": str(log_path.relative_to(project)),
        "logTail": _bounded_tail(log_text),
    }
