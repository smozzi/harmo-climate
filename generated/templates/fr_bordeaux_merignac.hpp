// Auto-generated linear harmonic climate model
// Station name : BORDEAUX-MERIGNAC
// Station code : 33281001
#pragma once
#include <cmath>
namespace harmoclimat {
static constexpr double longitude_deg = -0.69133299589157104;
static constexpr double latitude_deg = 44.830665588378906;
static constexpr double delta_utc_solar_h = -0.046088866392771419;
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
    14.091954601423192, -7.2651833492655769, -2.510297819740646, -0.19909384163562732, 0.61515482184630477, -0.11312755146137862, 0.021145471976450687
};
static constexpr int a1_coeffs_n_annual = 3;
static constexpr double a1_coeffs[] = {
    -3.5664500588510721, 1.5342126173513764, 0.15045546086339837, 0.42621571067415209, -0.2399936003139396, -0.042092642719350065, 0.012066255736904936
};
static constexpr int b1_coeffs_n_annual = 3;
static constexpr double b1_coeffs[] = {
    -0.91856981661202841, -0.31628339115543946, -0.26973687490332987, 0.047285260128342083, -0.24551620598773291, 0.19096021605094887, 0.061083801465847014
};
static constexpr int a2_coeffs_n_annual = 3;
static constexpr double a2_coeffs[] = {
    0.62210158759138423, 0.36012015468534203, -0.13349298452779518, -0.16625692671125206, 0.048021971697424393, -0.055014147871071631, -0.0094228485518988749
};
static constexpr int b2_coeffs_n_annual = 3;
static constexpr double b2_coeffs[] = {
    -0.067458193850883502, 0.28561170339701042, 0.099078721637260073, 0.18952747168283182, 0.048470967064381608, -0.075795563719496709, 0.0020424657523620158
};
static constexpr int a3_coeffs_n_annual = 3;
static constexpr double a3_coeffs[] = {
    0.13865601926119336, -0.30808326338152275, 0.020432100640686523, -0.108367774561864, 0.054725354317498351, 0.032811315335464615, -0.011262332145347876
};
static constexpr int b3_coeffs_n_annual = 3;
static constexpr double b3_coeffs[] = {
    -0.10984556355053653, 0.18656564665447442, 0.005252286018547333, -0.05206127460889054, 0.010352948550337904, -0.079910840473197228, -0.0048252303717978917
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
    0.007649961621689425, -0.002396507970186331, -0.0014062018259364702, 0.00020460854036237085, 0.00022813006017178764, -0.00010515755512967821, 0.00011234683174574294
};
static constexpr int a1_coeffs_n_annual = 3;
static constexpr double a1_coeffs[] = {
    -2.7420953772008907e-05, -0.00023562967113242541, 3.1388281755590984e-05, 2.8747920890525159e-05, 9.0643331388201215e-05, 5.5559286857276628e-05, -5.4554122909011473e-05
};
static constexpr int b1_coeffs_n_annual = 3;
static constexpr double b1_coeffs[] = {
    -1.8646790448206297e-05, -0.00014172880407620699, 2.4111586871540753e-06, -1.8126767160890124e-05, 3.1384316275175093e-05, 1.8284190103935187e-05, -4.2751066706081118e-06
};
static constexpr int a2_coeffs_n_annual = 3;
static constexpr double a2_coeffs[] = {
    -7.8850429579715935e-05, 0.00014717058883153544, 1.9290726117070402e-05, 3.9979719001424585e-05, -2.3483556472406636e-05, -3.4212593270687904e-05, -7.4302322334884695e-06
};
static constexpr int b2_coeffs_n_annual = 3;
static constexpr double b2_coeffs[] = {
    -8.7345648499410054e-05, 4.40141180722934e-05, 7.7598936872056135e-06, 4.4399401518438587e-05, -2.3763779277520093e-05, 8.9788202235845947e-06, -5.5134121563353565e-06
};
static constexpr int a3_coeffs_n_annual = 3;
static constexpr double a3_coeffs[] = {
    1.4290969990790096e-05, 2.4181898393009668e-05, -2.522898405268541e-05, -5.4228516636076906e-05, 1.2597023362926789e-06, -2.7006766744456326e-06, 1.3231366095735413e-05
};
static constexpr int b3_coeffs_n_annual = 3;
static constexpr double b3_coeffs[] = {
    -5.4597485059310473e-06, 6.7535823639847524e-05, 2.3604508824859471e-06, -3.7832697443938625e-06, -5.8610886562915867e-07, -2.0210093448171725e-05, 2.4689829456808144e-06
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
    1011.7632789683447, 1.1596896356679114, -0.090481980278960636, 1.4318184287374842, 0.88407386485626172, 0.38984039694638106, 0.6415153592593027
};
static constexpr int a1_coeffs_n_annual = 3;
static constexpr double a1_coeffs[] = {
    0.10606776436600399, -0.10609637603250927, 0.032368831850169526, -0.0058477472083994734, 0.029620391342078754, 0.0081294754441164473, -0.010960784239496502
};
static constexpr int b1_coeffs_n_annual = 3;
static constexpr double b1_coeffs[] = {
    0.10064850484269709, -0.096325825533711176, 0.019739535228711136, -0.025688355733522898, 0.045179474655752491, -0.00062753396166169103, -0.011387345711031399
};
static constexpr int a2_coeffs_n_annual = 3;
static constexpr double a2_coeffs[] = {
    -0.18876193476607112, 0.11010546508697881, 0.054952471136494006, 0.065491591896496379, 0.054545390253723694, -0.054414839900112193, -0.01128636908270933
};
static constexpr int b2_coeffs_n_annual = 3;
static constexpr double b2_coeffs[] = {
    -0.46585922173830802, -0.029244029379786495, -0.028063536707545324, 0.022123373488084293, -0.018678544363987387, 0.011307275409677273, 0.014994570714904131
};
static constexpr int a3_coeffs_n_annual = 3;
static constexpr double a3_coeffs[] = {
    0.013257860443104879, 0.13176921661496846, -0.03505291265032328, -0.021638859578751359, -0.0030720941060056023, -0.0086040934563004941, 0.0055066043599940433
};
static constexpr int b3_coeffs_n_annual = 3;
static constexpr double b3_coeffs[] = {
    0.04955250746588228, 0.10687439243620596, 0.01090092134966437, 0.022390682949335345, 0.013318111272195869, 0.004293214172771999, 0.0071973309476409796
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