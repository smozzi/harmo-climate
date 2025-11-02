"""Utilities to export linear harmonic models as a C++ header."""

from __future__ import annotations

import math
from pathlib import Path
from typing import Iterable, Mapping, Sequence


def _format_array(values: Sequence[float], indent: str = "    ", per_line: int = 6) -> str:
    formatted = [f"{float(v):.17g}" for v in values]
    lines = []
    for idx in range(0, len(formatted), per_line):
        chunk = ", ".join(formatted[idx : idx + per_line])
        lines.append(f"{indent}{chunk}")
    return ",\n".join(lines)


def _extract_parameters(model_payload: Mapping[str, object]) -> list[dict[str, object]]:
    layout: Iterable[Mapping[str, object]] = model_payload["model"]["params_layout"]  # type: ignore[index]
    coefficients: Sequence[float] = model_payload["model"]["coefficients"]  # type: ignore[index]
    params: list[dict[str, object]] = []
    for entry in layout:
        start = int(entry["start"])
        length = int(entry["length"])
        coeff_slice = [float(coefficients[start + i]) for i in range(length)]
        params.append(
            {
                "name": entry["name"],
                "n_annual": int(entry["n_annual"]),
                "coefficients": coeff_slice,
            }
        )
    return params


def _generate_model_namespace(namespace: str, payload: Mapping[str, object]) -> list[str]:
    lines: list[str] = []
    params = _extract_parameters(payload)
    n_diurnal = int(payload["model"]["n_diurnal"])  # type: ignore[index]

    lines.append(f"namespace {namespace} {{")
    lines.append(f"static constexpr int n_diurnal = {n_diurnal};")

    entry_map = {entry["name"]: entry for entry in params}
    if "c0" not in entry_map:
        raise ValueError("Linear model payload must include parameter 'c0'.")
    for entry in params:
        coeffs = entry["coefficients"]
        n_annual = entry["n_annual"]
        array_name = f"{entry['name']}_coeffs"
        lines.append(f"static constexpr int {array_name}_n_annual = {n_annual};")
        lines.append(f"static constexpr double {array_name}[] = {{")
        lines.append(_format_array(coeffs, indent="    ", per_line=7))
        lines.append("};")

    lines.append("inline double evaluate(double day_solar, double hour_solar) {")
    lines.append("    double result = detail::eval_annual(c0_coeffs, c0_coeffs_n_annual, day_solar);")
    lines.append("    const double ome_d = detail::omega_diurnal;")
    for m in range(1, n_diurnal + 1):
        cos_name = f"a{m}"
        sin_name = f"b{m}"
        if cos_name in entry_map:
            lines.append(
                f"    result += detail::eval_annual({cos_name}_coeffs, {cos_name}_coeffs_n_annual, day_solar) * "
                f"std::cos({m} * ome_d * hour_solar);"
            )
        if sin_name in entry_map:
            lines.append(
                f"    result += detail::eval_annual({sin_name}_coeffs, {sin_name}_coeffs_n_annual, day_solar) * "
                f"std::sin({m} * ome_d * hour_solar);"
            )
    lines.append("    return result;")
    lines.append("}")
    lines.append("} // namespace " + namespace)
    return lines


