// Auto-generated linear harmonic climate model
// Station name : PARIS-MONTSOURIS
// Station code : 75114001
#pragma once
#include <cmath>
namespace harmoclimat {
static constexpr double longitude_deg = 2.3378329277038574;
static constexpr double latitude_deg = 48.821666717529297;
static constexpr double delta_utc_solar_h = 0.15585552851359052;
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
    12.908384641266181, -7.4693690553522734, -2.7019859203583478, -0.055185256742790088, 0.55768836084732776, -0.019379718784389113, -0.14268349174330863
};
static constexpr int a1_coeffs_n_annual = 3;
static constexpr double a1_coeffs[] = {
    -2.5031898268383284, 1.6084443306898792, 0.036541332578219968, 0.34830649509272787, -0.027529504223492943, -0.11884260335925244, -0.00071109224763490832
};
static constexpr int b1_coeffs_n_annual = 3;
static constexpr double b1_coeffs[] = {
    -1.0345038541252878, 0.17683220314798173, -0.19473036600847254, 0.10405762840371227, -0.2005796132350946, 0.14845546407316126, 0.13359484760274368
};
static constexpr int a2_coeffs_n_annual = 3;
static constexpr double a2_coeffs[] = {
    0.4455910032802306, 0.13108152667897729, -0.062371276284037477, -0.2396705807860427, 0.048121547178596007, -0.021171229150262139, -0.0068167476230407576
};
static constexpr int b2_coeffs_n_annual = 3;
static constexpr double b2_coeffs[] = {
    -0.11972298142627552, 0.26148017788536881, 0.057110271344898093, 0.10768650292602962, 0.063940836388631739, -0.094548847475211903, -0.02819699965722228
};
static constexpr int a3_coeffs_n_annual = 3;
static constexpr double a3_coeffs[] = {
    0.061617501708870266, -0.26452973380633865, 0.015026278933571679, -0.0084841564418522593, 0.011085301709980887, 0.054775685190386406, -0.016691814334064886
};
static constexpr int b3_coeffs_n_annual = 3;
static constexpr double b3_coeffs[] = {
    -0.060047254119780416, 0.13059212489516889, -0.040169884618870257, -0.079969238188012104, -0.0042483803789532086, -0.034321578775501502, 9.6971739989475732e-05
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
    0.0067789508500612616, -0.0021247853579172641, -0.0014196125181470678, 0.00016536065495024489, 0.00022500824933772119, -8.5187385920078591e-05, 0.00011896967952298586
};
static constexpr int a1_coeffs_n_annual = 3;
static constexpr double a1_coeffs[] = {
    -3.7586439078382433e-05, -2.6579106667266529e-05, 6.5970357239988176e-06, 6.3518442769973084e-06, 4.1905561618438678e-05, 2.2797355790627258e-05, -2.910354569644766e-05
};
static constexpr int b1_coeffs_n_annual = 3;
static constexpr double b1_coeffs[] = {
    1.1706164253973395e-05, -0.0001080001637328355, -3.452624603211333e-05, -1.7057281717828548e-06, 6.5493283220508601e-05, 3.1717499734404521e-05, -3.3811835051853551e-05
};
static constexpr int a2_coeffs_n_annual = 3;
static constexpr double a2_coeffs[] = {
    -1.3148978009100399e-05, 4.3012394200473758e-05, 6.2227867432974134e-06, 6.2423476172866834e-07, -6.2570626281681472e-06, -1.3976965171642542e-05, 2.443664122072111e-06
};
static constexpr int b2_coeffs_n_annual = 3;
static constexpr double b2_coeffs[] = {
    -5.8928077748063523e-05, 5.1319261068699293e-05, 1.341155892854125e-05, 1.9802028617801892e-05, -1.171616427595444e-05, -2.3932190307223273e-06, -4.141749323455641e-06
};
static constexpr int a3_coeffs_n_annual = 3;
static constexpr double a3_coeffs[] = {
    2.854085183996827e-06, -3.9810002731011215e-06, -1.2530304452526007e-05, -1.2438427220506263e-05, 1.2380848543031761e-06, 6.3605068579109201e-07, 5.1580517120639704e-06
};
static constexpr int b3_coeffs_n_annual = 3;
static constexpr double b3_coeffs[] = {
    -4.5286244429493025e-06, 3.2289065532207362e-05, -4.4913600995735024e-06, -1.2377867160567743e-05, -2.8831420761887619e-06, -9.908177648329675e-06, 4.7480464206448084e-06
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
    1007.5582885297617, 0.50801530241621573, 0.014151652285575139, 0.65456189079568594, 0.76087567168868531, 0.27295879621171348, 0.74763823096162663
};
static constexpr int a1_coeffs_n_annual = 3;
static constexpr double a1_coeffs[] = {
    0.12772115753092639, -0.081467587351565757, 0.02433452251140842, 0.013595377916013895, 0.0091779649021944739, 0.009262434664940665, -0.0049109969875850475
};
static constexpr int b1_coeffs_n_annual = 3;
static constexpr double b1_coeffs[] = {
    0.20351834050756887, -0.18871808270567175, 0.025518993696381376, -0.016674781407698805, 0.030466446404281103, -0.0062862080507697492, -0.0282450570896839
};
static constexpr int a2_coeffs_n_annual = 3;
static constexpr double a2_coeffs[] = {
    -0.1236015633225384, 0.079928055025039163, 0.045815419840862903, 0.059886301016374148, 0.049245043726131227, -0.037588316630537195, -0.01961866741226382
};
static constexpr int b2_coeffs_n_annual = 3;
static constexpr double b2_coeffs[] = {
    -0.38673371549230157, -0.0051184737554603333, -0.019731643839884874, 0.037673123949350393, -0.021297697461895419, 0.0026513105422024943, 0.022565085189618132
};
static constexpr int a3_coeffs_n_annual = 3;
static constexpr double a3_coeffs[] = {
    0.010039447775554201, 0.10804270494295005, -0.030049357066920225, -0.012955418888390035, -0.0086249227847546711, -0.0038267971095606664, 0.0070238267176622043
};
static constexpr int b3_coeffs_n_annual = 3;
static constexpr double b3_coeffs[] = {
    0.040396053895225457, 0.10033408672390585, 0.0032635818600576833, 0.015729849877922746, 0.014483775197160027, -0.0017731958978144263, 0.0076602319401003682
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