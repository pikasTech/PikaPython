import os
import toml
import git


REPO_PATH = "../.."
PACKAGE_PATH = REPO_PATH + "/package"
PACKAGE_RELEASE_INFO = REPO_PATH + "/packages.toml"
WORK_DIR = os.getcwd()


class VersoinType:
    MAJOR = 1
    MINOR = 2
    PATCH = 3


class VersionInfo:
    version: str
    commit: str
    vmajor: int
    vminor: int
    vpatch: int

    def __init__(self, version_discription: str):
        # v1.0.0
        try:
            self.version = version_discription.split(" ")[0]
            self.commit = version_discription.split(" ")[1]
            self.vmajor = int(self.version.split(".")[0][1:])
            self.vminor = int(self.version.split(".")[1])
            self.vpatch = int(self.version.split(".")[2])
        except:
            raise ValueError("Invalid version discription")


class PackageRelease:
    name: str
    versions: list[VersionInfo]

    def __init__(self, pkg_dict: dict, name: str):
        self.name = name
        self.versions = []
        for package in pkg_dict:
            if package['name'] == name:
                for version_dicription in package['releases']:
                    try:
                        self.versions.append(VersionInfo(version_dicription))
                    except:
                        continue


class PackageReleaseList:
    pkg_dict: dict
    packages: list[PackageRelease]

    def __init__(self, file_path):
        # read releases.toml
        with open(file_path, "r") as f:
            self.pkg_dict = toml.load(f)

        self.packages = []
        for package in self.pkg_dict['packages']:
            self.packages.append(PackageRelease(
                self.pkg_dict['packages'], package['name']))

    def latestCommit(self, package_name: str):
        # find the package
        for package in self.packages:
            if package.name == package_name:
                # find the latest version
                latest_version = package.versions[0]
                for version in package.versions:
                    if version.vmajor > latest_version.vmajor:
                        latest_version = version
                    elif version.vmajor == latest_version.vmajor:
                        if version.vminor > latest_version.vminor:
                            latest_version = version
                        elif version.vminor == latest_version.vminor:
                            if version.vpatch > latest_version.vpatch:
                                latest_version = version
                return latest_version.commit

    def versionRelease(self, package_name: str, version_type: VersoinType, commit: str):
        # find the package
        for package in self.packages:
            if package.name == package_name:
                # find the latest version
                latest_version = package.versions[0]
                for version in package.versions:
                    if version.vmajor > latest_version.vmajor:
                        latest_version = version
                    elif version.vmajor == latest_version.vmajor:
                        if version.vminor > latest_version.vminor:
                            latest_version = version
                        elif version.vminor == latest_version.vminor:
                            if version.vpatch > latest_version.vpatch:
                                latest_version = version

                # release new version
                if version_type == VersoinType.MAJOR:
                    latest_version.vmajor += 1
                    latest_version.vminor = 0
                    latest_version.vpatch = 0
                elif version_type == VersoinType.MINOR:
                    latest_version.vminor += 1
                    latest_version.vpatch = 0
                elif version_type == VersoinType.PATCH:
                    latest_version.vpatch += 1

                # solve version overflow
                if latest_version.vpatch > 9:
                    latest_version.vpatch = 0
                    latest_version.vminor += 1
                if latest_version.vminor > 9:
                    latest_version.vminor = 0
                    latest_version.vmajor += 1
                new_version_str = f"v{latest_version.vmajor}.{latest_version.vminor}.{latest_version.vpatch}"
                # add new version to the package
                for package in self.pkg_dict['packages']:
                    if package['name'] == package_name:
                        package['releases'].append(
                            f"{new_version_str} {commit}")
                return new_version_str

    def dump(self, file_path):
        with open(file_path, "w") as f:
            # dump with formating
            toml.dump(self.pkg_dict, f)


repo = git.Repo(REPO_PATH)

commit_head = repo.head.commit.hexsha

pkgReleases = PackageReleaseList(PACKAGE_RELEASE_INFO)

# for each folder in package, run the following command
for folder in os.listdir(PACKAGE_PATH):
    if os.path.isdir(PACKAGE_PATH + "/" + folder):
        # check git diff
        diff = repo.git.diff("HEAD", pkgReleases.latestCommit(folder), WORK_DIR +
                             "/" + PACKAGE_PATH + "/" + folder)
        if diff != '':
            newVersion = pkgReleases.versionRelease(
                folder, VersoinType.PATCH, commit_head)
            print(f"Changes detected: {folder} --> {newVersion}")

pkgReleases.dump('test.toml')
exit()
