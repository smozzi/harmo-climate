"""Psychrometric conversion utilities for HarmoClimate.

Relies on the two-pole saturation formula from https://github.com/smozzi/water-saturation-pressure-2p.git.
"""

from __future__ import annotations

import numpy as np
import pandas as pd

from wsp2p import EPS as WSP2P_EPS, esat_water_hpa, T_from_e_water

__all__ = [
    "specific_humidity_kg_per_kg",
    "relative_humidity_percent_from_specific",
    "dew_point_c_from_e",
    "vapor_partial_pressure_hpa_from_q_p",
    "thermo_from_T_P_RH",
]

EPS = float(WSP2P_EPS)
ONE_MINUS_EPS = 1.0 - EPS

# Numerical safety bounds and physical limits used across vapor-pressure conversions.
MIN_E_HPA = 1e-9
MIN_DENOM = 1e-12
MAX_Q_KG_PER_KG = 0.05


def buck_enhancement_factor_water_eq6(
    temp_c: pd.Series | np.ndarray,
    pressure_hpa: pd.Series | np.ndarray,
) -> np.ndarray:
    """Return Buck (1981) Eq. 6 moist-air enhancement factor over water."""

    T = np.asarray(temp_c, dtype=np.float64)
    P = np.asarray(pressure_hpa, dtype=np.float64)

    # Coefficients (Buck 1981, Table 3, f_ws for water surfaces).
    A, B, C, D, E = 4.1e-4, 3.48e-6, 7.4e-10, 30.6, -3.8e-2
    return 1.0 + A + P * (B + C * (T + D + E * P) ** 2)


def _moist_air_vapor_pressure_hpa(
    temp_c: np.ndarray,
    pressure_hpa: np.ndarray,
    rh_percent: np.ndarray,
) -> tuple[np.ndarray, np.ndarray, np.ndarray]:
    """Return `(E, Es, f)` for moist-air RH inputs.

    Temperatures are in °C and pressures in hPa. Moist-air relative humidity
    follows RH(%) = 100 * e / (f(T, P) * Es(T)), where `Es` comes from
    `esat_water_hpa` and `f` is the Buck enhancement factor
    (`buck_enhancement_factor_water_eq6`).
    """

    Es = esat_water_hpa(temp_c)
    f = buck_enhancement_factor_water_eq6(temp_c, pressure_hpa)
    E = np.clip(
        (rh_percent / 100.0) * f * Es,
        MIN_E_HPA,
        pressure_hpa,
    )
    return E, Es, f


def _specific_humidity_from_vapor_pressure(
    vapor_pressure_hpa: np.ndarray,
    pressure_hpa: np.ndarray,
) -> np.ndarray:
    """Return specific humidity (kg/kg) from vapor partial pressure and pressure (hPa)."""

    e = np.asarray(vapor_pressure_hpa, dtype=np.float64)
    p = np.asarray(pressure_hpa, dtype=np.float64)
    denom = np.maximum(p - ONE_MINUS_EPS * e, MIN_DENOM)
    q = EPS * e / denom
    return np.clip(q, 0.0, MAX_Q_KG_PER_KG)

def specific_humidity_kg_per_kg(
    temp_c: pd.Series | np.ndarray,
    rh_percent: pd.Series | np.ndarray,
    pressure_hpa: pd.Series | np.ndarray,
) -> np.ndarray:
    """Return specific humidity (kg/kg) from temperature (°C), RH (%), and pressure (hPa).

    RH is moist-air RH defined as RH(%) = 100 * e / (f(T, P) * Es(T)), with
    `Es` from `esat_water_hpa` and the enhancement factor `f` from
    `buck_enhancement_factor_water_eq6`.
    """

    T = np.asarray(temp_c, dtype=np.float64)
    RH = np.clip(np.asarray(rh_percent, dtype=np.float64), 0.0, 100.0)
    P = np.asarray(pressure_hpa, dtype=np.float64)
    E, _, _ = _moist_air_vapor_pressure_hpa(T, P, RH)
    return _specific_humidity_from_vapor_pressure(E, P)


def relative_humidity_percent_from_specific(
    temp_c: pd.Series | np.ndarray,
    q_kg_per_kg: pd.Series | np.ndarray,
    pressure_hpa: pd.Series | np.ndarray,
) -> np.ndarray:
    """Return moist-air relative humidity (%) from temperature (°C), specific humidity (kg/kg), and pressure (hPa).

    RH(%) = 100 * e / (f(T, P) * Es(T)), using `esat_water_hpa` for `Es` and
    `buck_enhancement_factor_water_eq6` for the enhancement factor `f`.
    """

    q = np.asarray(q_kg_per_kg, dtype=np.float64)
    p = np.asarray(pressure_hpa, dtype=np.float64)
    T = np.asarray(temp_c, dtype=np.float64)
    e_hpa = vapor_partial_pressure_hpa_from_q_p(q, p)
    Es = esat_water_hpa(T)
    f = buck_enhancement_factor_water_eq6(T, p)
    rh = 100.0 * e_hpa / np.maximum(f * Es, MIN_DENOM)  # Numerical safety bound.
    return np.clip(rh, 0.0, 100.0)


def dew_point_c_from_e(
    e_hpa: pd.Series | np.ndarray,
) -> np.ndarray:
    """Return dew point over liquid water (°C) from vapor partial pressure (hPa)."""

    e = np.asarray(e_hpa, dtype=np.float64)
    e = np.clip(e, MIN_E_HPA, None)  # Numerical safety bound.
    return T_from_e_water(e)


def vapor_partial_pressure_hpa_from_q_p(
    q_kg_per_kg: pd.Series | np.ndarray,
    pressure_hpa: pd.Series | np.ndarray
) -> np.ndarray:
    """Return vapor partial pressure (hPa) from specific humidity (kg/kg) and pressure (hPa)."""

    q = np.asarray(q_kg_per_kg, dtype=np.float64)
    p = np.asarray(pressure_hpa, dtype=np.float64)
    q = np.clip(q, 0.0, MAX_Q_KG_PER_KG)
    denom = np.maximum(EPS + ONE_MINUS_EPS * q, MIN_DENOM)  # Numerical safety bound.
    e_hpa = (q * p) / denom
    return np.clip(e_hpa, MIN_E_HPA, p)


def thermo_from_T_P_RH(
    temp_c: pd.Series | np.ndarray,
    pressure_hpa: pd.Series | np.ndarray,
    rh_percent: pd.Series | np.ndarray,
) -> tuple[np.ndarray, np.ndarray, np.ndarray, np.ndarray]:
    """Return `(q, Td, E, Es)` derived from temperature (°C), pressure (hPa), and RH (%).

    RH is moist-air RH defined as RH(%) = 100 * e / (f(T, P) * Es(T)), using
    `esat_water_hpa` for saturation vapor pressure and
    `buck_enhancement_factor_water_eq6` for the enhancement factor.
    """

    T = np.asarray(temp_c, dtype=np.float64)
    P = np.asarray(pressure_hpa, dtype=np.float64)
    RH = np.clip(np.asarray(rh_percent, dtype=np.float64), 0.0, 100.0)

    E, Es, _ = _moist_air_vapor_pressure_hpa(T, P, RH)
    q = _specific_humidity_from_vapor_pressure(E, P)
    Td = dew_point_c_from_e(E)

    return q, Td, E, Es
