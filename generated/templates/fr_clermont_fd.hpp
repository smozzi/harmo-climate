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
    0.0066910382619202692, -0.002580210901343223, -0.0014556896898746696, 0.00026276088398246358, 0.00019743442174298802, -0.00011228098334972263, 0.0001036535105182667
};
static constexpr int a1_coeffs_n_annual = 3;
static constexpr double a1_coeffs[] = {
    -0.00023579517574369226, 2.5442280004002394e-05, 1.5892725400426518e-05, 4.7013298317530047e-05, 6.8146320203624244e-05, 5.0430752736641901e-07, -3.4938717980344459e-05
};
static constexpr int b1_coeffs_n_annual = 3;
static constexpr double b1_coeffs[] = {
    -9.8274721959966868e-05, -2.7044850505293716e-05, 2.4129367566548244e-06, -1.571881527622761e-06, 2.1005302429688571e-05, 1.8375907188899447e-05, -1.9849703243330071e-05
};
static constexpr int a2_coeffs_n_annual = 3;
static constexpr double a2_coeffs[] = {
    -5.139090906965627e-05, 0.00015667016472965952, 1.8279707647455156e-05, -6.307318307580222e-06, -1.9031233748857721e-05, -2.3589758055446544e-05, -1.3380615419399426e-05
};
static constexpr int b2_coeffs_n_annual = 3;
static constexpr double b2_coeffs[] = {
    -7.5844256298660654e-05, 6.3289995589312174e-05, 7.4475710474145288e-06, 3.6934973968146079e-05, -4.6182898036960407e-06, -2.0342556908588848e-06, 5.6944597304713517e-07
};
static constexpr int a3_coeffs_n_annual = 3;
static constexpr double a3_coeffs[] = {
    2.9646628463081916e-05, -9.3490739618216371e-06, -1.8827954358260375e-05, -4.5026646203833748e-05, 1.8148340967051638e-06, 2.1124596506868659e-06, 1.0199056097815031e-05
};
static constexpr int b3_coeffs_n_annual = 3;
static constexpr double b3_coeffs[] = {
    -9.5849288202944196e-06, 4.7535210147644618e-05, -4.7928021102260796e-07, -7.5448226476989672e-06, 5.1968865837317241e-06, -1.6413508656800203e-05, -2.960494965892221e-06
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