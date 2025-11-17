"""Psychrometric conversion utilities for HarmoClimate.

Relies on the two-pole saturation formula from https://github.com/smozzi/water-saturation-pressure-2p.git.
"""

from __future__ import annotations

import numpy as np
import pandas as pd

from wsp2p import (
    EPS as WSP2P_EPS,
    dewpoint_c_from_T_RH,
    esat_water_hpa,
    rh_percent as wsp2p_rh_percent,
    specific_humidity_kg_per_kg as wsp2p_specific_humidity_kg_per_kg,
    T_from_e_water,
)

__all__ = [
    "specific_humidity_kg_per_kg",
    "relative_humidity_percent_from_specific",
    "dew_or_frost_point_c_from_e",
    "vapor_partial_pressure_hpa_from_q_p",
    "thermo_from_T_P_RH",
]

EPS = float(WSP2P_EPS)
ONE_MINUS_EPS = 1.0 - EPS

def specific_humidity_kg_per_kg(
    temp_c: pd.Series | np.ndarray,
    rh_percent: pd.Series | np.ndarray,
    pressure_hpa: pd.Series | np.ndarray,
) -> np.ndarray:
    """Return specific humidity (kg/kg) from temperature (°C), RH (%), and pressure (hPa)."""

    return wsp2p_specific_humidity_kg_per_kg(temp_c, rh_percent, pressure_hpa)


def relative_humidity_percent_from_specific(
    temp_c: pd.Series | np.ndarray,
    q_kg_per_kg: pd.Series | np.ndarray,
    pressure_hpa: pd.Series | np.ndarray,
) -> np.ndarray:
    """Return relative humidity (%) from temperature (°C), specific humidity (kg/kg), and pressure (hPa)."""

    q = np.asarray(q_kg_per_kg, dtype=np.float64)
    p = np.asarray(pressure_hpa, dtype=np.float64)
    e_hpa = vapor_partial_pressure_hpa_from_q_p(q, p)
    return wsp2p_rh_percent(temp_c, e_hpa)


def dew_or_frost_point_c_from_e(
    e_hpa: pd.Series | np.ndarray,
) -> np.ndarray:
    """Return dew point (°C) from vapor partial pressure (hPa)."""

    e = np.asarray(e_hpa, dtype=np.float64)
    e = np.clip(e, 1e-9, None)
    return T_from_e_water(e)


def vapor_partial_pressure_hpa_from_q_p(
    q_kg_per_kg: pd.Series | np.ndarray,
    pressure_hpa: pd.Series | np.ndarray
) -> np.ndarray:
    """Return vapor partial pressure (hPa) from specific humidity (kg/kg) and pressure (hPa)."""

    q = np.asarray(q_kg_per_kg, dtype=np.float64)
    p = np.asarray(pressure_hpa, dtype=np.float64)
    q = np.clip(q, 0.0, 0.05)
    denom = np.maximum(EPS + ONE_MINUS_EPS * q, 1e-12)
    e_hpa = (q * p) / denom
    return np.clip(e_hpa, 1e-6, p * (1.0 - 1e-6))


def thermo_from_T_P_RH(
    temp_c: pd.Series | np.ndarray,
    pressure_hpa: pd.Series | np.ndarray,
    rh_percent: pd.Series | np.ndarray,
) -> tuple[np.ndarray, np.ndarray, np.ndarray, np.ndarray]:
    """Return `(q, Td, E, Es)` derived from temperature (°C), pressure (hPa), and RH (%)."""

    T = np.asarray(temp_c, dtype=np.float64)
    P = np.asarray(pressure_hpa, dtype=np.float64)
    RH = np.clip(np.asarray(rh_percent, dtype=np.float64), 0.0, 100.0)

    Es = esat_water_hpa(T)
    E = Es * (RH / 100.0)
    Td = dewpoint_c_from_T_RH(T, RH)
    q = wsp2p_specific_humidity_kg_per_kg(T, RH, P)

    return q, Td, E, Es
