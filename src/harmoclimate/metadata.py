"""Helpers to compute station-level metadata."""

from __future__ import annotations

import math
from dataclasses import dataclass
from typing import Iterable, Optional

import numpy as np
import pandas as pd

from .data_ingest import StationRecord


@dataclass
class StationMetadata:
    """Aggregated metadata for the target station."""

    station_name: str
    station_code: Optional[str]
    latitude_deg: float
    longitude_deg: float
    altitude_m: float
    delta_utc_solar_h: float


def _mean_or_nan(values: Iterable[float]) -> float:
    arr = [v for v in values if not math.isnan(v)]
    return float(np.mean(arr)) if arr else float("nan")


def summarize_station(
    records: Iterable[StationRecord],
    df: pd.DataFrame,
    fallback_name: Optional[str] = None,
) -> StationMetadata:
    """Aggregate per-row observations into a stable station description."""

    record_list = list(records)

    lon_values = [r.lon for r in record_list if not math.isnan(r.lon)]
    lat_values = [r.lat for r in record_list if not math.isnan(r.lat)]
    alti_values = [r.alti for r in record_list if not math.isnan(r.alti)]
    delta_values = [r.delta_utc_solar_h for r in record_list if not math.isnan(r.delta_utc_solar_h)]
    name_values = [r.station_name for r in record_list if r.station_name]
    code_values = [r.station_code for r in record_list if r.station_code]

    longitude_deg = _mean_or_nan(lon_values) if lon_values else float(df["LON"].mean())
    latitude_deg = _mean_or_nan(lat_values) if lat_values else float(df["LAT"].mean())
    altitude_m = _mean_or_nan(alti_values) if alti_values else float(df["ALTI"].mean())

    if delta_values:
        delta_utc_solar_h = _mean_or_nan(delta_values)
    else:
        delta_utc_solar_h = float(df["LON"].mean() / 15.0)

    station_name = name_values[0] if name_values else (fallback_name or "Unknown station")
    station_code = code_values[0] if code_values else None

    return StationMetadata(
        station_name=station_name,
        station_code=station_code,
        longitude_deg=float(longitude_deg),
        latitude_deg=float(latitude_deg),
        altitude_m=float(altitude_m),
        delta_utc_solar_h=float(delta_utc_solar_h),
    )


__all__ = ["StationMetadata", "summarize_station"]
