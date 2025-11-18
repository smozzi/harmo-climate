// Auto-generated linear harmonic climate model
// Station name : TOULOUSE-BLAGNAC
// Station code : 31069001
#pragma once
#include <cmath>
namespace harmoclimat {
static constexpr double longitude_deg = 1.3788330554962158;
static constexpr double latitude_deg = 43.620998382568359;
static constexpr double delta_utc_solar_h = 0.091922203699747726;
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
} // namespace detail
namespace temperature_model {
static constexpr int n_diurnal = 3;
static constexpr int c0_coeffs_n_annual = 3;
static constexpr double c0_coeffs[] = {
    14.273282358494898, -7.9468221923720659, -3.240048157518248, -0.23455854714241897, 0.80760180671496185, -0.24329229373409048, -0.09899275645824511
};
static constexpr int a1_coeffs_n_annual = 3;
static constexpr double a1_coeffs[] = {
    -3.4077878342215464, 1.4077890866932388, 0.24117268007581616, 0.34507890898646637, -0.19222675551018298, -0.046817532602610644, -0.035828842694789864
};
static constexpr int b1_coeffs_n_annual = 3;
static constexpr double b1_coeffs[] = {
    -1.3090030782748756, 0.050499614934325011, -0.10592294943868005, -0.078173135489371881, -0.36976676954865062, 0.16697963089439119, 0.13265421888010137
};
static constexpr int a2_coeffs_n_annual = 3;
static constexpr double a2_coeffs[] = {
    0.61807257554930661, 0.3132240580589789, -0.094797274645045254, -0.13356333360687134, 0.041250248892403141, -0.03242419002125449, -0.0061890885573274608
};
static constexpr int b2_coeffs_n_annual = 3;
static constexpr double b2_coeffs[] = {
    -0.10187423656977909, 0.28860223176940381, 0.092751777301719615, 0.14111644602857845, 0.06487675700315737, -0.041423162839531263, -0.023476727414181713
};
static constexpr int a3_coeffs_n_annual = 3;
static constexpr double a3_coeffs[] = {
    0.069138954719738771, -0.25414824862901647, 0.0055934364048353391, -0.092531707140221922, 0.039654749538216322, 0.019684449446784581, -0.012229541542973385
};
static constexpr int b3_coeffs_n_annual = 3;
static constexpr double b3_coeffs[] = {
    -0.056227893696010771, 0.15314482881334654, -0.025610247126378268, -0.056337136390658876, 0.010999124532849574, -0.060978476199091441, -0.014438825340052105
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
    0.0075655207916446266, -0.002500627389468654, -0.0014650065369329539, 0.000167854769203326, 0.00019068500725094302, -0.00013247437585228302, 8.6000546640107626e-05
};
static constexpr int a1_coeffs_n_annual = 3;
static constexpr double a1_coeffs[] = {
    -3.7147226025587869e-05, -0.00020685755998344635, -0.00010718394304316741, 3.2551690925089426e-07, 0.0001218624413943913, 1.9550927082701559e-05, -2.5832788580100212e-05
};
static constexpr int b1_coeffs_n_annual = 3;
static constexpr double b1_coeffs[] = {
    5.9384026417721911e-05, -0.00021773560354100446, -4.3816216846062125e-05, -6.0897096888450445e-05, 5.7866455271018331e-05, 4.3108039960470908e-05, -9.0500038114290147e-06
};
static constexpr int a2_coeffs_n_annual = 3;
static constexpr double a2_coeffs[] = {
    -2.6361708842968538e-05, 0.00010590791970008115, 4.6520888460299765e-05, 3.5397935096809873e-05, -1.2243683397535725e-05, -1.4028620833010018e-05, -2.1026567109827592e-05
};
static constexpr int b2_coeffs_n_annual = 3;
static constexpr double b2_coeffs[] = {
    -9.7522485927627501e-05, 3.4425723428106499e-05, 9.6853590164235267e-06, 5.8404111980007906e-05, 6.9862669690329007e-06, 4.5209148603485139e-07, -1.5647011743035043e-05
};
static constexpr int a3_coeffs_n_annual = 3;
static constexpr double a3_coeffs[] = {
    1.2164294796618475e-05, 3.1192043862113603e-06, -9.8769304933412169e-06, -3.5945428441520762e-05, -4.9638440965574397e-06, -5.9898657729719364e-06, 1.0791722399264784e-05
};
static constexpr int b3_coeffs_n_annual = 3;
static constexpr double b3_coeffs[] = {
    -3.0979287191372203e-06, 5.7105580668830879e-05, -3.8788054553280449e-06, -2.0599723184425632e-06, -1.247141479411152e-06, -1.7108458891979374e-05, -2.7186679193111081e-06
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
    999.77430083167451, 1.3914251684863694, -0.18447225208609919, 1.3075079020577522, 0.85863236606567017, 0.28918050895617836, 0.69893399813308221
};
static constexpr int a1_coeffs_n_annual = 3;
static constexpr double a1_coeffs[] = {
    0.24600669273965775, -0.17930675610158164, -0.010589574523294556, -0.0073416220794483263, 0.029795766413529587, 0.012914627179054224, -0.0089425932853584387
};
static constexpr int b1_coeffs_n_annual = 3;
static constexpr double b1_coeffs[] = {
    0.25741632064296982, -0.21058272856643051, -0.00014797118451112285, -0.0057474758981670721, 0.093541000829978613, -0.011908342154198153, -0.029852789653963915
};
static constexpr int a2_coeffs_n_annual = 3;
static constexpr double a2_coeffs[] = {
    -0.21382421843946572, 0.10618901358891954, 0.068170889759535808, 0.063988809525061252, 0.064075966886089741, -0.058740522329273785, -0.02294603327344772
};
static constexpr int b2_coeffs_n_annual = 3;
static constexpr double b2_coeffs[] = {
    -0.50019092743333593, -0.038838315698471779, -0.029526620650519846, 0.030724714225889054, -0.019238369613172528, 0.012029670811226825, 0.013816784710517309
};
static constexpr int a3_coeffs_n_annual = 3;
static constexpr double a3_coeffs[] = {
    0.020391492140010674, 0.13613132890096896, -0.033170124275465861, -0.016411402525987277, -0.0096046384025905603, -0.0063216261606432438, 0.0053904914537935894
};
static constexpr int b3_coeffs_n_annual = 3;
static constexpr double b3_coeffs[] = {
    0.053867248512759286, 0.11566339481281161, 0.011909852105886217, 0.021105489725588955, 0.017474180471377374, 0.0051775713558740956, 0.0058237031785057262
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
inline void predict(double day_utc, double hour_utc, double& temperature_c, double& specific_humidity_kg_kg, double& pressure_hpa){
    double hour_solar = detail::wrap_hour(hour_utc + delta_utc_solar_h);
    double day_solar  = detail::wrap_day(day_utc + (delta_utc_solar_h / 24.0));
    temperature_c = temperature_model::evaluate(day_solar, hour_solar);
    specific_humidity_kg_kg = specific_humidity_model::evaluate(day_solar, hour_solar);
    pressure_hpa = pressure_model::evaluate(day_solar, hour_solar);
}
} // namespace harmoclimat