"""Visualization helpers for HarmoClimate linear harmonic models."""

from __future__ import annotations

import json
import math
from pathlib import Path
from typing import Dict, Tuple

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

from .config import SAMPLES_PER_DAY
from .core import SOLAR_YEAR_DAYS, compute_solar_time, specific_humidity_kg_per_kg


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


def climate_predict_utc(
    day_utc: float,
    hour_utc: float,
    temperature_model: dict,
    specific_humidity_model: dict,
    pressure_model: dict,
    delta_utc_solar_h: float,
) -> Tuple[float, float, float]:
    """Return (temperature °C, specific humidity kg/kg, pressure hPa) using UTC coordinates."""

    hour_solar = _wrap_hour(hour_utc + delta_utc_solar_h)
    day_solar = _wrap_day(day_utc + (delta_utc_solar_h / 24.0))
    return climate_predict_solar(
        day_solar,
        hour_solar,
        temperature_model,
        specific_humidity_model,
        pressure_model,
    )


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
]:
    """Compute model-driven daily envelopes for one full year."""

    hours = np.linspace(0.0, 24.0, samples_per_day, endpoint=False)
    days = np.arange(1, n_days + 1, dtype=int)

    Tmin = np.empty(n_days)
    Tmax = np.empty(n_days)
    Tavg = np.empty(n_days)
    Qmin = np.empty(n_days)
    Qmax = np.empty(n_days)
    Qavg = np.empty(n_days)
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

        Tmin[idx] = float(T_array.min())
        Tmax[idx] = float(T_array.max())
        Tavg[idx] = float(T_array.mean())
        Qmin[idx] = float(Q_array.min())
        Qmax[idx] = float(Q_array.max())
        Qavg[idx] = float(Q_array.mean())
        Pmin[idx] = float(P_array.min())
        Pmax[idx] = float(P_array.max())
        Pavg[idx] = float(P_array.mean())

    return days, Tmin, Tmax, Tavg, Qmin, Qmax, Qavg, Pmin, Pmax, Pavg


def load_history_from_sample_data(parquet_path: Path) -> pd.DataFrame:
    """Load the cached dataset and emit key columns for comparison plots."""

    if not parquet_path.exists():
        raise FileNotFoundError(f"No historical file found: {parquet_path}")

    df = pd.read_parquet(parquet_path)
    solar_time = compute_solar_time(df["DT_UTC"], df["LON"])
    df = pd.concat([df, solar_time], axis=1)
    df = df[["yday_frac_solar", "hour_solar", "T", "RH", "P"]].dropna()
    df["Q"] = specific_humidity_kg_per_kg(df["T"], df["RH"], df["P"])
    max_day_index = int(math.floor(SOLAR_YEAR_DAYS))
    df["day"] = np.floor(df["yday_frac_solar"]).astype(int)
    df.loc[df["day"] > max_day_index, "day"] = max_day_index
    df["hour"] = np.mod(df["hour_solar"].astype(float), 24.0)
    return df


def historical_climatology_daily(df: pd.DataFrame):
    """Aggregate historical daily envelopes for temperature, specific humidity, and pressure."""

    grouped = df.groupby(["day", "hour"], as_index=False).agg(
        T_hour_mean=("T", "mean"),
        Q_hour_mean=("Q", "mean"),
        P_hour_mean=("P", "mean"),
    )
    daily = grouped.groupby("day", as_index=False).agg(
        temp_min_c=("T_hour_mean", "min"),
        temp_max_c=("T_hour_mean", "max"),
        temp_mean_c=("T_hour_mean", "mean"),
        q_min_kg_kg=("Q_hour_mean", "min"),
        q_max_kg_kg=("Q_hour_mean", "max"),
        q_mean_kg_kg=("Q_hour_mean", "mean"),
        p_min_hpa=("P_hour_mean", "min"),
        p_max_hpa=("P_hour_mean", "max"),
        p_mean_hpa=("P_hour_mean", "mean"),
    )
    full = pd.DataFrame({"day": np.arange(1, int(math.floor(SOLAR_YEAR_DAYS)) + 1)})
    daily = full.merge(daily, on="day", how="left").interpolate(limit_direction="both")
    return daily, grouped


