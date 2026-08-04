import hashlib
import io
import json
import os
import re
import shutil
import subprocess
import tarfile
import tempfile
from pathlib import Path, PurePosixPath

from .configuration import config_path, parse_dependency
from .errors import PackageError


CATALOG_NAME = "package-catalog.json"
CATALOG_SCHEMA = "pika.package-catalog/v1"
MANIFEST_SCHEMA = "pika.package-install/v1"
STATE_DIRECTORY = ".pikapython"
NETWORK_DISABLE_ENV = "PIKAPYTHON_CLI_NETWORK_DISABLE"
MODULE_PACKAGE_METADATA_FILES = {"LICENSE", "UPSTREAM.md"}
FULL_COMMIT_PATTERN = re.compile(r"^[0-9a-fA-F]{40}$")


def _run_git(arguments, cwd=None, binary=False):
    result = subprocess.run(
        ["git", *arguments],
        cwd=cwd,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=not binary,
        check=False,
    )
    if result.returncode != 0:
        error = result.stderr
        if isinstance(error, bytes):
            error = error.decode("utf-8", errors="replace")
        raise PackageError(
            "git_command_failed",
            error.strip() or "git command failed",
            stage="install",
        )
    return result.stdout


def _resolve_commit(repository, reference):
    candidates = ["HEAD"] if reference is None else [
        reference,
        "origin/%s" % reference,
        "refs/tags/%s" % reference,
    ]
    for candidate in candidates:
        result = subprocess.run(
            ["git", "rev-parse", "--verify", "%s^{commit}" % candidate],
            cwd=repository,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            check=False,
        )
        if result.returncode == 0:
            return result.stdout.strip()
    raise PackageError(
        "package_ref_not_found",
        "package source ref does not exist: %s" % reference,
        stage="install",
    )


def _clone_remote(source_url, reference, repository):
    if isinstance(reference, str) and FULL_COMMIT_PATTERN.fullmatch(reference):
        repository.mkdir()
        _run_git(["init"], cwd=repository)
        _run_git(["remote", "add", "origin", source_url], cwd=repository)
        _run_git(["fetch", "--depth", "1", "origin", reference], cwd=repository)
        return
    arguments = ["clone", "--no-checkout", "--depth", "1"]
    if reference is not None:
        arguments.extend(["--branch", reference, "--single-branch"])
    arguments.extend([source_url, str(repository)])
    _run_git(arguments)


def _safe_extract(archive_bytes, destination):
    with tarfile.open(fileobj=io.BytesIO(archive_bytes), mode="r:") as archive:
        for member in archive.getmembers():
            path = PurePosixPath(member.name)
            if (
                path.is_absolute()
                or ".." in path.parts
                or member.issym()
                or member.islnk()
            ):
                raise PackageError(
                    "unsafe_package_archive",
                    "package source contains an unsafe archive path",
                    stage="install",
                )
            target = destination.joinpath(*path.parts)
            if member.isdir():
                target.mkdir(parents=True, exist_ok=True)
                continue
            if not member.isfile():
                raise PackageError(
                    "unsupported_package_entry",
                    "package source contains an unsupported archive entry",
                    stage="install",
                )
            target.parent.mkdir(parents=True, exist_ok=True)
            source = archive.extractfile(member)
            if source is None:
                raise PackageError(
                    "package_archive_read_failed",
                    "package source archive cannot be read",
                    stage="install",
                )
            with source, target.open("wb") as output:
                shutil.copyfileobj(source, output)


def _source_path(project, source_url):
    candidate = Path(source_url).expanduser()
    if not candidate.is_absolute():
        candidate = project / candidate
    candidate = candidate.resolve()
    return candidate if candidate.exists() else None


def _local_git_hint():
    return (
        "Use the Git working tree root. For an unpacked source snapshot, "
        "initialize and commit a local Git repository or use a Git checkout; "
        "when using local HEAD, run "
        '"pikapython-cli config unset packages.ref".'
    )


