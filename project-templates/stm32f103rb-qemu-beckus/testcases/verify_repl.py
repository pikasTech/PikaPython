#!/usr/bin/env python3
import importlib.util
import os
import re
import select
import shutil
import subprocess
import sys
import time
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
from provider import ProviderError, qemu_command, validate_provider


READY = b"PIKA_F103_READY\n"
FLASH_CAPACITY = 128 * 1024
RAM_CAPACITY = 20 * 1024
CASE_IDS = (
    "repl-edit-backspace-001",
    "repl-edit-left-insert-001",
    "repl-edit-right-move-001",
    "repl-edit-delete-forward-001",
    "repl-edit-history-draft-001",
    "repl-fault-syntax-recovery-001",
    "repl-fault-undefined-name-recovery-001",
    "repl-control-end-input-001",
)
RESOURCE = re.compile(
    rb"PIKA_QEMU_RESOURCE heap_peak_bytes=(\d+) "
    rb"stack_peak_bytes=(\d+) heap_capacity_bytes=(\d+)"
)


class VerificationError(RuntimeError):
    pass


class QemuSession:
    def __init__(self, command):
        self.process = subprocess.Popen(
            command,
            cwd=ROOT,
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            bufsize=0,
        )
        self.buffer = bytearray()

    def _pump(self, deadline):
        if self.process.stdout is None:
            return False
        remaining = deadline - time.monotonic()
        if remaining <= 0:
            return False
        ready, _, _ = select.select(
            [self.process.stdout.fileno()], [], [], min(remaining, 0.1)
        )
        if not ready:
            return self.process.poll() is None
        chunk = os.read(self.process.stdout.fileno(), 4096)
        if chunk:
            self.buffer.extend(chunk)
            return True
        return False

    def take_until(self, marker, timeout=10):
        deadline = time.monotonic() + timeout
        while marker not in self.buffer:
            if not self._pump(deadline):
                self._raise_wait_error("marker %r" % marker)
        end = self.buffer.index(marker) + len(marker)
        value = bytes(self.buffer[:end])
        del self.buffer[:end]
        return value

    def take(self, length, timeout=10):
        deadline = time.monotonic() + timeout
        while len(self.buffer) < length:
            if not self._pump(deadline):
                self._raise_wait_error("%d output bytes" % length)
        value = bytes(self.buffer[:length])
        del self.buffer[:length]
        return value

    def _raise_wait_error(self, purpose):
        state = self.process.poll()
        output = bytes(self.buffer[-1024:]).hex(" ")
        raise VerificationError(
            "QEMU did not produce %s; status=%s outputHex=%s"
            % (purpose, state, output)
        )

    def feed(self, payload, chunks):
        if sum(chunks) != len(payload):
            raise VerificationError("transcript chunk lengths are inconsistent")
        if self.process.stdin is None:
            raise VerificationError("QEMU input is closed")
        offset = 0
        try:
            for length in chunks:
                self.process.stdin.write(payload[offset:offset + length])
                self.process.stdin.flush()
                offset += length
                time.sleep(0.01)
        except (BrokenPipeError, OSError) as error:
            raise VerificationError("QEMU rejected transcript input") from error

    def finish(self, request_end):
        if request_end:
            self.feed(b"\x04", [1])
        if self.process.stdin is not None:
            self.process.stdin.close()
            self.process.stdin = None
        try:
            remainder, _ = self.process.communicate(timeout=10)
        except subprocess.TimeoutExpired as error:
            self.process.terminate()
            try:
                self.process.wait(timeout=2)
            except subprocess.TimeoutExpired:
                self.process.kill()
                self.process.wait()
            raise VerificationError("QEMU did not exit after Ctrl-D") from error
        output = bytes(self.buffer) + (remainder or b"")
        self.buffer.clear()
        if self.process.returncode != 0:
            raise VerificationError(
                "QEMU exited with status %d; outputHex=%s"
                % (self.process.returncode, output[-1024:].hex(" "))
            )
        if b"PIKA_QEMU_EXIT status=0\n" not in output:
            raise VerificationError("QEMU output is missing a successful exit")
        match = RESOURCE.search(output)
        if match is None:
            raise VerificationError("QEMU output is missing its resource report")
        return tuple(int(item) for item in match.groups())


