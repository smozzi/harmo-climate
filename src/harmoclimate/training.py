"""Linear harmonic model fitting utilities for HarmoClimate."""

from __future__ import annotations

import math
from dataclasses import dataclass
from typing import Dict, Iterable, List

import numpy as np
import pandas as pd

from .config import AUTHOR_NAME, COUNTRY_CODE, MODEL_VERSION
from .core import SOLAR_YEAR_DAYS, compute_solar_time, specific_humidity_kg_per_kg


@dataclass
class ErrorMetrics:
    """Collection of scalar error metrics for a predicted variable."""

    mae: float
    bias: float
    err_p05: float
    err_p95: float

    def as_dict(self) -> Dict[str, float]:
        return {
            "mae": self.mae,
            "bias": self.bias,
            "err_p05": self.err_p05,
            "err_p95": self.err_p95,
        }


@dataclass
class ParameterLayout:
    """Describes how a slice of the coefficient vector maps to a model parameter."""

    name: str
    role: str
    n_annual: int
    start: int
    length: int

    def as_dict(self) -> Dict[str, object]:
        return {
            "name": self.name,
            "role": self.role,
            "n_annual": self.n_annual,
            "start": self.start,
            "length": self.length,
        }


@dataclass
class LinearModelFit:
    """Fitted linear harmonic model for a single target variable."""

    target_variable: str
    target_unit: str
    coefficients: np.ndarray
    params_layout: List[ParameterLayout]
    n_diurnal: int
    default_n_annual: int
    annual_per_param: Dict[str, int]
    metrics: ErrorMetrics

    def coefficients_list(self) -> List[float]:
        return [float(v) for v in self.coefficients]


@dataclass
class TrainingResult:
    """Bundle capturing linear models for temperature, specific humidity, and pressure."""

    temperature_model: LinearModelFit
    specific_humidity_model: LinearModelFit
    pressure_model: LinearModelFit


def _compute_error_metrics(errors: np.ndarray) -> ErrorMetrics:
    """Compute basic error diagnostics for residuals."""

    valid = errors[np.isfinite(errors)]
    if valid.size == 0:
        return ErrorMetrics(mae=math.nan, bias=math.nan, err_p05=math.nan, err_p95=math.nan)

    mae = float(np.mean(np.abs(valid)))
    bias = float(np.mean(valid))
    err_p05 = float(np.quantile(valid, 0.05))
    err_p95 = float(np.quantile(valid, 0.95))
    return ErrorMetrics(mae=mae, bias=bias, err_p05=err_p05, err_p95=err_p95)


def build_annual_basis(day: np.ndarray, n_annual: int) -> np.ndarray:
    """Construct the annual harmonic basis (constant + cos/sin pairs)."""

    omega = 2.0 * math.pi / SOLAR_YEAR_DAYS
    cols = [np.ones_like(day)]
    for k in range(1, n_annual + 1):
        cols.append(np.cos(k * omega * day))
        cols.append(np.sin(k * omega * day))
    return np.column_stack(cols)


def _role_for_parameter(name: str) -> str:
    if name == "c0":
        return "offset"
    if name.startswith("a"):
        return f"diurnal_cos_{int(name[1:])}"
    if name.startswith("b"):
        return f"diurnal_sin_{int(name[1:])}"
    raise ValueError(f"Unknown parameter name '{name}'")


def build_global_linear_matrix(
    df: pd.DataFrame,
    *,
    n_diurnal: int,
    annual_per_param: Dict[str, int],
    default_n_annual: int,
    target: str,
) -> tuple[np.ndarray, np.ndarray, List[Dict[str, int]]]:
    """Construct the design matrix for the factorized linear model."""

    required_cols = {"yday_frac_solar", "hour_solar", target}
    missing = required_cols - set(df.columns)
    if missing:
        raise KeyError(f"Missing required columns for training: {sorted(missing)}")

    local = df.copy()
    local = local.dropna(subset=["yday_frac_solar", "hour_solar", target])
    local["day"] = np.floor(local["yday_frac_solar"]).astype(float)
    max_day_index = math.floor(SOLAR_YEAR_DAYS)
    local.loc[local["day"] > max_day_index, "day"] = float(max_day_index)
    local["hour"] = np.mod(local["hour_solar"].astype(float), 24.0)

    day = local["day"].to_numpy(dtype=float)
    hour = local["hour"].to_numpy(dtype=float)
    y = local[target].to_numpy(dtype=float)

    omega = 2.0 * math.pi / 24.0
    param_names: List[str] = ["c0"]
    for m in range(1, n_diurnal + 1):
        param_names.extend([f"a{m}", f"b{m}"])

    X_blocks: List[np.ndarray] = []
    params_meta: List[Dict[str, int]] = []
    start_idx = 0

    for name in param_names:
        n_annual_param = annual_per_param.get(name, default_n_annual)
        annual_basis = build_annual_basis(day, n_annual_param)

        if name == "c0":
            block = annual_basis
        elif name.startswith("a"):
            harmonic = int(name[1:])
            block = annual_basis * np.cos(harmonic * omega * hour)[:, None]
        elif name.startswith("b"):
            harmonic = int(name[1:])
            block = annual_basis * np.sin(harmonic * omega * hour)[:, None]
        else:
            raise ValueError(f"Unhandled parameter name '{name}'")

        X_blocks.append(block)
        length = annual_basis.shape[1]
        params_meta.append(
            {
                "name": name,
                "n_annual": n_annual_param,
                "start": start_idx,
                "length": length,
            }
        )
        start_idx += length

    X = np.concatenate(X_blocks, axis=1)
    return X, y, params_meta


