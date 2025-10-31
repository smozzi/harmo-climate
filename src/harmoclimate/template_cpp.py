"""Utilities to export the trained model as a C++ header."""

from __future__ import annotations

import math
from pathlib import Path
from typing import Mapping


def _format_static_block(prefix: str, values: Mapping[str, float]) -> list[str]:
    lines = []
    for key, value in values.items():
        sanitized = key.replace(".", "_")
        lines.append(f"static constexpr double {prefix}_{sanitized} = {value:.17g};")
    return lines


def generate_cpp_header(params: Mapping[str, object], output_path: Path) -> None:
    """Render the compact harmonic model as a standalone C++ header."""

    metadata = params["metadata"]
    output_path.parent.mkdir(parents=True, exist_ok=True)

    lines: list[str] = []
    lines.append("// Auto-generated factorized model (annual + diurnal)")
    lines.append("// Target city / station (France):")
    lines.append(f"//   Station code : {metadata.get('station_code')}")
    lines.append(f"//   Station name : {metadata.get('station_usual_name')}")
    
    lines.append("#pragma once")
    lines.append("#include <cmath>")
    lines.append("namespace harmoclimat {")

    longitude_deg = metadata.get("longitude_deg", float("nan"))
    latitude_deg = metadata.get("latitude_deg", float("nan"))
    delta_utc_solar_h = metadata.get("delta_utc_solar_h", 0.0)

    lines.append(f"static constexpr double longitude_deg = {longitude_deg:.17g};")
    if not math.isnan(latitude_deg):
        lines.append(f"static constexpr double latitude_deg  = {latitude_deg:.17g};")
    lines.append(f"static constexpr double delta_utc_solar_h = {delta_utc_solar_h:.17g};")

    lines.extend(_format_static_block("annual_T", params["annual_T"]))
    lines.extend(_format_static_block("annual_RH", params["annual_RH"]))
    lines.extend(_format_static_block("diurn_T", params["diurn_T"]))
    lines.extend(_format_static_block("diurn_RH", params["diurn_RH"]))

    lines.append(
        "inline double wrap_day(double d){\n"
        "    while (d > 365.0) d -= 365.0;\n"
        "    while (d < 0.0)   d += 365.0;\n"
        "    return d;\n"
        "}"
    )
    lines.append(
        "inline double wrap_hour(double h){\n"
        "    while (h >= 24.0) h -= 24.0;\n"
        "    while (h < 0.0)   h += 24.0;\n"
        "    return h;\n"
        "}"
    )
    lines.append(
        "inline double predict_annual_T(double day_solar){\n"
        "    const double wA = 6.2831853071795864769 / 365.0;\n"
        "    double th = wA * day_solar;\n"
        "    return annual_T_mu + annual_T_A1 * std::cos(th - annual_T_ph1)\n"
        "                       + annual_T_A2 * std::cos(2.0*th - annual_T_ph2);\n"
        "}"
    )
    lines.append(
        "inline double predict_annual_RH(double day_solar){\n"
        "    const double wA = 6.2831853071795864769 / 365.0;\n"
        "    double th = wA * day_solar;\n"
        "    return annual_RH_mu + annual_RH_A1 * std::cos(th - annual_RH_ph1)\n"
        "                        + annual_RH_A2 * std::cos(2.0*th - annual_RH_ph2);\n"
        "}"
    )
    lines.append(
        "inline double predict_diurn_T(double day_solar, double hour_solar){\n"
        "    const double two_pi = 6.2831853071795864769;\n"
        "    const double wA = two_pi / 365.0;\n"
        "    const double wD = two_pi / 24.0;\n"
        "    double thA = wA * day_solar;\n"
        "    double thD = wD * hour_solar;\n"
        "    double Aday  = diurn_T_a0 + diurn_T_a1 * std::cos(thA - diurn_T_psiA);\n"
        "    double phday = diurn_T_ph0 + diurn_T_k1 * std::cos(thA - diurn_T_psiP);\n"
        "    double base  = std::cos(thD - phday);\n"
        "    double sec   = std::cos(2.0 * (thD - phday));\n"
        "    return Aday * (base + diurn_T_alpha * sec);\n"
        "}"
    )
    lines.append(
        "inline double predict_diurn_RH(double day_solar, double hour_solar, double temp_res){\n"
        "    const double two_pi = 6.2831853071795864769;\n"
        "    const double wA = two_pi / 365.0;\n"
        "    const double wD = two_pi / 24.0;\n"
        "    double thA = wA * day_solar;\n"
        "    double thD = wD * hour_solar;\n"
        "    double Aday  = diurn_RH_core_a0 + diurn_RH_core_a1 * std::cos(thA - diurn_RH_core_psiA);\n"
        "    double phday = diurn_RH_core_ph0 + diurn_RH_core_k1 * std::cos(thA - diurn_RH_core_psiP);\n"
        "    double base  = std::cos(thD - phday);\n"
        "    double sec   = std::cos(2.0 * (thD - phday));\n"
        "    double g = Aday * (base + diurn_RH_core_alpha * sec);\n"
        "    return g + diurn_RH_beta_c * temp_res;\n"
        "}"
    )
    lines.append(
        "inline void predict(double day_utc, double hour_utc, double& T, double& RH){\n"
        "    double hour_solar = wrap_hour(hour_utc + delta_utc_solar_h);\n"
        "    double day_solar  = wrap_day(day_utc + (delta_utc_solar_h / 24.0));\n"
        "    double T_ann = predict_annual_T(day_solar);\n"
        "    double T_res = predict_diurn_T(day_solar, hour_solar);\n"
        "    T = T_ann + T_res;\n"
        "    double RH_ann = predict_annual_RH(day_solar);\n"
        "    double RH_res = predict_diurn_RH(day_solar, hour_solar, T_res);\n"
        "    RH = RH_ann + RH_res;\n"
        "}"
    )
    lines.append("} // namespace harmoclimat")

    with open(output_path, "w", encoding="utf-8") as handle:
        handle.write("\n".join(lines))
    print(f"[OK] C++ header generated: {output_path}")


__all__ = ["generate_cpp_header"]
