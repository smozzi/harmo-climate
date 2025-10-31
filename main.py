"""Command-line interface for the HarmoClimate training pipeline."""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

SRC_ROOT = Path(__file__).resolve().parent / "src"
if str(SRC_ROOT) not in sys.path:
    sys.path.insert(0, str(SRC_ROOT))

from harmoclimate.pipeline import generate_pipeline, regenerate_pipeline, run_pipeline  # noqa: E402


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="Run HarmoClimate model generation workflows.")
    subparsers = parser.add_subparsers(dest="command")

    generate_parser = subparsers.add_parser(
        "generate",
        help="Stream data for a station code and generate model artefacts.",
    )
    generate_parser.add_argument(
        "station_code",
        help="Météo-France station code (NUM_POSTE) used to filter the source dataset.",
    )

    regenerate_parser = subparsers.add_parser(
        "regenerate",
        help="Regenerate an existing model using cached data or by refetching the archives.",
    )
    regenerate_parser.add_argument(
        "model_json",
        help="Path or file name of the previously generated model JSON (e.g. fr_bourges.json).",
    )

    return parser


def main(argv: list[str] | None = None) -> int:
    parser = build_parser()
    args = parser.parse_args(argv)

    if args.command == "generate":
        generate_pipeline(args.station_code)
        return 0
    if args.command == "regenerate":
        regenerate_pipeline(args.model_json)
        return 0

    # Backwards-compatible behaviour: default to the configured station.
    run_pipeline()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
