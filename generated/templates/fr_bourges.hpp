// Auto-generated linear harmonic climate model
// Station name : BOURGES
// Station code : 18033001
#pragma once
#include <cmath>
namespace harmoclimat {
static constexpr double longitude_deg = 2.3598330020904541;
static constexpr double latitude_deg = 47.059165954589844;
static constexpr double delta_utc_solar_h = 0.15732220013936357;
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
    12.338911233715443, -7.87144373576447, -2.9320672030940216, -0.048396137067969872, 0.64125326288162199, -0.10260852402825053, -0.12097976478291116
};
static constexpr int a1_coeffs_n_annual = 3;
static constexpr double a1_coeffs[] = {
    -3.3272167410183853, 1.7311429839189734, 0.13257019106075144, 0.54169244512722858, -0.1089894075639582, -0.057074718949119176, -0.0033879692453398632
};
static constexpr int b1_coeffs_n_annual = 3;
static constexpr double b1_coeffs[] = {
    -1.1909813819912332, 0.12722126043979767, -0.21247696191357174, 0.14597824961635616, -0.3190595450803077, 0.17091365728253161, 0.16265002038767234
};
static constexpr int a2_coeffs_n_annual = 3;
static constexpr double a2_coeffs[] = {
    0.53365621384206208, 0.33431405496397121, -0.099271448106414167, -0.27856479325417771, 0.037848828795110673, -0.056893531058785703, 0.0077982774995799141
};
static constexpr int b2_coeffs_n_annual = 3;
static constexpr double b2_coeffs[] = {
    -0.12148188043181173, 0.3306937113925203, 0.078979497852520425, 0.14827573614546546, 0.075709005151600034, -0.090988878413125293, -0.02339697686921316
};
static constexpr int a3_coeffs_n_annual = 3;
static constexpr double a3_coeffs[] = {
    0.13360947594355657, -0.33395283336286596, 0.0041198451172013706, -0.087299656487951849, 0.040217351523124674, 0.051768960442728412, -0.023275462885030945
};
static constexpr int b3_coeffs_n_annual = 3;
static constexpr double b3_coeffs[] = {
    -0.097907707210590861, 0.16693143766304713, -0.014754186333351517, -0.063956537070537473, 0.013936567753100326, -0.066669795391519737, -0.012686212701893174
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
    0.0069932836845918722, -0.0023179222451797758, -0.0013673821153306145, 0.00020807020226445695, 0.00014144109826160276, -9.6491228398155879e-05, 0.00014235229841486543
};
static constexpr int a1_coeffs_n_annual = 3;
static constexpr double a1_coeffs[] = {
    -0.00013072794978053381, -0.00013993315510620969, -2.5240119281010622e-07, 9.0270965951479056e-05, 9.9408104834557072e-05, 1.9770833375475337e-05, -4.9206602930022508e-05
};
static constexpr int b1_coeffs_n_annual = 3;
static constexpr double b1_coeffs[] = {
    -4.374773863349884e-05, -0.00010988639379880712, -3.021720056823458e-05, 6.399922332667769e-06, 6.1063521305300598e-05, 3.5227255813155496e-05, -3.9251334091068464e-05
};
static constexpr int a2_coeffs_n_annual = 3;
static constexpr double a2_coeffs[] = {
    -6.4477997838556994e-05, 0.00017169866361268062, 2.0073351660035716e-05, -4.3443428008146299e-07, -1.8976567443609522e-05, -3.2874453098530941e-05, -1.6013890886019419e-05
};
static constexpr int b2_coeffs_n_annual = 3;
static constexpr double b2_coeffs[] = {
    -9.1353172317759905e-05, 7.4396789568863765e-05, 2.1576336474481742e-05, 5.2660120395042779e-05, -1.4914507416767232e-05, -9.0182338097440325e-06, -2.692215999896063e-06
};
static constexpr int a3_coeffs_n_annual = 3;
static constexpr double a3_coeffs[] = {
    2.4937465650252957e-05, -1.061745717373949e-05, -1.7999962708290475e-05, -5.6275158253593158e-05, 2.0449015246156831e-07, 8.8957975339047352e-06, 1.1325166988098997e-05
};
static constexpr int b3_coeffs_n_annual = 3;
static constexpr double b3_coeffs[] = {
    -9.6927227246588885e-06, 5.7586980049001603e-05, -1.2570835827637179e-06, -1.3681337458072099e-05, 3.8692368324056381e-06, -2.1980587268503873e-05, -5.8642713657167281e-06
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
    997.61880295482706, 0.82917069445450531, -0.19411515335501303, 0.94712335830191174, 0.86096561908336833, 0.27426929427419416, 0.71266906890236337
};
static constexpr int a1_coeffs_n_annual = 3;
static constexpr double a1_coeffs[] = {
    0.11456339480370296, -0.069967953375816211, 0.021160591441245321, 0.001435504870020834, 0.0068883345143438118, -0.0037976719643588171, 0.0015174388453018902
};
static constexpr int b1_coeffs_n_annual = 3;
static constexpr double b1_coeffs[] = {
    0.23087801110799283, -0.20809084490141383, 0.014880831771839959, -0.019772065268075682, 0.050622619854442344, -0.010541028257372242, -0.039049912842092693
};
static constexpr int a2_coeffs_n_annual = 3;
static constexpr double a2_coeffs[] = {
    -0.13613003262361811, 0.091757306852535706, 0.045945352232505489, 0.059993120563093284, 0.05157271614460953, -0.038077707887456742, -0.024305201510626359
};
static constexpr int b2_coeffs_n_annual = 3;
static constexpr double b2_coeffs[] = {
    -0.41524492556897841, -0.021459636449450988, -0.017318244430675291, 0.043622163466361399, -0.012307316124739896, 0.0073062250508094733, 0.018466022112115395
};
static constexpr int a3_coeffs_n_annual = 3;
static constexpr double a3_coeffs[] = {
    0.0136458032718846, 0.10916955805490194, -0.030357294989037487, -0.016371432610385164, -0.010214482136397373, -0.0038526334644515083, 0.0075454729455489335
};
static constexpr int b3_coeffs_n_annual = 3;
static constexpr double b3_coeffs[] = {
    0.044822427664054101, 0.10253710672523834, 0.0080516479867313705, 0.018418091695817852, 0.012110362795746902, -0.0018742591785910331, 0.0043586799849032864
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