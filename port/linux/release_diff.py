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

# checkout master
repo.git.checkout("master")

# find new released package and package version
for pkg in pkgReleases.packages:
    # find pkg in pkgReleases_diff
    pkg_diff = pkgReleases_diff.findPackage(pkg.name)
    if None == pkg_diff:
        # print("New package: " + pkg.name + pkg.latestVersion().version)
        print(f"|{pkg.name}| Create | {pkg.latestVersion().version}|")
        continue

    pkg_diff = pkgReleases_diff.findPackage(pkg.name)
    if pkg_diff.latestVersion().version != pkg.latestVersion().version:
        print(
            f"|{pkg.name}| Update | {pkg_diff.latestVersion().version} --> {pkg.latestVersion().version}|")

exit()
