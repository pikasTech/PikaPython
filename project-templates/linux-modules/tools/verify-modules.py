#!/usr/bin/env python3
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

import yaml


TIME_REFERENCE = """import time
print("sleep-zero:", time.sleep(0) is None)
print("sleep-bool:", time.sleep(False) is None)
try:
    time.sleep(-1)
    print("sleep-negative: accepted")
except ValueError:
    print("sleep-negative: rejected")
"""
TIME_EXTENSION = b"""extension-sleep: 7
extension-sleep-ms: 30
extension-wrap-positive: 5
extension-wrap-negative: -5
"""
EXPECTED = {
    "main.py": b"PikaPython module suite ready\n",
    "case_aes.py": b"""aes-128-ecb: True True
aes-192-ecb: True True
aes-256-ecb: True True
aes-128-cbc: True True
invalid-key: rejected
missing-iv: rejected
invalid-block: rejected
invalid-mode: rejected
""",
    "case_fuzzypid.py": b"""FUZZYPID_CONSTRUCTOR_RECOVERY_OK
FUZZYPID_VECTOR_OK
""",
}


def run(command, root):
    return subprocess.run(
        command,
        cwd=root,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        timeout=30,
    )


def cpython_case(root, name):
    with tempfile.TemporaryDirectory() as directory:
        isolated = Path(directory)
        shutil.copy2(root / name, isolated / "main.py")
        return run([sys.executable, "main.py"], isolated)


def expected_output(root, boot_entry):
    if boot_entry == "case_time.py":
        with tempfile.TemporaryDirectory() as directory:
            reference = run(
                [sys.executable, "-c", TIME_REFERENCE],
                Path(directory),
            )
        if reference.returncode != 0 or reference.stderr:
            raise RuntimeError(
                "CPython time reference failed: rc=%d stdout=%r stderr=%r"
                % (reference.returncode, reference.stdout, reference.stderr)
            )
        return reference.stdout + TIME_EXTENSION, "CPython common and V2 extension"
    if boot_entry in {"case_re.py", "case_socket.py"}:
        reference = cpython_case(root, boot_entry)
        if reference.returncode != 0 or reference.stderr:
            raise RuntimeError(
                "CPython reference failed: rc=%d stdout=%r stderr=%r"
                % (reference.returncode, reference.stdout, reference.stderr)
            )
        return reference.stdout, "CPython common"
    if boot_entry in EXPECTED:
        return EXPECTED[boot_entry], "V2 extension"
    raise ValueError(
        "unsupported bootEntry for this verifier: %s" % boot_entry
    )


def main():
    root = Path(__file__).resolve().parents[1]
    config = yaml.safe_load(
        (root / "pikapython.yaml").read_text(encoding="utf-8")
    )
    boot_entry = config.get("bootEntry", "main.py")
    program = root / ".pikapython" / "build" / "linux" / "linux-console"
    if not program.is_file():
        print("Linux program is missing; run pikapython-cli build", file=sys.stderr)
        return 1
    try:
        expected, verification = expected_output(root, boot_entry)
    except (RuntimeError, ValueError) as error:
        print(str(error), file=sys.stderr)
        return 1
    actual = run([str(program)], root)
    if actual.returncode != 0 or actual.stderr or actual.stdout != expected:
        print("Expected: %r" % expected, file=sys.stderr)
        print("PikaPython: rc=%d stdout=%r stderr=%r" % (
            actual.returncode, actual.stdout, actual.stderr), file=sys.stderr)
        return 1
    print("%s behavior verified for bootEntry=%s" % (
        verification, boot_entry
    ))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
