#!/usr/bin/env python3
import subprocess
import sys

from provider import ProviderError, qemu_command, template_root, validate_provider


def main():
    root = template_root()
    firmware = root / ".pikapython" / "build" / "qemu" / "qemu-firmware.elf"
    try:
        docker = validate_provider()
        command = qemu_command(docker, firmware)
    except ProviderError as error:
        print("QEMU start failed: %s" % error, file=sys.stderr)
        return 2
    return subprocess.run(command, cwd=root, check=False).returncode


if __name__ == "__main__":
    raise SystemExit(main())
