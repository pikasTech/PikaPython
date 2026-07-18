import argparse
import json
from pathlib import Path

import git

from release_helper import PackageReleaseList


REPO_ROOT = Path(__file__).resolve().parents[2]


def build_release_diff(repo: git.Repo, baseline: str):
    current = PackageReleaseList(REPO_ROOT / "packages.toml")
    baseline_text = repo.git.show(f"{baseline}:packages.toml")
    previous = PackageReleaseList.from_text(baseline_text)
    previous_by_name = {package.name: package for package in previous.packages}
    current_by_name = {package.name: package for package in current.packages}
    packages = []

    for name, package in current_by_name.items():
        old_package = previous_by_name.get(name)
        if old_package is None:
            packages.append(
                {
                    "name": name,
                    "state": "create",
                    "version": package.latestVersion().version,
                }
            )
            continue
        old_version = old_package.latestVersion().version
        new_version = package.latestVersion().version
        if old_version != new_version:
            packages.append(
                {
                    "name": name,
                    "state": "update",
                    "from": old_version,
                    "to": new_version,
                }
            )

    for name, package in previous_by_name.items():
        if name not in current_by_name:
            packages.append(
                {
                    "name": name,
                    "state": "delete",
                    "version": package.latestVersion().version,
                }
            )

    commits = [
        {"commit": commit.hexsha, "subject": commit.summary}
        for commit in repo.iter_commits(f"{baseline}..HEAD")
    ]
    return {
        "ok": True,
        "baseline": repo.commit(baseline).hexsha,
        "head": repo.head.commit.hexsha,
        "packages": packages,
        "commits": commits,
    }


def render_markdown(result):
    lines = [
        f"# Diff from PikaPython {result['baseline']}",
        "",
        "## Package diff",
        "",
        "| package | state | version |",
        "| --- | --- | --- |",
    ]
    for package in result["packages"]:
        if package["state"] == "update":
            version = f"{package['from']} -> {package['to']}"
        else:
            version = package["version"]
        lines.append(f"| {package['name']} | {package['state']} | {version} |")
    lines.extend(["", "## Git diff", ""])
    lines.extend(
        f"- `{commit['commit'][:10]}` {commit['subject']}"
        for commit in result["commits"]
    )
    return "\n".join(lines) + "\n"


def main():
    parser = argparse.ArgumentParser(
        description="Compare release metadata without changing the worktree."
    )
    parser.add_argument("baseline", help="Git tag or commit used as the baseline")
    parser.add_argument(
        "--markdown-output", type=Path, help="Also write a Markdown report"
    )
    args = parser.parse_args()

    try:
        result = build_release_diff(git.Repo(REPO_ROOT), args.baseline)
        if args.markdown_output is not None:
            args.markdown_output.write_text(
                render_markdown(result), encoding="utf-8"
            )
            result["markdownOutput"] = str(args.markdown_output)
        print(json.dumps(result, ensure_ascii=False))
    except Exception as exc:
        print(
            json.dumps(
                {
                    "ok": False,
                    "baseline": args.baseline,
                    "error": str(exc),
                },
                ensure_ascii=False,
            )
        )
        raise SystemExit(1)


if __name__ == "__main__":
    main()
