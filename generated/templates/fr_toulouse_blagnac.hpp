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
    0.0075353171250303677, -0.0024899673822610584, -0.0014588229419435507, 0.00016693415402077068, 0.00018958600416560723, -0.00013193549909273667, 8.5735798539370714e-05
};
static constexpr int a1_coeffs_n_annual = 3;
static constexpr double a1_coeffs[] = {
    -3.6572463728386862e-05, -0.00020656428721388436, -0.00010698542090191677, 4.0424311651503727e-07, 0.0001215165612912826, 1.9473560156746554e-05, -2.5746777741022318e-05
};
static constexpr int b1_coeffs_n_annual = 3;
static constexpr double b1_coeffs[] = {
    5.9250139753521842e-05, -0.00021702436948117972, -4.3696164196684281e-05, -6.0614227225473144e-05, 5.7689588696917999e-05, 4.2925204158639769e-05, -9.0478222533984631e-06
};
static constexpr int a2_coeffs_n_annual = 3;
static constexpr double a2_coeffs[] = {
    -2.6330706929326574e-05, 0.00010554714471872822, 4.6384179932272669e-05, 3.5284695911092915e-05, -1.2207769811892153e-05, -1.398137191402033e-05, -2.0950133922096568e-05
};
static constexpr int b2_coeffs_n_annual = 3;
static constexpr double b2_coeffs[] = {
    -9.7119174013727693e-05, 3.4265301541193055e-05, 9.623086078220392e-06, 5.81729482885295e-05, 6.9642346103754229e-06, 4.5961601220268219e-07, -1.5577741576715288e-05
};
static constexpr int a3_coeffs_n_annual = 3;
static constexpr double a3_coeffs[] = {
    1.2106997300734084e-05, 3.1311075823715389e-06, -9.8320992573328759e-06, -3.5814808009684964e-05, -4.9568004960134888e-06, -5.9707156401124964e-06, 1.0752646027876166e-05
};
static constexpr int b3_coeffs_n_annual = 3;
static constexpr double b3_coeffs[] = {
    -3.0742445785684322e-06, 5.686437727293238e-05, -3.8613374623090607e-06, -2.0376821337420613e-06, -1.2370126815347032e-06, -1.7044579381765469e-05, -2.7116145070928456e-06
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