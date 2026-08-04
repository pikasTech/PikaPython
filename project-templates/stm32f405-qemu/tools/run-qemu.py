#!/usr/bin/env python3
import shutil
import subprocess
import sys
from pathlib import Path


def main():
    root = Path(__file__).resolve().parents[1]
    firmware = root / ".pikapython" / "build" / "qemu" / "qemu-firmware.elf"
    executable = shutil.which("qemu-system-arm")
    if executable is None:
        print("qemu-system-arm was not found", file=sys.stderr)
        return 2
    if not firmware.is_file():
        print("firmware is missing; run pikapython-cli build", file=sys.stderr)
        return 2
    command = [
        executable,
        "-M",
        "netduinoplus2",
        "-cpu",
        "cortex-m4",
        "-display",
        "none",
        "-monitor",
        "none",
        "-serial",
        "none",
        "-chardev",
        "stdio,id=pika_stdio,signal=off",
        "-semihosting-config",
        "enable=on,target=native,chardev=pika_stdio",
        "-kernel",
        str(firmware),
    ]
    return subprocess.run(command, cwd=root, check=False).returncode


if __name__ == "__main__":
    raise SystemExit(main())