def load_cases():
    source = ROOT / ".pikapython" / "source"
    module_path = source / "tools" / "repl_transcript.py"
    suite_path = source / "testcases" / "repl" / "suite.yaml"
    if not module_path.is_file() or not suite_path.is_file():
        raise VerificationError(
            "installed transcript sources are missing; run pikapython-cli install"
        )
    spec = importlib.util.spec_from_file_location(
        "pika_installed_repl_transcript", module_path
    )
    if spec is None or spec.loader is None:
        raise VerificationError("installed transcript loader is unavailable")
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    suite = module.load_suite(suite_path)
    cases = {case["id"]: case for case in suite["cases"]}
    missing = [case_id for case_id in CASE_IDS if case_id not in cases]
    if missing:
        raise VerificationError(
            "installed transcript suite is missing: %s" % ", ".join(missing)
        )
    return module.digest_suite(suite), [cases[case_id] for case_id in CASE_IDS]


def case_payload(case):
    payload = bytearray()
    for event in case["events"]:
        if event["name"] != "feedHex":
            raise VerificationError(
                "case %s requires unsupported transport event %s"
                % (case["id"], event["name"])
            )
        payload.extend(event["bytes"] * event["repeat"])
    return bytes(payload)


def compare(case_id, actual, expected):
    if actual == expected:
        return
    shared = min(len(actual), len(expected))
    offset = next(
        (index for index in range(shared) if actual[index] != expected[index]),
        shared,
    )
    raise VerificationError(
        "%s differs at byte %d; actualHex=%s expectedHex=%s"
        % (case_id, offset, actual.hex(" "), expected.hex(" "))
    )


def run_case(command, case):
    session = QemuSession(command)
    session.take_until(READY)
    session.feed(case_payload(case), case["chunks"])
    actual = session.take(len(case["expected"]))
    compare(case["id"], actual, case["expected"])
    return session.finish(case["finalState"] != "ended")


def verify_persistent_main(command):
    session = QemuSession(command)
    prefix = session.take_until(READY)
    lines = prefix.replace(b"\r\n", b"\n").splitlines()
    if lines.count(b"40") != 1:
        raise VerificationError("main.py must print 40 exactly once")
    payload = b"boot_value+2\r"
    expected = b">>> boot_value+2\r\n42\r\n>>> "
    session.feed(payload, [4, 8, 1])
    compare("persistent-main-state", session.take(len(expected)), expected)
    return session.finish(True)


def inspect_firmware(firmware, runtime_resources):
    size = shutil.which("arm-none-eabi-size")
    if size is None:
        raise VerificationError("arm-none-eabi-size was not found")
    summary = subprocess.run(
        [size, str(firmware)], capture_output=True, text=True, check=False
    )
    sections = subprocess.run(
        [size, "-A", str(firmware)], capture_output=True, text=True, check=False
    )
    if summary.returncode != 0 or sections.returncode != 0:
        raise VerificationError("firmware resource inspection failed")
    fields = summary.stdout.strip().splitlines()[-1].split()
    text_bytes, data_bytes, bss_bytes = map(int, fields[:3])
    section_sizes = {}
    for line in sections.stdout.splitlines():
        values = line.split()
        if len(values) >= 2 and values[0].startswith("."):
            section_sizes[values[0]] = int(values[1])
    reserved = section_sizes.get(".heap", 0) + section_sizes.get(".stack", 0)
    static_ram = data_bytes + bss_bytes - reserved
    flash = text_bytes + data_bytes
    heap_peak = max(item[0] for item in runtime_resources)
    stack_peak = max(item[1] for item in runtime_resources)
    heap_capacity = max(item[2] for item in runtime_resources)
    peak_ram = static_ram + heap_peak + stack_peak
    if static_ram < 0 or flash > FLASH_CAPACITY or peak_ram > RAM_CAPACITY:
        raise VerificationError("firmware exceeds the STM32F103RB resource budget")
    if static_ram + heap_capacity + section_sizes.get(".stack", 0) > RAM_CAPACITY:
        raise VerificationError("firmware RAM regions overlap")
    return flash, static_ram, heap_peak, stack_peak, peak_ram


def main():
    firmware = ROOT / ".pikapython" / "build" / "qemu" / "qemu-firmware.elf"
    try:
        docker = validate_provider()
        command = qemu_command(docker, firmware)
        digest, cases = load_cases()
        resources = [verify_persistent_main(command)]
        for case in cases:
            resources.append(run_case(command, case))
        values = inspect_firmware(firmware, resources)
    except (ProviderError, VerificationError, OSError, ValueError) as error:
        print("QEMU verification failed: %s" % error, file=sys.stderr)
        return 1
    flash, static_ram, heap_peak, stack_peak, peak_ram = values
    print("Beckus QEMU verified: main.py and %d public REPL cases" % len(cases))
    print("Transcript suite digest: %s" % digest)
    print(
        "Resources: flash_bytes=%d static_ram_bytes=%d heap_peak_bytes=%d "
        "stack_peak_bytes=%d peak_ram_bytes=%d"
        % (flash, static_ram, heap_peak, stack_peak, peak_ram)
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