def prepare_snapshot(project, packages, transaction):
    source_url = packages.get("sourceUrl")
    if not isinstance(source_url, str) or not source_url:
        raise PackageError(
            "package_source_missing",
            "packages.sourceUrl is required before install",
            'Run "pikapython-cli config set packages.sourceUrl <path-or-url>".',
            stage="install",
        )
    reference = packages.get("ref")
    local_source = _source_path(project, source_url)
    if local_source is not None:
        repository = local_source
        source_type = "local-git"
        if not repository.is_dir():
            raise PackageError(
                "package_source_not_git",
                "packages.sourceUrl is not a Git working tree root",
                _local_git_hint(),
                stage="install",
            )
        result = subprocess.run(
            ["git", "rev-parse", "--show-toplevel"],
            cwd=repository,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            check=False,
        )
        if result.returncode == 0:
            repository_root = Path(result.stdout.strip()).resolve()
        else:
            repository_root = None
        if repository_root != repository.resolve():
            raise PackageError(
                "package_source_not_git",
                "packages.sourceUrl is not a Git working tree root",
                _local_git_hint(),
                stage="install",
            )
    else:
        if os.environ.get(NETWORK_DISABLE_ENV) == "1":
            raise PackageError(
                "network_disabled",
                "remote package source is disabled for this workflow",
                stage="install",
            )
        repository = transaction / "repository"
        _clone_remote(source_url, reference, repository)
        source_type = "remote-git"
    commit = _resolve_commit(repository, reference)
    archive = _run_git(
        ["archive", "--format=tar", commit],
        cwd=repository,
        binary=True,
    )
    snapshot = transaction / "snapshot"
    snapshot.mkdir()
    _safe_extract(archive, snapshot)
    return snapshot, {
        "type": source_type,
        "url": source_url,
        "ref": reference,
        "commit": commit,
    }


def _load_catalog(snapshot):
    path = snapshot / CATALOG_NAME
    try:
        value = json.loads(path.read_text(encoding="utf-8"))
    except FileNotFoundError as error:
        raise PackageError(
            "package_catalog_missing",
            "%s is missing from the package source" % CATALOG_NAME,
            stage="install",
        ) from error
    except (OSError, UnicodeError, json.JSONDecodeError) as error:
        raise PackageError(
            "package_catalog_invalid",
            "cannot read %s: %s" % (CATALOG_NAME, error),
            stage="install",
        ) from error
    if (
        not isinstance(value, dict)
        or set(value) != {"schema", "packages"}
        or value.get("schema") != CATALOG_SCHEMA
        or not isinstance(value.get("packages"), dict)
    ):
        raise PackageError(
            "package_catalog_invalid",
            "unsupported package catalog",
            stage="install",
        )
    return value["packages"]


def _relative_path(value, field):
    if not isinstance(value, str) or not value:
        raise PackageError(
            "package_catalog_invalid",
            "%s must be a non-empty relative path" % field,
            stage="install",
        )
    path = PurePosixPath(value)
    if path.is_absolute() or ".." in path.parts or chr(92) in value:
        raise PackageError(
            "package_catalog_invalid",
            "%s must be a portable relative path" % field,
            stage="install",
        )
    return path


def _validate_destination(destination):
    parts = destination.parts
    suffix = destination.suffix
    if len(parts) == 1:
        if suffix not in {".py", ".pyi"}:
            raise PackageError(
                "package_layout_invalid",
                "public package files must be .py or .pyi",
                stage="install",
            )
        return
    if parts[0] == "pikapython-kernel" and len(parts) == 2:
        if suffix not in {".c", ".h", ".inc"}:
            raise PackageError(
                "package_layout_invalid",
                "kernel package files must be flat C, H, or INC files",
                stage="install",
            )
        return
    if parts[0] == "pikapython-packages" and len(parts) >= 3:
        if (
            suffix not in {".c", ".h"}
            and destination.name not in MODULE_PACKAGE_METADATA_FILES
        ):
            raise PackageError(
                "package_layout_invalid",
                "module package files must be C, H, or supported metadata",
                stage="install",
            )
        return
    raise PackageError(
        "package_layout_invalid",
        "package destination is outside the supported layout: %s"
        % destination.as_posix(),
        stage="install",
    )


def _glob_base(pattern):
    parts = PurePosixPath(pattern).parts
    for index, part in enumerate(parts):
        if any(marker in part for marker in ("*", "?", "[")):
            return PurePosixPath(*parts[:index])
    return PurePosixPath(*parts[:-1])


