// Auto-generated linear harmonic climate model
// Station name : NANTES-BOUGUENAIS
// Station code : 44020001
#pragma once
#include <cmath>
namespace harmoclimat {
static constexpr double longitude_deg = -1.6088329553604126;
static constexpr double latitude_deg = 47.150001525878906;
static constexpr double delta_utc_solar_h = -0.10725553035736081;
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
    12.728188206466713, -6.5759573968034308, -2.2328929803296642, 0.026901854540639952, 0.47977742977637733, -0.072416438257494456, 0.056231277674891812
};
static constexpr int a1_coeffs_n_annual = 3;
static constexpr double a1_coeffs[] = {
    -3.2696381500789888, 1.6318412486242817, 0.069892122132642565, 0.37128646269444548, -0.21500255298126883, -0.09076306075568033, 0.033541319871650696
};
static constexpr int b1_coeffs_n_annual = 3;
static constexpr double b1_coeffs[] = {
    -0.6362870171511249, -0.26132955014861042, -0.22598097854309879, 0.01477844338869678, -0.17970744494656998, 0.1457953459673062, 0.081696877447844318
};
static constexpr int a2_coeffs_n_annual = 3;
static constexpr double a2_coeffs[] = {
    0.56778215458165993, 0.31894791157256613, -0.16483897621341201, -0.17592492543221908, 0.092985983405035125, -0.057577757221309582, -0.0079860339504522725
};
static constexpr int b2_coeffs_n_annual = 3;
static constexpr double b2_coeffs[] = {
    -0.14691886787565045, 0.24584955990744067, 0.090787102941803116, 0.18786324216547876, 0.039452856953069299, -0.071698482563388358, -0.0099116177181879702
};
static constexpr int a3_coeffs_n_annual = 3;
static constexpr double a3_coeffs[] = {
    0.097571308799922624, -0.29302838742285364, 0.040829558878322597, -0.10090847557556169, 0.056251114510049202, 0.036019355768959031, -0.023230179813063277
};
static constexpr int b3_coeffs_n_annual = 3;
static constexpr double b3_coeffs[] = {
    -0.096997577564912812, 0.20068306219730489, -0.034371174906326941, -0.044568805293161803, 0.0020083119328543867, -0.070864662766263145, 0.00020834504426741439
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
    0.007317356589511782, -0.0021428146230863137, -0.0012339204793233266, 0.00019154344570926601, 0.0002112502539743432, -6.0480225724089841e-05, 8.0001849151675701e-05
};
static constexpr int a1_coeffs_n_annual = 3;
static constexpr double a1_coeffs[] = {
    -0.00020783031921995602, -4.8226276483205764e-05, 4.0072315957022046e-06, 4.2298881582385062e-05, 7.5159516816080616e-05, 2.7587301021684381e-05, -3.6613862775745267e-05
};
static constexpr int b1_coeffs_n_annual = 3;
static constexpr double b1_coeffs[] = {
    -3.3600005532814955e-06, -0.00010608025483155186, -1.7019448991096372e-05, 1.1585864248856673e-06, 1.8532593923249521e-05, 7.4168367215422989e-06, 2.9401665655888251e-06
};
static constexpr int a2_coeffs_n_annual = 3;
static constexpr double a2_coeffs[] = {
    -4.6673617920456229e-05, 0.00015748513920670664, 2.7896681465742833e-05, 1.5292995370168001e-05, -3.8217077905762852e-05, -3.5032447741167241e-05, 2.0274282192789244e-06
};
static constexpr int b2_coeffs_n_annual = 3;
static constexpr double b2_coeffs[] = {
    -7.4676143637981759e-05, 2.514737694970404e-05, 1.3208638827637658e-05, 4.591196286174164e-05, -8.5774576223200186e-06, 1.7513398449223589e-06, -2.7954266562197526e-06
};
static constexpr int a3_coeffs_n_annual = 3;
static constexpr double a3_coeffs[] = {
    1.3821778866868985e-05, 1.3936769334400967e-06, -2.4111678050003162e-05, -5.1695153655661368e-05, 1.3155944015673549e-05, 2.1908313103286284e-07, 1.0825370723193384e-05
};
static constexpr int b3_coeffs_n_annual = 3;
static constexpr double b3_coeffs[] = {
    -1.8918687553151817e-05, 7.4027669708326543e-05, 1.6008006915513118e-06, 4.080146472144332e-07, -7.9030335957658927e-06, -2.1274477219679633e-05, -3.5753633243438755e-06
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
    1014.0555875863689, 0.58785096639685908, 0.062131741968042456, 1.1848749675165837, 0.88078523642822926, 0.51660596623448285, 0.6084287041850952
};
static constexpr int a1_coeffs_n_annual = 3;
static constexpr double a1_coeffs[] = {
    0.082283028074959713, -0.061099151923442285, 0.028806194476754289, 0.0068556736577496174, 0.013019705223840555, 0.014948612171541432, -0.0044940417478592934
};
static constexpr int b1_coeffs_n_annual = 3;
static constexpr double b1_coeffs[] = {
    0.083158730106470388, -0.11549874874769715, 0.010914573984519133, -0.0096449755598636746, 0.018348224298548762, 0.01202743336282743, -0.014566602125218053
};
static constexpr int a2_coeffs_n_annual = 3;
static constexpr double a2_coeffs[] = {
    -0.14733614875796416, 0.085638489941110854, 0.052064377859354673, 0.066796487903533375, 0.041168812022946893, -0.047647277081548978, -0.013347740187199504
};
static constexpr int b2_coeffs_n_annual = 3;
static constexpr double b2_coeffs[] = {
    -0.40973114116092502, -0.046310208584758601, -0.023765926672741303, 0.019631666581160164, -0.02217001038490676, 0.0092316240369627062, 0.0047901437781747878
};
static constexpr int a3_coeffs_n_annual = 3;
static constexpr double a3_coeffs[] = {
    0.010086157537844233, 0.10998527621068648, -0.038852586041378603, -0.020786025076014, -0.00097881663538641555, -0.0080508492097825027, 0.011311781190370199
};
static constexpr int b3_coeffs_n_annual = 3;
static constexpr double b3_coeffs[] = {
    0.039011667861301516, 0.10755695455091718, 0.0040022363265778734, 0.021476806456962778, 0.012146072440122512, -0.0010389882586628095, 0.0032831325385156971
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