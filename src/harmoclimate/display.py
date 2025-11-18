"""Visualization helpers for HarmoClimate linear harmonic models."""

from __future__ import annotations

import json
import math
from pathlib import Path
from typing import Dict, Sequence, Tuple

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from matplotlib.figure import Figure

from .config import SAMPLES_PER_DAY
from .core import SOLAR_YEAR_DAYS, load_parquet_dataset, prepare_dataset
from .psychrometrics import (
    dew_point_c_from_e,
    relative_humidity_percent_from_specific,
    vapor_partial_pressure_hpa_from_q_p,
)

MODEL_LINEWIDTH = 1.8
HISTORY_LINEWIDTH = 1.5
GRID_LINEWIDTH = 0.5
DISPLAY_VARIABLE_CHOICES = ("T", "RH", "TD", "Q", "E", "P")
DISPLAY_VARIABLE_DEFAULT = ("T", "Q", "P")


def normalize_display_variables(variables: Sequence[str] | None) -> Tuple[str, ...]:
    """Return a sanitized tuple of display variable codes in user-specified order."""

    if variables is None:
        return DISPLAY_VARIABLE_DEFAULT

    normalized: list[str] = []
    seen: set[str] = set()
    for entry in variables:
        code = str(entry).strip().upper()
        if not code:
            continue
        if code not in DISPLAY_VARIABLE_CHOICES:
            raise ValueError(
                f"Unsupported variable '{entry}'. Expected one of {', '.join(DISPLAY_VARIABLE_CHOICES)}."
            )
        if code not in seen:
            normalized.append(code)
            seen.add(code)

    if not normalized:
        return DISPLAY_VARIABLE_DEFAULT
    return tuple(normalized)


def load_linear_model(json_path: Path) -> dict:
    """Load a single-target linear model JSON and validate required metadata."""

    with open(json_path, "r", encoding="utf-8") as handle:
        payload = json.load(handle)

    if "metadata" not in payload or "model" not in payload:
        raise ValueError(f"{json_path.name} must contain 'metadata' and 'model' objects.")

    metadata = payload["metadata"]
    required_meta = {
        "author",
        "version",
        "generated_at_utc",
        "country_code",
        "station_usual_name",
        "longitude_deg",
        "delta_utc_solar_h",
        "target_variable",
        "target_unit",
    }
    missing_meta = required_meta - metadata.keys()
    if missing_meta:
        raise ValueError(f"Missing required metadata fields: {sorted(missing_meta)}")

    model = payload["model"]
    if "coefficients" not in model or "params_layout" not in model:
        raise ValueError(f"{json_path.name} must include model.coefficients and model.params_layout.")

    return payload


def _parameter_map(model_payload: dict) -> Dict[str, Tuple[np.ndarray, int]]:
    """Return a mapping of parameter name -> (coefficients, n_annual)."""

    model = model_payload["model"]
    coefficients = model.get("coefficients", [])
    layout = model.get("params_layout", [])

    mapping: Dict[str, Tuple[np.ndarray, int]] = {}
    for entry in layout:
        name = entry["name"]
        start = int(entry["start"])
        length = int(entry["length"])
        n_annual = int(entry["n_annual"])
        coeff_slice = np.asarray(coefficients[start : start + length], dtype=np.float64)
        mapping[name] = (coeff_slice, n_annual)
    return mapping


def _wrap_day(day: float) -> float:
    period = SOLAR_YEAR_DAYS
    day %= period
    if day < 0.0:
        day += period
    return day


def _wrap_hour(hour: float) -> float:
    hour %= 24.0
    if hour < 0.0:
        hour += 24.0
    return hour


def _eval_annual(coeffs: np.ndarray, n_annual: int, day: float) -> float:
    omega = 2.0 * math.pi / SOLAR_YEAR_DAYS
    value = float(coeffs[0])
    for k in range(1, n_annual + 1):
        angle = k * omega * day
        value += float(coeffs[2 * k - 1]) * math.cos(angle)
        value += float(coeffs[2 * k]) * math.sin(angle)
    return value


