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
} // namespace detail
namespace temperature_model {
static constexpr int n_diurnal = 3;
static constexpr int c0_coeffs_n_annual = 3;
static constexpr double c0_coeffs[] = {
    12.728277082006315, -6.5570516323361314, -2.2877820404891387, 0.019698896729602629, 0.4792814456564109, -0.072376478667707037, 0.054237791276088507
};
static constexpr int a1_coeffs_n_annual = 3;
static constexpr double a1_coeffs[] = {
    -3.2696716466303668, 1.6314332537019745, 0.083558244314584765, 0.3749313849373912, -0.20862107002928543, -0.091275273494050901, 0.031839402975447031
};
static constexpr int b1_coeffs_n_annual = 3;
static constexpr double b1_coeffs[] = {
    -0.63629561178714877, -0.25927771529270338, -0.2282711174404766, 0.017874388724304812, -0.17913982463418618, 0.14391861983380402, 0.085317542598761925
};
static constexpr int a2_coeffs_n_annual = 3;
static constexpr double a2_coeffs[] = {
    0.56778466459303845, 0.32027125813636786, -0.16216103510655255, -0.17766615495634189, 0.08998422062157875, -0.05740699271173158, -0.0093900200750750493
};
static constexpr int b2_coeffs_n_annual = 3;
static constexpr double b2_coeffs[] = {
    -0.1469316038533528, 0.24513615605096478, 0.092847329097956954, 0.18712699372288627, 0.042589975348187668, -0.071376124472443847, -0.011786174205237561
};
static constexpr int a3_coeffs_n_annual = 3;
static constexpr double a3_coeffs[] = {
    0.097574086596800946, -0.29341162747398203, 0.038342111708110596, -0.10181556235678513, 0.054503378003764084, 0.036592657596644457, -0.02253201048564309
};
static constexpr int b3_coeffs_n_annual = 3;
static constexpr double b3_coeffs[] = {
    -0.096999726341560982, 0.2009319961379365, -0.032681396385584816, -0.04464706330353807, 0.0011922094605967535, -0.070894325127908164, -0.0014325914163772547
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
    0.0073231003479193703, -0.0021347010594143993, -0.0012533027873923594, 0.00018764476112488948, 0.00021439844216045918, -6.1432638081510024e-05, 7.8850629953794348e-05
};
static constexpr int a1_coeffs_n_annual = 3;
static constexpr double a1_coeffs[] = {
    -0.00020809821849703572, -4.9192623624357171e-05, 3.2410335548329814e-06, 4.1641387651749917e-05, 7.6299600332674898e-05, 2.856429263966981e-05, -3.6118245858694537e-05
};
static constexpr int b1_coeffs_n_annual = 3;
static constexpr double b1_coeffs[] = {
    -3.4731147344446645e-06, -0.00010621005008283427, -1.799511187670359e-05, 9.0552602259843619e-07, 1.8493195282054691e-05, 7.3529712204097758e-06, 3.2150093497979458e-06
};
static constexpr int a2_coeffs_n_annual = 3;
static constexpr double a2_coeffs[] = {
    -4.6804884799712909e-05, 0.00015778162547138919, 2.9354553709230261e-05, 1.5921685678591443e-05, -3.8107927309631622e-05, -3.5231333803389174e-05, 1.200578859141743e-06
};
static constexpr int b2_coeffs_n_annual = 3;
static constexpr double b2_coeffs[] = {
    -7.4801049709975845e-05, 2.5152197737625874e-05, 1.3433140678708899e-05, 4.6179631958756558e-05, -7.7868862196170593e-06, 1.8630526715867886e-06, -2.7387935220849853e-06
};
static constexpr int a3_coeffs_n_annual = 3;
static constexpr double a3_coeffs[] = {
    1.3853588907551904e-05, 1.5500579384535936e-06, -2.4131501090341661e-05, -5.2086424246640323e-05, 1.2283634570573128e-05, -3.3780251997692354e-08, 1.0833242252221861e-05
};
static constexpr int b3_coeffs_n_annual = 3;
static constexpr double b3_coeffs[] = {
    -1.8941885478511997e-05, 7.4110030120498144e-05, 2.1925577213680695e-06, 5.0677936382541112e-07, -7.8992177795153093e-06, -2.1215187122343512e-05, -4.0760275394620578e-06
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
    1014.0554798850156, 0.58416210567238647, 0.066582046983405668, 1.1694083670454378, 0.90077488789981974, 0.50622546591405126, 0.62829333838810875
};
static constexpr int a1_coeffs_n_annual = 3;
static constexpr double a1_coeffs[] = {
    0.082287098311462109, -0.061311411687168582, 0.028294502056899009, 0.0066698434197446661, 0.013199539564730897, 0.014693868775624518, -0.0041022941671719744
};
static constexpr int b1_coeffs_n_annual = 3;
static constexpr double b1_coeffs[] = {
    0.083159438441359909, -0.11575158459705204, 0.0099902724480110325, -0.010210601913265173, 0.018135314296603981, 0.012254416531947691, -0.014255163703673326
};
static constexpr int a2_coeffs_n_annual = 3;
static constexpr double a2_coeffs[] = {
    -0.14733644705989585, 0.085204687880787791, 0.052772928778590945, 0.066137389040378358, 0.042244328837646221, -0.0472366711474475, -0.014557482203763511
};
static constexpr int b2_coeffs_n_annual = 3;
static constexpr double b2_coeffs[] = {
    -0.40972920773745169, -0.046061337382676204, -0.024174058844819952, 0.019992428135843853, -0.021852614178145505, 0.0089918702080430367, 0.0051395114025082093
};
static constexpr int a3_coeffs_n_annual = 3;
static constexpr double a3_coeffs[] = {
    0.010086503889078119, 0.11032355612431202, -0.037899052479330807, -0.020763748764910519, -0.0013115167863614434, -0.0083814147331915528, 0.011166984260137593
};
static constexpr int b3_coeffs_n_annual = 3;
static constexpr double b3_coeffs[] = {
    0.039012074276847761, 0.10751152641222589, 0.0049365768732292092, 0.021234687750577776, 0.012588616286797558, -0.0012061494378108445, 0.0033005640081210813
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