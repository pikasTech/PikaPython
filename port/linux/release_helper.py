import os
from enum import Enum

import tomlkit


REPO_PATH = "../.."
PACKAGE_PATH = REPO_PATH + "/package"
LINUX_PACKAGE_PATH = REPO_PATH + "/port/linux/package/pikascript/pikascript-lib"
PACKAGE_RELEASE_PATH = REPO_PATH + "/packages.toml"
WORK_DIR = os.getcwd()


class VersionType(Enum):
    MAJOR = 1
    MINOR = 2
    PATCH = 3


# Keep the historical misspelling available for existing local release scripts.
VersoinType = VersionType


class VersionInfo:
    version: str
    commit: str
    vmajor: int
    vminor: int
    vpatch: int

    def __init__(self, version_discription: str):
        # v1.0.0
        try:
            fields = version_discription.split()
            if len(fields) != 2:
                raise ValueError
            self.version, self.commit = fields
            version_fields = self.version.removeprefix("v").split(".")
            if not self.version.startswith("v") or len(version_fields) != 3:
                raise ValueError
            self.vmajor, self.vminor, self.vpatch = map(int, version_fields)
        except (TypeError, ValueError):
            raise ValueError("Invalid version description")


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
    
    def latestVersion(self):
        # find the latest version
        latest_version = self.versions[0]
        for version in self.versions:
            if version.vmajor > latest_version.vmajor:
                latest_version = version
            elif version.vmajor == latest_version.vmajor:
                if version.vminor > latest_version.vminor:
                    latest_version = version
                elif version.vminor == latest_version.vminor:
                    if version.vpatch > latest_version.vpatch:
                        latest_version = version
        return latest_version


class PackageReleaseList:
    pkg_dict: dict
    packages: list[PackageRelease]

    def __init__(self, file_path):
        with open(file_path, "r", encoding="utf-8") as f:
            self.pkg_dict = tomlkit.load(f)
        self._load_packages()

    @classmethod
    def from_text(cls, text: str):
        release_list = cls.__new__(cls)
        release_list.pkg_dict = tomlkit.loads(text)
        release_list._load_packages()
        return release_list

    def _load_packages(self):
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

    def versionRelease(self, package_name: str, version_type: VersionType, commit: str):
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
                major = latest_version.vmajor
                minor = latest_version.vminor
                patch = latest_version.vpatch
                if version_type == VersionType.MAJOR:
                    major += 1
                    minor = 0
                    patch = 0
                elif version_type == VersionType.MINOR:
                    minor += 1
                    patch = 0
                elif version_type == VersionType.PATCH:
                    patch += 1
                else:
                    raise ValueError(f"Invalid version type: {version_type}")

                new_version_str = f"v{major}.{minor}.{patch}"
                # add new version to the package
                for package in self.pkg_dict['packages']:
                    if package['name'] == package_name:
                        package['releases'].append(
                            f"{new_version_str} {commit}")
                return new_version_str

    def dump(self, file_path):
        with open(file_path, "w", encoding="utf-8") as f:
            tomlkit.dump(self.pkg_dict, f)

    def findPackage(self, pkg_name:str):
        for package in self.packages:
            if package.name == pkg_name:
                return package
        return None
