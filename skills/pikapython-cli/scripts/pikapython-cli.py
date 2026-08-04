#!/usr/bin/env python3
import sys
from pathlib import Path


SKILL_ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(SKILL_ROOT))

from pikapython_cli.__main__ import run


if __name__ == "__main__":
    raise SystemExit(run())
