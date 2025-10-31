"""Backward-compatible entry point for the HarmoClimate training pipeline."""

from __future__ import annotations

import sys
from pathlib import Path

SRC_ROOT = Path(__file__).resolve().parent / "src"
if str(SRC_ROOT) not in sys.path:
    sys.path.insert(0, str(SRC_ROOT))

from harmoclimate.pipeline import run_pipeline


if __name__ == "__main__":
    run_pipeline()
