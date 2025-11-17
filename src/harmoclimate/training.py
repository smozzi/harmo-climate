"""Linear harmonic model training utilities for HarmoClimate."""

from __future__ import annotations

import math
from dataclasses import dataclass
from typing import Dict, Iterable, List, Optional, Tuple

import numpy as np
import pandas as pd

from .config import AUTHOR_NAME, COUNTRY_CODE, MODEL_VERSION
from .core import SOLAR_YEAR_DAYS, prepare_dataset


# ----------------------------- Global settings -----------------------------

FINAL_TRAINING_PERIOD_LABEL = "1999–2025"
RIDGE_LAMBDA_DEFAULT = 0.0
DEFAULT_DAYS_INCLUSIVE_MAX = int(math.floor(SOLAR_YEAR_DAYS))

# ----------------------------- Data classes -------------------------------


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
class YearlyDesignStats:
    """Per-year sufficient statistics for a given target variable."""

    year: int
    X: np.ndarray
    y: np.ndarray
    S: np.ndarray
    b: np.ndarray
    n: int
    utc_day_index: np.ndarray
    utc_hour: np.ndarray
    params_meta: List[Dict[str, int]]


@dataclass
class YearlyValidationMetrics:
    """Per-year validation diagnostics for the leave-one-year-out protocol."""

    year: int
    mse_model: float
    mse_ref: float
    rmse: float
    skill: float
    n: int

    def as_dict(self) -> Dict[str, object]:
        return {
            "year": int(self.year),
            "mse_model": float(self.mse_model),
            "mse_ref": float(self.mse_ref),
            "rmse": float(self.rmse),
            "skill": float(self.skill),
            "n": int(self.n),
        }


@dataclass
class LeaveOneYearOutReport:
    """Summary of leave-one-year-out validation for a given target."""

    years: List[YearlyValidationMetrics]
    global_rmse: float
    global_skill: float
    total_observations: int
    hyperparameters: Dict[str, object]
    ridge_lambda: float
    final_training_period: str
    protocol: str = "leave_one_year_out"
    evaluation_time_base: str = "UTC"
    model_time_base: str = "solar"
    baseline: str = "climatology_mean per (utc_day, utc_hour), LOYO"

    def to_payload(self) -> Dict[str, object]:
        return {
            "protocol": self.protocol,
            "final_training_period": self.final_training_period,
            "hyperparameters": dict(self.hyperparameters),
            "global": {
                "rmse": float(self.global_rmse),
                "skill": float(self.global_skill),
            },
            "years_validated": [entry.as_dict() for entry in self.years],
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
    validation: Optional[LeaveOneYearOutReport] = None

    def coefficients_list(self) -> List[float]:
        return [float(v) for v in self.coefficients]


@dataclass
class TrainingResult:
    """Bundle capturing linear models for temperature, specific humidity, and pressure."""

    temperature_model: LinearModelFit
    specific_humidity_model: LinearModelFit
    pressure_model: LinearModelFit


# ----------------------------- Utilities ----------------------------------


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


def solve_normal_equations(S: np.ndarray, b: np.ndarray, ridge_lambda: float) -> np.ndarray:
    """Solve (S + λI) β = b with a numerically stable fallback."""

    if S.ndim != 2 or S.shape[0] != S.shape[1]:
        raise ValueError("Matrix S must be square for normal equation solving.")
    if b.ndim != 1 or b.shape[0] != S.shape[0]:
        raise ValueError("Vector b must align with S dimensions.")

    S_reg = S.astype(float, copy=True)
    if ridge_lambda > 0.0:
        S_reg = S_reg + ridge_lambda * np.eye(S_reg.shape[0], dtype=float)

    try:
        return np.linalg.solve(S_reg, b)
    except np.linalg.LinAlgError:
        solution, *_ = np.linalg.lstsq(S_reg, b, rcond=None)
        return solution


def prepare_training_frame(df: pd.DataFrame) -> pd.DataFrame:
    """Attach solar descriptors, derived fields, and calendar metadata."""

    working = prepare_dataset(df)
    working["year"] = working["DT_UTC"].dt.year.astype("Int64")
    working = working.dropna(subset=["year"]).copy()
    working["year"] = working["year"].astype(int)
    return working


# ----------------------------- Design matrix ------------------------------


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
) -> tuple[
    np.ndarray,
    np.ndarray,
    List[Dict[str, int]],
    np.ndarray,
    np.ndarray,
]:
    """Construct the design matrix for the factorized linear model.

    UTC no-leap calendar for evaluation keys: Feb 29 samples are excluded from
    `(utc_day_index, utc_hour)`.

    Returns:
        (
            X,
            y,
            params_meta,
            utc_day_index,
            utc_hour,
        )
    """

    required_cols = {"yday_frac_solar", "hour_solar", target}
    missing = required_cols - set(df.columns)
    if missing:
        raise KeyError(f"Missing required columns for training: {sorted(missing)}")

    local = df.copy()
    local = local.dropna(subset=["yday_frac_solar", "hour_solar", target])

    local["day"] = local["yday_frac_solar"].astype(float)
    local["hour"] = np.mod(local["hour_solar"].astype(float), 24.0)

    day = local["day"].to_numpy(dtype=float)
    hour = local["hour"].to_numpy(dtype=float)
    y = local[target].to_numpy(dtype=float)

    utc_requirement_msg = (
        "LOYO evaluation runs in UTC and requires hourly UTC timestamps (`DT_UTC` at whole hours)."
    )
    if "DT_UTC" not in local.columns:
        raise KeyError(utc_requirement_msg)
    utc_series = pd.to_datetime(local["DT_UTC"], utc=True, errors="coerce")
    if utc_series.isna().any() or not utc_series.equals(utc_series.dt.floor("h")):
        raise ValueError(utc_requirement_msg)
    local["DT_UTC"] = utc_series
    month = np.asarray(utc_series.dt.month, dtype=int)
    day_of_month = np.asarray(utc_series.dt.day, dtype=int)
    hour_utc = np.asarray(utc_series.dt.hour, dtype=int)
    day_of_year = np.asarray(utc_series.dt.dayofyear, dtype=int)
    is_leap_year = np.asarray(utc_series.dt.is_leap_year, dtype=bool)

    utc_day_index = day_of_year.astype(int, copy=True)
    leap_adjust_mask = is_leap_year & (utc_day_index > 59)
    utc_day_index[leap_adjust_mask] -= 1
    feb29_mask = (month == 2) & (day_of_month == 29)
    utc_day_index[feb29_mask] = -1
    utc_hour = hour_utc.astype(int, copy=True)

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
            {"name": name, "n_annual": n_annual_param, "start": start_idx, "length": length}
        )
        start_idx += length

    X = np.concatenate(X_blocks, axis=1)
    return X, y, params_meta, utc_day_index, utc_hour