def _copy_export(
    snapshot, publish, export, destinations, origins, owners, package_name
):
    if not isinstance(export, dict) or not {"source", "destination"}.issubset(export):
        raise PackageError(
            "package_catalog_invalid",
            "package export is invalid",
            stage="install",
        )
    if set(export).difference({"source", "destination", "exclude"}):
        raise PackageError(
            "package_catalog_invalid",
            "package export contains unknown fields",
            stage="install",
        )
    source_pattern = _relative_path(export["source"], "export.source").as_posix()
    source_base = snapshot.joinpath(*_glob_base(source_pattern).parts)
    destination_text = export["destination"]
    destination_base = _relative_path(destination_text, "export.destination")
    excluded = {
        _relative_path(item, "export.exclude").as_posix()
        for item in export.get("exclude", [])
    }
    matches = [
        path for path in sorted(snapshot.glob(source_pattern))
        if path.is_file()
        and path.relative_to(snapshot).as_posix() not in excluded
    ]
    if not matches:
        raise PackageError(
            "package_export_empty",
            "package export matched no files: %s" % source_pattern,
            stage="install",
        )
    if not destination_text.endswith("/") and len(matches) != 1:
        raise PackageError(
            "package_catalog_invalid",
            "a multi-file export requires a directory destination",
            stage="install",
        )
    for source in matches:
        destination = (
            destination_base / source.relative_to(source_base)
            if destination_text.endswith("/")
            else destination_base
        )
        _validate_destination(destination)
        relative = destination.as_posix()
        if relative in destinations:
            raise PackageError(
                "package_file_conflict",
                "multiple packages own %s" % relative,
                stage="install",
            )
        destinations.add(relative)
        origins[relative] = source.relative_to(snapshot).as_posix()
        owners[relative] = package_name
        target = publish.joinpath(*destination.parts)
        target.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(source, target)


def _validated_descriptor(catalog, name, version):
    descriptor = catalog.get(name)
    if not isinstance(descriptor, dict):
        raise PackageError(
            "package_not_found",
            "package is not present in the source: %s" % name,
            stage="install",
        )
    if (
        not {"version", "exports"}.issubset(descriptor)
        or set(descriptor).difference(
            {"version", "exports", "capability", "dependencies"}
        )
    ):
        raise PackageError(
            "package_catalog_invalid",
            "package descriptor is invalid: %s" % name,
            stage="install",
        )
    if descriptor["version"] != version:
        raise PackageError(
            "package_version_not_found",
            "package %s does not provide version %s" % (name, version),
            stage="install",
        )
    dependencies = descriptor.get("dependencies", [])
    if not isinstance(dependencies, list):
        raise PackageError(
            "package_catalog_invalid",
            "package dependencies must be a list: %s" % name,
            stage="install",
        )
    for dependency in dependencies:
        try:
            parse_dependency(dependency)
        except PackageError as error:
            raise PackageError(
                "package_catalog_invalid",
                "package dependency is invalid: %s" % name,
                stage="install",
            ) from error
    return descriptor


def _resolve_dependencies(catalog, dependencies):
    selected = {}
    states = {}
    resolved = []

    def visit(specification, chain):
        name, version = parse_dependency(specification)
        selected_version = selected.get(name)
        if selected_version is not None and selected_version != version:
            raise PackageError(
                "package_dependency_conflict",
                "package %s requires both %s and %s"
                % (name, selected_version, version),
                stage="install",
            )
        state = states.get(name)
        if state == "visiting":
            raise PackageError(
                "package_dependency_cycle",
                "package dependency cycle: %s"
                % " -> ".join(chain + [name]),
                stage="install",
            )
        if state == "resolved":
            return
        descriptor = _validated_descriptor(catalog, name, version)
        selected[name] = version
        states[name] = "visiting"
        for dependency in descriptor.get("dependencies", []):
            visit(dependency, chain + [name])
        states[name] = "resolved"
        resolved.append((name, version, descriptor))

    for dependency in dependencies:
        visit(dependency, [])
    return resolved


def stage_packages(snapshot, dependencies, publish):
    catalog = _load_catalog(snapshot)
    installed = []
    destinations = set()
    origins = {}
    owners = {}
    for name, version, descriptor in _resolve_dependencies(
        catalog, dependencies
    ):
        exports = descriptor["exports"]
        capability = descriptor.get("capability")
        if capability is not None and (
            not isinstance(capability, str) or not capability
        ):
            raise PackageError(
                "package_catalog_invalid",
                "package capability must be a non-empty string: %s" % name,
                stage="install",
            )
        if not isinstance(exports, list) or not exports:
            raise PackageError(
                "package_catalog_invalid",
                "package exports must be a non-empty list: %s" % name,
                stage="install",
            )
        for export in exports:
            _copy_export(
                snapshot,
                publish,
                export,
                destinations,
                origins,
                owners,
                name,
            )
        installed_package = {"name": name, "version": version}
        if capability is not None:
            installed_package["capability"] = capability
        installed.append(installed_package)
    return installed, sorted(destinations), origins, owners


def _read_previous_manifest(state):
    path = state / "install-manifest.json"
    if not path.exists():
        return None
    try:
        value = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, UnicodeError, json.JSONDecodeError) as error:
        raise PackageError(
            "install_manifest_invalid",
            "cannot read the previous install manifest",
            stage="install",
        ) from error
    if (
        not isinstance(value, dict)
        or value.get("schema") != MANIFEST_SCHEMA
        or not isinstance(value.get("managedTargets"), list)
    ):
        raise PackageError(
            "install_manifest_invalid",
            "the previous install manifest is unsupported",
            stage="install",
        )
    return value


