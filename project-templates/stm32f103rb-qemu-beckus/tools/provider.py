import json
import shutil
import subprocess
from pathlib import Path


IMAGE = "pika-qemu-stm32-beckus:2.1.3"
VERSION = "2.1.3"
MACHINE = "stm32-p103"
CPU = "cortex-m3"
COMMIT = "5ae4df8a7267149f6ff5a9f8fa4f6b8adbc28fbc"
FIRMWARE_MOUNT = "/work/qemu-firmware.elf"
SERIAL_MOUNT_DIRECTORY = "/work/serial"


class ProviderError(RuntimeError):
    pass


def template_root():
    return Path(__file__).resolve().parents[1]


def docker_executable():
    executable = shutil.which("docker")
    if executable is None:
        raise ProviderError(
            "Docker was not found; install Docker before using this template"
        )
    return executable


def _run_checked(command, purpose):
    result = subprocess.run(
        command,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
        check=False,
    )
    if result.returncode != 0:
        detail = (result.stderr or result.stdout).strip()
        raise ProviderError("%s failed: %s" % (purpose, detail))
    return result.stdout


def validate_provider():
    docker = docker_executable()
    try:
        raw = _run_checked(
            [docker, "image", "inspect", IMAGE],
            "Beckus provider inspection",
        )
    except ProviderError as error:
        raise ProviderError(
            "%s; run python3 tools/build-provider.py" % error
        ) from error
    try:
        image = json.loads(raw)[0]
        labels = image["Config"]["Labels"] or {}
    except (IndexError, KeyError, TypeError, json.JSONDecodeError) as error:
        raise ProviderError("Beckus provider metadata is invalid") from error
    expected_labels = {
        "org.opencontainers.image.version": VERSION,
        "org.opencontainers.image.revision": COMMIT,
        "io.pikapython.qemu.machine": MACHINE,
    }
    for name, expected in expected_labels.items():
        if labels.get(name) != expected:
            raise ProviderError(
                "Beckus provider label %s must be %s" % (name, expected)
            )
    version = _run_checked(
        [docker, "run", "--rm", "--network", "none", IMAGE, "--version"],
        "Beckus QEMU version check",
    )
    if "QEMU emulator version %s" % VERSION not in version:
        raise ProviderError("Beckus QEMU version is not %s" % VERSION)
    machines = _run_checked(
        [docker, "run", "--rm", "--network", "none", IMAGE, "-M", "help"],
        "Beckus QEMU machine check",
    )
    if MACHINE not in machines:
        raise ProviderError("Beckus QEMU does not provide %s" % MACHINE)
    return docker


def qemu_command(docker, firmware, serial_output=None):
    firmware = Path(firmware).resolve()
    if not firmware.is_file():
        raise ProviderError(
            "firmware is missing; run pikapython-cli build first"
        )
    command = [
        docker,
        "run",
        "--rm",
        "-i",
        "--network",
        "none",
        "--volume",
        "%s:%s:ro" % (firmware, FIRMWARE_MOUNT),
    ]
    serial_backend = "stdio"
    if serial_output is not None:
        serial_output = Path(serial_output).resolve()
        if not serial_output.parent.is_dir():
            raise ProviderError(
                "serial output directory is missing: %s"
                % serial_output.parent
            )
        command.extend([
            "--volume",
            "%s:%s"
            % (serial_output.parent, SERIAL_MOUNT_DIRECTORY),
        ])
        serial_backend = "file:%s/%s" % (
            SERIAL_MOUNT_DIRECTORY,
            serial_output.name,
        )
    command.extend([
        IMAGE,
        "-M",
        MACHINE,
        "-cpu",
        CPU,
        "-display",
        "none",
        "-monitor",
        "none",
        "-serial",
        serial_backend,
        "-semihosting",
        "-kernel",
        FIRMWARE_MOUNT,
    ])
    return command
