#!/usr/bin/env python3
# SPEC: PJ2026-0501 kernel v0.24; capability prebuild CLI.
import argparse
import json
from pathlib import Path

from capability_config import CapabilityConfigError, write_prebuild


class JsonArgumentParser(argparse.ArgumentParser):
    def error(self, message):
        raise CapabilityConfigError("invalid_arguments", message)


def emit(ok, data=None, code=None, error=None):
    response = {"ok": ok, "operation": "capability.prebuild"}
    if data is not None:
        response["data"] = data
    if code is not None:
        response["code"] = code
    if error is not None:
        response["error"] = error
    print(json.dumps(response, ensure_ascii=True, indent=2, sort_keys=True))
    return 0 if ok else 1


def main():
    parser = JsonArgumentParser(prog="capability-prebuild-cli.py")
    parser.add_argument("--config", required=True)
    parser.add_argument("--profile", required=True)
    parser.add_argument("--output", required=True)
    try:
        args = parser.parse_args()
        manifest = write_prebuild(
            Path(args.config).resolve(),
            args.profile,
            Path(args.output).resolve(),
        )
        return emit(
            True,
            {
                "profile": manifest["profile"],
                "closure": manifest["closure"],
                "configDigest": manifest["configDigest"],
            },
        )
    except CapabilityConfigError as error:
        return emit(False, code=error.code, error=str(error))


if __name__ == "__main__":
    raise SystemExit(main())