def predict_model_solar(model_payload: dict, day_solar: float, hour_solar: float) -> float:
    """Evaluate a linear model in solar coordinates."""

    mapping = _parameter_map(model_payload)
    if "c0" not in mapping:
        raise ValueError("Model payload does not include mandatory parameter 'c0'.")

    day = math.floor(_wrap_day(day_solar))
    max_day_index = int(math.floor(SOLAR_YEAR_DAYS))
    if day > max_day_index:
        day = max_day_index
    hour = _wrap_hour(hour_solar)

    omega_hour = 2.0 * math.pi / 24.0
    result = _eval_annual(*mapping["c0"], day)

    n_diurnal = int(model_payload["model"].get("n_diurnal", 0))
    for m in range(1, n_diurnal + 1):
        cos_name = f"a{m}"
        sin_name = f"b{m}"
        if cos_name in mapping:
            result += _eval_annual(*mapping[cos_name], day) * math.cos(m * omega_hour * hour)
        if sin_name in mapping:
            result += _eval_annual(*mapping[sin_name], day) * math.sin(m * omega_hour * hour)
    return result


def climate_predict_solar(
    day_solar: float,
    hour_solar: float,
    temperature_model: dict,
    specific_humidity_model: dict,
    pressure_model: dict,
) -> Tuple[float, float, float]:
    """Return (temperature °C, specific humidity kg/kg, pressure hPa) in solar coordinates."""

    temp_c = predict_model_solar(temperature_model, day_solar, hour_solar)
    q = predict_model_solar(specific_humidity_model, day_solar, hour_solar)
    p_hpa = predict_model_solar(pressure_model, day_solar, hour_solar)
    return temp_c, q, p_hpa


def model_daily_stats_one_year_factorized(
    temperature_model: dict,
    specific_humidity_model: dict,
    pressure_model: dict,
    n_days: int = int(math.floor(SOLAR_YEAR_DAYS)),
    samples_per_day: int = SAMPLES_PER_DAY,
) -> Tuple[
    np.ndarray,
    np.ndarray,
    np.ndarray,
    np.ndarray,
    np.ndarray,
    np.ndarray,
    np.ndarray,
    np.ndarray,
    np.ndarray,
    np.ndarray,
    np.ndarray,
    np.ndarray,
    np.ndarray,
    np.ndarray,
    np.ndarray,
    np.ndarray,
    np.ndarray,
    np.ndarray,
    np.ndarray,
    np.ndarray,
    np.ndarray,
    np.ndarray,
]:
    """Compute model-driven daily envelopes for one full year."""

    hours = np.linspace(0.0, 24.0, samples_per_day, endpoint=False)
    days = np.arange(1, n_days + 1, dtype=int)

    Tmin = np.empty(n_days)
    Tmax = np.empty(n_days)
    Tavg = np.empty(n_days)
    RHmin = np.empty(n_days)
    RHmax = np.empty(n_days)
    RHavg = np.empty(n_days)
    Tdmin = np.empty(n_days)
    Tdmax = np.empty(n_days)
    Tdavg = np.empty(n_days)
    Qmin = np.empty(n_days)
    Qmax = np.empty(n_days)
    Qavg = np.empty(n_days)
    Emin = np.empty(n_days)
    Emax = np.empty(n_days)
    Eavg = np.empty(n_days)
    Pmin = np.empty(n_days)
    Pmax = np.empty(n_days)
    Pavg = np.empty(n_days)

    for idx, d in enumerate(days):
        temps: list[float] = []
        q_vals: list[float] = []
        pressures: list[float] = []
        for h in hours:
            temp_c, q, p_hpa = climate_predict_solar(
                float(d),
                float(h),
                temperature_model,
                specific_humidity_model,
                pressure_model,
            )
            temps.append(temp_c)
            q_vals.append(q)
            pressures.append(p_hpa)
        T_array = np.asarray(temps)
        Q_array = np.asarray(q_vals)
        P_array = np.asarray(pressures)
        RH_array = relative_humidity_percent_from_specific(T_array, Q_array, P_array)
        E_array = vapor_partial_pressure_hpa_from_q_p(Q_array, P_array)
        Td_array = dew_point_c_from_e(E_array)

        Tmin[idx] = float(T_array.min())
        Tmax[idx] = float(T_array.max())
        Tavg[idx] = float(T_array.mean())
        RHmin[idx] = float(RH_array.min())
        RHmax[idx] = float(RH_array.max())
        RHavg[idx] = float(RH_array.mean())
        Tdmin[idx] = float(Td_array.min())
        Tdmax[idx] = float(Td_array.max())
        Tdavg[idx] = float(Td_array.mean())
        Qmin[idx] = float(Q_array.min())
        Qmax[idx] = float(Q_array.max())
        Qavg[idx] = float(Q_array.mean())
        Emin[idx] = float(E_array.min())
        Emax[idx] = float(E_array.max())
        Eavg[idx] = float(E_array.mean())
        Pmin[idx] = float(P_array.min())
        Pmax[idx] = float(P_array.max())
        Pavg[idx] = float(P_array.mean())

    return (
        days,
        Tmin,
        Tmax,
        Tavg,
        RHmin,
        RHmax,
        RHavg,
        Tdmin,
        Tdmax,
        Tdavg,
        Qmin,
        Qmax,
        Qavg,
        Emin,
        Emax,
        Eavg,
        Pmin,
        Pmax,
        Pavg,
    )


