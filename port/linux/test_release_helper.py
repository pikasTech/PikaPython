import tempfile
import unittest
from pathlib import Path

from release_helper import PackageReleaseList, VersionInfo, VersionType


PACKAGES_TOML = """[[packages]]
name = "demo"
releases = [
  "v1.2.9 1111111111111111111111111111111111111111",
  "lts2021 2222222222222222222222222222222222222222",
  "v1.2.9.1 3333333333333333333333333333333333333333"
]

[[packages]]
name = "untouched"
releases = [ "v0.1.0 4444444444444444444444444444444444444444" ]
"""


class VersionInfoTest(unittest.TestCase):
    def test_rejects_non_semver_release_names(self):
        with self.assertRaises(ValueError):
            VersionInfo("lts2021 2222222222222222222222222222222222222222")
        with self.assertRaises(ValueError):
            VersionInfo("v1.2.9.1 3333333333333333333333333333333333333333")


class PackageReleaseListTest(unittest.TestCase):
    def setUp(self):
        self.temp_dir = tempfile.TemporaryDirectory()
        self.path = Path(self.temp_dir.name) / "packages.toml"
        self.path.write_text(PACKAGES_TOML, encoding="utf-8")

    def tearDown(self):
        self.temp_dir.cleanup()

    def test_patch_does_not_carry_to_minor(self):
        releases = PackageReleaseList(self.path)

        version = releases.versionRelease(
            "demo", VersionType.PATCH, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        )

        self.assertEqual("v1.2.10", version)

    def test_dump_preserves_unrelated_formatting(self):
        releases = PackageReleaseList(self.path)
        releases.versionRelease(
            "demo", VersionType.PATCH, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        )
        releases.dump(self.path)

        result = self.path.read_text(encoding="utf-8")
        self.assertIn(
            '"v1.2.10 aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"', result
        )
        self.assertIn(
            'releases = [ "v0.1.0 4444444444444444444444444444444444444444" ]',
            result,
        )
        before_untouched = PACKAGES_TOML.split("[[packages]]\nname = \"untouched\"", 1)[1]
        after_untouched = result.split("[[packages]]\nname = \"untouched\"", 1)[1]
        self.assertEqual(before_untouched, after_untouched)


if __name__ == "__main__":
    unittest.main()
