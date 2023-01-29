import os
import git
import sys
from release_helper import *

repo = git.Repo(REPO_PATH)
commit_head = repo.head.commit.hexsha
pkgReleases = PackageReleaseList(PACKAGE_RELEASE_PATH)

# argv[1] is the commit hash
if len(sys.argv) > 1:
    commit_diff = sys.argv[1]
else:
    print("No commit hash specified")
    commit_diff = "f8b529a956da57d8623247bea594e65469cac1c5"

# checkout to the commit_diff
repo.git.checkout(commit_diff)
pkgReleases_diff = PackageReleaseList(PACKAGE_RELEASE_PATH)

exit()
