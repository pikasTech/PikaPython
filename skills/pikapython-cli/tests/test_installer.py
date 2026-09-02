import os
import stat
import tempfile
import unittest
from pathlib import Path
from unittest import mock

from pikapython_cli import installer


class InstallerFilesystemTest(unittest.TestCase):
    def test_same_tree_requires_matching_paths_types_and_content(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            left = root / "left"
            right = root / "right"
            for tree in (left, right):
                (tree / "nested").mkdir(parents=True)
                (tree / "nested" / "value.txt").write_bytes(b"same\n")

            self.assertTrue(installer._same_tree(left, right))

            (right / "nested" / "value.txt").write_bytes(b"different\n")
            self.assertFalse(installer._same_tree(left, right))
            (right / "nested" / "value.txt").write_bytes(b"same\n")
            (right / "empty").mkdir()
            self.assertFalse(installer._same_tree(left, right))

    def test_remove_tree_retries_after_making_read_only_file_writable(self):
        with tempfile.TemporaryDirectory() as directory:
            target = Path(directory) / "transaction"
            target.mkdir()
            read_only = target / "pack-file"
            read_only.write_bytes(b"pack")
            read_only.chmod(stat.S_IREAD)

            def simulated_rmtree(path, onerror):
                def remove(name):
                    self.assertTrue(os.stat(name).st_mode & stat.S_IWRITE)
                    Path(name).unlink()

                onerror(remove, str(read_only), (PermissionError, None, None))
                Path(path).rmdir()

            with mock.patch.object(
                installer.shutil, "rmtree", side_effect=simulated_rmtree
            ):
                installer._remove_tree(target)

            self.assertFalse(target.exists())


if __name__ == "__main__":
    unittest.main()
