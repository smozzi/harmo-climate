// Auto-generated linear harmonic climate model
// Station name : MARIGNANE
// Station code : 13054001
#pragma once
#include <cmath>
namespace harmoclimat {
static constexpr double longitude_deg = 5.2160000801086426;
static constexpr double latitude_deg = 43.437667846679688;
static constexpr double delta_utc_solar_h = 0.34773333867390954;
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
    15.976921622603196, -8.2362501892909279, -3.9604893634465395, -0.052268389055534097, 0.64163358855389196, -0.31082684295680457, -0.22417284300919801
};
static constexpr int a1_coeffs_n_annual = 3;
static constexpr double a1_coeffs[] = {
    -3.5558472350930672, 1.1244525945568762, 0.11405474797571541, 0.18688397706998552, -0.12923068710143962, -0.044636046385376098, -0.0021031334414879691
};
static constexpr int b1_coeffs_n_annual = 3;
static constexpr double b1_coeffs[] = {
    -0.83716920137140605, -0.52005097325730365, -0.29166980701282086, 0.02434266408501461, -0.32022313413279269, 0.14859444315258979, 0.098101453844835998
};
static constexpr int a2_coeffs_n_annual = 3;
static constexpr double a2_coeffs[] = {
    0.65609671455419583, 0.38975006232557907, -0.040577579809303363, -0.065322817147139833, 0.095804788372498451, -0.030026082821181483, -0.051099069097033843
};
static constexpr int b2_coeffs_n_annual = 3;
static constexpr double b2_coeffs[] = {
    -0.26088725934919216, 0.46782873704412148, 0.14614477738533396, 0.1476366916492558, 0.078074436466523858, -0.074937984562585935, -0.0054501692830644759
};
static constexpr int a3_coeffs_n_annual = 3;
static constexpr double a3_coeffs[] = {
    0.062106842408745783, -0.22276587832485348, 0.0030854081134217295, -0.097673660665518261, -0.0057249470125099551, 0.00281034738784397, -0.0078155963892732189
};
static constexpr int b3_coeffs_n_annual = 3;
static constexpr double b3_coeffs[] = {
    -0.080881694313942307, 0.18350741811176255, -0.019081621275460991, -0.060260049816978752, 0.0030157015005472152, -0.043727456474603438, -0.029725108309693039
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
    0.0075604003457249609, -0.0025255292093532885, -0.001851064479853529, 1.6912934978781311e-05, 0.00022413123123999739, -0.00015306991323827372, 2.6708244035159711e-05
};
static constexpr int a1_coeffs_n_annual = 3;
static constexpr double a1_coeffs[] = {
    -0.00014003660760009549, -0.00019312396013059914, -1.5490068004165049e-05, 9.1569256127088834e-05, 9.6678266562379468e-05, 3.0178347900268348e-06, -2.9405605454871531e-05
};
static constexpr int b1_coeffs_n_annual = 3;
static constexpr double b1_coeffs[] = {
    -0.00013384226135565656, 2.1655263661833482e-07, 5.2841998916211993e-05, -1.0212325047206258e-05, -7.4273875179112962e-05, 1.3065145542316681e-06, 1.5651985717829192e-05
};
static constexpr int a2_coeffs_n_annual = 3;
static constexpr double a2_coeffs[] = {
    -7.5473207201575261e-05, 0.00016840449085743007, 4.5046652122427783e-05, 1.1455180578621303e-05, -3.5113266877877667e-05, -1.860042775165082e-05, -1.2319596258901414e-06
};
static constexpr int b2_coeffs_n_annual = 3;
static constexpr double b2_coeffs[] = {
    -8.6855216015042409e-05, 6.2666608999138322e-05, 1.5055906955065959e-06, 1.8421334763116921e-05, -5.5217931207757211e-06, -4.7056946559639484e-06, 6.5166335343087286e-06
};
static constexpr int a3_coeffs_n_annual = 3;
static constexpr double a3_coeffs[] = {
    2.4894533850654691e-05, 1.2337184589099337e-05, -2.3975878905178756e-05, -4.6036586878579037e-05, 6.0465373610546005e-06, 2.1392862385836911e-06, 2.9220483079198833e-07
};
static constexpr int b3_coeffs_n_annual = 3;
static constexpr double b3_coeffs[] = {
    -4.0830635109061685e-06, 5.6107386857642725e-05, 2.3874022167878438e-06, -7.1384869540116707e-06, 7.0105856620330632e-06, -9.6619316147325957e-06, -8.4782209632998923e-06
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
    1012.1520112261381, 1.6819275508693103, -0.12169362761267637, 0.67437518557166798, 0.51283434039434916, 0.054537613009533588, 0.73312320031476097
};
static constexpr int a1_coeffs_n_annual = 3;
static constexpr double a1_coeffs[] = {
    0.22969398397470733, -0.05457854464422663, 0.007209162090751264, -0.01310271130546083, 0.00039204427018928379, -0.00026212182672091122, -0.0080411665680983849
};
static constexpr int b1_coeffs_n_annual = 3;
static constexpr double b1_coeffs[] = {
    0.19412381049759359, -0.068637955480287574, 0.015502123857546433, 0.0062363884830176205, 0.081153253144783583, -0.017193084189585164, -0.021031183444105272
};
static constexpr int a2_coeffs_n_annual = 3;
static constexpr double a2_coeffs[] = {
    -0.14580662997359187, 0.11392351993936693, 0.072214861438987657, 0.057150375847171739, 0.056914896792344037, -0.042358954171343464, -0.025019582303148588
};
static constexpr int b2_coeffs_n_annual = 3;
static constexpr double b2_coeffs[] = {
    -0.51049450367134619, -0.063467805015511997, -0.040208082716364266, 0.039815255764713807, -0.010536638957041029, 0.010383838527260348, 0.015142266050099893
};
static constexpr int a3_coeffs_n_annual = 3;
static constexpr double a3_coeffs[] = {
    0.011749852445263567, 0.12722728651173065, -0.0256501829316097, -0.016991959147538951, -0.01227979738934956, -0.008153783739286621, 0.0065041555026264684
};
static constexpr int b3_coeffs_n_annual = 3;
static constexpr double b3_coeffs[] = {
    0.05502146937787053, 0.11785249501480845, 0.022622382447157682, 0.02317966067672346, 0.021194549849769803, -0.0047302591414366589, 0.005584701853400203
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