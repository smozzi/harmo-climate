"""Evaluation protocols for HarmoClimate linear models.

Leave-one-year-out (LOYO) validation now evaluates on a no-leap UTC grid while
models remain trained on the solar feature space. The climatology baseline
averages each (utc_day_of_year, utc_hour) bucket excluding the held-out year
and aggregates errors using observation-count weighting.
"""

from __future__ import annotations

import math
from collections import defaultdict
from typing import Dict, List, Tuple

import numpy as np

from .training import (
    FINAL_TRAINING_PERIOD_LABEL,
    LeaveOneYearOutReport,
    YearlyDesignStats,
    YearlyValidationMetrics,
    solve_normal_equations,
)


def accumulate_climatology_maps(
    stats: List[YearlyDesignStats],
) -> Tuple[
    Dict[Tuple[int, int], float],
    Dict[Tuple[int, int], int],
    Dict[int, Dict[Tuple[int, int], float]],
    Dict[int, Dict[Tuple[int, int], int]],
]:
    """Accumulate per-grid sums/counts for the UTC climatology baseline.

    Returns dictionaries for (utc_day_index, utc_hour) → value, keeping both the
    global aggregates and per-year contributions to support held-out exclusion.
    """

    total_sum = defaultdict(float)
    total_count = defaultdict(int)
    yearly_sum: Dict[int, Dict[Tuple[int, int], float]] = {}
    yearly_count: Dict[int, Dict[Tuple[int, int], int]] = {}

    for stat in stats:
        sums = defaultdict(float)
        counts = defaultdict(int)

        for utc_day, utc_hour, val in zip(stat.utc_day_index, stat.utc_hour, stat.y):
            day = int(utc_day)
            hour = int(utc_hour)
            if 1 <= day <= 365 and 0 <= hour < 24 and np.isfinite(val):
                key = (day, hour)
                sums[key] += float(val)
                counts[key] += 1

        yearly_sum[stat.year] = dict(sums)
        yearly_count[stat.year] = dict(counts)

        for key, value in sums.items():
            total_sum[key] += value
        for key, value in counts.items():
            total_count[key] += value

    return dict(total_sum), dict(total_count), yearly_sum, yearly_count


def evaluate_loyo(
    stats: List[YearlyDesignStats],
    *,
    ridge_lambda: float,
    reference_spec: Dict[str, object],
) -> LeaveOneYearOutReport:
    """Run leave-one-year-out validation with a UTC climatology baseline.

    The baseline computes a mean per (utc_day_index, utc_hour) cell on a
    no-leap (1–365) grid and excludes the held-out year before each
    comparison. Aggregation of RMSE/skill statistics is weighted by the number
    of valid observations per fold.
    """

    if not stats:
        return LeaveOneYearOutReport(
            years=[],
            global_rmse=math.nan,
            global_skill=math.nan,
            total_observations=0,
            hyperparameters={"reference": dict(reference_spec or {})},
            ridge_lambda=ridge_lambda,
            final_training_period=FINAL_TRAINING_PERIOD_LABEL,
        )

    stats_by_year = {entry.year: entry for entry in stats}
    common_years = sorted(stats_by_year.keys())

    feature_dim = stats[0].S.shape[0]
    S_total = np.zeros((feature_dim, feature_dim), dtype=float)
    b_total = np.zeros(feature_dim, dtype=float)
    total_obs = 0

    for entry in stats:
        if entry.S.shape != (feature_dim, feature_dim):
            raise ValueError("Inconsistent feature dimension across yearly statistics.")
        S_total += entry.S
        b_total += entry.b
        total_obs += entry.n

    total_sum, total_count, yearly_sum, yearly_count = accumulate_climatology_maps(stats)

    year_metrics: List[YearlyValidationMetrics] = []
    weighted_mse = 0.0
    weighted_mse_ref = 0.0
    accumulated_obs = 0

    for year in common_years:
        stat = stats_by_year[year]

        if total_obs - stat.n <= 0:
            continue

        S_excl = S_total - stat.S
        b_excl = b_total - stat.b
        beta = solve_normal_equations(S_excl, b_excl, ridge_lambda)

        y_pred = stat.X @ beta
        residuals_model = stat.y - y_pred

        sums_excl = yearly_sum.get(year, {})
        counts_excl = yearly_count.get(year, {})

        valid_mask = np.zeros(stat.n, dtype=bool)
        ref_values = np.full(stat.n, np.nan, dtype=float)

        for idx in range(stat.n):
            utc_day = int(stat.utc_day_index[idx])
            utc_hour = int(stat.utc_hour[idx])
            if not (1 <= utc_day <= 365 and 0 <= utc_hour < 24):
                continue

            key = (utc_day, utc_hour)
            total_cnt = total_count.get(key, 0)
            cnt_excl = total_cnt - counts_excl.get(key, 0)
            if cnt_excl <= 0:
                continue

            sum_excl = total_sum.get(key, 0.0) - sums_excl.get(key, 0.0)
            ref_values[idx] = sum_excl / cnt_excl
            valid_mask[idx] = True

        finite_mask = valid_mask & np.isfinite(residuals_model) & np.isfinite(ref_values)
        if not np.any(finite_mask):
            continue

        model_errors = residuals_model[finite_mask]
        reference_errors = stat.y[finite_mask] - ref_values[finite_mask]

        mse_model = float(np.mean(np.square(model_errors)))
        mse_ref = float(np.mean(np.square(reference_errors)))
        rmse_model = math.sqrt(mse_model)
        skill = float("nan") if mse_ref <= 0.0 else float(1.0 - (mse_model / mse_ref))
        n_valid = int(model_errors.size)

        year_metrics.append(
            YearlyValidationMetrics(
                year=year,
                mse_model=mse_model,
                mse_ref=mse_ref,
                rmse=rmse_model,
                skill=skill,
                n=n_valid,
            )
        )
        weighted_mse += n_valid * mse_model
        weighted_mse_ref += n_valid * mse_ref
        accumulated_obs += n_valid

    if accumulated_obs == 0:
        global_rmse = math.nan
        global_skill = math.nan
    else:
        global_rmse = math.sqrt(weighted_mse / accumulated_obs)
        global_skill = float("nan") if weighted_mse_ref == 0.0 else float(
            1.0 - (weighted_mse / weighted_mse_ref)
        )

    hyperparameters = {
        "reference": dict(reference_spec or {}),
        "evaluation_time_base": "UTC",
        "model_time_base": "solar",
        "baseline": "climatology_mean per (utc_day, utc_hour), LOYO",
    }

    return LeaveOneYearOutReport(
        years=year_metrics,
        global_rmse=global_rmse,
        global_skill=global_skill,
        total_observations=accumulated_obs,
        hyperparameters=hyperparameters,
        ridge_lambda=ridge_lambda,
        final_training_period=FINAL_TRAINING_PERIOD_LABEL,
    )


__all__ = [
    "accumulate_climatology_maps",
    "evaluate_loyo",
]
