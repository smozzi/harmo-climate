// Auto-generated linear harmonic climate model
// Station name : BOURGES
// Station code : 18033001
#pragma once
#include <cmath>
namespace harmoclimat {
static constexpr double longitude_deg = 2.3598330020904541;
static constexpr double latitude_deg = 47.059165954589844;
static constexpr double delta_utc_solar_h = 0.15732220013936357;
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
    12.338807848512918, -7.89511496329572, -2.8664861125208452, -0.038085889551782964, 0.642197228613684, -0.10795280013804051, -0.11826141892538143
};
static constexpr int a1_coeffs_n_annual = 3;
static constexpr double a1_coeffs[] = {
    -3.327185365829477, 1.7318547144339247, 0.11812444237163125, 0.53970946737398806, -0.11779840815050871, -0.056970950576891756, -0.0015906981711272252
};
static constexpr int b1_coeffs_n_annual = 3;
static constexpr double b1_coeffs[] = {
    -1.1909559035272153, 0.12530008561231806, -0.21365694030363402, 0.14045758258788665, -0.32125638343559398, 0.17496958683258418, 0.15845789735933585
};
static constexpr int a2_coeffs_n_annual = 3;
static constexpr double a2_coeffs[] = {
    0.53365426333994581, 0.33355450728933739, -0.10206550255883853, -0.27783268166756847, 0.042401552685402681, -0.056747849111780942, 0.0089504264652532674
};
static constexpr int b2_coeffs_n_annual = 3;
static constexpr double b2_coeffs[] = {
    -0.12147522561735874, 0.33133574428394263, 0.076232528883039399, 0.14960526666604232, 0.073333642313974379, -0.091566032668426306, -0.021121110299604298
};
static constexpr int a3_coeffs_n_annual = 3;
static constexpr double a3_coeffs[] = {
    0.13360760884653633, -0.33387818380809769, 0.006858696383891463, -0.086642903455233164, 0.041652141625699295, 0.051149845714328207, -0.024526398583880497
};
static constexpr int b3_coeffs_n_annual = 3;
static constexpr double b3_coeffs[] = {
    -0.097892865838344167, 0.16684997833774642, -0.016169111237799509, -0.06368886734143725, 0.014984397517899144, -0.067047251611740133, -0.010977055786785116
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
    0.0069604599006852473, -0.0023167084843625394, -0.0013410890028829258, 0.00020968520886838256, 0.00013727998504971034, -9.3189672811208593e-05, 0.00014362989958518146
};
static constexpr int a1_coeffs_n_annual = 3;
static constexpr double a1_coeffs[] = {
    -0.00012983473383927193, -0.00013876917265749668, 1.1863437668638503e-06, 9.0896515217432552e-05, 9.7215812956135458e-05, 1.8531269079934721e-05, -4.9152726448257133e-05
};
static constexpr int b1_coeffs_n_annual = 3;
static constexpr double b1_coeffs[] = {
    -4.3357141271630846e-05, -0.0001093792864655147, -2.9017873891505065e-05, 7.1846312303052002e-06, 6.0630400164383747e-05, 3.3922113646060659e-05, -3.9830558390416435e-05
};
static constexpr int a2_coeffs_n_annual = 3;
static constexpr double a2_coeffs[] = {
    -6.4133382044605943e-05, 0.00017074761989658533, 1.8477293148547472e-05, -6.3027407707647332e-07, -1.8908590769567166e-05, -3.3048695882100742e-05, -1.5141882577950188e-05
};
static constexpr int b2_coeffs_n_annual = 3;
static constexpr double b2_coeffs[] = {
    -9.0803668218970301e-05, 7.4017918067064624e-05, 2.0808231148218941e-05, 5.208025900319935e-05, -1.5679268164018562e-05, -8.9355798892647654e-06, -2.4451098913415162e-06
};
static constexpr int a3_coeffs_n_annual = 3;
static constexpr double a3_coeffs[] = {
    2.4796358907464147e-05, -1.0677452622348264e-05, -1.7809383742504158e-05, -5.5894874798252904e-05, 1.1336717892249992e-06, 9.0738518170069225e-06, 1.1031146460812619e-05
};
static constexpr int b3_coeffs_n_annual = 3;
static constexpr double b3_coeffs[] = {
    -9.6293565612573878e-06, 5.7282768636392115e-05, -1.7201072130977549e-06, -1.348750620429203e-05, 4.0579841426257001e-06, -2.2043195888718741e-05, -5.2772445040500382e-06
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
    997.61887887472733, 0.83064991674180311, -0.20209418541396684, 0.96424794939671565, 0.8442392344915941, 0.29190107454166325, 0.70086970714207597
};
static constexpr int a1_coeffs_n_annual = 3;
static constexpr double a1_coeffs[] = {
    0.11456906410352072, -0.069792408025781044, 0.02173731230224743, 0.0014580112759522252, 0.0068823513858044926, -0.0036011482316236302, 0.0014371995573136081
};
static constexpr int b1_coeffs_n_annual = 3;
static constexpr double b1_coeffs[] = {
    0.23087288478425799, -0.20793548282849583, 0.016628193927413642, -0.018821993994660557, 0.050907345220911629, -0.011530439091801835, -0.03865346793437708
};
static constexpr int a2_coeffs_n_annual = 3;
static constexpr double a2_coeffs[] = {
    -0.13612934104677191, 0.09211173270560867, 0.045179994527902952, 0.060801813687242925, 0.050559575521025697, -0.038639422496962082, -0.023252809075209973
};
static constexpr int b2_coeffs_n_annual = 3;
static constexpr double b2_coeffs[] = {
    -0.41524727666102751, -0.02163850934974924, -0.017139063885668177, 0.043345223740375124, -0.013025790598364045, 0.0077810079066007124, 0.018317508635038171
};
static constexpr int a3_coeffs_n_annual = 3;
static constexpr double a3_coeffs[] = {
    0.01365014262668493, 0.10892327999629844, -0.031260868050049198, -0.016506635307072522, -0.0099353627891565384, -0.0035920918656500199, 0.0075978561696280504
};
static constexpr int b3_coeffs_n_annual = 3;
static constexpr double b3_coeffs[] = {
    0.04482463092214254, 0.10259286900160697, 0.0071780604897465801, 0.018530361107804347, 0.011816901951292749, -0.0017664275663660535, 0.0044661674516530223
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