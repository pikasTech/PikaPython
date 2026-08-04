#!/usr/bin/env python3
import ast
import shutil
import subprocess
import sys
import tempfile
import zlib
from pathlib import Path


PREFIX = b"compressed: "
PAYLOAD = b"PikaPython V2 zlib cross-runtime payload"


def run(command, root):
    return subprocess.run(
        command,
        cwd=root,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        timeout=20,
    )


def split_compressed_line(output):
    lines = output.splitlines(keepends=True)
    if not lines or not lines[0].startswith(PREFIX):
        raise ValueError("compressed stream line is missing")
    literal = lines[0][len(PREFIX) :].strip().decode("ascii")
    compressed = ast.literal_eval(literal)
    if not isinstance(compressed, bytes):
        raise ValueError("compressed stream is not bytes")
    return compressed, b"".join(lines[1:])


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
    try:
        reference_stream, reference_stable = split_compressed_line(reference.stdout)
        actual_stream, actual_stable = split_compressed_line(actual.stdout)
        reference_payload = zlib.decompress(reference_stream)
        actual_payload = zlib.decompress(actual_stream)
    except Exception as error:
        print("zlib wire verification failed: %s" % error, file=sys.stderr)
        return 1
    if (
        reference.returncode != 0
        or reference.stderr
        or actual.returncode != 0
        or actual.stderr
        or reference_payload != PAYLOAD
        or actual_payload != PAYLOAD
        or actual_stable != reference_stable
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
    print(
        "CPython behavior and bidirectional zlib streams verified: %d byte(s)"
        % len(actual.stdout)
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