def _build_layout(meta: Iterable[Dict[str, int]]) -> List[ParameterLayout]:
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


# ----------------------------- Training helpers ---------------------------


def compute_sufficient_stats(
    df: pd.DataFrame,
    *,
    target: str,
    n_diurnal: int,
    default_n_annual: int,
    annual_per_param: Dict[str, int] | None,
) -> List[YearlyDesignStats]:
    """Compute per-year design matrices and sufficient statistics."""

    if target not in df.columns:
        raise KeyError(f"Target column '{target}' is missing from the training frame.")
    if "year" not in df.columns:
        raise KeyError("Training frame is missing the 'year' column; call prepare_training_frame.")

    annual_per_param = dict(annual_per_param or {})
    stats: List[YearlyDesignStats] = []

    for year, group in df.groupby("year"):
        if group.empty:
            continue

        (
            X,
            y,
            meta,
            utc_day_index,
            utc_hour,
        ) = build_global_linear_matrix(
            group,
            n_diurnal=n_diurnal,
            annual_per_param=annual_per_param,
            default_n_annual=default_n_annual,
            target=target,
        )
        if y.size == 0 or X.size == 0:
            continue

        X = np.asarray(X, dtype=float)
        y = np.asarray(y, dtype=float)

        S = X.T @ X
        b = X.T @ y

        stats.append(
            YearlyDesignStats(
                year=int(year),
                X=X,
                y=y,
                S=S,
                b=b,
                n=int(y.size),
                utc_day_index=utc_day_index.astype(int, copy=False),
                utc_hour=utc_hour.astype(int, copy=False),
                params_meta=[dict(entry) for entry in meta],
            )
        )

    return stats