def load_history_from_sample_data(parquet_path: Path) -> pd.DataFrame:
    """Load the cached dataset and emit key columns for comparison plots."""

    df = load_parquet_dataset(parquet_path)
    df = prepare_dataset(
        df,
        columns=(
            "yday_frac_solar",
            "hour_solar",
            "T",
            "RH",
            "Td",
            "P",
            "Q",
            "E",
        ),
    )
    df = df.dropna(
        subset=[
            "yday_frac_solar",
            "hour_solar",
            "T",
            "RH",
            "Td",
            "P",
            "Q",
            "E",
        ],
    )
    max_day_index = int(math.floor(SOLAR_YEAR_DAYS))
    df["day"] = np.floor(df["yday_frac_solar"]).astype(int)
    df.loc[df["day"] > max_day_index, "day"] = max_day_index
    df["hour"] = np.mod(df["hour_solar"].astype(float), 24.0)
    return df


def historical_climatology_daily(df: pd.DataFrame):
    """Aggregate historical daily envelopes for temperature, RH, Td, Q, E, and pressure."""

    grouped = df.groupby(["day", "hour"], as_index=False).agg(
        T_hour_mean=("T", "mean"),
        RH_hour_mean=("RH", "mean"),
        Td_hour_mean=("Td", "mean"),
        Q_hour_mean=("Q", "mean"),
        E_hour_mean=("E", "mean"),
        P_hour_mean=("P", "mean"),
    )
    daily = grouped.groupby("day", as_index=False).agg(
        temp_min_c=("T_hour_mean", "min"),
        temp_max_c=("T_hour_mean", "max"),
        temp_mean_c=("T_hour_mean", "mean"),
        rh_min_percent=("RH_hour_mean", "min"),
        rh_max_percent=("RH_hour_mean", "max"),
        rh_mean_percent=("RH_hour_mean", "mean"),
        td_min_c=("Td_hour_mean", "min"),
        td_max_c=("Td_hour_mean", "max"),
        td_mean_c=("Td_hour_mean", "mean"),
        q_min_kg_kg=("Q_hour_mean", "min"),
        q_max_kg_kg=("Q_hour_mean", "max"),
        q_mean_kg_kg=("Q_hour_mean", "mean"),
        e_min_hpa=("E_hour_mean", "min"),
        e_max_hpa=("E_hour_mean", "max"),
        e_mean_hpa=("E_hour_mean", "mean"),
        p_min_hpa=("P_hour_mean", "min"),
        p_max_hpa=("P_hour_mean", "max"),
        p_mean_hpa=("P_hour_mean", "mean"),
    )
    full = pd.DataFrame({"day": np.arange(1, int(math.floor(SOLAR_YEAR_DAYS)) + 1)})
    daily = full.merge(daily, on="day", how="left").interpolate(limit_direction="both")
    return daily, grouped


