// Auto-generated linear harmonic climate model
// Station name : LILLE-LESQUIN
// Station code : 59343001
#pragma once
#include <cmath>
namespace harmoclimat {
static constexpr double longitude_deg = 3.0975000858306885;
static constexpr double latitude_deg = 50.569999694824219;
static constexpr double delta_utc_solar_h = 0.20650000572204597;
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
    11.408654848672338, -6.987343700258803, -2.9179685617235673, -0.13995770381612049, 0.44077815794611264, 0.012565579018429829, -0.060345782938747879
};
static constexpr int a1_coeffs_n_annual = 3;
static constexpr double a1_coeffs[] = {
    -2.7465633763375581, 1.6902694771226157, 0.049014444714791952, 0.41139367751984252, -0.024524873204841938, -0.14359093439849255, 0.017862772977913777
};
static constexpr int b1_coeffs_n_annual = 3;
static constexpr double b1_coeffs[] = {
    -0.74141510777474617, -0.021568555941127606, -0.25756068173587549, 0.14275433841410234, -0.13148605507450678, 0.13632606935425207, 0.12325410803101551
};
static constexpr int a2_coeffs_n_annual = 3;
static constexpr double a2_coeffs[] = {
    0.43840019787620538, 0.23368618419467413, -0.081562313110486687, -0.23618081952564074, 0.039379526511920573, -0.021897427442306258, -0.0059644778873301166
};
static constexpr int b2_coeffs_n_annual = 3;
static constexpr double b2_coeffs[] = {
    -0.093737503912091166, 0.19952948390758393, 0.094608922789077191, 0.13189989993482448, 0.056463503200789279, -0.08273141955274442, -0.03330344278907231
};
static constexpr int a3_coeffs_n_annual = 3;
static constexpr double a3_coeffs[] = {
    0.092218577551689662, -0.27312455071837172, 0.017600094219537673, -0.068122973971125586, 0.029995955384786294, 0.061557225750277327, -0.015785071123948505
};
static constexpr int b3_coeffs_n_annual = 3;
static constexpr double b3_coeffs[] = {
    -0.080588409870119615, 0.14796525686736237, -0.019617163506910854, -0.055832235047913174, -0.00089679804827941202, -0.05099949784114486, -0.0092626923201209795
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
    0.0067472781291950364, -0.0021114305461012086, -0.0014786373396394146, 0.00012573080172146892, 0.00031587569128623563, -6.5652894618037109e-05, 4.9194352718802513e-05
};
static constexpr int a1_coeffs_n_annual = 3;
static constexpr double a1_coeffs[] = {
    -0.00010642381655754186, -7.9556820843128178e-05, 2.7388273360794818e-05, 3.2220392735061879e-05, 5.7302913923002904e-05, 4.1670144631889228e-05, -4.1292225614869232e-05
};
static constexpr int b1_coeffs_n_annual = 3;
static constexpr double b1_coeffs[] = {
    4.709274954776716e-06, -0.00011853904686661271, -2.050009468237592e-05, 2.4013644695466681e-05, 2.0411795030979139e-05, 2.8492225718873586e-05, -1.147197577585788e-05
};
static constexpr int a2_coeffs_n_annual = 3;
static constexpr double a2_coeffs[] = {
    -4.3831312073804726e-05, 0.00011582756078701734, 2.2950615714708279e-05, 2.3083412070871334e-05, -2.7225806227821227e-05, -3.1737011714503131e-05, -3.0523546436277582e-06
};
static constexpr int b2_coeffs_n_annual = 3;
static constexpr double b2_coeffs[] = {
    -6.4437024583830258e-05, 4.2223953904779052e-05, 1.4792973464846804e-05, 4.2311045468933443e-05, -6.8397838169561603e-06, -9.8458129340064387e-06, -3.8215775036561805e-06
};
static constexpr int a3_coeffs_n_annual = 3;
static constexpr double a3_coeffs[] = {
    9.9544154480239271e-06, -1.7986501686724361e-06, -1.9646218058000707e-05, -4.3153152156472973e-05, 5.1694677578985267e-06, 6.8839085136652522e-06, 1.1244147028806764e-05
};
static constexpr int b3_coeffs_n_annual = 3;
static constexpr double b3_coeffs[] = {
    -1.3946596794111811e-05, 5.0107509346425714e-05, 4.1812040768078126e-06, -7.0143951680340723e-06, -4.074191169113308e-06, -1.7920774925034604e-05, -2.6652400199562051e-07
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
    1010.3909377864024, -0.095078501955901354, 0.25977347386858224, 0.36417538430435442, 0.66514263832737852, 0.30914287603371382, 0.72746563798975983
};
static constexpr int a1_coeffs_n_annual = 3;
static constexpr double a1_coeffs[] = {
    0.033560892859495421, 0.0033342945176492388, 0.023750213072460901, 0.0098299327585000679, 0.0049706137511222326, 0.023217229415634463, 0.0053046686557678899
};
static constexpr int b1_coeffs_n_annual = 3;
static constexpr double b1_coeffs[] = {
    0.058277992330785518, -0.092701894506559079, 0.031866926317183887, -0.024477953002711456, -0.0072044581183638573, 0.010818370287293551, -0.0052548937371748317
};
static constexpr int a2_coeffs_n_annual = 3;
static constexpr double a2_coeffs[] = {
    -0.10396053173322184, 0.084653394921198791, 0.041840301200829481, 0.04899738027170257, 0.040910343369644511, -0.041669161243383164, -0.009645049776572193
};
static constexpr int b2_coeffs_n_annual = 3;
static constexpr double b2_coeffs[] = {
    -0.33784088341629376, -0.016830356266891073, -0.032903841581298957, 0.03451181582601557, -0.014417074728272748, 0.0033532364537678097, 0.01819868053364334
};
static constexpr int a3_coeffs_n_annual = 3;
static constexpr double a3_coeffs[] = {
    0.010516750653604447, 0.094535288050565464, -0.028608653925413244, -0.014678477995138722, -0.012649051875013806, -0.0049927712484426822, 0.001947906855380358
};
static constexpr int b3_coeffs_n_annual = 3;
static constexpr double b3_coeffs[] = {
    0.038092107970944984, 0.092687879086331956, 0.0027565430584841422, 0.01549969136777361, 0.01073183603496356, 0.0023333002838517611, 0.0075703663371655058
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