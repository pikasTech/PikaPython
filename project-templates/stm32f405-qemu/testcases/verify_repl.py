#!/usr/bin/env python3
import shutil
import subprocess
import sys
from pathlib import Path


def fail(message, output=b""):
    print(message, file=sys.stderr)
    if output:
        sys.stderr.buffer.write(output)
    return 1


def main():
    root = Path(__file__).resolve().parents[1]
    firmware = root / ".pikapython" / "build" / "qemu" / "qemu-firmware.elf"
    executable = shutil.which("qemu-system-arm")
    if executable is None:
        return fail("qemu-system-arm was not found")
    if not firmware.is_file():
        return fail("firmware is missing; run pikapython-cli build")
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
    input_bytes = b"40 + 3\x7f2\r12+34\x1b[D5\rmissing_name\r\x04"
    try:
        result = subprocess.run(
            command,
            cwd=root,
            input=input_bytes,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            timeout=20,
            check=False,
        )
    except subprocess.TimeoutExpired as error:
        return fail("QEMU REPL timed out", error.stdout or b"")
    if result.returncode != 0:
        return fail("QEMU REPL exited with status %d" % result.returncode, result.stdout)
    expected = [
        b"PikaPython QEMU ready",
        b"42",
        b"366",
        b"NameError",
    ]
    cursor = 0
    for token in expected:
        found = result.stdout.find(token, cursor)
        if found < 0:
            return fail("QEMU REPL output is missing %r" % token, result.stdout)
        cursor = found + len(token)
    print(
        "QEMU REPL verified: main.py, editing, recovery, Ctrl-D"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