def plot_year_temperature(days, Tmin, Tmax, Tavg, station_name: str, save_path: Path):
    fig, ax = plt.subplots(figsize=(12, 5))
    ax.fill_between(days, Tmin, Tmax, alpha=0.20, label="Temperature min–max (°C)")
    ax.plot(days, Tavg, linewidth=1.8, label="Mean temperature (°C)")
    ax.set_xlabel("Day of year")
    ax.set_ylabel("Temperature (°C)")
    ax.grid(True, which="both", linewidth=0.5, alpha=0.4)
    ax.legend(loc="upper right")
    plt.title(f"{station_name} — Daily temperature (linear model)")
    plt.tight_layout()
    plt.savefig(save_path, dpi=150)
    plt.close(fig)
    print(f"Figure saved -> {save_path}")


def plot_year_specific_humidity(days, Qmin, Qmax, Qavg, station_name: str, save_path: Path):
    Qmin_gkg = np.asarray(Qmin, dtype=float) * 1_000.0
    Qmax_gkg = np.asarray(Qmax, dtype=float) * 1_000.0
    Qavg_gkg = np.asarray(Qavg, dtype=float) * 1_000.0

    fig, ax = plt.subplots(figsize=(12, 5))
    ax.fill_between(days, Qmin_gkg, Qmax_gkg, alpha=0.20, label="Specific humidity min–max (g/kg)")
    ax.plot(days, Qavg_gkg, linewidth=1.8, label="Mean specific humidity (g/kg)")
    ax.set_xlabel("Day of year")
    ax.set_ylabel("Specific humidity (g/kg)")
    ax.grid(True, which="both", linewidth=0.5, alpha=0.4)
    ax.legend(loc="upper right")
    plt.title(f"{station_name} — Daily specific humidity (linear model)")
    plt.tight_layout()
    plt.savefig(save_path, dpi=150)
    plt.close(fig)
    print(f"Figure saved -> {save_path}")


def plot_year_pressure(days, Pmin, Pmax, Pavg, station_name: str, save_path: Path):
    fig, ax = plt.subplots(figsize=(12, 5))
    ax.fill_between(days, Pmin, Pmax, alpha=0.20, label="Pressure min–max (hPa)")
    ax.plot(days, Pavg, linewidth=1.8, label="Mean pressure (hPa)")
    ax.set_xlabel("Day of year")
    ax.set_ylabel("Pressure (hPa)")
    ax.grid(True, which="both", linewidth=0.5, alpha=0.4)
    ax.legend(loc="upper right")
    plt.title(f"{station_name} — Daily pressure (linear model)")
    plt.tight_layout()
    plt.savefig(save_path, dpi=150)
    plt.close(fig)
    print(f"Figure saved -> {save_path}")


def plot_comparison_temperature(model_df: pd.DataFrame, hist_df: pd.DataFrame, station_name: str, save_path: Path):
    days = model_df["day"].values
    fig, ax = plt.subplots(figsize=(12, 5))
    ax.fill_between(days, model_df["temp_min_c"], model_df["temp_max_c"], alpha=0.18, label="Model: T min–max")
    ax.plot(days, model_df["temp_mean_c"], linewidth=2.0, label="Model: T mean")
    ax.plot(days, hist_df["temp_min_c"], linewidth=1.5, linestyle="--", label="History: T min")
    ax.plot(days, hist_df["temp_mean_c"], linewidth=1.5, linestyle="--", label="History: T mean")
    ax.plot(days, hist_df["temp_max_c"], linewidth=1.5, linestyle="--", label="History: T max")
    ax.set_xlabel("Day of year")
    ax.set_ylabel("Temperature (°C)")
    ax.grid(True, which="both", linewidth=0.5, alpha=0.4)
    ax.legend(loc="upper right")
    plt.title(f"{station_name} — Temperature: model vs historical mean")
    plt.tight_layout()
    plt.savefig(save_path, dpi=150)
    plt.close(fig)
    print(f"Figure saved -> {save_path}")