def plot_year(
    days: np.ndarray,
    Tmin: np.ndarray,
    Tmax: np.ndarray,
    Tavg: np.ndarray,
    RHmin: np.ndarray,
    RHmax: np.ndarray,
    RHavg: np.ndarray,
    Tdmin: np.ndarray,
    Tdmax: np.ndarray,
    Tdavg: np.ndarray,
    Qmin: np.ndarray,
    Qmax: np.ndarray,
    Qavg: np.ndarray,
    Emin: np.ndarray,
    Emax: np.ndarray,
    Eavg: np.ndarray,
    Pmin: np.ndarray,
    Pmax: np.ndarray,
    Pavg: np.ndarray,
    station_name: str,
    save_path: Path | str | None = None,
    hist_daily: pd.DataFrame | None = None,
    *,
    variables: Sequence[str] | None = None,
) -> Path | Figure:
    """Render annual envelopes for selected variables and optional history overlays."""

    enabled = normalize_display_variables(variables)
    axes_count = len(enabled)
    height = 4.0 + 1.8 * axes_count
    fig, axes = plt.subplots(axes_count, 1, figsize=(12, height), sharex=True)
    axes_list = [axes] if axes_count == 1 else list(axes)
    axis_map = {code: axes_list[idx] for idx, code in enumerate(enabled)}
    fig.suptitle(f"{station_name} — Annual model envelopes", fontsize=14)

    hist_sorted = None
    if hist_daily is not None and not hist_daily.empty:
        hist_sorted = hist_daily.sort_values("day")
        hist_temp_min = hist_sorted["temp_min_c"].to_numpy(dtype=float)
        hist_temp_max = hist_sorted["temp_max_c"].to_numpy(dtype=float)
        hist_temp_mean = hist_sorted["temp_mean_c"].to_numpy(dtype=float)
        hist_rh_min = hist_sorted["rh_min_percent"].to_numpy(dtype=float)
        hist_rh_max = hist_sorted["rh_max_percent"].to_numpy(dtype=float)
        hist_rh_mean = hist_sorted["rh_mean_percent"].to_numpy(dtype=float)
        hist_td_min = hist_sorted["td_min_c"].to_numpy(dtype=float)
        hist_td_max = hist_sorted["td_max_c"].to_numpy(dtype=float)
        hist_td_mean = hist_sorted["td_mean_c"].to_numpy(dtype=float)
        hist_q_min = hist_sorted["q_min_kg_kg"].to_numpy(dtype=float) * 1_000.0
        hist_q_max = hist_sorted["q_max_kg_kg"].to_numpy(dtype=float) * 1_000.0
        hist_q_mean = hist_sorted["q_mean_kg_kg"].to_numpy(dtype=float) * 1_000.0
        hist_e_min = hist_sorted["e_min_hpa"].to_numpy(dtype=float)
        hist_e_max = hist_sorted["e_max_hpa"].to_numpy(dtype=float)
        hist_e_mean = hist_sorted["e_mean_hpa"].to_numpy(dtype=float)
        hist_p_min = hist_sorted["p_min_hpa"].to_numpy(dtype=float)
        hist_p_max = hist_sorted["p_max_hpa"].to_numpy(dtype=float)
        hist_p_mean = hist_sorted["p_mean_hpa"].to_numpy(dtype=float)

    for code in enabled:
        ax = axis_map[code]
        ax.grid(True, which="both", linewidth=GRID_LINEWIDTH, alpha=0.4)
        if code == "T":
            ax.fill_between(days, Tmin, Tmax, alpha=0.20, label="Model (min-max)")
            ax.plot(days, Tavg, linewidth=MODEL_LINEWIDTH, label="Model mean")
            ax.set_ylabel("(°C)")
            ax.set_title("Temperature", loc="center", fontsize=11)
            if hist_sorted is not None:
                ax.fill_between(days, hist_temp_min, hist_temp_max, alpha=0.10, color="gray", label="History (min-max)")
                ax.plot(
                    days,
                    hist_temp_mean,
                    linewidth=HISTORY_LINEWIDTH,
                    linestyle="--",
                    color="black",
                    label="History mean",
                )
        elif code == "RH":
            RHmin_pct = np.asarray(RHmin, dtype=float)
            RHmax_pct = np.asarray(RHmax, dtype=float)
            RHavg_pct = np.asarray(RHavg, dtype=float)
            ax.fill_between(days, RHmin_pct, RHmax_pct, alpha=0.20, label="Model (min-max)")
            ax.plot(days, RHavg_pct, linewidth=MODEL_LINEWIDTH, label="Model mean")
            ax.set_ylabel("(%)")
            ax.set_title("Relative humidity", loc="center", fontsize=11)
            if hist_sorted is not None:
                ax.fill_between(days, hist_rh_min, hist_rh_max, alpha=0.10, color="gray", label="History (min-max)")
                ax.plot(
                    days,
                    hist_rh_mean,
                    linewidth=HISTORY_LINEWIDTH,
                    linestyle="--",
                    color="black",
                    label="History mean",
                )
        elif code == "TD":
            Tdmin_c = np.asarray(Tdmin, dtype=float)
            Tdmax_c = np.asarray(Tdmax, dtype=float)
            Tdavg_c = np.asarray(Tdavg, dtype=float)
            ax.fill_between(days, Tdmin_c, Tdmax_c, alpha=0.20, label="Model (min-max)")
            ax.plot(days, Tdavg_c, linewidth=MODEL_LINEWIDTH, label="Model mean")
            ax.set_ylabel("(°C)")
            ax.set_title("Dew point", loc="center", fontsize=11)
            if hist_sorted is not None:
                ax.fill_between(days, hist_td_min, hist_td_max, alpha=0.10, color="gray", label="History (min-max)")
                ax.plot(
                    days,
                    hist_td_mean,
                    linewidth=HISTORY_LINEWIDTH,
                    linestyle="--",
                    color="black",
                    label="History mean",
                )
        elif code == "Q":
            Qmin_gkg = np.asarray(Qmin, dtype=float) * 1_000.0
            Qmax_gkg = np.asarray(Qmax, dtype=float) * 1_000.0
            Qavg_gkg = np.asarray(Qavg, dtype=float) * 1_000.0
            ax.fill_between(days, Qmin_gkg, Qmax_gkg, alpha=0.20, label="Model (min-max)")
            ax.plot(days, Qavg_gkg, linewidth=MODEL_LINEWIDTH, label="Model mean")
            ax.set_ylabel("(g/kg)")
            ax.set_title("Specific humidity", loc="center", fontsize=11)
            if hist_sorted is not None:
                ax.fill_between(days, hist_q_min, hist_q_max, alpha=0.10, color="gray", label="History (min-max)")
                ax.plot(
                    days,
                    hist_q_mean,
                    linewidth=HISTORY_LINEWIDTH,
                    linestyle="--",
                    color="black",
                    label="History mean",
                )
        elif code == "E":
            Emin_hpa = np.asarray(Emin, dtype=float)
            Emax_hpa = np.asarray(Emax, dtype=float)
            Eavg_hpa = np.asarray(Eavg, dtype=float)
            ax.fill_between(days, Emin_hpa, Emax_hpa, alpha=0.20, label="Model (min-max)")
            ax.plot(days, Eavg_hpa, linewidth=MODEL_LINEWIDTH, label="Model mean")
            ax.set_ylabel("(hPa)")
            ax.set_title("Vapor pressure", loc="center", fontsize=11)
            if hist_sorted is not None:
                ax.fill_between(days, hist_e_min, hist_e_max, alpha=0.10, color="gray", label="History (min-max)")
                ax.plot(
                    days,
                    hist_e_mean,
                    linewidth=HISTORY_LINEWIDTH,
                    linestyle="--",
                    color="black",
                    label="History mean",
                )
        elif code == "P":
            ax.fill_between(days, Pmin, Pmax, alpha=0.20, label="Model (min-max)")
            ax.plot(days, Pavg, linewidth=MODEL_LINEWIDTH, label="Model mean")
            ax.set_ylabel("(hPa)")
            ax.set_title("Pressure", loc="center", fontsize=11)
            if hist_sorted is not None:
                ax.fill_between(days, hist_p_min, hist_p_max, alpha=0.10, color="gray", label="History (min-max)")
                ax.plot(
                    days,
                    hist_p_mean,
                    linewidth=HISTORY_LINEWIDTH,
                    linestyle="--",
                    color="black",
                    label="History mean",
                )
        ax.legend(loc="upper right")

    axes_list[-1].set_xlabel("Day of year")
    plt.tight_layout(rect=(0, 0, 1, 0.97))

    if save_path is not None:
        save_path = Path(save_path)
        save_path.parent.mkdir(parents=True, exist_ok=True)
        fig.savefig(save_path, dpi=150)
        plt.close(fig)
        print(f"Figure saved -> {save_path}")
        return save_path

    plt.show()
    return fig


