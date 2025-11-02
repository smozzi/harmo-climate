"""Core astronomical and thermodynamic helpers for HarmoClimate."""

from __future__ import annotations

import numpy as np
import pandas as pd

SOLAR_YEAR_DAYS: float = 365.242189
SOLAR_EPOCH_UTC = pd.Timestamp("2000-01-01 00:00:00", tz="UTC")


def specific_humidity_kg_per_kg(
    temp_c: pd.Series | np.ndarray,
    rh_percent: pd.Series | np.ndarray,
    pressure_hpa: pd.Series | np.ndarray,
) -> np.ndarray:
    """Compute specific humidity (kg/kg) from temperature (°C), RH (%), and pressure (hPa)."""

    temp = np.asarray(temp_c, dtype=float)
    rh = np.clip(np.asarray(rh_percent, dtype=float), 0.0, 100.0)
    pressure = np.asarray(pressure_hpa, dtype=float)

    saturation_pressure_hpa = 6.112 * np.exp((17.67 * temp) / (temp + 243.5))
    vapor_pressure_hpa = (rh / 100.0) * saturation_pressure_hpa
    denominator = np.maximum(pressure - 0.378 * vapor_pressure_hpa, 1e-6)
    return 0.622 * vapor_pressure_hpa / denominator


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

def relative_humidity_percent_from_specific(
    temp_c: pd.Series | np.ndarray,
    q_kg_per_kg: pd.Series | np.ndarray,
    pressure_hpa: pd.Series | np.ndarray,
) -> np.ndarray:
    """Compute relative humidity (%) from temperature (°C), specific humidity (kg/kg), and pressure (hPa)."""

    temp = np.asarray(temp_c, dtype=float)
    q = np.asarray(q_kg_per_kg, dtype=float)
    pressure = np.asarray(pressure_hpa, dtype=float)

    # saturation vapor pressure over water (Magnus-Tetens)
    es_hpa = 6.112 * np.exp((17.67 * temp) / (temp + 243.5))

    # actual vapor pressure from specific humidity
    # q = 0.622 * e / (P - 0.378 e)  →  e = q P / (0.622 + 0.378 q)
    numerator = q * pressure
    denominator = 0.622 + 0.378 * q
    e_hpa = numerator / np.maximum(denominator, 1e-6)

    rh = 100.0 * e_hpa / np.maximum(es_hpa, 1e-6)
    return np.clip(rh, 0.0, 100.0)

def dew_point_c_from_q_and_pressure(
    q_kg_per_kg: pd.Series | np.ndarray,
    pressure_hpa: pd.Series | np.ndarray,
) -> np.ndarray:
    """Compute dew point (°C) from specific humidity (kg/kg) and pressure (hPa)."""

    q = np.asarray(q_kg_per_kg, dtype=float)
    p = np.asarray(pressure_hpa, dtype=float)

    # actual vapor pressure from specific humidity
    # q = 0.622 * e / (P - 0.378 e)  →  e = q P / (0.622 + 0.378 q)
    e_hpa = (q * p) / np.maximum(0.622 + 0.378 * q, 1e-6)

    alpha = np.log(np.maximum(e_hpa, 1e-6) / 6.112)
    td = (243.5 * alpha) / (17.67 - alpha)
    return td


__all__ = [
    "SOLAR_YEAR_DAYS",
    "SOLAR_EPOCH_UTC",
    "specific_humidity_kg_per_kg",
    "compute_solar_time",
    "relative_humidity_percent_from_specific",
    "dew_point_c_from_q_and_pressure"
]