def plot_comparison_specific_humidity(model_df: pd.DataFrame, hist_df: pd.DataFrame, station_name: str, save_path: Path):
    days = model_df["day"].values
    model_min = np.asarray(model_df["q_min_kg_kg"], dtype=float) * 1_000.0
    model_max = np.asarray(model_df["q_max_kg_kg"], dtype=float) * 1_000.0
    model_mean = np.asarray(model_df["q_mean_kg_kg"], dtype=float) * 1_000.0
    hist_min = np.asarray(hist_df["q_min_kg_kg"], dtype=float) * 1_000.0
    hist_max = np.asarray(hist_df["q_max_kg_kg"], dtype=float) * 1_000.0
    hist_mean = np.asarray(hist_df["q_mean_kg_kg"], dtype=float) * 1_000.0

    fig, ax = plt.subplots(figsize=(12, 5))
    ax.fill_between(days, model_min, model_max, alpha=0.18, label="Model: Q min–max")
    ax.plot(days, model_mean, linewidth=2.0, label="Model: Q mean")
    ax.plot(days, hist_min, linewidth=1.5, linestyle="--", label="History: Q min")
    ax.plot(days, hist_mean, linewidth=1.5, linestyle="--", label="History: Q mean")
    ax.plot(days, hist_max, linewidth=1.5, linestyle="--", label="History: Q max")
    ax.set_xlabel("Day of year")
    ax.set_ylabel("Specific humidity (g/kg)")
    ax.grid(True, which="both", linewidth=0.5, alpha=0.4)
    ax.legend(loc="upper right")
    plt.title(f"{station_name} — Specific humidity: model vs historical mean")
    plt.tight_layout()
    plt.savefig(save_path, dpi=150)
    plt.close(fig)
    print(f"Figure saved -> {save_path}")


def plot_comparison_pressure(model_df: pd.DataFrame, hist_df: pd.DataFrame, station_name: str, save_path: Path):
    days = model_df["day"].values
    fig, ax = plt.subplots(figsize=(12, 5))
    ax.fill_between(days, model_df["p_min_hpa"], model_df["p_max_hpa"], alpha=0.18, label="Model: P min–max")
    ax.plot(days, model_df["p_mean_hpa"], linewidth=2.0, label="Model: P mean")
    ax.plot(days, hist_df["p_min_hpa"], linewidth=1.5, linestyle="--", label="History: P min")
    ax.plot(days, hist_df["p_mean_hpa"], linewidth=1.5, linestyle="--", label="History: P mean")
    ax.plot(days, hist_df["p_max_hpa"], linewidth=1.5, linestyle="--", label="History: P max")
    ax.set_xlabel("Day of year")
    ax.set_ylabel("Pressure (hPa)")
    ax.grid(True, which="both", linewidth=0.5, alpha=0.4)
    ax.legend(loc="upper right")
    plt.title(f"{station_name} — Pressure: model vs historical mean")
    plt.tight_layout()
    plt.savefig(save_path, dpi=150)
    plt.close(fig)
    print(f"Figure saved -> {save_path}")


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
    return hours, np.array(temps), np.array(qs), np.array(ps)


def history_intraday_mean(hourly_hist: pd.DataFrame, day: int):
    sub = hourly_hist[hourly_hist["day"] == day].sort_values("hour")
    if sub.empty:
        return np.array([]), np.array([]), np.array([]), np.array([])
    return (
        sub["hour"].to_numpy(),
        sub["T_hour_mean"].to_numpy(),
        sub["Q_hour_mean"].to_numpy(),
        sub["P_hour_mean"].to_numpy(),
    )


