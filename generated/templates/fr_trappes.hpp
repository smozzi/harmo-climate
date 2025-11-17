// Auto-generated linear harmonic climate model
// Station name : TRAPPES
// Station code : 78621001
#pragma once
#include <cmath>
namespace harmoclimat {
static constexpr double longitude_deg = 2.0098330974578857;
static constexpr double latitude_deg = 48.774333953857422;
static constexpr double delta_utc_solar_h = 0.13398887316385899;
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
    11.638417248198536, -7.2860402773670989, -2.7651352112209611, -0.044153115418846034, 0.54290889358358851, -0.047516072076165219, -0.12232928437787799
};
static constexpr int a1_coeffs_n_annual = 3;
static constexpr double a1_coeffs[] = {
    -2.839000559868365, 1.6734541579735218, 0.059233616397449812, 0.46235861796508015, -0.1037034748290309, -0.11561928168502522, 0.021297686204226111
};
static constexpr int b1_coeffs_n_annual = 3;
static constexpr double b1_coeffs[] = {
    -0.98053122487262179, 0.18320825510568561, -0.24676939415561053, 0.055979845320789659, -0.22071605296215802, 0.1616350384356478, 0.16324906395807348
};
static constexpr int a2_coeffs_n_annual = 3;
static constexpr double a2_coeffs[] = {
    0.42981410654745716, 0.25065175764058034, -0.096718765329682183, -0.21782099742015315, 0.04199690553927779, -0.039791547872909894, 0.0085185900201664739
};
static constexpr int b2_coeffs_n_annual = 3;
static constexpr double b2_coeffs[] = {
    -0.070512021315172854, 0.21486057017803328, 0.062262802104403832, 0.12157531804135027, 0.065871171311746926, -0.072472988975499725, -0.012494750406889023
};
static constexpr int a3_coeffs_n_annual = 3;
static constexpr double a3_coeffs[] = {
    0.09998240569426288, -0.2632606549854361, 0.020235906674844609, -0.072906854476621, 0.034245534957051502, 0.052417981865384045, -0.018903657968694802
};
static constexpr int b3_coeffs_n_annual = 3;
static constexpr double b3_coeffs[] = {
    -0.082142484795762749, 0.12768763536910713, -0.0028997851935377498, -0.025667486586360259, 0.0038051630985164293, -0.058332169375628309, -0.02437001635066912
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
    0.0068005862940739067, -0.0021304285251582088, -0.001389986815263323, 0.00019692179796021293, 0.0002155967587428337, -8.603328057260872e-05, 0.00011213710672761077
};
static constexpr int a1_coeffs_n_annual = 3;
static constexpr double a1_coeffs[] = {
    -8.3382850456719963e-05, -7.715436264340783e-05, 1.6812623462797578e-05, 4.5080992569175968e-05, 5.7193741360969521e-05, 3.4004245224247018e-05, -4.4703994959043345e-05
};
static constexpr int b1_coeffs_n_annual = 3;
static constexpr double b1_coeffs[] = {
    -1.0016437650566515e-05, -8.3414000072070413e-05, -3.815796026069602e-05, -1.2612996898010653e-05, 5.473813326469819e-05, 2.8067302136306971e-05, -2.7884234323971044e-05
};
static constexpr int a2_coeffs_n_annual = 3;
static constexpr double a2_coeffs[] = {
    -5.1084080450355681e-05, 0.00011324876739698698, 1.9936187501615427e-05, 1.1526552122272021e-05, -1.9886471275632613e-05, -2.7011720711988132e-05, -2.0299318779896269e-06
};
static constexpr int b2_coeffs_n_annual = 3;
static constexpr double b2_coeffs[] = {
    -7.2076591352867694e-05, 5.9358805967690092e-05, 1.2421369929023666e-05, 2.5957559928504654e-05, -1.0471148668422828e-05, 9.3278673326979941e-07, 2.0218589313517231e-06
};
static constexpr int a3_coeffs_n_annual = 3;
static constexpr double a3_coeffs[] = {
    6.6227806694973282e-06, 2.1951158767815458e-06, -2.0757111825204764e-05, -3.2914842922707291e-05, 6.3193461473204465e-06, -3.1901558321197801e-07, 8.5487381841260982e-06
};
static constexpr int b3_coeffs_n_annual = 3;
static constexpr double b3_coeffs[] = {
    -8.7430492538022857e-06, 4.1827271656114851e-05, 7.8500670537736448e-07, -3.7511369891642077e-06, -2.5394338648402946e-06, -1.4173285748250501e-05, -1.0911011440097635e-06
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
    996.34802359794583, 0.1817794710385949, -0.069285545285264122, 0.6805358971085389, 0.80692168576019663, 0.31954170217171252, 0.71621426481975425
};
static constexpr int a1_coeffs_n_annual = 3;
static constexpr double a1_coeffs[] = {
    0.081588345917747815, -0.043450423472505044, 0.025922188800005089, 0.022225491894518355, 0.019609073035294196, 0.005752824145672613, -0.01058573273522664
};
static constexpr int b1_coeffs_n_annual = 3;
static constexpr double b1_coeffs[] = {
    0.13241835467173196, -0.16834656798657935, 0.019764090767331571, -0.006019910185220018, 0.015775664291959549, -0.0039141948608743763, -0.022725364606522257
};
static constexpr int a2_coeffs_n_annual = 3;
static constexpr double a2_coeffs[] = {
    -0.11592167848260827, 0.082384994071363488, 0.043596747990207262, 0.055389815411893438, 0.049544608936017434, -0.040743612418548944, -0.016451893676419634
};
static constexpr int b2_coeffs_n_annual = 3;
static constexpr double b2_coeffs[] = {
    -0.38477786997741414, -0.0018242037542893622, -0.02638308524206073, 0.038745279129351606, -0.015271440622232041, 0.001800844311250874, 0.01976920427997169
};
static constexpr int a3_coeffs_n_annual = 3;
static constexpr double a3_coeffs[] = {
    0.010142376639071325, 0.10344404168348978, -0.028423597259763504, -0.015539907388401867, -0.0067027125197090731, -0.0031850941944694595, 0.00956363611922415
};
static constexpr int b3_coeffs_n_annual = 3;
static constexpr double b3_coeffs[] = {
    0.043032668204778239, 0.098163282425223958, 0.0072316965729976061, 0.011722747448364908, 0.011709938787653864, -0.0037337287000053067, 0.0076480186140743308
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