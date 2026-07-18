import tempfile
import unittest
from pathlib import Path

from release_check import (
    parse_cmsis_version,
    parse_header_version,
    parse_version_config,
)
from release_diff import render_markdown
from release_helper import PackageReleaseList


class ReleaseMetadataParserTest(unittest.TestCase):
    def setUp(self):
        self.temp_dir = tempfile.TemporaryDirectory()
        self.root = Path(self.temp_dir.name)

    def tearDown(self):
        self.temp_dir.cleanup()

    def write(self, name, text):
        path = self.root / name
        path.write_text(text, encoding="utf-8")
        return path

    def test_version_sources(self):
        header = self.write(
            "PikaVersion.h",
            "#define PIKA_VERSION_MAJOR 1\n"
            "#define PIKA_VERSION_MINOR 14\n"
            "#define PIKA_VERSION_MICRO 0\n",
        )
        config = self.write(
            "version_config.py",
            'MajorVersion = "1"\nMinorVersion = "14"\nMicroVersion = "0"\n',
        )
        cmsis = self.write("makepdsc.py", 'version = "1.14.0"\n')

        self.assertEqual("1.14.0", parse_header_version(header))
        self.assertEqual("1.14.0", parse_version_config(config))
        self.assertEqual("1.14.0", parse_cmsis_version(cmsis))

    def test_package_release_list_from_git_text(self):
        releases = PackageReleaseList.from_text(
            '[[packages]]\nname = "demo"\n'
            'releases = [ "v1.2.3 aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" ]\n'
        )

        self.assertEqual("v1.2.3", releases.findPackage("demo").latestVersion().version)


class ReleaseDiffRenderingTest(unittest.TestCase):
    def test_render_markdown(self):
        markdown = render_markdown(
            {
                "baseline": "a" * 40,
                "packages": [
                    {
                        "name": "demo",
                        "state": "update",
                        "from": "v1.0.0",
                        "to": "v1.1.0",
                    }
                ],
                "commits": [{"commit": "b" * 40, "subject": "fix demo"}],
            }
        )

        self.assertIn("| demo | update | v1.0.0 -> v1.1.0 |", markdown)
        self.assertIn("`bbbbbbbbbb` fix demo", markdown)


if __name__ == "__main__":
    unittest.main()
