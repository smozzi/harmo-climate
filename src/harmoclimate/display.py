"""Visualization helpers for HarmoClimate models."""

from __future__ import annotations

import json
import math
from pathlib import Path
from typing import Tuple

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

from .config import SAMPLES_PER_DAY


def load_factorized_params(json_path: Path) -> dict:
    """Load the exported parameter bundle and validate minimal metadata."""

    with open(json_path, "r", encoding="utf-8") as handle:
        params = json.load(handle)

    if "metadata" not in params:
        raise ValueError(f"{json_path.name} must contain a 'metadata' object.")

    required_keys = {
        "author",
        "version",
        "generated_at_utc",
        "country_code",
        "station_usual_name",
        "longitude_deg",
        "delta_utc_solar_h",
    }
    meta = params["metadata"]
    missing = required_keys - meta.keys()
    if missing:
        raise ValueError(f"Missing required metadata fields: {sorted(missing)}")

    return params


def predict_annual_T_solar(day_solar: float, params: dict) -> float:
    p = params["annual_T"]
    wA = 2.0 * math.pi / 365.0
    th = wA * day_solar
    return p["mu"] + p["A1"] * math.cos(th - p["ph1"]) + p["A2"] * math.cos(2.0 * th - p["ph2"])


def predict_annual_RH_solar(day_solar: float, params: dict) -> float:
    p = params["annual_RH"]
    wA = 2.0 * math.pi / 365.0
    th = wA * day_solar
    return p["mu"] + p["A1"] * math.cos(th - p["ph1"]) + p["A2"] * math.cos(2.0 * th - p["ph2"])


def predict_diurnal_T_solar(day_solar: float, hour_solar: float, params: dict) -> float:
    p = params["diurn_T"]
    two_pi = 2.0 * math.pi
    wA = two_pi / 365.0
    wD = two_pi / 24.0
    thA = wA * day_solar
    thD = wD * hour_solar
    Aday = p["a0"] + p["a1"] * math.cos(thA - p["psiA"])
    phday = p["ph0"] + p["k1"] * math.cos(thA - p["psiP"])
    base = math.cos(thD - phday)
    sec = math.cos(2.0 * (thD - phday))
    return Aday * (base + p["alpha"] * sec)


def predict_diurnal_RH_solar(day_solar: float, hour_solar: float, temp_res: float, params: dict) -> float:
    p = params["diurn_RH"]
    a0 = p["core.a0"]
    a1 = p["core.a1"]
    psiA = p["core.psiA"]
    ph0 = p["core.ph0"]
    k1 = p["core.k1"]
    psiP = p["core.psiP"]
    alpha = p["core.alpha"]

    two_pi = 2.0 * math.pi
    wA = two_pi / 365.0
    wD = two_pi / 24.0
    thA = wA * day_solar
    thD = wD * hour_solar

    Aday = a0 + a1 * math.cos(thA - psiA)
    phday = ph0 + k1 * math.cos(thA - psiP)
    base = math.cos(thD - phday)
    sec = math.cos(2.0 * (thD - phday))
    core_val = Aday * (base + alpha * sec)

    beta_c = p["beta_c"]
    return core_val + beta_c * temp_res


def climate_predict_factorized_solar(day_solar: float, hour_solar: float, params: dict) -> Tuple[float, float]:
    day_int = math.floor(day_solar)
    T_ann = predict_annual_T_solar(day_int, params)
    RH_ann = predict_annual_RH_solar(day_int, params)
    T_res = predict_diurnal_T_solar(day_int, hour_solar, params)
    RH_res = predict_diurnal_RH_solar(day_int, hour_solar, T_res, params)
    T = T_ann + T_res
    RH = max(0.0, min(100.0, RH_ann + RH_res))
    return T * 10.0, RH


