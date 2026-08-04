# SPEC: PJ2026-050110 PikaPython CLI v0.16; external project lifecycle.
import os
import shutil
import tempfile
from contextlib import contextmanager
from pathlib import Path

from . import adapters, prebuild
from .configuration import config_path, parse_dependency
from .errors import PackageError


INSTALL_SCHEMA = "pika.package-install/v1"
STATE_DIRECTORY = ".pikapython"
DEFAULT_OUTPUT = "pikapython-generated"


@contextmanager
def _build_lock(project):
    path = project / STATE_DIRECTORY / "build.lock"
    path.parent.mkdir(parents=True, exist_ok=True)
    handle = path.open("a+", encoding="utf-8")
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


def _load_install(project, config):
    path = project / STATE_DIRECTORY / "install-manifest.json"
    if not path.is_file():
        raise PackageError(
            "dependencies_not_installed",
            "installed dependency manifest was not found",
            'Run "pikapython-cli install" before build.',
            stage="prebuild",
        )
    manifest = prebuild.read_json(path, "install_manifest_invalid")
    if (
        manifest.get("schema") != INSTALL_SCHEMA
        or not isinstance(manifest.get("packages"), list)
        or not isinstance(manifest.get("files"), list)
        or not isinstance(manifest.get("fileOrigins"), dict)
    ):
        raise PackageError(
            "install_manifest_invalid",
            "installed dependency manifest is unsupported",
            stage="prebuild",
        )
    expected = sorted(
        (parse_dependency(item) for item in config["dependencies"]),
        key=lambda item: item[0],
    )
    requested = manifest.get("requestedPackages", manifest["packages"])
    if not isinstance(requested, list):
        raise PackageError(
            "install_manifest_invalid",
            "installed dependency manifest has invalid requested packages",
            stage="prebuild",
        )
    actual = sorted(
        ((item.get("name"), item.get("version")) for item in requested),
        key=lambda item: item[0],
    )
    if expected != actual:
        raise PackageError(
            "installed_dependencies_mismatch",
            "installed dependencies do not match pikapython.yaml",
            'Run "pikapython-cli install" before build.',
            stage="prebuild",
        )
    source = project / STATE_DIRECTORY / "source"
    if not source.is_dir():
        raise PackageError(
            "installed_source_missing",
            "installed Git source snapshot is missing",
            stage="prebuild",
        )
    return manifest, source.resolve()


def _select_target(config, requested, prebuild_only):
    targets = config.get("targets", {})
    if requested is not None:
        if requested not in targets:
            raise PackageError(
                "target_not_found",
                "target does not exist: %s" % requested,
                stage="target-selection",
            )
        return requested, targets[requested]
    default = config.get("defaultTarget")
    if default is not None:
        return default, targets[default]
    if len(targets) == 1:
        target_id = next(iter(targets))
        return target_id, targets[target_id]
    if len(targets) > 1 and not prebuild_only:
        raise PackageError(
            "target_ambiguous",
            "multiple targets exist and no target was selected",
            'Use "pikapython-cli build --target <id>".',
            stage="target-selection",
        )
    return None, None


def _output_path(project, target, output_text):
    selected = output_text
    if selected is None and target is not None:
        selected = target.get("outputDirectory")
    selected = selected or DEFAULT_OUTPUT
    output = (project / selected).resolve()
    if project not in output.parents or output == project:
        raise PackageError(
            "invalid_output",
            "generated output must be inside the project",
            stage="prebuild",
        )
    output.parent.mkdir(parents=True, exist_ok=True)
    return output


def _run_prebuild(project, source, config, install, output):
    transaction_root = project / STATE_DIRECTORY / "transactions"
    transaction_root.mkdir(parents=True, exist_ok=True)
    transaction = Path(
        tempfile.mkdtemp(prefix="build-", dir=transaction_root)
    )
    stage = transaction / "publish"
    stage.mkdir()
    try:
        statistics = prebuild.generate(
            stage, project, source, config, install
        )
        prebuild.publish(stage, output)
        return statistics
    finally:
        if transaction.exists():
            shutil.rmtree(transaction)


def build(
    config, requested_target=None, output_text=None, prebuild_only=False
):
    project = config_path().parent
    with _build_lock(project):
        install, source = _load_install(project, config)
        target_id, target = _select_target(
            config, requested_target, prebuild_only
        )
        external = config.get("projectKind", "pikapython") == "external"
        if external:
            output = project
            statistics = {
                "prebuildSkipped": True,
                "programModuleCount": 0,
                "sourceModuleCount": 0,
                "skippedModuleCount": 0,
                "skippedModules": [],
            }
            generated_directory = None
        else:
            output = _output_path(project, target, output_text)
            statistics = _run_prebuild(
                project, source, config, install, output
            )
            generated_directory = str(output.relative_to(project))
        result = {
            "status": "prebuild-only",
            "built": False,
            "generatedDirectory": generated_directory,
            "target": target_id,
            **statistics,
        }
        if (
            not external
            and target is not None
            and target["adapter"] == "keil"
        ):
            result["integration"] = adapters.sync_keil(
                project, target, output
            )
        if prebuild_only:
            result["reason"] = "requested"
            return result
        if target is None:
            result["warning"] = "target_config_missing"
            result["hint"] = (
                "Use pikapython-cli target add to configure a target."
            )
            return result
        if target["adapter"] == "cmake":
            result["toolchain"] = adapters.build_cmake(
                project, target, output, source
            )
        elif target["adapter"] == "keil":
            result["toolchain"] = adapters.build_keil(project, target)
        else:
            raise PackageError(
                "adapter_not_implemented",
                "target adapter is not implemented: %s" % target["adapter"],
                stage="compile",
            )
        result["status"] = "built"
        result["built"] = True
        return result
