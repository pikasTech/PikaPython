import os
import subprocess
from pathlib import Path

import yaml

from . import __version__


ARCHIVE_COMMIT = "8c7a7661e67cea92fc3f4178d889fa9e55c69825"
FULL_COMMIT_LENGTH = 40


def _valid_commit(value):
    return isinstance(value, str) and len(value) == FULL_COMMIT_LENGTH and all(
        character in "0123456789abcdefABCDEF" for character in value
    )


def _git_commit():
    source_file = Path(__file__).resolve()
    for repository in source_file.parents:
        if not (repository / ".git").exists():
            continue
        try:
            relative = source_file.relative_to(repository).as_posix()
            tracked = subprocess.run(
                ["git", "-C", str(repository), "ls-files", "--error-unmatch", relative],
                stdout=subprocess.PIPE,
                stderr=subprocess.DEVNULL,
                text=True,
                check=False,
            )
            if tracked.returncode != 0:
                continue
            result = subprocess.run(
                ["git", "-C", str(repository), "rev-parse", "HEAD"],
                stdout=subprocess.PIPE,
                stderr=subprocess.DEVNULL,
                text=True,
                check=False,
            )
        except OSError:
            continue
        commit = result.stdout.strip()
        if _valid_commit(commit):
            return commit, "git"
    return None, None


def get_identity():
    configured = os.environ.get("PIKAPYTHON_CLI_SOURCE_COMMIT")
    if _valid_commit(configured):
        return {
            "packageVersion": __version__,
            "sourceCommit": configured.lower(),
            "sourceKind": "environment",
        }
    if _valid_commit(ARCHIVE_COMMIT):
        return {
            "packageVersion": __version__,
            "sourceCommit": ARCHIVE_COMMIT.lower(),
            "sourceKind": "git-archive",
        }
    commit, source_kind = _git_commit()
    return {
        "packageVersion": __version__,
        "sourceCommit": commit,
        "sourceKind": source_kind or "unknown",
    }


def get_project_ref(project=None):
    path = Path(project or Path.cwd()) / "pikapython.yaml"
    try:
        value = yaml.safe_load(path.read_text(encoding="utf-8"))
    except (OSError, UnicodeError, yaml.YAMLError):
        return None
    if not isinstance(value, dict):
        return None
    packages = value.get("packages")
    if not isinstance(packages, dict):
        return None
    reference = packages.get("ref")
    return reference if isinstance(reference, str) else None
