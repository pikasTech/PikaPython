#!/usr/bin/env python3
import argparse
import subprocess
import sys
from pathlib import Path

def fail(message, stdout=b"", stderr=b""):
    print(message, file=sys.stderr)
    if stdout:
        print("--- stdout ---", file=sys.stderr)
        sys.stderr.buffer.write(stdout)
    if stderr:
        print("--- stderr ---", file=sys.stderr)
        sys.stderr.buffer.write(stderr)
    return 1


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--expect", action="append", default=[])
    parser.add_argument("--reject", action="append", default=[])
    parser.add_argument("--input", default="\x04")
    parser.add_argument("--timeout", type=int, default=20)
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[1]
    program = root / ".pikapython" / "build" / "linux" / "linux-console"
    if not program.is_file():
        return fail("Linux console is missing; run pikapython-cli build")
    command = [str(program)]
    try:
        result = subprocess.run(
            command,
            cwd=root,
            input=args.input.encode("utf-8"),
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            timeout=args.timeout,
            check=False,
        )
    except subprocess.TimeoutExpired as error:
        return fail(
            "Linux console execution timed out",
            error.stdout or b"",
            error.stderr or b"",
        )
    if result.returncode != 0:
        return fail(
            "Linux console exited with status %d" % result.returncode,
            result.stdout,
            result.stderr,
        )
    cursor = 0
    for value in args.expect:
        token = value.encode("utf-8")
        found = result.stdout.find(token, cursor)
        if found < 0:
            return fail(
                "Linux output is missing %r" % token,
                result.stdout,
                result.stderr,
            )
        cursor = found + len(token)
    for value in args.reject:
        token = value.encode("utf-8")
        if token in result.stdout:
            return fail(
                "Linux output unexpectedly contains %r" % token,
                result.stdout,
                result.stderr,
            )
    print("Linux output verified: %d expected token(s)" % len(args.expect))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