def model_intraday_solar(
    temperature_model: dict,
    specific_humidity_model: dict,
    pressure_model: dict,
    day_solar: int,
    samples_per_day: int = SAMPLES_PER_DAY,
):
    hours = np.linspace(0.0, 24.0, samples_per_day, endpoint=False)
    temps, qs, ps = [], [], []
    for h in hours:
        temp_c, q, p = climate_predict_solar(
            float(day_solar),
            float(h),
            temperature_model,
            specific_humidity_model,
            pressure_model,
        )
        temps.append(temp_c)
        qs.append(q)
        ps.append(p)
    temps_arr = np.asarray(temps, dtype=float)
    qs_arr = np.asarray(qs, dtype=float)
    ps_arr = np.asarray(ps, dtype=float)
    e_arr = vapor_partial_pressure_hpa_from_q_p(qs_arr, ps_arr)
    td_arr = dew_point_c_from_e(e_arr)
    rh_arr = relative_humidity_percent_from_specific(temps_arr, qs_arr, ps_arr)
    return hours, temps_arr, qs_arr, ps_arr, rh_arr, td_arr, e_arr


def history_intraday_mean(hourly_hist: pd.DataFrame, day: int):
    sub = hourly_hist[hourly_hist["day"] == day].sort_values("hour")
    if sub.empty:
        return (
            np.array([]),
            np.array([]),
            np.array([]),
            np.array([]),
            np.array([]),
            np.array([]),
            np.array([]),
        )
    return (
        sub["hour"].to_numpy(),
        sub["T_hour_mean"].to_numpy(),
        sub["RH_hour_mean"].to_numpy(),
        sub["Td_hour_mean"].to_numpy(),
        sub["Q_hour_mean"].to_numpy(),
        sub["E_hour_mean"].to_numpy(),
        sub["P_hour_mean"].to_numpy(),
    )


