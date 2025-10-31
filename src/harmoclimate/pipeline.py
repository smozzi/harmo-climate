"""End-to-end pipeline for training and exporting HarmoClimate models."""

from __future__ import annotations

import json
import math
from dataclasses import replace
from datetime import datetime, timezone
from pathlib import Path
from typing import Iterable, Sequence

from .config import (
    DATA_DIR,
    GENERATED_DIR,
    MODEL_DIR,
    COUNTRY_CODE,
    STATION_CODE,
    TEMPLATE_DIR,
    ArtifactPaths,
    build_artifact_paths,
    build_urls_for_station,
    slugify_station_name,
)
from .data_ingest import StationRecord, StreamResult, load_filtered_dataset, stream_filter_to_disk
from .metadata import StationMetadata, summarize_station
from .template_cpp import generate_cpp_header
from .training import build_parameter_bundle, train_models


def ensure_directories() -> None:
    for path in (GENERATED_DIR, DATA_DIR, MODEL_DIR, TEMPLATE_DIR):
        path.mkdir(parents=True, exist_ok=True)


def _finalize_pipeline(
    *,
    df,
    artifact_paths: ArtifactPaths,
    station_records: Iterable[StationRecord],
    station_name: str,
    station_code: str | None,
) -> tuple[StationMetadata, float, float]:
    """Train models, export artefacts, and summarise metadata."""

    station_meta = summarize_station(station_records, df, fallback_name=station_name)
    if station_meta.station_code is None and station_code:
        station_meta = replace(station_meta, station_code=str(station_code))

    print(f"[Info] Mean station longitude = {station_meta.longitude_deg:.5f} °")
    if not math.isnan(station_meta.latitude_deg):
        print(f"[Info] Mean station latitude  = {station_meta.latitude_deg:.5f} °")
    else:
        print("[Info] Mean station latitude  = (not available in source)")
    print(f"[Info] UTC -> solar offset     = {station_meta.delta_utc_solar_h:.5f} h")
    print(f"[Info] Station usual name      = {station_meta.station_name}")
    if station_meta.station_code:
        print(f"[Info] Station code           = {station_meta.station_code}")

    result = train_models(df)

    print(f"Global MAE Temperature : {result.metrics_temperature.mae:.2f} °C")
    print(f"Global MAE Humidity    : {result.metrics_humidity.mae:.2f} %")

    metadata_payload = {
        "station_code": station_meta.station_code,
        "station_usual_name": station_meta.station_name,
        "longitude_deg": station_meta.longitude_deg,
        "latitude_deg": station_meta.latitude_deg,
        "altitude_m": station_meta.altitude_m,
        "delta_utc_solar_h": station_meta.delta_utc_solar_h,
        "metrics": {
            "temperature": result.metrics_temperature.as_dict(),
            "relative_humidity": result.metrics_humidity.as_dict(),
        },
    }

    generation_date = datetime.now(timezone.utc).isoformat()
    params = build_parameter_bundle(result, metadata_payload, generation_date)

    with open(artifact_paths.model_json, "w", encoding="utf-8") as handle:
        json.dump(params, handle, indent=2)
    print(f"[OK] Parameters exported to {artifact_paths.model_json}")

    generate_cpp_header(params, artifact_paths.cpp_header)

    return station_meta


def _as_float(value: object) -> float:
    """Best-effort float coercion."""

    try:
        return float(value)
    except (TypeError, ValueError):
        return math.nan


def _record_from_metadata(metadata: dict[str, object]) -> StationRecord:
    """Build a synthetic station record from cached metadata."""

    station_code_raw = metadata.get("station_code")
    station_code = str(station_code_raw).strip() if station_code_raw is not None else None
    if station_code == "":
        station_code = None

    station_name = str(
        metadata.get("station_usual_name")
        or metadata.get("station_name")
        or metadata.get("station_usual_name".upper())
        or "",
    ).strip()

    return StationRecord(
        station_code=station_code,
        station_name=station_name,
        lon=_as_float(metadata.get("longitude_deg")),
        lat=_as_float(metadata.get("latitude_deg")),
        alti=_as_float(metadata.get("altitude_m")),
        delta_utc_solar_h=_as_float(metadata.get("delta_utc_solar_h")),
    )


