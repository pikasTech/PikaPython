import shutil
import subprocess

from .errors import PackageError
from . import keil_adapter


def _output_tail(value, maximum=4000):
    text = value.strip()
    return text if len(text) <= maximum else text[-maximum:]


def _failure_hint(detail):
    normalized = detail.lower()
    if "stdlib.h: no such file or directory" in normalized:
        return (
            "The target C library headers are missing. For an ARM GNU "
            "toolchain on Debian/Ubuntu, install them with "
            '"apt-get install -y --no-install-recommends '
            'libnewlib-arm-none-eabi"; on other hosts, install the '
            "toolchain's matching newlib or sysroot headers."
        )
    return "Inspect the configured CMake target and bounded command output."


def _run(command, project, stage):
    result = subprocess.run(
        command,
        cwd=project,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
        check=False,
    )
    record = {
        "command": command,
        "exitCode": result.returncode,
        "stdoutTail": _output_tail(result.stdout),
        "stderrTail": _output_tail(result.stderr),
    }
    if result.returncode != 0:
        detail = record["stderrTail"] or record["stdoutTail"]
        raise PackageError(
            "toolchain_failed",
            "CMake %s failed%s"
            % (stage, ": %s" % detail if detail else ""),
            _failure_hint(detail),
            stage=stage,
        )
    return record


def _expand_argument(value, paths):
    expanded = value
    for name, path in paths.items():
        expanded = expanded.replace("{%s}" % name, str(path))
    return expanded


def build_cmake(project, descriptor, generated, source):
    executable_name = descriptor.get("toolExecutable", "cmake")
    executable = shutil.which(executable_name)
    if executable is None:
        raise PackageError(
            "toolchain_not_found",
            "CMake executable was not found: %s" % executable_name,
            "Install CMake or set target.toolExecutable.",
            stage="compile",
        )
    project_file = (project / descriptor["projectFile"]).resolve()
    if not project_file.is_file() or project_file.name != "CMakeLists.txt":
        raise PackageError(
            "project_file_not_found",
            "CMake project file was not found: %s"
            % descriptor["projectFile"],
            stage="configure",
        )
    build_directory = (project / descriptor["buildDirectory"]).resolve()
    build_directory.mkdir(parents=True, exist_ok=True)
    paths = {
        "project": project,
        "source": source,
        "generated": generated,
        "build": build_directory,
    }
    arguments = [
        _expand_argument(item, paths)
        for item in descriptor.get("arguments", [])
    ]
    configure = _run(
        [
            executable,
            "-S",
            str(project_file.parent),
            "-B",
            str(build_directory),
            *arguments,
        ],
        project,
        "configure",
    )
    compile_result = _run(
        [
            executable,
            "--build",
            str(build_directory),
            "--target",
            descriptor["targetName"],
        ],
        project,
        "compile",
    )
    return {
        "adapter": "cmake",
        "targetName": descriptor["targetName"],
        "buildDirectory": descriptor["buildDirectory"],
        "configure": configure,
        "compile": compile_result,
    }


def sync_keil(project, descriptor, generated):
    return keil_adapter.sync_project(project, descriptor, generated)


def build_keil(project, descriptor):
    return keil_adapter.compile_project(project, descriptor)
