import os
import shutil
import subprocess

default_repo_url = "https://gitee.com/lyon1998/pikapython"

# clone pikapython repo
repo_path = "/tmp/pikapython"
if not os.path.exists(repo_path):
    subprocess.run(["git", "clone", default_repo_url, repo_path])
else:
    # if the repo exists, clean all uncommitted changes
    subprocess.run(["git", "reset", "--hard"], cwd=repo_path)
    subprocess.run(["git", "clean", "-fd"], cwd=repo_path)
    # switch to master branch
    subprocess.run(["git", "checkout", "master"], cwd=repo_path)
    # pull the latest changes
    subprocess.run(["git", "pull"], cwd=repo_path)

# create the base of the patch
base_path = "/tmp/base"
if os.path.exists(base_path):
    shutil.rmtree(base_path)
os.mkdir(base_path)

# install the base package
for file_name in ["pikaPackage.exe", "requestment.txt"]:
    shutil.copy(file_name, base_path)
with open(os.path.join(base_path, "main.py"), "w") as f:
    f.write("")
subprocess.run(["./pikaPackage.exe"], cwd=base_path)
os.remove(os.path.join(base_path, "main.py"))

# generate the patch
for root, apply_dirs, files in os.walk('.'):
    if 'pikascript-api' in root:  # skip the pikascript-api folder
        continue
    for file in files:
        src_file = os.path.join(root, file)
        rel_path = os.path.relpath(root, '.')
        target_file = os.path.join(base_path, rel_path, file)
        print(f"Processing file: {src_file}")  
        if os.path.exists(target_file):
            diff_command = ["git", "diff", "--no-index",  target_file, src_file]
            result = subprocess.run(diff_command, stdout=subprocess.PIPE)
            if result.stdout:  
                diff_output = result.stdout.decode()
                print(diff_output)  
                with open("/tmp/base/changes.patch", "a") as patch_file:
                    patch_file.write(diff_output)
        else:
            print(f"No base file, skipped: {target_file}")

# if there is no patch, exit
if not os.path.exists("/tmp/base/changes.patch"):
    # green print
    print("\033[92mNothing need to be patched\033[0m")
    exit()

# read the patch
with open("/tmp/base/changes.patch") as patch_file:
    patch_data = patch_file.read()

# split the patch into a list of patches
patches = []
patch_item = ''

for line in patch_data.split('\n'):
    if line.startswith('diff --git'):
        if patch_item:   # If patch_item is not empty, append it to patches list
            patches.append(patch_item)
        patch_item = ''   # Reset patch_item for the next patch
    patch_item += line + '\n'

# Append the last patch_item after the loop ends
if patch_item:
    patches.append(patch_item)

main_repo_path = "/tmp/pikapython"
apply_dirs = ["src", "port/linux/package/pikascript", "package"]
search_dirs = [os.path.join(main_repo_path, dir) for dir in apply_dirs]

success_count = 0
success_list = []
fail_count = 0
fail_list = []

for patch in patches:
    lines = patch.split('\n')
    if len(lines) < 1:
        print("No valid patch data, skipped")
        continue

    header = lines[0]
    if not header.startswith('diff --git a/'):
        print("No valid patch header, skipped")
        continue

    paths = header[13:].split(' b/')
    if len(paths) != 2:
        print("No valid file path in header, skipped")
        continue

    from_file = paths[0]
    to_file = paths[1]

    print(f"Searching for file: {from_file}")

    # serch file
    is_found = False
    for dir in search_dirs:
        file_name = os.path.basename(from_file)
        # find file
        if is_found:
            break

        for root, apply_dirs, files in os.walk(dir):
            if file_name in files:
                is_found = True
                apply_file = os.path.join(root, file_name)
                apply_file = os.path.relpath(apply_file, main_repo_path)
                # replace \ with /
                apply_file = apply_file.replace('\\', '/')
                print(f"Found file: {apply_file}")
                # replace from_file and to_file with apply_file
                patch = patch.replace(from_file, apply_file)
                patch = patch.replace(to_file, apply_file)
                print(f"Applying patch: {patch}")
                # Strip the trailing whitespace for each line
                lines = [line for line in patch.split('\n')]
                with open("/tmp/base/temp.patch", 'w') as f:
                    for line in lines:
                        f.write(line + '\n')
                apply_patch_command = ["git", "apply", "/tmp/base/temp.patch"]
                result = subprocess.run(apply_patch_command, stdout=subprocess.PIPE, cwd=main_repo_path)
                # check the result
                if result.returncode != 0:
                    fail_count += 1
                    fail_list.append(apply_file)
                    print("\033[91mFailed to apply patch:\033[0m", apply_file)
                else:
                    success_count += 1
                    success_list.append(apply_file)
                    print("\033[92mSuccessfully applied patch\033[0m", apply_file)
                break  


print("\n\n===========================================\n\n")
print(f"\033[92mTotal patches applied successfully: {success_count}\033[0m")
print("\033[92mSuccessfully applied patches:\033[0m")
for file in success_list:
    print(file)

if fail_count > 0:
    print(f"\033[91mTotal patches failed to apply: {fail_count}\033[0m")
    print("\033[91mFailed to apply patches:\033[0m")
    for file in fail_list:
        print(file)

# setup remote url
repo_url = input("Please enter your remote repository URL (default: '{}'): ".format(default_repo_url)) or default_repo_url
subprocess.run(["git", "remote", "set-url", "origin", repo_url], cwd = main_repo_path)

# setup commit message
commit_msg = input("Please enter your commit message (default: 'Apply patches'): ") or "Apply patches"
subprocess.run(["git", "commit", "-a" ,"-m", commit_msg], cwd=main_repo_path)

# confirm push
confirm = input("Are you sure you want to push to the repository ([Y]/N)? ")
if confirm.lower() != 'n':
    # git push
    subprocess.run(["git", "push"], cwd=main_repo_path)
else:
    print("Push cancelled.")
