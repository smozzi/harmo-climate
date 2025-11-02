// Auto-generated linear harmonic climate model
// Station name : CLERMONT-FD
// Station code : 63113001
#pragma once
#include <cmath>
namespace harmoclimat {
static constexpr double longitude_deg = 3.1493330001831055;
static constexpr double latitude_deg = 45.786834716796875;
static constexpr double delta_utc_solar_h = 0.20995553334554037;
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
    12.300752340666429, -7.8432117162799662, -3.0999018210277587, -0.083580411980610295, 0.58196916980466951, -0.23752629842092884, -0.13924390633681452
};
static constexpr int a1_coeffs_n_annual = 3;
static constexpr double a1_coeffs[] = {
    -3.6649709576210716, 1.6873984254973251, 0.14095190194712981, 0.43738579698481206, -0.17461927013394415, -0.038173449283237085, 0.060349989461152852
};
static constexpr int b1_coeffs_n_annual = 3;
static constexpr double b1_coeffs[] = {
    -1.0054424444132792, -0.13839201740919846, -0.25911521493088063, 0.13977999869579372, -0.30284577369155996, 0.18544426998208277, 0.17265378155741357
};
static constexpr int a2_coeffs_n_annual = 3;
static constexpr double a2_coeffs[] = {
    0.6761474572340801, 0.44407600297604888, -0.12073857924533764, -0.24765507667444592, 0.036928447697237118, -0.06011237379461002, 0.015842910085502371
};
static constexpr int b2_coeffs_n_annual = 3;
static constexpr double b2_coeffs[] = {
    -0.11511304890879348, 0.33969394324156738, 0.11122739099180552, 0.14638622552490249, 0.11724947328304107, -0.083942558866954564, -0.033556661513523781
};
static constexpr int a3_coeffs_n_annual = 3;
static constexpr double a3_coeffs[] = {
    0.12649057381030135, -0.32689739499025711, 0.023681269384633177, -0.10590283967172308, 0.056686663901122918, 0.030469909472287022, -0.021272685205538613
};
static constexpr int b3_coeffs_n_annual = 3;
static constexpr double b3_coeffs[] = {
    -0.1073871069437943, 0.16968744880396675, 0.0096593095422569924, -0.071471505119231393, 0.0024811620118146416, -0.066479078921889659, -0.026912104260380803
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
    0.0066599921224002809, -0.0025786042034499556, -0.0014272008983089491, 0.00026423972196911778, 0.00019236758455266681, -0.0001093220410016487, 0.00010620419483246555
};
static constexpr int a1_coeffs_n_annual = 3;
static constexpr double a1_coeffs[] = {
    -0.00023438232220125224, 2.6063180022807887e-05, 1.590870053061211e-05, 4.7617323408341342e-05, 6.6689524908589872e-05, -3.5548297837020317e-07, -3.4786528641397322e-05
};
static constexpr int b1_coeffs_n_annual = 3;
static constexpr double b1_coeffs[] = {
    -9.7647719565810848e-05, -2.6698855325608572e-05, 2.7817312383268056e-06, -1.2351228578721723e-06, 2.076439973442001e-05, 1.7636893963542961e-05, -2.0174374213296354e-05
};
static constexpr int a2_coeffs_n_annual = 3;
static constexpr double a2_coeffs[] = {
    -5.1088113344555681e-05, 0.00015566181816651333, 1.680851210177401e-05, -6.5308112006160998e-06, -1.8679820419280535e-05, -2.3786206741935558e-05, -1.2801791045916794e-05
};
static constexpr int b2_coeffs_n_annual = 3;
static constexpr double b2_coeffs[] = {
    -7.5388337533713386e-05, 6.2868789546702617e-05, 6.8516474347163837e-06, 3.6647855932528247e-05, -5.1791108390654529e-06, -1.9978238411021439e-06, 6.2799057140016659e-07
};
static constexpr int a3_coeffs_n_annual = 3;
static constexpr double a3_coeffs[] = {
    2.9464746448358735e-05, -9.4162313581714913e-06, -1.864551701718026e-05, -4.4671696636916555e-05, 2.559946269794449e-06, 2.3364913235869798e-06, 1.0124391282697786e-05
};
static constexpr int b3_coeffs_n_annual = 3;
static constexpr double b3_coeffs[] = {
    -9.5241405298838831e-06, 4.7273151900035762e-05, -8.6987274830772946e-07, -7.4139563677194565e-06, 5.2445454449707951e-06, -1.6456184711904927e-05, -2.4465625468120338e-06
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
    978.6580022519471, 0.72160359963731158, -0.47953092204446945, 0.95983809548569421, 0.85709060876029464, 0.21852481677897426, 0.69333446630042517
};
static constexpr int a1_coeffs_n_annual = 3;
static constexpr double a1_coeffs[] = {
    0.29983302652109844, -0.15743899168967251, 0.028363584877377886, -0.029036257635966085, 0.0053910445126566569, -0.0039062920436859451, 0.00033950472155603798
};
static constexpr int b1_coeffs_n_annual = 3;
static constexpr double b1_coeffs[] = {
    0.21807484132935992, -0.16261004206443178, 0.018094619246637411, -0.036456267410725704, 0.069448044550544541, -0.018449827597873322, -0.044069710300259385
};
static constexpr int a2_coeffs_n_annual = 3;
static constexpr double a2_coeffs[] = {
    -0.19882316720393459, 0.10652549187147928, 0.052547546298673656, 0.066282225840107345, 0.048091488010461765, -0.040378454427564187, -0.029468912773889435
};
static constexpr int b2_coeffs_n_annual = 3;
static constexpr double b2_coeffs[] = {
    -0.43339226676674308, -0.037196715464283263, -0.028799784761307113, 0.037361383473671579, -0.021837917542627327, 0.016714495966699779, 0.018786456297455418
};
static constexpr int a3_coeffs_n_annual = 3;
static constexpr double a3_coeffs[] = {
    0.017191410978435302, 0.12304289111616697, -0.033221595452355118, -0.012082010067822547, -0.013375791671876249, -0.0057499590742569537, 0.0060408868159668137
};
static constexpr int b3_coeffs_n_annual = 3;
static constexpr double b3_coeffs[] = {
    0.045762699670360653, 0.10248651310377535, 0.016221619682872359, 0.020376629029355626, 0.022628623089137778, 0.00069317794333761831, 0.0070417373912413797
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