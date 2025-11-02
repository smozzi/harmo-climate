// Auto-generated linear harmonic climate model
// Station name : STRASBOURG-ENTZHEIM
// Station code : 67124001
#pragma once
#include <cmath>
namespace harmoclimat {
static constexpr double longitude_deg = 7.6403331756591797;
static constexpr double latitude_deg = 48.54949951171875;
static constexpr double delta_utc_solar_h = 0.50935554504394531;
namespace detail {
static constexpr double two_pi = 6.2831853071795864769;
static constexpr double solar_year_days = 365.242189;
static constexpr double omega_annual = two_pi / solar_year_days;
static constexpr double omega_diurnal = two_pi / 24.0;
inline double eval_annual(const double* coeffs, int n_annual, double day){
    double value = coeffs[0];
    for(int k = 1; k <= n_annual; ++k){
        double angle = k * omega_annual * day;
        value += coeffs[2*k - 1] * std::cos(angle);
        value += coeffs[2*k] * std::sin(angle);
    }
    return value;
}
inline double wrap_day(double d){
    while (d >= solar_year_days) d -= solar_year_days;
    while (d < 0.0)   d += solar_year_days;
    return d;
}
inline double wrap_hour(double h){
    while (h >= 24.0) h -= 24.0;
    while (h < 0.0)   h += 24.0;
    return h;
}
inline double clamp(double value, double min_value, double max_value){
    if (value < min_value) return min_value;
    if (value > max_value) return max_value;
    return value;
}
inline double vapor_pressure_from_specific(double specific_humidity, double pressure_hpa){
    const double denom = std::fmax(0.622 + 0.378 * specific_humidity, 1e-6);
    return (specific_humidity * pressure_hpa) / denom;
}
inline double relative_humidity_from_specific(double temperature_c, double specific_humidity, double pressure_hpa){
    const double es_hpa = 6.112 * std::exp((17.67 * temperature_c) / (temperature_c + 243.5));
    const double e_hpa = vapor_pressure_from_specific(specific_humidity, pressure_hpa);
    const double rh = 100.0 * e_hpa / std::fmax(es_hpa, 1e-6);
    return clamp(rh, 0.0, 100.0);
}
inline double dew_point_c_from_specific(double specific_humidity, double pressure_hpa){
    const double e_hpa = vapor_pressure_from_specific(specific_humidity, pressure_hpa);
    const double alpha = std::log(std::fmax(e_hpa, 1e-6) / 6.112);
    return (243.5 * alpha) / (17.67 - alpha);
}
} // namespace detail
namespace temperature_model {
static constexpr int n_diurnal = 3;
static constexpr int c0_coeffs_n_annual = 3;
static constexpr double c0_coeffs[] = {
    11.630650832967838, -8.6642175729352999, -3.3799443912777023, -0.060107942956771465, 0.34009022841402065, 0.078761398510290265, -0.06180574992666793
};
static constexpr int a1_coeffs_n_annual = 3;
static constexpr double a1_coeffs[] = {
    -3.466490026217699, 1.9782875539895595, 0.2672942860050454, 0.62860413096250112, -0.044599907674297938, -0.071386100053381127, 0.031033628271094123
};
static constexpr int b1_coeffs_n_annual = 3;
static constexpr double b1_coeffs[] = {
    -1.0344888467592293, 0.081910749598349999, -0.39932902255143199, 0.24492801829695357, -0.14955891140966027, 0.14528212390982206, 0.18136037187924245
};
static constexpr int a2_coeffs_n_annual = 3;
static constexpr double a2_coeffs[] = {
    0.51660093518218464, 0.31528560626369162, -0.11105884786840194, -0.33075688947396614, 0.0069306172727363691, -0.041761965370271742, 0.025516816434589088
};
static constexpr int b2_coeffs_n_annual = 3;
static constexpr double b2_coeffs[] = {
    -0.013777007753879576, 0.25047554019346646, 0.093945102741038816, 0.090364022990262347, 0.10520417284413885, -0.11603865473321338, -0.053935273286500579
};
static constexpr int a3_coeffs_n_annual = 3;
static constexpr double a3_coeffs[] = {
    0.11983186220888715, -0.32290740079369856, 0.0029849169586648552, -0.068893817468091534, 0.049926970208758845, 0.065959846187909899, -0.041463730845155318
};
static constexpr int b3_coeffs_n_annual = 3;
static constexpr double b3_coeffs[] = {
    -0.11276475420189641, 0.14502967816880075, 0.022164303239123044, -0.040468134375445455, -0.0077361490617146502, -0.041692703623002141, -0.021819109924817757
};
inline double evaluate(double day_solar, double hour_solar) {
    double result = detail::eval_annual(c0_coeffs, c0_coeffs_n_annual, day_solar);
    const double ome_d = detail::omega_diurnal;
    result += detail::eval_annual(a1_coeffs, a1_coeffs_n_annual, day_solar) * std::cos(1 * ome_d * hour_solar);
    result += detail::eval_annual(b1_coeffs, b1_coeffs_n_annual, day_solar) * std::sin(1 * ome_d * hour_solar);
    result += detail::eval_annual(a2_coeffs, a2_coeffs_n_annual, day_solar) * std::cos(2 * ome_d * hour_solar);
    result += detail::eval_annual(b2_coeffs, b2_coeffs_n_annual, day_solar) * std::sin(2 * ome_d * hour_solar);
    result += detail::eval_annual(a3_coeffs, a3_coeffs_n_annual, day_solar) * std::cos(3 * ome_d * hour_solar);
    result += detail::eval_annual(b3_coeffs, b3_coeffs_n_annual, day_solar) * std::sin(3 * ome_d * hour_solar);
    return result;
}
} // namespace temperature_model
namespace specific_humidity_model {
static constexpr int n_diurnal = 3;
static constexpr int c0_coeffs_n_annual = 3;
static constexpr double c0_coeffs[] = {
    0.0067613674277560518, -0.0027040515349499761, -0.0017564194355969859, 0.00023866853021784406, 0.00041210003431730506, -3.451986033575856e-05, 3.3004508471681648e-05
};
static constexpr int a1_coeffs_n_annual = 3;
static constexpr double a1_coeffs[] = {
    -0.00017910864943409245, -7.4313579558253566e-05, 8.9550396932511987e-05, 9.3229637904585286e-05, 2.7867759923169546e-05, 3.8775589608939479e-05, -2.9662929707622925e-05
};
static constexpr int b1_coeffs_n_annual = 3;
static constexpr double b1_coeffs[] = {
    -4.7042362738830178e-05, -7.7451315075399e-05, 9.2791022209714068e-06, 1.1446166790615399e-05, -2.410130830027171e-05, 1.9271999959748331e-05, -2.6663788786152279e-07
};
static constexpr int a2_coeffs_n_annual = 3;
static constexpr double a2_coeffs[] = {
    -6.3964321476835196e-05, 0.00016058613490949419, 4.0202067533033472e-05, 2.6018868708426428e-06, -2.9809449023310083e-05, -3.5666251951943189e-05, -9.6780922241373247e-06
};
static constexpr int b2_coeffs_n_annual = 3;
static constexpr double b2_coeffs[] = {
    -9.7769342380414647e-05, 0.00010633570192590776, 2.3916531175001678e-05, 2.9798519043803629e-05, -1.389676434368876e-05, -1.375729029533305e-05, 6.2269007710601715e-06
};
static constexpr int a3_coeffs_n_annual = 3;
static constexpr double a3_coeffs[] = {
    1.2997969989192383e-05, 2.6460000252810944e-06, -1.9296145079149736e-05, -5.5720784460574063e-05, -5.553150088267547e-06, 9.2692095819512477e-06, 1.6119937804058293e-05
};
static constexpr int b3_coeffs_n_annual = 3;
static constexpr double b3_coeffs[] = {
    -2.5532528606364058e-06, 3.4559143894424959e-05, -3.4535497250800678e-06, -1.3376122835584632e-05, 8.1551538956025627e-06, -7.6147384607895087e-06, -5.0417910548942556e-06
};
inline double evaluate(double day_solar, double hour_solar) {
    double result = detail::eval_annual(c0_coeffs, c0_coeffs_n_annual, day_solar);
    const double ome_d = detail::omega_diurnal;
    result += detail::eval_annual(a1_coeffs, a1_coeffs_n_annual, day_solar) * std::cos(1 * ome_d * hour_solar);
    result += detail::eval_annual(b1_coeffs, b1_coeffs_n_annual, day_solar) * std::sin(1 * ome_d * hour_solar);
    result += detail::eval_annual(a2_coeffs, a2_coeffs_n_annual, day_solar) * std::cos(2 * ome_d * hour_solar);
    result += detail::eval_annual(b2_coeffs, b2_coeffs_n_annual, day_solar) * std::sin(2 * ome_d * hour_solar);
    result += detail::eval_annual(a3_coeffs, a3_coeffs_n_annual, day_solar) * std::cos(3 * ome_d * hour_solar);
    result += detail::eval_annual(b3_coeffs, b3_coeffs_n_annual, day_solar) * std::sin(3 * ome_d * hour_solar);
    return result;
}
} // namespace specific_humidity_model
namespace pressure_model {
static constexpr int n_diurnal = 3;
static constexpr int c0_coeffs_n_annual = 3;
static constexpr double c0_coeffs[] = {
    999.08236985666679, 1.1608386804609032, -0.25483416713980644, 0.48860690987370931, 0.7064926159488838, -0.056720597283068185, 0.72078680568479025
};
static constexpr int a1_coeffs_n_annual = 3;
static constexpr double a1_coeffs[] = {
    0.15833585434814246, -0.13602415389193745, 0.011814883536310153, -0.007421679535608224, -0.0074860911970312839, 0.0061902049909476724, 0.013541101200510176
};
static constexpr int b1_coeffs_n_annual = 3;
static constexpr double b1_coeffs[] = {
    0.32970182108874435, -0.21132067281226358, 0.049630550150585512, -0.046324056874054866, 0.013243557248835537, -0.016082484590349756, -0.0084788255319075273
};
static constexpr int a2_coeffs_n_annual = 3;
static constexpr double a2_coeffs[] = {
    -0.13282888137989499, 0.083470392288703094, 0.0517945180659991, 0.050666788132774206, 0.05635327687327954, -0.037434929567383229, -0.036258325373195163
};
static constexpr int b2_coeffs_n_annual = 3;
static constexpr double b2_coeffs[] = {
    -0.38210109185742819, -0.0017732564346491437, -0.022602713341033392, 0.045734562915490357, -0.0047287827733855092, 0.011988167394664427, 0.017741299070070313
};
static constexpr int a3_coeffs_n_annual = 3;
static constexpr double a3_coeffs[] = {
    0.013849954871867699, 0.098666593342282533, -0.023350736819740796, -0.014263102970221046, -0.013418945368952634, -0.0037779291791057322, 0.0029215471056449295
};
static constexpr int b3_coeffs_n_annual = 3;
static constexpr double b3_coeffs[] = {
    0.038645520712958334, 0.09925266431639182, 0.019349615850133523, 0.011331289713197645, 0.015494118602170674, -0.0061383675235287625, 0.0049374448485236984
};
inline double evaluate(double day_solar, double hour_solar) {
    double result = detail::eval_annual(c0_coeffs, c0_coeffs_n_annual, day_solar);
    const double ome_d = detail::omega_diurnal;
    result += detail::eval_annual(a1_coeffs, a1_coeffs_n_annual, day_solar) * std::cos(1 * ome_d * hour_solar);
    result += detail::eval_annual(b1_coeffs, b1_coeffs_n_annual, day_solar) * std::sin(1 * ome_d * hour_solar);
    result += detail::eval_annual(a2_coeffs, a2_coeffs_n_annual, day_solar) * std::cos(2 * ome_d * hour_solar);
    result += detail::eval_annual(b2_coeffs, b2_coeffs_n_annual, day_solar) * std::sin(2 * ome_d * hour_solar);
    result += detail::eval_annual(a3_coeffs, a3_coeffs_n_annual, day_solar) * std::cos(3 * ome_d * hour_solar);
    result += detail::eval_annual(b3_coeffs, b3_coeffs_n_annual, day_solar) * std::sin(3 * ome_d * hour_solar);
    return result;
}
} // namespace pressure_model
inline double predict_temperature(double day_utc, double hour_utc){
    double hour_solar = detail::wrap_hour(hour_utc + delta_utc_solar_h);
    double day_solar  = detail::wrap_day(day_utc + (delta_utc_solar_h / 24.0));
    return temperature_model::evaluate(day_solar, hour_solar);
}
inline double predict_specific_humidity(double day_utc, double hour_utc){
    double hour_solar = detail::wrap_hour(hour_utc + delta_utc_solar_h);
    double day_solar  = detail::wrap_day(day_utc + (delta_utc_solar_h / 24.0));
    return specific_humidity_model::evaluate(day_solar, hour_solar);
}
inline double predict_pressure(double day_utc, double hour_utc){
    double hour_solar = detail::wrap_hour(hour_utc + delta_utc_solar_h);
    double day_solar  = detail::wrap_day(day_utc + (delta_utc_solar_h / 24.0));
    return pressure_model::evaluate(day_solar, hour_solar);
}
inline double predict_relative_humidity(double day_utc, double hour_utc){
    double hour_solar = detail::wrap_hour(hour_utc + delta_utc_solar_h);
    double day_solar  = detail::wrap_day(day_utc + (delta_utc_solar_h / 24.0));
    const double temperature_c = temperature_model::evaluate(day_solar, hour_solar);
    const double specific_humidity_kg_kg = specific_humidity_model::evaluate(day_solar, hour_solar);
    const double pressure_hpa = pressure_model::evaluate(day_solar, hour_solar);
    return detail::relative_humidity_from_specific(temperature_c, specific_humidity_kg_kg, pressure_hpa);
}
inline double predict_dew_point_c(double day_utc, double hour_utc){
    double hour_solar = detail::wrap_hour(hour_utc + delta_utc_solar_h);
    double day_solar  = detail::wrap_day(day_utc + (delta_utc_solar_h / 24.0));
    const double specific_humidity_kg_kg = specific_humidity_model::evaluate(day_solar, hour_solar);
    const double pressure_hpa = pressure_model::evaluate(day_solar, hour_solar);
    return detail::dew_point_c_from_specific(specific_humidity_kg_kg, pressure_hpa);
}
inline void predict(double day_utc, double hour_utc, double& temperature_c, double& specific_humidity_kg_kg, double& pressure_hpa){
    double hour_solar = detail::wrap_hour(hour_utc + delta_utc_solar_h);
    double day_solar  = detail::wrap_day(day_utc + (delta_utc_solar_h / 24.0));
    temperature_c = temperature_model::evaluate(day_solar, hour_solar);
    specific_humidity_kg_kg = specific_humidity_model::evaluate(day_solar, hour_solar);
    pressure_hpa = pressure_model::evaluate(day_solar, hour_solar);
}
} // namespace harmoclimat