def model_daily_stats_one_year_factorized(params: dict, n_days: int = 365, samples_per_day: int = SAMPLES_PER_DAY):
    hours = np.linspace(0.0, 24.0, samples_per_day, endpoint=False)
    days = np.arange(1, n_days + 1, dtype=int)

    Tmin = np.empty(n_days)
    Tmax = np.empty(n_days)
    Tavg = np.empty(n_days)
    RHmin = np.empty(n_days)
    RHmax = np.empty(n_days)
    RHavg = np.empty(n_days)

    for idx, d in enumerate(days):
        T_vals, RH_vals = [], []
        for h in hours:
            T, RH = climate_predict_factorized_solar(float(d), float(h), params)
            T_vals.append(T)
            RH_vals.append(RH)
        T_vals = np.asarray(T_vals)
        RH_vals = np.asarray(RH_vals)

        Tmin[idx] = T_vals.min()
        Tmax[idx] = T_vals.max()
        Tavg[idx] = T_vals.mean()
        RHmin[idx] = RH_vals.min()
        RHmax[idx] = RH_vals.max()
        RHavg[idx] = RH_vals.mean()

    return days, Tmin, Tmax, Tavg, RHmin, RHmax, RHavg


def load_history_from_sample_data(parquet_path: Path) -> pd.DataFrame:
    if not parquet_path.exists():
        raise FileNotFoundError(f"No historical file found: {parquet_path}")

    df = pd.read_parquet(parquet_path)

    df = df[["yday_frac_solar", "hour_solar", "T_C", "RH"]].dropna()
    df["T_C"] = df["T_C"] * 10
    df["day"] = np.floor(df["yday_frac_solar"]).astype(int)
    df.loc[df["day"] == 366, "day"] = 365
    df["hour"] = np.mod(df["hour_solar"].astype(float), 24.0)
    return df


def historical_climatology_daily(df: pd.DataFrame):
    grouped = df.groupby(["day", "hour"], as_index=False).agg(
        T_hour_mean=("T_C", "mean"),
        RH_hour_mean=("RH", "mean"),
    )
    daily = grouped.groupby("day", as_index=False).agg(
        temp_min_c=("T_hour_mean", "min"),
        temp_max_c=("T_hour_mean", "max"),
        temp_mean_c=("T_hour_mean", "mean"),
        rh_min_pct=("RH_hour_mean", "min"),
        rh_max_pct=("RH_hour_mean", "max"),
        rh_mean_pct=("RH_hour_mean", "mean"),
    )
    full = pd.DataFrame({"day": np.arange(1, 366)})
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
    plt.title(f"{station_name} — Daily temperature (factorized model)")
    plt.tight_layout()
    plt.savefig(save_path, dpi=150)
    plt.close(fig)
    print(f"Figure saved -> {save_path}")


def plot_year_humidity(days, RHmin, RHmax, RHavg, station_name: str, save_path: Path):
    fig, ax = plt.subplots(figsize=(12, 5))
    ax.fill_between(days, RHmin, RHmax, alpha=0.20, label="RH min–max (%)")
    ax.plot(days, RHavg, linewidth=1.8, label="Mean RH (%)")
    ax.set_xlabel("Day of year")
    ax.set_ylabel("Relative humidity (%)")
    ax.set_ylim(0, 100)
    ax.grid(True, which="both", linewidth=0.5, alpha=0.4)
    ax.legend(loc="upper right")
    plt.title(f"{station_name} — Daily relative humidity (factorized model)")
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
    plt.title(f"{station_name} — Temperature: factorized model vs historical mean")
    plt.tight_layout()
    plt.savefig(save_path, dpi=150)
    plt.close(fig)
    print(f"Figure saved -> {save_path}")