def fit_from_stats(
    stats: List[YearlyDesignStats],
    ridge_lambda: float,
) -> Tuple[np.ndarray, ErrorMetrics, List[ParameterLayout]]:
    """Fit coefficients from sufficient statistics and return diagnostics."""

    if not stats:
        raise ValueError("No sufficient statistics supplied for fitting.")

    feature_dim = stats[0].S.shape[0]
    S_total = np.zeros((feature_dim, feature_dim), dtype=float)
    b_total = np.zeros(feature_dim, dtype=float)

    for entry in stats:
        if entry.S.shape != (feature_dim, feature_dim):
            raise ValueError("Inconsistent feature dimension across yearly statistics.")
        S_total += entry.S
        b_total += entry.b

    coefficients = solve_normal_equations(S_total, b_total, ridge_lambda)

    residuals: List[np.ndarray] = []
    for entry in stats:
        y_pred = entry.X @ coefficients
        valid_mask = np.isfinite(entry.y) & np.isfinite(y_pred)
        if np.any(valid_mask):
            residuals.append(entry.y[valid_mask] - y_pred[valid_mask])

    all_residuals = np.concatenate(residuals) if residuals else np.empty(0, dtype=float)
    metrics = _compute_error_metrics(all_residuals)
    layout = _build_layout(stats[0].params_meta)
    return coefficients.astype(float), metrics, layout


def _is_prepared(df: pd.DataFrame) -> bool:
    required = {
        "yday_frac_solar",
        "hour_solar",
        "year",
        "Q",
    }
    return required.issubset(df.columns)


def _train_target(
    df: pd.DataFrame,
    *,
    target_variable: str,
    target_unit: str,
    n_diurnal: int,
    default_n_annual: int,
    annual_per_param: Dict[str, int],
    ridge_lambda: float,
) -> LinearModelFit:
    stats = compute_sufficient_stats(
        df,
        target=target_variable,
        n_diurnal=n_diurnal,
        default_n_annual=default_n_annual,
        annual_per_param=annual_per_param,
    )
    if not stats:
        raise ValueError(f"No samples available to train target '{target_variable}'.")

    coefficients, metrics, layout = fit_from_stats(stats, ridge_lambda)

    return LinearModelFit(
        target_variable=target_variable,
        target_unit=target_unit,
        coefficients=coefficients,
        params_layout=layout,
        n_diurnal=n_diurnal,
        default_n_annual=default_n_annual,
        annual_per_param=dict(annual_per_param),
        metrics=metrics,
        validation=None,
    )


def train_models(
    df: pd.DataFrame,
    *,
    n_diurnal: int = 3,
    default_n_annual: int = 3,
    annual_per_param: Dict[str, int] | None = None,
    ridge_lambda: float = RIDGE_LAMBDA_DEFAULT,
) -> TrainingResult:
    """Model is trained on solar features; no external evaluation is performed here."""

    annual_per_param = dict(annual_per_param or {})

    working = df if _is_prepared(df) else prepare_training_frame(df)

    temperature_model = _train_target(
        working,
        target_variable="T",
        target_unit="degC",
        n_diurnal=n_diurnal,
        default_n_annual=default_n_annual,
        annual_per_param=annual_per_param,
        ridge_lambda=ridge_lambda,
    )
    specific_humidity_model = _train_target(
        working,
        target_variable="Q",
        target_unit="kg/kg",
        n_diurnal=n_diurnal,
        default_n_annual=default_n_annual,
        annual_per_param=annual_per_param,
        ridge_lambda=ridge_lambda,
    )
    pressure_model = _train_target(
        working,
        target_variable="P",
        target_unit="hPa",
        n_diurnal=n_diurnal,
        default_n_annual=default_n_annual,
        annual_per_param=annual_per_param,
        ridge_lambda=ridge_lambda,
    )

    return TrainingResult(
        temperature_model=temperature_model,
        specific_humidity_model=specific_humidity_model,
        pressure_model=pressure_model,
    )


# ----------------------------- Payload export -----------------------------


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
    if model.validation is not None:
        payload["metadata"]["training_loyo_rmse"] = model.validation.global_rmse
        payload["metadata"]["training_loyo_skill"] = model.validation.global_skill
    return payload


__all__ = [
    "FINAL_TRAINING_PERIOD_LABEL",
    "RIDGE_LAMBDA_DEFAULT",
    "ErrorMetrics",
    "LeaveOneYearOutReport",
    "LinearModelFit",
    "ParameterLayout",
    "TrainingResult",
    "YearlyDesignStats",
    "YearlyValidationMetrics",
    "build_annual_basis",
    "build_global_linear_matrix",
    "build_parameter_payload",
    "compute_sufficient_stats",
    "fit_from_stats",
    "prepare_training_frame",
    "solve_normal_equations",
    "train_models",
]
