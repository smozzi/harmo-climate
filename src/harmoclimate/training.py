"""Model definitions and training utilities for HarmoClimate."""

from __future__ import annotations

import math
from dataclasses import dataclass
from typing import Dict, Tuple

import numpy as np
import pandas as pd
import torch
from torch import nn

from .config import COUNTRY_CODE, MODEL_VERSION, RANDOM_SEED


def _to_float_state_dict(module: nn.Module) -> Dict[str, float]:
    return {k: float(v.detach().cpu().numpy()) for k, v in module.state_dict().items()}


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


class AnnualHarmonic(nn.Module):
    """Very light annual harmonic model."""

    def __init__(self, mu=10.0, A1=5.0, ph1=0.0, A2=1.0, ph2=1.0):
        super().__init__()
        self.mu = nn.Parameter(torch.tensor(mu, dtype=torch.float32))
        self.A1 = nn.Parameter(torch.tensor(A1, dtype=torch.float32))
        self.ph1 = nn.Parameter(torch.tensor(ph1, dtype=torch.float32))
        self.A2 = nn.Parameter(torch.tensor(A2, dtype=torch.float32))
        self.ph2 = nn.Parameter(torch.tensor(ph2, dtype=torch.float32))

    def forward(self, day: torch.Tensor) -> torch.Tensor:  # type: ignore[override]
        wA = 2.0 * math.pi / 365.0
        th = wA * day
        return self.mu + self.A1 * torch.cos(th - self.ph1) + self.A2 * torch.cos(2.0 * th - self.ph2)


class DiurnalConditioned(nn.Module):
    """Diurnal temperature offset model in solar time."""

    def __init__(self):
        super().__init__()
        self.a0 = nn.Parameter(torch.tensor(4.0, dtype=torch.float32))
        self.a1 = nn.Parameter(torch.tensor(1.0, dtype=torch.float32))
        self.psiA = nn.Parameter(torch.tensor(0.0, dtype=torch.float32))
        self.ph0 = nn.Parameter(torch.tensor(0.0, dtype=torch.float32))
        self.k1 = nn.Parameter(torch.tensor(0.2, dtype=torch.float32))
        self.psiP = nn.Parameter(torch.tensor(0.0, dtype=torch.float32))
        self.alpha = nn.Parameter(torch.tensor(0.2, dtype=torch.float32))

    def forward(self, day: torch.Tensor, hour: torch.Tensor) -> torch.Tensor:  # type: ignore[override]
        two_pi = 2.0 * math.pi
        wA = two_pi / 365.0
        wD = two_pi / 24.0
        thA = wA * day
        thD = wD * hour

        Aday = self.a0 + self.a1 * torch.cos(thA - self.psiA)
        phday = self.ph0 + self.k1 * torch.cos(thA - self.psiP)

        base = torch.cos(thD - phday)
        sec = torch.cos(2.0 * (thD - phday))
        return Aday * (base + self.alpha * sec)


class DiurnalHumidity(nn.Module):
    """Diurnal humidity offset conditioned by temperature residual."""

    def __init__(self):
        super().__init__()
        self.core = DiurnalConditioned()
        self.beta_c = nn.Parameter(torch.tensor(-0.05, dtype=torch.float32))

    def forward(self, day: torch.Tensor, hour: torch.Tensor, temp_res: torch.Tensor) -> torch.Tensor:  # type: ignore[override]
        base = self.core(day, hour)
        return base + self.beta_c * temp_res


def train_module(module: nn.Module, x, y, lr=5e-2, epochs=800, weight_decay=0.0) -> nn.Module:
    module.train()
    opt = torch.optim.Adam(module.parameters(), lr=lr, weight_decay=weight_decay)
    loss_fn = nn.MSELoss()
    for ep in range(1, epochs + 1):
        opt.zero_grad()
        pred = module(*x) if isinstance(x, tuple) else module(x)
        loss = loss_fn(pred, y)
        loss.backward()
        opt.step()
        if ep % 200 == 0:
            print(f"[train] epoch={ep:4d} loss={loss.item():.5f}")
    module.eval()
    return module


@dataclass
class TrainingResult:
    """Bundle capturing fitted modules, evaluation metrics, and aggregates."""

    annual_T: AnnualHarmonic
    annual_RH: AnnualHarmonic
    diurn_T: DiurnalConditioned
    diurn_RH: DiurnalHumidity
    metrics_temperature: ErrorMetrics
    metrics_humidity: ErrorMetrics
    daily_aggregate: pd.DataFrame


def _compute_error_metrics(errors: torch.Tensor) -> ErrorMetrics:
    """Compute MAE, bias, and 5/95th percentiles for a vector of errors."""

    errors_np = np.asarray(errors.detach().cpu().numpy(), dtype=np.float64)
    valid = errors_np[np.isfinite(errors_np)]
    if valid.size == 0:
        return ErrorMetrics(mae=math.nan, bias=math.nan, err_p05=math.nan, err_p95=math.nan)

    mae = float(np.mean(np.abs(valid)))
    bias = float(np.mean(valid))
    err_p05 = float(np.quantile(valid, 0.05))
    err_p95 = float(np.quantile(valid, 0.95))
    return ErrorMetrics(mae=mae, bias=bias, err_p05=err_p05, err_p95=err_p95)