def plot_intraday(
    temperature_model: dict,
    specific_humidity_model: dict,
    pressure_model: dict,
    hourly_hist: pd.DataFrame | None,
    station_name: str,
    samples_per_day: int = SAMPLES_PER_DAY,
    *,
    day: int = 15,
    save_path: Path | str | None = None,
    variables: Sequence[str] | None = None,
):
    """Render a static intraday plot for a specific solar day.

    When ``save_path`` is provided, the figure is written to disk and the resulting
    ``Path`` is returned; otherwise a Matplotlib figure is shown and returned.
    """

    max_day_index = int(math.floor(SOLAR_YEAR_DAYS))
    if day < 1 or day > max_day_index:
        raise ValueError(f"day must be within [1, {max_day_index}] (received {day}).")

    h_mod, T_mod, Q_mod, P_mod, RH_mod, Td_mod, E_mod = model_intraday_solar(
        temperature_model,
        specific_humidity_model,
        pressure_model,
        day,
        samples_per_day=samples_per_day,
    )

    h_hist = np.array([])
    T_hist = np.array([])
    RH_hist = np.array([])
    Td_hist = np.array([])
    Q_hist = np.array([])
    E_hist = np.array([])
    P_hist = np.array([])
    if hourly_hist is not None:
        h_hist, T_hist, RH_hist, Td_hist, Q_hist, E_hist, P_hist = history_intraday_mean(hourly_hist, day)

    enabled = normalize_display_variables(variables)
    axes_count = len(enabled)
    height = 4.0 + 1.5 * axes_count
    fig, axes = plt.subplots(axes_count, 1, figsize=(9, height), sharex=True)
    axes_list = [axes] if axes_count == 1 else list(axes)
    axis_map = {code: axes_list[idx] for idx, code in enumerate(enabled)}
    fig.suptitle(f"{station_name} — Intraday profile (solar time) — day {day}", fontsize=12)

    Q_mod_gkg = Q_mod * 1_000.0
    Q_hist_gkg = Q_hist * 1_000.0 if len(h_hist) > 0 else Q_hist

    for code in enabled:
        ax = axis_map[code]
        ax.grid(True, linewidth=GRID_LINEWIDTH, alpha=0.4)
        ax.set_xlim(0, 24)
        if code == "T":
            ax.plot(h_mod, T_mod, label="Model", linewidth=MODEL_LINEWIDTH)
            if len(h_hist) > 0:
                ax.plot(h_hist, T_hist, "--", label="History mean", linewidth=HISTORY_LINEWIDTH, color="black")
            ax.set_ylabel("(°C)")
            ax.set_title("Temperature", loc="center", fontsize=11)
        elif code == "RH":
            ax.plot(h_mod, RH_mod, label="Model", linewidth=MODEL_LINEWIDTH)
            if len(h_hist) > 0:
                ax.plot(h_hist, RH_hist, "--", label="History mean", linewidth=HISTORY_LINEWIDTH, color="black")
            ax.set_ylabel("(%)")
            ax.set_title("Relative humidity", loc="center", fontsize=11)
        elif code == "TD":
            ax.plot(h_mod, Td_mod, label="Model", linewidth=MODEL_LINEWIDTH)
            if len(h_hist) > 0:
                ax.plot(h_hist, Td_hist, "--", label="History mean", linewidth=HISTORY_LINEWIDTH, color="black")
            ax.set_ylabel("(°C)")
            ax.set_title("Dew point", loc="center", fontsize=11)
        elif code == "Q":
            ax.plot(h_mod, Q_mod_gkg, label="Model", linewidth=MODEL_LINEWIDTH)
            if len(h_hist) > 0:
                ax.plot(h_hist, Q_hist_gkg, "--", label="History mean", linewidth=HISTORY_LINEWIDTH, color="black")
            ax.set_ylabel("(g/kg)")
            ax.set_title("Specific humidity", loc="center", fontsize=11)
        elif code == "E":
            ax.plot(h_mod, E_mod, label="Model", linewidth=MODEL_LINEWIDTH)
            if len(h_hist) > 0:
                ax.plot(h_hist, E_hist, "--", label="History mean", linewidth=HISTORY_LINEWIDTH, color="black")
            ax.set_ylabel("(hPa)")
            ax.set_title("Vapor pressure", loc="center", fontsize=11)
        elif code == "P":
            ax.plot(h_mod, P_mod, label="Model", linewidth=MODEL_LINEWIDTH)
            if len(h_hist) > 0:
                ax.plot(h_hist, P_hist, "--", label="History mean", linewidth=HISTORY_LINEWIDTH, color="black")
            ax.set_ylabel("(hPa)")
            ax.set_title("Pressure", loc="center", fontsize=11)
        ax.legend(loc="best")

    axes_list[-1].set_xlabel("Solar hour")
    plt.tight_layout()

    if save_path is not None:
        save_path = Path(save_path)
        save_path.parent.mkdir(parents=True, exist_ok=True)
        fig.savefig(save_path, dpi=150)
        plt.close(fig)
        print(f"Figure saved -> {save_path}")
        return save_path

    plt.show()
    return fig

__all__ = [
    "DISPLAY_VARIABLE_CHOICES",
    "DISPLAY_VARIABLE_DEFAULT",
    "climate_predict_solar",
    "historical_climatology_daily",
    "history_intraday_mean",
    "load_linear_model",
    "load_history_from_sample_data",
    "model_daily_stats_one_year_factorized",
    "model_intraday_solar",
    "normalize_display_variables",
    "plot_intraday",
    "plot_year",
    "predict_model_solar",
]
