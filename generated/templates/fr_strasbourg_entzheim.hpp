// Auto-generated linear harmonic climate model
// Station name : STRASBOURG-ENTZHEIM
// Station code : 67124001
#pragma once
#include <cmath>
namespace harmoclimat {
static constexpr double longitude_deg = 7.6403331756591797;
static constexpr double latitude_deg = 48.54949951171875;
static constexpr double delta_utc_solar_h = 0.50935554504394531;
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
    11.630518918902004, -8.6354072227274354, -3.4525344766818407, -0.06824806756369263, 0.33769199703360003, 0.079119632465550482, -0.060599182098329783
};
static constexpr int a1_coeffs_n_annual = 3;
static constexpr double a1_coeffs[] = {
    -3.4664513312875953, 1.9759727695978093, 0.28363982745567901, 0.62977994298818551, -0.034744336948298045, -0.072079816192325993, 0.028099957215941648
};
static constexpr int b1_coeffs_n_annual = 3;
static constexpr double b1_coeffs[] = {
    -1.0344693455968779, 0.08524589973026174, -0.39889667102169896, 0.2477060256478667, -0.14594445968623831, 0.14100469180073971, 0.18478104019676955
};
static constexpr int a2_coeffs_n_annual = 3;
static constexpr double a2_coeffs[] = {
    0.516592515623858, 0.3161379496510226, -0.10833137952848913, -0.33087845758837153, 0.0015137807200344769, -0.042388285486814711, 0.024468443723720288
};
static constexpr int b2_coeffs_n_annual = 3;
static constexpr double b2_coeffs[] = {
    -0.013781171098897692, 0.24969688015262206, 0.09611270355251568, 0.088658087549164252, 0.10664130437948892, -0.11474998274169461, -0.056702544669739077
};
static constexpr int a3_coeffs_n_annual = 3;
static constexpr double a3_coeffs[] = {
    0.11983025133703726, -0.32293210058123195, 0.00027010645329179179, -0.069777231632116757, 0.048801479419916596, 0.067087446783114499, -0.039751682077251389
};
static constexpr int b3_coeffs_n_annual = 3;
static constexpr double b3_coeffs[] = {
    -0.11276502385268942, 0.14483578520853047, 0.023422576209745874, -0.040200544788389622, -0.0084671684888113601, -0.041155758395372485, -0.023054964722820859
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
    0.0067660856965110691, -0.0026922135341018293, -0.0017813796250765223, 0.00023052235513686546, 0.00041545286691553748, -3.5447006749660009e-05, 3.2154394354145446e-05
};
static constexpr int a1_coeffs_n_annual = 3;
static constexpr double a1_coeffs[] = {
    -0.00017911809963723079, -7.6139452271279751e-05, 8.8565234632958031e-05, 9.3625359362157435e-05, 2.9928585678008762e-05, 3.9692461041984231e-05, -2.9026330971976235e-05
};
static constexpr int b1_coeffs_n_annual = 3;
static constexpr double b1_coeffs[] = {
    -4.7157882626616172e-05, -7.78360370301606e-05, 8.4813312486870228e-06, 1.2105513288102077e-05, -2.3873627978644278e-05, 1.9291757718739744e-05, 1.1233230418894084e-07
};
static constexpr int a2_coeffs_n_annual = 3;
static constexpr double a2_coeffs[] = {
    -6.4146644585288105e-05, 0.00016076692944330215, 4.1755159233270974e-05, 3.0530330107574282e-06, -2.988020939128131e-05, -3.560911419373062e-05, -1.0713500421107375e-05
};
static constexpr int b2_coeffs_n_annual = 3;
static constexpr double b2_coeffs[] = {
    -9.795668519303524e-05, 0.00010639934799575692, 2.4845512533056181e-05, 3.0158936402322038e-05, -1.3415556999544626e-05, -1.3997702488623936e-05, 5.7923819037627641e-06
};
static constexpr int a3_coeffs_n_annual = 3;
static constexpr double a3_coeffs[] = {
    1.3000898531691105e-05, 2.8043694899626406e-06, -1.9298366506900801e-05, -5.5777638672044319e-05, -6.5129195971908092e-06, 8.9215522795850978e-06, 1.6448223948679245e-05
};
static constexpr int b3_coeffs_n_annual = 3;
static constexpr double b3_coeffs[] = {
    -2.549853321909023e-06, 3.4625520304892142e-05, -3.1690644798330309e-06, -1.3553155294084269e-05, 7.8943165402562523e-06, -7.5341570262848068e-06, -5.2703028241658307e-06
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
    999.08242375186876, 1.1644622643399463, -0.24326083442366145, 0.47365633550991409, 0.71918608767039938, -0.073817445445626059, 0.71464237459180546
};
static constexpr int a1_coeffs_n_annual = 3;
static constexpr double a1_coeffs[] = {
    0.15832786579647445, -0.13612019419755136, 0.010737988195042162, -0.0073782736488331238, -0.0074035402711531209, 0.0059676827570740417, 0.013537714580558949
};
static constexpr int b1_coeffs_n_annual = 3;
static constexpr double b1_coeffs[] = {
    0.32969753277942737, -0.21176956872892755, 0.047929597587619603, -0.046737572310464465, 0.012416156045134769, -0.015470583484487493, -0.008701205356337784
};
static constexpr int a2_coeffs_n_annual = 3;
static constexpr double a2_coeffs[] = {
    -0.13283093473361204, 0.083017440728146666, 0.052495277912443836, 0.049705824403621299, 0.05714099881293648, -0.036498112337906674, -0.037098974835119182
};
static constexpr int b2_coeffs_n_annual = 3;
static constexpr double b2_coeffs[] = {
    -0.38210144006878349, -0.0015663989733029665, -0.022628067566586006, 0.045819486427414853, -0.0039730786064290742, 0.011568603979332723, 0.018092140991610973
};
static constexpr int a3_coeffs_n_annual = 3;
static constexpr double a3_coeffs[] = {
    0.013845155949382796, 0.098857612687783813, -0.02252083684807062, -0.013997156440630581, -0.013658867104523516, -0.00386002832911163, 0.0028203255697922969
};
static constexpr int b3_coeffs_n_annual = 3;
static constexpr double b3_coeffs[] = {
    0.038642474814747037, 0.09907749042291851, 0.020211126917309887, 0.011051983111658422, 0.015716304041345216, -0.0062699573367123182, 0.0048313120040352703
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