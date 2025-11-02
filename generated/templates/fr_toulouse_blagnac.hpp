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
    14.273203768940986, -7.9732528234533238, -3.17315986565723, -0.22174192863768594, 0.81197931193453432, -0.24876243736705062, -0.093794292720808206
};
static constexpr int a1_coeffs_n_annual = 3;
static constexpr double a1_coeffs[] = {
    -3.4077743067575086, 1.4095220819663659, 0.22945030933596816, 0.3416729402854522, -0.19818952286823088, -0.04764250558021009, -0.034068162613629792
};
static constexpr int b1_coeffs_n_annual = 3;
static constexpr double b1_coeffs[] = {
    -1.3090113826530378, 0.049402127851868156, -0.10629543798642141, -0.084496839137842283, -0.36827618578299393, 0.17043515912045981, 0.12832811116760998
};
static constexpr int a2_coeffs_n_annual = 3;
static constexpr double a2_coeffs[] = {
    0.61807153913805946, 0.31243911261831375, -0.097443887958531414, -0.13280727149753943, 0.043406530376489165, -0.032715137091853662, -0.0055069740356545336
};
static constexpr int b2_coeffs_n_annual = 3;
static constexpr double b2_coeffs[] = {
    -0.10186789533328275, 0.28936996307729529, 0.090281329045171499, 0.14228785977948627, 0.062380039243881545, -0.042136722417008991, -0.022369019813519578
};
static constexpr int a3_coeffs_n_annual = 3;
static constexpr double a3_coeffs[] = {
    0.06914417026935675, -0.25410974141864606, 0.00776821268452143, -0.09193285093152706, 0.041274060421355403, 0.019428472011332222, -0.012717720752067072
};
static constexpr int b3_coeffs_n_annual = 3;
static constexpr double b3_coeffs[] = {
    -0.056232806509433332, 0.15294548695195653, -0.026872560820063333, -0.056141441486779319, 0.011875260669223332, -0.061292280731812386, -0.012799898855184335
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
    0.0075296592208835547, -0.0024995688029777609, -0.0014366898827390009, 0.00017045942474124286, 0.00018699501070839724, -0.00013106661724575223, 8.8833548206215254e-05
};
static constexpr int a1_coeffs_n_annual = 3;
static constexpr double a1_coeffs[] = {
    -3.6973579580172944e-05, -0.00020580353013791315, -0.00010430074972368885, 2.0122859652638342e-06, 0.00012086372638943422, 1.8706520454203745e-05, -2.6153675440847125e-05
};
static constexpr int b1_coeffs_n_annual = 3;
static constexpr double b1_coeffs[] = {
    5.9193344954671483e-05, -0.00021658967842252106, -4.1609380970158788e-05, -5.9826878146022444e-05, 5.8629919676639833e-05, 4.2694002381139185e-05, -1.0304792224363783e-05
};
static constexpr int a2_coeffs_n_annual = 3;
static constexpr double a2_coeffs[] = {
    -2.6151828785413197e-05, 0.00010543635403431238, 4.5213536892766142e-05, 3.4998248700709711e-05, -1.2721450268433062e-05, -1.4467814025312803e-05, -2.0488683475714929e-05
};
static constexpr int b2_coeffs_n_annual = 3;
static constexpr double b2_coeffs[] = {
    -9.693694720924929e-05, 3.4144777566788487e-05, 9.2848676504335487e-06, 5.8147031375207063e-05, 5.9983296605533135e-06, 1.0773682741683877e-07, -1.5601168618363055e-05
};
static constexpr int a3_coeffs_n_annual = 3;
static constexpr double a3_coeffs[] = {
    1.2088419197757319e-05, 3.0643125466375028e-06, -9.826881624812619e-06, -3.578774495416582e-05, -4.319093545638931e-06, -5.6984543582424068e-06, 1.0866671121785677e-05
};
static constexpr int b3_coeffs_n_annual = 3;
static constexpr double b3_coeffs[] = {
    -3.0925892378087988e-06, 5.6789680860982924e-05, -4.3376129283683256e-06, -2.0782061545338285e-06, -1.2306781289459674e-06, -1.7063243141410308e-05, -2.2696011151051767e-06
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
    999.77437796609433, 1.3922650363656408, -0.19690475211469391, 1.3239646153316473, 0.83520240445488736, 0.30484139324695769, 0.68846236583833365
};
static constexpr int a1_coeffs_n_annual = 3;
static constexpr double a1_coeffs[] = {
    0.2460079703806364, -0.17938616848557115, -0.0089999192940125004, -0.006842704980720293, 0.029903090913635601, 0.012732404020434251, -0.0094548941202014826
};
static constexpr int b1_coeffs_n_annual = 3;
static constexpr double b1_coeffs[] = {
    0.2574174189197232, -0.21045913269992245, 0.0016332256494236386, -0.0041318614126552595, 0.093701417344510485, -0.012771459233162715, -0.029435102017307025
};
static constexpr int a2_coeffs_n_annual = 3;
static constexpr double a2_coeffs[] = {
    -0.21382456675902514, 0.10675703412109076, 0.067276467916949273, 0.065009591423622351, 0.063037958880926212, -0.059376264594694507, -0.021482036576699823
};
static constexpr int b2_coeffs_n_annual = 3;
static constexpr double b2_coeffs[] = {
    -0.50019358026129601, -0.039115279882976237, -0.029185717550840148, 0.030410293004127998, -0.019688826196558581, 0.012458097536712474, 0.013502911134891457
};
static constexpr int a3_coeffs_n_annual = 3;
static constexpr double a3_coeffs[] = {
    0.020397731925699002, 0.13585356908030685, -0.034306891679725927, -0.016586600310652538, -0.009303015393704251, -0.0061917743347508335, 0.0055827593032505667
};
static constexpr int b3_coeffs_n_annual = 3;
static constexpr double b3_coeffs[] = {
    0.053868518633810702, 0.11577513888150169, 0.010930570070364106, 0.021452511824962925, 0.017160924159498058, 0.0053850954765257402, 0.0057049884613298799
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