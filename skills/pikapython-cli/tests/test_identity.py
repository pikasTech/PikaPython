import os
import tempfile
import unittest
from pathlib import Path
from unittest import mock

from pikapython_cli import identity
from pikapython_cli import prebuild


class IdentityTest(unittest.TestCase):
    def test_explicit_commit_is_reported_without_network_or_git_lookup(self):
        commit = "E8C3397589C7ABBF7A56CDF53F4D2B2D310A0F3B"
        with mock.patch.dict(
            os.environ,
            {"PIKAPYTHON_CLI_SOURCE_COMMIT": commit},
            clear=False,
        ):
            value = identity.get_identity()
        self.assertEqual(commit.lower(), value["sourceCommit"])
        self.assertEqual("environment", value["sourceKind"])
        self.assertEqual("0.1.0", value["packageVersion"])

    def test_invalid_explicit_commit_is_not_claimed_as_identity(self):
        with mock.patch.dict(
            os.environ,
            {"PIKAPYTHON_CLI_SOURCE_COMMIT": "stale-ref"},
            clear=False,
        ):
            value = identity.get_identity()
        self.assertNotEqual("stale-ref", value["sourceCommit"])

    def test_binding_inputs_include_project_root_stubs(self):
        with tempfile.TemporaryDirectory() as directory:
            project = Path(directory)
            stub = project / "_lvdt.pyi"
            stub.write_text("def snapshot() -> tuple: ...\n", encoding="utf-8")
            inputs = prebuild._binding_inputs(
                project,
                project / "source",
                {"files": [], "fileOrigins": {}},
            )
        self.assertEqual([("_lvdt.pyi", stub)], inputs)


if __name__ == "__main__":
    unittest.main()
