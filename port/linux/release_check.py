import argparse
import json
import re
from pathlib import Path

import git

from release_helper import PackageReleaseList


REPO_ROOT = Path(__file__).resolve().parents[2]
CORE_PACKAGES = ("pikascript-core", "PikaStdLib")


def parse_header_version(path: Path):
    text = path.read_text(encoding="utf-8")
    values = {}
    for field in ("MAJOR", "MINOR", "MICRO"):
        match = re.search(rf"^#define\s+PIKA_VERSION_{field}\s+(\d+)\s*$", text, re.M)
        if match is None:
            raise ValueError(f"PIKA_VERSION_{field} not found in {path}")
        values[field] = match.group(1)
    return f"{values['MAJOR']}.{values['MINOR']}.{values['MICRO']}"


def parse_version_config(path: Path):
    text = path.read_text(encoding="utf-8")
    values = {}
    for field in ("MajorVersion", "MinorVersion", "MicroVersion"):
        match = re.search(rf'^{field}\s*=\s*["\'](\d+)["\']\s*$', text, re.M)
        if match is None:
            raise ValueError(f"{field} not found in {path}")
        values[field] = match.group(1)
    return (
        f"{values['MajorVersion']}."
        f"{values['MinorVersion']}."
        f"{values['MicroVersion']}"
    )


def parse_cmsis_version(path: Path):
    text = path.read_text(encoding="utf-8")
    match = re.search(r'^version\s*=\s*["\'](\d+\.\d+\.\d+)["\']\s*$', text, re.M)
    if match is None:
        raise ValueError(f"version not found in {path}")
    return match.group(1)


def collect_checks(
    repo: git.Repo,
    expected_version=None,
    tag=None,
    require_tag_at_head=False,
    require_clean=False,
):
    checks = []
    errors = []
    warnings = []

    def record(name, actual, expected):
        passed = actual == expected
        checks.append(
            {
                "name": name,
                "ok": passed,
                "actual": actual,
                "expected": expected,
            }
        )
        if not passed:
            errors.append(
                {
                    "code": "version-mismatch",
                    "check": name,
                    "actual": actual,
                    "expected": expected,
                }
            )

    config_version = parse_version_config(
        REPO_ROOT / "port/linux/version_config.py"
    )
    expected = expected_version or config_version
    record("version_config", config_version, expected)
    record("runtime_header", parse_header_version(REPO_ROOT / "src/PikaVersion.h"), expected)
    record(
        "cmsis_pack",
        parse_cmsis_version(REPO_ROOT / "port/cmsis-pack/pikascript/makepdsc.py"),
        expected,
    )

    releases = PackageReleaseList(REPO_ROOT / "packages.toml")
    for package_name in CORE_PACKAGES:
        package = releases.findPackage(package_name)
        actual = package.latestVersion().version if package is not None else None
        record(f"package:{package_name}", actual, f"v{expected}")

    invalid_commits = []
    for package in releases.packages:
        try:
            commit = package.latestVersion().commit
            repo.commit(commit)
        except (ValueError, git.BadName):
            invalid_commits.append(package.name)
    checks.append(
        {
            "name": "package_commits",
            "ok": not invalid_commits,
            "invalidPackages": invalid_commits,
        }
    )
    if invalid_commits:
        errors.append(
            {
                "code": "invalid-package-commit",
                "packages": invalid_commits,
            }
        )

    if tag is not None:
        expected_tag = f"v{expected}"
        record("tag_name", tag, expected_tag)
        try:
            tag_commit = repo.commit(tag)
            checks.append(
                {
                    "name": "tag_exists",
                    "ok": True,
                    "commit": tag_commit.hexsha,
                }
            )
            if require_tag_at_head:
                record("tag_at_head", tag_commit.hexsha, repo.head.commit.hexsha)
        except (ValueError, git.BadName):
            checks.append({"name": "tag_exists", "ok": False, "tag": tag})
            errors.append({"code": "tag-not-found", "tag": tag})

    dirty = repo.is_dirty(untracked_files=True)
    checks.append({"name": "worktree_clean", "ok": not dirty})
    if dirty:
        problem = {"code": "dirty-worktree"}
        if require_clean:
            errors.append(problem)
        else:
            warnings.append(problem)

    return {
        "ok": not errors,
        "expectedVersion": expected,
        "head": repo.head.commit.hexsha,
        "checks": checks,
        "errors": errors,
        "warnings": warnings,
    }


def main():
    parser = argparse.ArgumentParser(
        description="Validate PikaPython release metadata and Git state."
    )
    parser.add_argument("--expected-version")
    parser.add_argument("--tag")
    parser.add_argument("--require-tag-at-head", action="store_true")
    parser.add_argument("--require-clean", action="store_true")
    args = parser.parse_args()

    try:
        result = collect_checks(
            git.Repo(REPO_ROOT),
            expected_version=args.expected_version,
            tag=args.tag,
            require_tag_at_head=args.require_tag_at_head,
            require_clean=args.require_clean,
        )
    except Exception as exc:
        result = {
            "ok": False,
            "errors": [{"code": "release-check-failed", "message": str(exc)}],
        }
    print(json.dumps(result, ensure_ascii=False))
    raise SystemExit(0 if result["ok"] else 1)


if __name__ == "__main__":
    main()
