#!/usr/bin/env python3
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path


def run(command, root):
    return subprocess.run(
        command,
        cwd=root,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        timeout=20,
    )


def main():
    root = Path(__file__).resolve().parents[1]
    program = root / ".pikapython" / "build" / "linux" / "linux-console"
    if not program.is_file():
        print("Linux program is missing; run pikapython-cli build", file=sys.stderr)
        return 1
    with tempfile.TemporaryDirectory() as directory:
        shutil.copy2(root / "main.py", Path(directory) / "main.py")
        reference = run([sys.executable, "main.py"], Path(directory))
    actual = run([str(program)], root)
    if (
        reference.returncode != 0
        or reference.stderr
        or actual.returncode != 0
        or actual.stderr
        or actual.stdout != reference.stdout
    ):
        print(
            "CPython: rc=%d stdout=%r stderr=%r"
            % (reference.returncode, reference.stdout, reference.stderr),
            file=sys.stderr,
        )
        print(
            "PikaPython: rc=%d stdout=%r stderr=%r"
            % (actual.returncode, actual.stdout, actual.stderr),
            file=sys.stderr,
        )
        return 1
    print("CPython behavior verified: %d byte(s)" % len(actual.stdout))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