def generate_cpp_header(
    temperature_payload: Mapping[str, object],
    specific_humidity_payload: Mapping[str, object],
    pressure_payload: Mapping[str, object],
    output_path: Path,
) -> None:
    """Render the linear harmonic models as a standalone C++ header."""

    metadata = temperature_payload["metadata"]  # type: ignore[index]
    station_name = metadata.get("station_usual_name", "")
    station_code = metadata.get("station_code", "")

    longitude_deg = float(metadata.get("longitude_deg", float("nan")))
    latitude_deg = float(metadata.get("latitude_deg", float("nan")))
    delta_utc_solar_h = float(metadata.get("delta_utc_solar_h", 0.0))

    output_path.parent.mkdir(parents=True, exist_ok=True)

    lines: list[str] = []
    lines.append("// Auto-generated linear harmonic climate model")
    lines.append(f"// Station name : {station_name}")
    lines.append(f"// Station code : {station_code}")
    lines.append("#pragma once")
    lines.append("#include <cmath>")
    lines.append("namespace harmoclimat {")

    lines.append(f"static constexpr double longitude_deg = {longitude_deg:.17g};")
    if not math.isnan(latitude_deg):
        lines.append(f"static constexpr double latitude_deg = {latitude_deg:.17g};")
    lines.append(f"static constexpr double delta_utc_solar_h = {delta_utc_solar_h:.17g};")

    lines.append("namespace detail {")
    lines.append("static constexpr double two_pi = 6.2831853071795864769;")
    lines.append("static constexpr double solar_year_days = 365.242189;")
    lines.append("static constexpr double omega_annual = two_pi / solar_year_days;")
    lines.append("static constexpr double omega_diurnal = two_pi / 24.0;")
    lines.append(
        "inline double eval_annual(const double* coeffs, int n_annual, double day){\n"
        "    double value = coeffs[0];\n"
        "    for(int k = 1; k <= n_annual; ++k){\n"
        "        double angle = k * omega_annual * day;\n"
        "        value += coeffs[2*k - 1] * std::cos(angle);\n"
        "        value += coeffs[2*k] * std::sin(angle);\n"
        "    }\n"
        "    return value;\n"
        "}"
    )
    lines.append("inline double wrap_day(double d){")
    lines.append("    while (d >= solar_year_days) d -= solar_year_days;")
    lines.append("    while (d < 0.0)   d += solar_year_days;")
    lines.append("    return d;")
    lines.append("}")
    lines.append("inline double wrap_hour(double h){")
    lines.append("    while (h >= 24.0) h -= 24.0;")
    lines.append("    while (h < 0.0)   h += 24.0;")
    lines.append("    return h;")
    lines.append("}")
    lines.append("inline double clamp(double value, double min_value, double max_value){")
    lines.append("    if (value < min_value) return min_value;")
    lines.append("    if (value > max_value) return max_value;")
    lines.append("    return value;")
    lines.append("}")
    lines.append("inline double vapor_pressure_from_specific(double specific_humidity, double pressure_hpa){")
    lines.append("    const double denom = std::fmax(0.622 + 0.378 * specific_humidity, 1e-6);")
    lines.append("    return (specific_humidity * pressure_hpa) / denom;")
    lines.append("}")
    lines.append("inline double relative_humidity_from_specific(double temperature_c, double specific_humidity, double pressure_hpa){")
    lines.append("    const double es_hpa = 6.112 * std::exp((17.67 * temperature_c) / (temperature_c + 243.5));")
    lines.append("    const double e_hpa = vapor_pressure_from_specific(specific_humidity, pressure_hpa);")
    lines.append("    const double rh = 100.0 * e_hpa / std::fmax(es_hpa, 1e-6);")
    lines.append("    return clamp(rh, 0.0, 100.0);")
    lines.append("}")
    lines.append("inline double dew_point_c_from_specific(double specific_humidity, double pressure_hpa){")
    lines.append("    const double e_hpa = vapor_pressure_from_specific(specific_humidity, pressure_hpa);")
    lines.append("    const double alpha = std::log(std::fmax(e_hpa, 1e-6) / 6.112);")
    lines.append("    return (243.5 * alpha) / (17.67 - alpha);")
    lines.append("}")
    lines.append("} // namespace detail")

    lines.extend(_generate_model_namespace("temperature_model", temperature_payload))
    lines.extend(_generate_model_namespace("specific_humidity_model", specific_humidity_payload))
    lines.extend(_generate_model_namespace("pressure_model", pressure_payload))

    lines.append("inline double predict_temperature(double day_utc, double hour_utc){")
    lines.append("    double hour_solar = detail::wrap_hour(hour_utc + delta_utc_solar_h);")
    lines.append("    double day_solar  = detail::wrap_day(day_utc + (delta_utc_solar_h / 24.0));")
    lines.append("    return temperature_model::evaluate(day_solar, hour_solar);")
    lines.append("}")

    lines.append("inline double predict_specific_humidity(double day_utc, double hour_utc){")
    lines.append("    double hour_solar = detail::wrap_hour(hour_utc + delta_utc_solar_h);")
    lines.append("    double day_solar  = detail::wrap_day(day_utc + (delta_utc_solar_h / 24.0));")
    lines.append("    return specific_humidity_model::evaluate(day_solar, hour_solar);")
    lines.append("}")

    lines.append("inline double predict_pressure(double day_utc, double hour_utc){")
    lines.append("    double hour_solar = detail::wrap_hour(hour_utc + delta_utc_solar_h);")
    lines.append("    double day_solar  = detail::wrap_day(day_utc + (delta_utc_solar_h / 24.0));")
    lines.append("    return pressure_model::evaluate(day_solar, hour_solar);")
    lines.append("}")
    lines.append("inline double predict_relative_humidity(double day_utc, double hour_utc){")
    lines.append("    double hour_solar = detail::wrap_hour(hour_utc + delta_utc_solar_h);")
    lines.append("    double day_solar  = detail::wrap_day(day_utc + (delta_utc_solar_h / 24.0));")
    lines.append("    const double temperature_c = temperature_model::evaluate(day_solar, hour_solar);")
    lines.append("    const double specific_humidity_kg_kg = specific_humidity_model::evaluate(day_solar, hour_solar);")
    lines.append("    const double pressure_hpa = pressure_model::evaluate(day_solar, hour_solar);")
    lines.append("    return detail::relative_humidity_from_specific(temperature_c, specific_humidity_kg_kg, pressure_hpa);")
    lines.append("}")
    lines.append("inline double predict_dew_point_c(double day_utc, double hour_utc){")
    lines.append("    double hour_solar = detail::wrap_hour(hour_utc + delta_utc_solar_h);")
    lines.append("    double day_solar  = detail::wrap_day(day_utc + (delta_utc_solar_h / 24.0));")
    lines.append("    const double specific_humidity_kg_kg = specific_humidity_model::evaluate(day_solar, hour_solar);")
    lines.append("    const double pressure_hpa = pressure_model::evaluate(day_solar, hour_solar);")
    lines.append("    return detail::dew_point_c_from_specific(specific_humidity_kg_kg, pressure_hpa);")
    lines.append("}")

    lines.append(
        "inline void predict(double day_utc, double hour_utc, double& temperature_c, "
        "double& specific_humidity_kg_kg, double& pressure_hpa){"
    )
    lines.append("    double hour_solar = detail::wrap_hour(hour_utc + delta_utc_solar_h);")
    lines.append("    double day_solar  = detail::wrap_day(day_utc + (delta_utc_solar_h / 24.0));")
    lines.append("    temperature_c = temperature_model::evaluate(day_solar, hour_solar);")
    lines.append("    specific_humidity_kg_kg = specific_humidity_model::evaluate(day_solar, hour_solar);")
    lines.append("    pressure_hpa = pressure_model::evaluate(day_solar, hour_solar);")
    lines.append("}")

    lines.append("} // namespace harmoclimat")

    with open(output_path, "w", encoding="utf-8") as handle:
        handle.write("\n".join(lines))
    print(f"[OK] C++ header generated: {output_path}")


__all__ = ["generate_cpp_header"]