def _write_json_atomic(path, value):
    descriptor, name = tempfile.mkstemp(
        prefix=".manifest-",
        suffix=".json",
        dir=str(path.parent),
    )
    temporary = Path(name)
    try:
        with os.fdopen(descriptor, "w", encoding="utf-8", newline="\n") as handle:
            json.dump(value, handle, ensure_ascii=True, indent=2, sort_keys=True)
            handle.write("\n")
            handle.flush()
            os.fsync(handle.fileno())
        os.replace(temporary, path)
    finally:
        if temporary.exists():
            temporary.unlink()


def publish_install(project, state, publish, snapshot, manifest):
    previous = _read_previous_manifest(state)
    previous_targets = set(previous["managedTargets"] if previous else [])
    new_targets = set(manifest["managedTargets"])
    for target in sorted(new_targets.difference(previous_targets)):
        if (project / target).exists():
            raise PackageError(
                "package_file_conflict",
                "install target already exists and is not package-owned: %s"
                % target,
                stage="install",
            )
    backup = publish.parent / "backup"
    backup.mkdir()
    source_target = state / "source"
    source_backup = publish.parent / "source-backup"
    moved_targets = []
    installed_targets = []
    moved_source = False
    try:
        for target in sorted(previous_targets.union(new_targets)):
            path = project / target
            if path.exists():
                os.replace(path, backup / target)
                moved_targets.append(target)
        if source_target.exists():
            os.replace(source_target, source_backup)
            moved_source = True
        for target in sorted(new_targets):
            os.replace(publish / target, project / target)
            installed_targets.append(target)
        os.replace(snapshot, source_target)
        _write_json_atomic(state / "install-manifest.json", manifest)
    except Exception:
        for target in reversed(installed_targets):
            path = project / target
            if path.is_dir():
                shutil.rmtree(path)
            elif path.exists():
                path.unlink()
        if source_target.exists():
            shutil.rmtree(source_target)
        if moved_source and source_backup.exists():
            os.replace(source_backup, source_target)
        for target in reversed(moved_targets):
            backup_path = backup / target
            if backup_path.exists():
                os.replace(backup_path, project / target)
        raise


def install(config):
    project = config_path().parent
    state = project / STATE_DIRECTORY
    transactions = state / "transactions"
    transactions.mkdir(parents=True, exist_ok=True)
    transaction = Path(tempfile.mkdtemp(prefix="install-", dir=str(transactions)))
    try:
        if config.get("projectKind", "pikapython") == "external":
            snapshot = transaction / "source"
            snapshot.mkdir()
            publish = transaction / "publish"
            publish.mkdir()
            manifest = {
                "schema": MANIFEST_SCHEMA,
                "source": {
                    "type": "external-project",
                    "path": ".",
                },
                "requestedPackages": [],
                "packages": [],
                "files": [],
                "managedTargets": [],
                "fileOrigins": {},
                "filePackages": {},
                "fileListDigest": hashlib.sha256(b"").hexdigest(),
            }
            publish_install(
                project, state, publish, snapshot, manifest
            )
            return {
                "source": manifest["source"],
                "packages": [],
                "fileCount": 0,
                "manifest": "%s/install-manifest.json" % STATE_DIRECTORY,
            }
        snapshot, source = prepare_snapshot(
            project,
            config.get("packages", {}),
            transaction,
        )
        publish = transaction / "publish"
        publish.mkdir()
        packages, files, origins, owners = stage_packages(
            snapshot,
            config["dependencies"],
            publish,
        )
        managed_targets = sorted({path.split("/", 1)[0] for path in files})
        file_digest = hashlib.sha256(
            "\n".join(files).encode("utf-8")
        ).hexdigest()
        manifest = {
            "schema": MANIFEST_SCHEMA,
            "source": source,
            "requestedPackages": [
                {"name": parse_dependency(item)[0],
                 "version": parse_dependency(item)[1]}
                for item in config["dependencies"]
            ],
            "packages": packages,
            "files": files,
            "managedTargets": managed_targets,
            "fileOrigins": {path: origins[path] for path in files},
            "filePackages": {path: owners[path] for path in files},
            "fileListDigest": file_digest,
        }
        publish_install(project, state, publish, snapshot, manifest)
        return {
            "source": source,
            "packages": packages,
            "fileCount": len(files),
            "manifest": "%s/install-manifest.json" % STATE_DIRECTORY,
        }
    finally:
        if transaction.exists():
            shutil.rmtree(transaction)
