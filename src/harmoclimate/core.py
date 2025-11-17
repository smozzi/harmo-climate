"""Core astronomical and thermodynamic helpers for HarmoClimate."""

from __future__ import annotations

from pathlib import Path
from typing import Iterable

import numpy as np
import pandas as pd

from .psychrometrics import (
    dew_or_frost_point_c_from_e,
    relative_humidity_percent_from_specific,
    specific_humidity_kg_per_kg,
    thermo_from_T_P_RH,
    vapor_partial_pressure_hpa_from_q_p,
)
SOLAR_YEAR_DAYS: float = 365.242189
SOLAR_EPOCH_UTC = pd.Timestamp("2000-01-01 00:00:00", tz="UTC")
DATASET_COLUMNS: tuple[str, ...] = (
    "STATION_CODE",
    "STATION_NAME",
    "DT_UTC",
    "T",
    "RH",
    "P",
    "LON",
    "LAT",
    "ALTI",
)
_DERIVED_DATASET_COLUMNS: tuple[str, ...] = (
    "yday_frac_solar",
    "hour_solar",
    "delta_utc_solar_h",
    "Q",
    "Td",
    "E",
)
DEFAULT_PREPARED_COLUMNS: tuple[str, ...] = (
    "DT_UTC",
    "LON",
    "T",
    "RH",
    "P",
    *_DERIVED_DATASET_COLUMNS,
)


def load_parquet_dataset(parquet_path: Path) -> pd.DataFrame:
    """Load a cached Parquet dataset and optionally constrain the returned columns."""

    if not parquet_path.exists():
        raise FileNotFoundError(f"No Parquet dataset found at {parquet_path}")

    df = pd.read_parquet(parquet_path)

    missing = set(DATASET_COLUMNS) - set(df.columns)
    if missing:
        raise KeyError(f"Dataset at {parquet_path} is missing required columns: {sorted(missing)}")

    return df


def compute_solar_time(
    dt_utc: pd.Series | np.ndarray,
    lon_deg: pd.Series | np.ndarray,
) -> pd.DataFrame:
    """
    Convert UTC datetimes and longitudes into solar descriptors.

    Returns:
        DataFrame with columns:
            - yday_frac_solar: solar-day index in a tropical year (no hour component),
            - hour_solar: local solar hour in [0, 24),
            - delta_utc_solar_h: UTC→solar offset in hours.
    """

    if isinstance(dt_utc, pd.Series):
        dt_series = pd.to_datetime(dt_utc.copy(), errors="coerce")
    else:
        dt_series = pd.to_datetime(pd.Series(dt_utc), errors="coerce")

    if isinstance(lon_deg, pd.Series):
        lon_series = lon_deg.astype("float64").copy()
    else:
        lon_series = pd.Series(lon_deg, dtype="float64")

    # align indices if possible
    if len(lon_series) != len(dt_series):
        raise ValueError("Longitude and datetime inputs must share the same length.")
    lon_series.index = dt_series.index

    # ensure UTC tz-awareness
    if dt_series.dt.tz is None:
        dt_series = dt_series.dt.tz_localize("UTC")
    else:
        dt_series = dt_series.dt.tz_convert("UTC")

    dt_utc_date = dt_series.dt.floor("D")
    delta_days = (dt_utc_date - SOLAR_EPOCH_UTC).dt.total_seconds() / 86400.0
    solar_day = np.mod(delta_days, SOLAR_YEAR_DAYS)

    hour_utc = (
        dt_series.dt.hour.astype(float)
        + dt_series.dt.minute.astype(float) / 60.0
        + dt_series.dt.second.astype(float) / 3600.0
    )
    delta_utc_solar_h = lon_series / 15.0
    hour_solar = (hour_utc + delta_utc_solar_h) % 24.0

    solar_day = (solar_day + (lon_series / 360.0) * SOLAR_YEAR_DAYS) % SOLAR_YEAR_DAYS

    return pd.DataFrame(
        {
            "yday_frac_solar": solar_day.astype(np.float32),
            "hour_solar": hour_solar.astype(np.float32),
            "delta_utc_solar_h": delta_utc_solar_h.astype(np.float32),
        },
        index=dt_series.index,
    )


def prepare_dataset(
    df: pd.DataFrame,
    columns: Iterable[str] | None = None,
) -> pd.DataFrame:
    """
    Return a dataset copy populated with derived solar descriptors and moist thermodynamics.

    Args:
        df: Raw station dataset containing at least the core meteorological columns.
        columns: Optional iterable of column names to keep in the returned frame. Defaults to
            `DEFAULT_PREPARED_COLUMNS`.
    """

    required = {"DT_UTC", "LON", "T", "RH", "P"}
    missing = required - set(df.columns)
    if missing:
        raise KeyError(f"Dataset is missing required columns: {sorted(missing)}")

    working = df.copy()
    working["DT_UTC"] = pd.to_datetime(working["DT_UTC"], utc=True, errors="coerce")
    working["LON"] = pd.to_numeric(working["LON"], errors="coerce")
    working = working.dropna(subset=["DT_UTC", "LON"])

    available_base = set(working.columns)
    derived_names = set(_DERIVED_DATASET_COLUMNS)

    if columns is None:
        requested = list(DEFAULT_PREPARED_COLUMNS)
    else:
        requested = list(columns)

    unknown = set(requested) - (available_base | derived_names)
    if unknown:
        raise KeyError(f"Requested columns are not available: {sorted(unknown)}")

    solar_needed = any(name in requested for name in ("yday_frac_solar", "hour_solar", "delta_utc_solar_h"))
    q_needed = any(name in requested for name in ("Q", "Td", "E"))
    td_needed = "Td" in requested
    e_needed = "E" in requested or td_needed

    if solar_needed:
        solar_time = compute_solar_time(working["DT_UTC"], working["LON"])
        for column in solar_time.columns:
            working[column] = solar_time[column]

    if q_needed:
        working["Q"] = specific_humidity_kg_per_kg(working["T"], working["RH"], working["P"])

    if e_needed:
        working["E"] = vapor_partial_pressure_hpa_from_q_p(working["Q"], working["P"])

    if td_needed:
        working["Td"] = dew_or_frost_point_c_from_e(working["E"])

    # Ensure all requested columns exist before selection (e.g. derived but absent due to NaNs).
    missing_requested = [name for name in requested if name not in working.columns]
    if missing_requested:
        raise KeyError(f"Unable to populate requested columns: {missing_requested}")

    return working.loc[:, requested].copy()


__all__ = [
    "SOLAR_YEAR_DAYS",
    "SOLAR_EPOCH_UTC",
    "DATASET_COLUMNS",
    "specific_humidity_kg_per_kg",
    "compute_solar_time",
    "load_parquet_dataset",
    "prepare_dataset",
    "relative_humidity_percent_from_specific",
    "dew_or_frost_point_c_from_e",
    "vapor_partial_pressure_hpa_from_q_p",
    "thermo_from_T_P_RH",
]