def _build_layout(meta: Iterable[Dict[str, int]], target_unit: str) -> List[ParameterLayout]:
    layout: List[ParameterLayout] = []
    for entry in meta:
        name = entry["name"]
        role = _role_for_parameter(name)
        layout.append(
            ParameterLayout(
                name=name,
                role=role,
                n_annual=int(entry["n_annual"]),
                start=int(entry["start"]),
                length=int(entry["length"]),
            )
        )
    return layout


def _fit_linear_model(
    df: pd.DataFrame,
    *,
    target_variable: str,
    target_unit: str,
    n_diurnal: int,
    default_n_annual: int,
    annual_per_param: Dict[str, int],
) -> LinearModelFit:
    X, y, params_meta = build_global_linear_matrix(
        df,
        n_diurnal=n_diurnal,
        annual_per_param=annual_per_param,
        default_n_annual=default_n_annual,
        target=target_variable,
    )

    coef, *_ = np.linalg.lstsq(X, y, rcond=None)
    predictions = X @ coef
    residuals = y - predictions

    layout = _build_layout(params_meta, target_unit)
    metrics = _compute_error_metrics(residuals)

    return LinearModelFit(
        target_variable=target_variable,
        target_unit=target_unit,
        coefficients=coef.astype(float),
        params_layout=layout,
        n_diurnal=n_diurnal,
        default_n_annual=default_n_annual,
        annual_per_param=dict(annual_per_param),
        metrics=metrics,
    )


def train_models(
    df: pd.DataFrame,
    *,
    n_diurnal: int = 3,
    default_n_annual: int = 3,
    annual_per_param: Dict[str, int] | None = None,
) -> TrainingResult:
    """Fit linear harmonic models for temperature, specific humidity, and pressure."""

    annual_per_param = dict(annual_per_param or {})
    working = df.copy()
    if "DT_UTC" not in working.columns or "LON" not in working.columns:
        raise KeyError("Dataset must include 'DT_UTC' and 'LON' columns for solar-time features.")
    if "P" not in working.columns:
        raise KeyError("Dataset is missing pressure column 'P'.")

    solar_time = compute_solar_time(working["DT_UTC"], working["LON"])
    for column in solar_time.columns:
        working[column] = solar_time[column]

    working["Q"] = specific_humidity_kg_per_kg(working["T"], working["RH"], working["P"])

    temperature_model = _fit_linear_model(
        working,
        target_variable="T",
        target_unit="degC",
        n_diurnal=n_diurnal,
        default_n_annual=default_n_annual,
        annual_per_param=annual_per_param,
    )

    specific_humidity_model = _fit_linear_model(
        working,
        target_variable="Q",
        target_unit="kg/kg",
        n_diurnal=n_diurnal,
        default_n_annual=default_n_annual,
        annual_per_param=annual_per_param,
    )

    pressure_model = _fit_linear_model(
        working,
        target_variable="P",
        target_unit="hPa",
        n_diurnal=n_diurnal,
        default_n_annual=default_n_annual,
        annual_per_param=annual_per_param,
    )

    return TrainingResult(
        temperature_model=temperature_model,
        specific_humidity_model=specific_humidity_model,
        pressure_model=pressure_model,
    )


def build_parameter_payload(
    model: LinearModelFit,
    metadata: Dict[str, object],
    generation_date_utc: str,
) -> Dict[str, object]:
    """Build the JSON-friendly payload for a single fitted model."""

    payload = {
        "metadata": {
            "version": MODEL_VERSION,
            "generated_at_utc": generation_date_utc,
            "country_code": COUNTRY_CODE,
            "author": AUTHOR_NAME,
            "target_variable": model.target_variable,
            "target_unit": model.target_unit,
            **metadata,
        },
        "model": {
            "n_diurnal": model.n_diurnal,
            "params_layout": [entry.as_dict() for entry in model.params_layout],
            "coefficients": model.coefficients_list(),
        },
    }
    payload["metadata"]["error_envelope"] = {
        "mae": model.metrics.mae,
        "bias": model.metrics.bias,
        "p05": model.metrics.err_p05,
        "p95": model.metrics.err_p95,
    }
    payload["metadata"]["time_basis"] = {
        "type": "solar",
        "days": SOLAR_YEAR_DAYS,
        "calendar": "no-leap",
    }
    return payload


__all__ = [
    "ErrorMetrics",
    "ParameterLayout",
    "LinearModelFit",
    "TrainingResult",
    "build_annual_basis",
    "build_global_linear_matrix",
    "build_parameter_payload",
    "train_models",
]