def train_models(df: pd.DataFrame, random_seed: int = RANDOM_SEED) -> TrainingResult:
    """Train the four harmonic modules from the filtered dataset."""

    df = df.copy()
    df["day"] = df["yday_frac_solar"].astype(float).apply(math.floor).astype(np.float32)
    df["hour"] = df["hour_solar"].astype(np.float32)

    daily = df.groupby("day").agg(T_daily=("T_C", "mean"), RH_daily=("RH", "mean")).reset_index()

    torch.manual_seed(random_seed)
    x_day = torch.from_numpy(daily["day"].to_numpy(np.float32))
    y_Td = torch.from_numpy(daily["T_daily"].to_numpy(np.float32))
    y_RHd = torch.from_numpy(daily["RH_daily"].to_numpy(np.float32))

    annual_T = AnnualHarmonic(mu=float(y_Td.mean()), A1=5.0, ph1=0.0, A2=1.0, ph2=1.0)
    annual_T = train_module(annual_T, x_day, y_Td, lr=5e-2, epochs=1000)

    annual_RH = AnnualHarmonic(mu=float(y_RHd.mean()), A1=10.0, ph1=math.pi, A2=2.0, ph2=1.0)
    annual_RH = train_module(annual_RH, x_day, y_RHd, lr=5e-2, epochs=1000)

    all_day = torch.from_numpy(df["day"].to_numpy(np.float32))
    all_hour = torch.from_numpy(df["hour"].to_numpy(np.float32))
    T_all = torch.from_numpy(df["T_C"].to_numpy(np.float32))
    RH_all = torch.from_numpy(df["RH"].to_numpy(np.float32))

    with torch.no_grad():
        annT_all = annual_T(all_day)
        annRH_all = annual_RH(all_day)

    T_res = T_all - annT_all
    RH_res = RH_all - annRH_all

    diurn_T = DiurnalConditioned()
    diurn_T = train_module(diurn_T, (all_day, all_hour), T_res, lr=1e-2, epochs=1200, weight_decay=1e-5)

    diurn_RH = DiurnalHumidity()
    diurn_RH = _train_diurnal_humidity(diurn_RH, all_day, all_hour, T_res, RH_res, lr=1e-2, epochs=2000)

    with torch.no_grad():
        T_pred = annual_T(all_day) + diurn_T(all_day, all_hour)
        RH_pred = annual_RH(all_day) + diurn_RH(all_day, all_hour, T_res)
        temp_errors = (T_all - T_pred) * 10.0
        rh_errors = RH_all - RH_pred

    metrics_temperature = _compute_error_metrics(temp_errors)
    metrics_humidity = _compute_error_metrics(rh_errors)

    return TrainingResult(
        annual_T=annual_T,
        annual_RH=annual_RH,
        diurn_T=diurn_T,
        diurn_RH=diurn_RH,
        metrics_temperature=metrics_temperature,
        metrics_humidity=metrics_humidity,
        daily_aggregate=daily,
    )


def _train_diurnal_humidity(
    module: DiurnalHumidity,
    day: torch.Tensor,
    hour: torch.Tensor,
    temp_res: torch.Tensor,
    rh_res: torch.Tensor,
    lr: float = 1e-2,
    epochs: int = 2000,
) -> DiurnalHumidity:
    module.train()
    opt = torch.optim.Adam(module.parameters(), lr=lr)
    loss_fn = nn.MSELoss()
    for ep in range(1, epochs + 1):
        opt.zero_grad()
        pred = module(day, hour, temp_res)
        loss = loss_fn(pred, rh_res)
        loss.backward()
        opt.step()
        if ep % 200 == 0:
            print(f"[train RH] epoch={ep:4d} loss={loss.item():.5f}")
    module.eval()
    return module


def build_parameter_bundle(
    result: TrainingResult,
    metadata: Dict[str, object],
    generation_date_utc: str,
) -> Dict[str, object]:
    """Assemble the JSON-friendly payload exported by the pipeline."""

    params = {
        "metadata": {
            "version": MODEL_VERSION,
            "generated_at_utc": generation_date_utc,
            "country_code": COUNTRY_CODE,
            **metadata,
        },
        "annual_T": _to_float_state_dict(result.annual_T),
        "annual_RH": _to_float_state_dict(result.annual_RH),
        "diurn_T": _to_float_state_dict(result.diurn_T),
        "diurn_RH": _to_float_state_dict(result.diurn_RH),
    }
    return params


__all__ = [
    "ErrorMetrics",
    "AnnualHarmonic",
    "DiurnalConditioned",
    "DiurnalHumidity",
    "TrainingResult",
    "build_parameter_bundle",
    "train_models",
]