def interactive_intraday(
    temperature_model: dict,
    specific_humidity_model: dict,
    pressure_model: dict,
    hourly_hist: pd.DataFrame,
    station_name: str,
    samples_per_day: int = SAMPLES_PER_DAY,
):
    from ipywidgets import IntSlider, Output, VBox
    from IPython.display import display

    output = Output()
    slider = IntSlider(
        value=15,
        min=1,
        max=int(math.floor(SOLAR_YEAR_DAYS)),
        step=1,
        description="Solar day",
        continuous_update=True,
        style={"description_width": "80px"},
        layout={"width": "60%"},
    )

    def plot_for_day(day: int):
        h_mod, T_mod, Q_mod, P_mod = model_intraday_solar(
            temperature_model,
            specific_humidity_model,
            pressure_model,
            day,
            samples_per_day=samples_per_day,
        )
        h_hist, T_hist, Q_hist, P_hist = history_intraday_mean(hourly_hist, day)

        fig, (axT, axQ, axP) = plt.subplots(3, 1, figsize=(9, 8), sharex=True)
        fig.suptitle(f"{station_name} — Intraday profile (solar time) — day {day}", fontsize=12)

        axT.plot(h_mod, T_mod, label="Model: T", linewidth=2.0)
        if len(h_hist) > 0:
            axT.plot(h_hist, T_hist, "--", label="History: mean T", linewidth=1.4)
        axT.set_ylabel("Temperature (°C)")
        axT.set_xlim(0, 24)
        axT.grid(True, alpha=0.4)
        axT.legend(loc="best")

        Q_mod_gkg = Q_mod * 1_000.0
        axQ.plot(h_mod, Q_mod_gkg, label="Model: Q", linewidth=2.0)
        if len(h_hist) > 0:
            axQ.plot(h_hist, Q_hist * 1_000.0, "--", label="History: mean Q", linewidth=1.4)
        axQ.set_ylabel("Specific humidity (g/kg)")
        axQ.set_xlim(0, 24)
        axQ.grid(True, alpha=0.4)
        axQ.legend(loc="best")

        axP.plot(h_mod, P_mod, label="Model: P", linewidth=2.0)
        if len(h_hist) > 0:
            axP.plot(h_hist, P_hist, "--", label="History: mean P", linewidth=1.4)
        axP.set_xlabel("Solar hour")
        axP.set_ylabel("Pressure (hPa)")
        axP.set_xlim(0, 24)
        axP.grid(True, alpha=0.4)
        axP.legend(loc="best")

        plt.tight_layout()
        plt.show()

    def on_change(change):
        if change["name"] == "value":
            day = change["new"]
            with output:
                output.clear_output(wait=True)
                plot_for_day(day)

    slider.observe(on_change)

    with output:
        plot_for_day(slider.value)

    display(VBox([slider, output]))



# Backwards-compatible aliases ------------------------------------------------
load_factorized_params = load_linear_model
plot_year_absolute_humidity = plot_year_specific_humidity
plot_comparison_absolute_humidity = plot_comparison_specific_humidity
plot_year_humidity = plot_year_specific_humidity
plot_comparison_humidity = plot_comparison_specific_humidity


__all__ = [
    "climate_predict_solar",
    "climate_predict_utc",
    "historical_climatology_daily",
    "history_intraday_mean",
    "interactive_intraday",
    "load_linear_model",
    "load_factorized_params",
    "load_history_from_sample_data",
    "model_daily_stats_one_year_factorized",
    "model_intraday_solar",
    "plot_comparison_specific_humidity",
    "plot_comparison_pressure",
    "plot_comparison_temperature",
    "plot_comparison_absolute_humidity",
    "plot_comparison_humidity",
    "plot_year_specific_humidity",
    "plot_year_pressure",
    "plot_year_temperature",
    "plot_year_absolute_humidity",
    "plot_year_humidity",
    "predict_model_solar",
]
