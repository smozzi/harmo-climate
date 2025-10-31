"""End-to-end pipeline for training and exporting HarmoClimate models."""

from __future__ import annotations

import json
import math
from datetime import datetime, timezone

from .config import (
    CPP_HEADER_PATH,
    DATA_DIR,
    GENERATED_DIR,
    MODEL_JSON_PATH,
    MODEL_DIR,
    PARQUET_PATH,
    TEMPLATE_DIR,
    URLS,
)
from .data_ingest import load_filtered_dataset, stream_filter_to_disk
from .metadata import StationMetadata, summarize_station
from .template_cpp import generate_cpp_header
from .training import build_parameter_bundle, train_models


def ensure_directories() -> None:
    for path in (GENERATED_DIR, DATA_DIR, MODEL_DIR, TEMPLATE_DIR):
        path.mkdir(parents=True, exist_ok=True)


def run_pipeline() -> tuple[StationMetadata, float, float]:
    """Execute the full training, export, and template generation workflow."""

    ensure_directories()

    station_records = stream_filter_to_disk(URLS, PARQUET_PATH)
    df = load_filtered_dataset(PARQUET_PATH)
    station_meta = summarize_station(station_records, df)

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

    print(f"Global MAE Temperature : {result.mae_temperature:.2f} °C")
    print(f"Global MAE Humidity    : {result.mae_humidity:.2f} %")

    metadata_payload = {
        "station_code": station_meta.station_code,
        "station_usual_name": station_meta.station_name,
        "longitude_deg": station_meta.longitude_deg,
        "latitude_deg": station_meta.latitude_deg,
        "altitude_m": station_meta.altitude_m,
        "delta_utc_solar_h": station_meta.delta_utc_solar_h,
        "mae_temperature": result.mae_temperature,
        "mae_humidity": result.mae_humidity,
    }

    generation_date = datetime.now(timezone.utc).isoformat()
    params = build_parameter_bundle(result, metadata_payload, generation_date)

    with open(MODEL_JSON_PATH, "w", encoding="utf-8") as handle:
        json.dump(params, handle, indent=2)
    print(f"[OK] Parameters exported to {MODEL_JSON_PATH}")

    generate_cpp_header(params, CPP_HEADER_PATH)

    return station_meta, result.mae_temperature, result.mae_humidity


__all__ = ["run_pipeline"]
