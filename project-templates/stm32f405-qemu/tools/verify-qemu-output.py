#!/usr/bin/env python3
import argparse
import re
import shutil
import subprocess
import sys
from pathlib import Path


def fail(message, output=b""):
    print(message, file=sys.stderr)
    if output:
        sys.stderr.buffer.write(output)
    return 1


def parse_hex(value):
    compact = "".join(value.split())
    if len(compact) % 2 != 0:
        raise argparse.ArgumentTypeError("input hex must contain complete bytes")
    try:
        return bytes.fromhex(compact)
    except ValueError as error:
        raise argparse.ArgumentTypeError("input hex is invalid") from error


def read_firmware_size(size_executable, firmware):
    result = subprocess.run(
        [size_executable, str(firmware)],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        text=True,
    )
    if result.returncode != 0:
        raise RuntimeError(result.stderr.strip())
    fields = result.stdout.strip().splitlines()[-1].split()
    if len(fields) < 3:
        raise RuntimeError("firmware size output is invalid")
    text_bytes, data_bytes, bss_bytes = map(int, fields[:3])
    section_result = subprocess.run(
        [size_executable, "-A", str(firmware)],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        text=True,
    )
    if section_result.returncode != 0:
        raise RuntimeError(section_result.stderr.strip())
    reserved_bytes = 0
    for line in section_result.stdout.splitlines():
        section = line.split()
        if len(section) >= 2 and section[0] in (".heap", ".stack"):
            reserved_bytes += int(section[1])
    static_ram_bytes = data_bytes + bss_bytes - reserved_bytes
    if static_ram_bytes < 0:
        raise RuntimeError("firmware static RAM accounting is invalid")
    return {
        "flash": text_bytes + data_bytes,
        "static_ram": static_ram_bytes,
    }


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--expect", action="append", default=[])
    parser.add_argument("--reject", action="append", default=[])
    parser.add_argument("--input-hex", type=parse_hex, default=b"\x04")
    parser.add_argument("--timeout", type=int, default=20)
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[1]
    firmware = root / ".pikapython" / "build" / "qemu" / "qemu-firmware.elf"
    empty_firmware = (
        root / ".pikapython" / "build" / "qemu"
        / "qemu-platform-empty.elf"
    )
    executable = shutil.which("qemu-system-arm")
    if executable is None:
        return fail("qemu-system-arm was not found")
    if not firmware.is_file():
        return fail("firmware is missing; run pikapython-cli build")
    if not empty_firmware.is_file():
        return fail("platform baseline firmware is missing")
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
    try:
        result = subprocess.run(
            command,
            cwd=root,
            input=args.input_hex,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            timeout=args.timeout,
            check=False,
        )
    except subprocess.TimeoutExpired as error:
        return fail("QEMU execution timed out", error.stdout or b"")
    if result.returncode != 0:
        return fail("QEMU exited with status %d" % result.returncode, result.stdout)
    resource = re.search(
        rb"PIKA_QEMU_RESOURCE heap_peak_bytes=(\d+) stack_peak_bytes=(\d+)",
        result.stdout,
    )
    if resource is None:
        return fail("QEMU output is missing its resource report", result.stdout)
    cursor = 0
    for value in args.expect:
        token = value.encode("utf-8")
        found = result.stdout.find(token, cursor)
        if found < 0:
            return fail("QEMU output is missing %r" % token, result.stdout)
        cursor = found + len(token)
    for value in args.reject:
        token = value.encode("utf-8")
        if token in result.stdout:
            return fail("QEMU output unexpectedly contains %r" % token, result.stdout)
    size_executable = shutil.which("arm-none-eabi-size")
    if size_executable is None:
        return fail("arm-none-eabi-size was not found")
    try:
        full_size = read_firmware_size(size_executable, firmware)
        platform_size = read_firmware_size(
            size_executable, empty_firmware)
    except RuntimeError as error:
        return fail("firmware size inspection failed: %s" % error)
    flash_bytes = full_size["flash"] - platform_size["flash"]
    static_ram_bytes = (
        full_size["static_ram"] - platform_size["static_ram"])
    if flash_bytes <= 0 or static_ram_bytes < 0:
        return fail("platform resource differential is invalid")
    heap_peak_bytes = int(resource.group(1))
    stack_peak_bytes = int(resource.group(2))
    peak_ram_bytes = static_ram_bytes + heap_peak_bytes + stack_peak_bytes
    print("QEMU output verified: %d expected token(s)" % len(args.expect))
    print(
        "QEMU resources: flash_bytes=%d static_ram_bytes=%d "
        "heap_peak_bytes=%d stack_peak_bytes=%d peak_ram_bytes=%d "
        "full_flash_bytes=%d full_static_ram_bytes=%d "
        "platform_flash_bytes=%d platform_static_ram_bytes=%d"
        % (
            flash_bytes,
            static_ram_bytes,
            heap_peak_bytes,
            stack_peak_bytes,
            peak_ram_bytes,
            full_size["flash"],
            full_size["static_ram"],
            platform_size["flash"],
            platform_size["static_ram"],
        )
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