def _resolve_model_path(model_json: str | Path) -> Path:
    """Resolve a model JSON path, falling back to the generated models directory."""

    candidate = Path(model_json)
    if candidate.exists():
        return candidate

    fallback = MODEL_DIR / candidate.name
    if fallback.exists():
        return fallback

    raise FileNotFoundError(f"Could not locate model JSON at {candidate} or {fallback}")


def _slug_from_model_path(model_path: Path) -> str:
    """Extract the station slug from a model file name."""

    basename = model_path.stem
    expected_prefix = f"{COUNTRY_CODE.lower()}_"
    if basename.startswith(expected_prefix):
        return basename[len(expected_prefix) :]
    return basename


def run_pipeline(station_code: str = STATION_CODE, urls: Sequence[str] | None = None) -> tuple[StationMetadata, float, float]:
    """Execute the full training, export, and template generation workflow."""

    ensure_directories()

    resolved_urls = list(urls) if urls is not None else build_urls_for_station(station_code)
    stream_result: StreamResult = stream_filter_to_disk(resolved_urls, station_code=station_code)
    artifact_paths = build_artifact_paths(stream_result.station_slug)
    df = load_filtered_dataset(artifact_paths.parquet)

    return _finalize_pipeline(
        df=df,
        artifact_paths=artifact_paths,
        station_records=stream_result.station_records,
        station_name=stream_result.station_name,
        station_code=station_code,
    )


def generate_pipeline(station_code: str) -> tuple[StationMetadata, float, float]:
    """Fetch remote data for a station code and run the end-to-end pipeline."""

    return run_pipeline(station_code=station_code)


def regenerate_pipeline(model_json: str | Path) -> tuple[StationMetadata, float, float]:
    """Rebuild artefacts from a cached dataset based on an existing model JSON."""

    ensure_directories()

    model_path = _resolve_model_path(model_json)
    with open(model_path, "r", encoding="utf-8") as handle:
        payload = json.load(handle)

    metadata = payload.get("metadata", {})
    station_code_raw = metadata.get("station_code")
    station_code = str(station_code_raw).strip() if station_code_raw is not None else None
    if station_code == "":
        station_code = None

    station_name = str(metadata.get("station_usual_name") or station_code or model_path.stem).strip()
    station_slug_hint = slugify_station_name(station_name) if station_name else ""
    if not station_slug_hint:
        station_slug_hint = _slug_from_model_path(model_path)

    artifact_paths = build_artifact_paths(station_slug_hint)

    if artifact_paths.model_json != model_path:
        artifact_paths = ArtifactPaths(
            parquet=artifact_paths.parquet,
            model_json=model_path,
            cpp_header=artifact_paths.cpp_header,
        )

    station_records: list[StationRecord] = []

    if artifact_paths.parquet.exists():
        print(f"[Cache] Using cached dataset at {artifact_paths.parquet}")
        df = load_filtered_dataset(artifact_paths.parquet)
        if metadata:
            station_records.append(_record_from_metadata(metadata))
    else:
        if not station_code:
            raise RuntimeError(
                "The provided model JSON does not include a station_code; cannot refresh remote data.",
            )
        print("[Info] Cached dataset not found; streaming source archives again.")
        stream_result: StreamResult = stream_filter_to_disk(
            build_urls_for_station(station_code),
            station_code=station_code,
        )
        artifact_paths = build_artifact_paths(stream_result.station_slug)
        df = load_filtered_dataset(artifact_paths.parquet)
        station_records.extend(stream_result.station_records)
        station_name = stream_result.station_name

    return _finalize_pipeline(
        df=df,
        artifact_paths=artifact_paths,
        station_records=station_records,
        station_name=station_name,
        station_code=station_code,
    )


__all__ = ["ensure_directories", "run_pipeline", "generate_pipeline", "regenerate_pipeline"]
