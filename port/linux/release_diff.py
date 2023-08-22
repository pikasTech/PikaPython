import subprocess
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

tag = commit_diff  # 替换为您要使用的标签
output_file = "diff.md"  # 替换为您要写入的文件名

f = open(output_file, "w")

# checkout to the commit_diff
repo.git.checkout(commit_diff)
pkgReleases_diff = PackageReleaseList(PACKAGE_RELEASE_PATH)

# checkout master
repo.git.checkout("master")

f.write(f"# Diff from PikaPython {commit_diff}\n")

f.write(f"## package diff\n")
f.write("|package|state|version|\n")
f.write("|---|---|---|\n")

# find new released package and package version
for pkg in pkgReleases.packages:
    # find pkg in pkgReleases_diff
    pkg_diff = pkgReleases_diff.findPackage(pkg.name)
    if None == pkg_diff:
        # print("New package: " + pkg.name + pkg.latestVersion().version)
        out_str = f"|{pkg.name}| Create | {pkg.latestVersion().version}|"
        print(out_str)
        f.write(out_str + '\n')
        continue

    pkg_diff = pkgReleases_diff.findPackage(pkg.name)
    if pkg_diff.latestVersion().version != pkg.latestVersion().version:
            out_str = f"|{pkg.name}| Update | {pkg_diff.latestVersion().version} --> {pkg.latestVersion().version}|"
            print(out_str)
            f.write(out_str + '\n')

# 构建 git log 命令并调用 subprocess 执行
git_log_cmd = f"git log {tag}..HEAD --pretty=format:%s"
result = subprocess.run(git_log_cmd, stdout=subprocess.PIPE, shell=True)

# 从结果中获取输出并将其写入文件
output_str = result.stdout.decode("utf-8").replace('\n', '\n\n')
f.write('## git diff\n')
f.write(output_str)
f.close()
exit()