def plot_comparison_humidity(model_df: pd.DataFrame, hist_df: pd.DataFrame, station_name: str, save_path: Path):
    days = model_df["day"].values
    fig, ax = plt.subplots(figsize=(12, 5))
    ax.fill_between(days, model_df["rh_min_pct"], model_df["rh_max_pct"], alpha=0.18, label="Model: RH min–max")
    ax.plot(days, model_df["rh_mean_pct"], linewidth=2.0, label="Model: RH mean")
    ax.plot(days, hist_df["rh_min_pct"], linewidth=1.5, linestyle="--", label="History: RH min")
    ax.plot(days, hist_df["rh_mean_pct"], linewidth=1.5, linestyle="--", label="History: RH mean")
    ax.plot(days, hist_df["rh_max_pct"], linewidth=1.5, linestyle="--", label="History: RH max")
    ax.set_xlabel("Day of year")
    ax.set_ylabel("Relative humidity (%)")
    ax.set_ylim(0, 100)
    ax.grid(True, which="both", linewidth=0.5, alpha=0.4)
    ax.legend(loc="upper right")
    plt.title(f"{station_name} — Relative humidity: factorized model vs historical mean")
    plt.tight_layout()
    plt.savefig(save_path, dpi=150)
    plt.close(fig)
    print(f"Figure saved -> {save_path}")


def model_intraday_solar(params: dict, day_solar: int, samples_per_day: int = SAMPLES_PER_DAY):
    hours = np.linspace(0.0, 24.0, samples_per_day, endpoint=False)
    T_vals, RH_vals = [], []
    for h in hours:
        T, RH = climate_predict_factorized_solar(float(day_solar), float(h), params)
        T_vals.append(T)
        RH_vals.append(RH)
    return hours, np.array(T_vals), np.array(RH_vals)


def history_intraday_mean(hourly_hist: pd.DataFrame, day: int):
    sub = hourly_hist[hourly_hist["day"] == day].sort_values("hour")
    if sub.empty:
        return np.array([]), np.array([]), np.array([])
    return sub["hour"].to_numpy(), sub["T_hour_mean"].to_numpy(), sub["RH_hour_mean"].to_numpy()


def interactive_intraday(params: dict, hourly_hist: pd.DataFrame, station_name: str, samples_per_day: int = SAMPLES_PER_DAY):
    from ipywidgets import IntSlider, Output, VBox
    from IPython.display import display

    output = Output()
    slider = IntSlider(
        value=15,
        min=1,
        max=365,
        step=1,
        description="Solar day",
        continuous_update=True,
        style={"description_width": "80px"},
        layout={"width": "60%"},
    )

    def plot_for_day(day: int):
        h_mod, T_mod, RH_mod = model_intraday_solar(params, day, samples_per_day=samples_per_day)
        h_hist, T_hist, RH_hist = history_intraday_mean(hourly_hist, day)

        fig, (axT, axH) = plt.subplots(2, 1, figsize=(9, 6), sharex=True)
        fig.suptitle(f"{station_name} — Intraday profile (solar time) — day {day}", fontsize=12)

        axT.plot(h_mod, T_mod, label="Model: T", linewidth=2.0)
        if len(h_hist) > 0:
            axT.plot(h_hist, T_hist, "--", label="History: mean T", linewidth=1.4)
        axT.set_ylabel("Temperature (°C)")
        axT.set_xlim(0, 24)
        axT.grid(True, alpha=0.4)
        axT.legend(loc="best")

        axH.plot(h_mod, RH_mod, label="Model: RH", linewidth=2.0)
        if len(h_hist) > 0:
            axH.plot(h_hist, RH_hist, "--", label="History: mean RH", linewidth=1.4)
        axH.set_xlabel("Solar hour")
        axH.set_ylabel("Relative humidity (%)")
        axH.set_xlim(0, 24)
        axH.set_ylim(0, 100)
        axH.grid(True, alpha=0.4)
        axH.legend(loc="best")

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


__all__ = [
    "climate_predict_factorized_solar",
    "historical_climatology_daily",
    "history_intraday_mean",
    "interactive_intraday",
    "load_factorized_params",
    "load_history_from_sample_data",
    "model_daily_stats_one_year_factorized",
    "model_intraday_solar",
    "plot_comparison_humidity",
    "plot_comparison_temperature",
    "plot_year_humidity",
    "plot_year_temperature",
    "predict_annual_RH_solar",
    "predict_annual_T_solar",
    "predict_diurnal_RH_solar",
    "predict_diurnal_T_solar",
]
