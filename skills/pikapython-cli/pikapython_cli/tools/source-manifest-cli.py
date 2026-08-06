#!/usr/bin/env python3
# SPEC: PJ2026-0501 kernel v0.8; portable source manifest queries.
import argparse
import json
import sys
from pathlib import Path, PurePosixPath


SCHEMA = "pika.source-manifest/v1"
GROUPS = (
    "runtime",
    "application",
    "frontend",
    "repl",
    "host-runner",
    "host-prebuild",
    "network",
    "public-headers",
)
SOURCE_GROUPS = (
    "runtime",
    "application",
    "frontend",
    "repl",
    "host-runner",
    "host-prebuild",
    "network",
)
ROOT = Path(__file__).resolve().parents[4]
MANIFEST = Path(__file__).with_name("source-manifest.json")


class ManifestError(Exception):
    pass


def emit(ok, operation, data=None, code=None, error=None):
    value = {"ok": ok, "operation": operation}
    if data is not None:
        value["data"] = data
    if code is not None:
        value["code"] = code
    if error is not None:
        value["error"] = error
    print(json.dumps(value, ensure_ascii=True, indent=2))
    return 0 if ok else 1


def load_manifest():
    try:
        value = json.loads(MANIFEST.read_text(encoding="utf-8"))
    except (OSError, UnicodeError, json.JSONDecodeError) as error:
        raise ManifestError("manifest_read_failed: %s" % error) from error
    validate_manifest(value)
    return value


def validate_path(relative, suffix):
    if not isinstance(relative, str):
        raise ManifestError("manifest path must be a string")
    path = PurePosixPath(relative)
    if (
        path.is_absolute()
        or ".." in path.parts
        or chr(92) in relative
        or path.parent != PurePosixPath("pikapython-kernel")
        or path.suffix != suffix
    ):
        raise ManifestError("non-portable manifest path: %s" % relative)
    local = ROOT.joinpath(*path.parts)
    if not local.is_file():
        raise ManifestError("manifest path does not exist: %s" % relative)


def validate_manifest(value):
    if not isinstance(value, dict) or value.get("schema") != SCHEMA:
        raise ManifestError("unsupported source manifest schema")
    if value.get("includeDirectories") != ["pikapython-kernel"]:
        raise ManifestError("source manifest must expose only pikapython-kernel include directory")
    groups = value.get("groups")
    if not isinstance(groups, dict) or set(groups) != set(GROUPS):
        raise ManifestError("source manifest groups do not match the contract")
    all_sources = []
    for group in GROUPS:
        paths = groups[group]
        if (
            not isinstance(paths, list)
            or paths != sorted(paths)
            or len(paths) != len(set(paths))
        ):
            raise ManifestError("manifest group is not sorted and unique: %s" % group)
        suffix = ".h" if group == "public-headers" else ".c"
        for relative in paths:
            validate_path(relative, suffix)
        if group in SOURCE_GROUPS:
            all_sources.extend(paths)
    if len(all_sources) != len(set(all_sources)):
        raise ManifestError("source manifest contains duplicate compilation units")
    repository_sources = sorted(
        path.relative_to(ROOT).as_posix() for path in (ROOT / "pikapython-kernel").glob("*.c")
    )
    if sorted(all_sources) != repository_sources:
        raise ManifestError("source manifest does not account for every pikapython-kernel C unit")


def parse_arguments():
    parser = argparse.ArgumentParser(prog="source-manifest-cli.py")
    actions = parser.add_subparsers(dest="action", required=True)
    actions.add_parser("verify")
    listing = actions.add_parser("list")
    listing.add_argument("--group", required=True, choices=GROUPS)
    listing.add_argument("--format", choices=("json", "lines"), default="json")
    return parser.parse_args()


def main():
    args = parse_arguments()
    operation = "manifest.%s" % args.action
    try:
        manifest = load_manifest()
        if args.action == "verify":
            return emit(
                True,
                operation,
                {
                    "schema": manifest["schema"],
                    "groups": list(GROUPS),
                    "includeDirectories": manifest["includeDirectories"],
                },
            )
        paths = manifest["groups"][args.group]
        if args.format == "lines":
            for relative in paths:
                print(relative)
            return 0
        return emit(
            True,
            operation,
            {"group": args.group, "paths": paths},
        )
    except ManifestError as error:
        return emit(
            False,
            operation,
            code="invalid_source_manifest",
            error=str(error),
        )


if __name__ == "__main__":
    raise SystemExit(main())
