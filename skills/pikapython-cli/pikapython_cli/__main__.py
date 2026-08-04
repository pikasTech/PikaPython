import os
import sys


def _remove_application_import_path():
    application = os.path.realpath(os.getcwd())
    sys.path[:] = [
        entry
        for entry in sys.path
        if os.path.realpath(entry or application) != application
    ]


def run():
    _remove_application_import_path()
    from .cli import main

    return main()


if __name__ == "__main__":
    raise SystemExit(run())
