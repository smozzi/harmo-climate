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
} // namespace detail
namespace temperature_model {
static constexpr int n_diurnal = 3;
static constexpr int c0_coeffs_n_annual = 3;
static constexpr double c0_coeffs[] = {
    12.30099489076159, -7.8171304352020972, -3.1641652776892522, -0.091110819089788575, 0.57994179783641409, -0.23432164805799008, -0.14623253612938589
};
static constexpr int a1_coeffs_n_annual = 3;
static constexpr double a1_coeffs[] = {
    -3.6650372855359095, 1.6858412120370794, 0.15511378307581614, 0.43938844607387106, -0.16752514525705034, -0.038827984759631624, 0.0597413862412026
};
static constexpr int b1_coeffs_n_annual = 3;
static constexpr double b1_coeffs[] = {
    -1.005441989175917, -0.13632843711435705, -0.25999058795564739, 0.14425476092685813, -0.30081814496421044, 0.18112230024663836, 0.17693990189263056
};
static constexpr int a2_coeffs_n_annual = 3;
static constexpr double a2_coeffs[] = {
    0.67615778957852479, 0.445145552583808, -0.11707364842899838, -0.24799901807403302, 0.032748071973354977, -0.060436976844267229, 0.01421668522170938
};
static constexpr int b2_coeffs_n_annual = 3;
static constexpr double b2_coeffs[] = {
    -0.11513112794465272, 0.33869200603529143, 0.1140771650575733, 0.14455348893029837, 0.11961372636338484, -0.082930431037294416, -0.035629888075942445
};
static constexpr int a3_coeffs_n_annual = 3;
static constexpr double a3_coeffs[] = {
    0.12651512817499019, -0.32707467121190359, 0.020970732665651682, -0.10679376723473101, 0.05482088803717295, 0.030881346995497397, -0.020629891560387022
};
static constexpr int b3_coeffs_n_annual = 3;
static constexpr double b3_coeffs[] = {
    -0.10739034978866981, 0.16961692272551146, 0.011091420673083358, -0.071345583672169158, 0.0013438533306240504, -0.065648276630958777, -0.028561873355865918
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
    0.0066648872509572328, -0.0025696069991655887, -0.0014497644262817667, 0.00026151457339520883, 0.00019639518146904775, -0.00011182338800299759, 0.00010332229313386738
};
static constexpr int a1_coeffs_n_annual = 3;
static constexpr double a1_coeffs[] = {
    -0.00023448645964061545, 2.4830548938100736e-05, 1.5614383708005331e-05, 4.6932547851004543e-05, 6.8021597425349349e-05, 4.9942343394194343e-07, -3.4831091144044432e-05
};
static constexpr int b1_coeffs_n_annual = 3;
static constexpr double b1_coeffs[] = {
    -9.7838263941400766e-05, -2.7024725292671395e-05, 2.3676989122101803e-06, -1.5471585515423866e-06, 2.0955418951576934e-05, 1.8305361151115736e-05, -1.9791856911804095e-05
};
static constexpr int a2_coeffs_n_annual = 3;
static constexpr double a2_coeffs[] = {
    -5.1260587427377012e-05, 0.00015611585422540067, 1.8257774514222107e-05, -6.2475266852199287e-06, -1.8966812738702704e-05, -2.351114232725779e-05, -1.3339633724112038e-05
};
static constexpr int b2_coeffs_n_annual = 3;
static constexpr double b2_coeffs[] = {
    -7.5537952053283247e-05, 6.3027236418923763e-05, 7.4015165355561946e-06, 3.6797544063357267e-05, -4.598539036494324e-06, -2.0209734165473755e-06, 5.7520835466471105e-07
};
static constexpr int a3_coeffs_n_annual = 3;
static constexpr double a3_coeffs[] = {
    2.9514335761588115e-05, -9.2736562565436439e-06, -1.8747867625061469e-05, -4.4872962642085477e-05, 1.7916021705400455e-06, 2.0992526170591165e-06, 1.0165842815960713e-05
};
static constexpr int b3_coeffs_n_annual = 3;
static constexpr double b3_coeffs[] = {
    -9.5357213252986005e-06, 4.7337838456283628e-05, -4.7982110939399861e-07, -7.5071618206245071e-06, 5.1840783760800814e-06, -1.6354201903322267e-05, -2.9526712945127425e-06
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
    978.65786920274377, 0.72521855788441369, -0.47371288531043454, 0.94797268275477098, 0.87584401371325571, 0.19830517189936925, 0.70525577779256998
};
static constexpr int a1_coeffs_n_annual = 3;
static constexpr double a1_coeffs[] = {
    0.29983349329006126, -0.15755492830800569, 0.027019347712323151, -0.02889118158083551, 0.0048173519156862682, -0.0041809315730384367, 5.040985756168611e-05
};
static constexpr int b1_coeffs_n_annual = 3;
static constexpr double b1_coeffs[] = {
    0.2180751710110943, -0.16272327264693734, 0.016650893138201402, -0.037686079603497923, 0.068688684856653692, -0.017712932104642892, -0.045060266812608357
};
static constexpr int a2_coeffs_n_annual = 3;
static constexpr double a2_coeffs[] = {
    -0.19882485318010112, 0.10608881940530439, 0.053439897014915666, 0.065486544100337982, 0.049164024969669075, -0.039712286191108476, -0.030443051181760487
};
static constexpr int b2_coeffs_n_annual = 3;
static constexpr double b2_coeffs[] = {
    -0.43339032189868554, -0.036978034493630373, -0.02913333866526252, 0.037664116896050169, -0.021259490801707161, 0.016220478292694754, 0.019075824006090664
};
static constexpr int a3_coeffs_n_annual = 3;
static constexpr double a3_coeffs[] = {
    0.01719158064338365, 0.1232963533487218, -0.032189585897430845, -0.011894055308851495, -0.013552042252148937, -0.0059322774195650256, 0.0058914096501475948
};
static constexpr int b3_coeffs_n_annual = 3;
static constexpr double b3_coeffs[] = {
    0.045759718426940529, 0.10235232173762698, 0.017047656651176656, 0.019987627996127313, 0.022947475231462323, 0.00041720651708782762, 0.0069805096073949306
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