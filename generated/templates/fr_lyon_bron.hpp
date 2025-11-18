// Auto-generated linear harmonic climate model
// Station name : LYON-BRON
// Station code : 69029001
#pragma once
#include <cmath>
namespace harmoclimat {
static constexpr double longitude_deg = 4.9491667747497559;
static constexpr double latitude_deg = 45.721332550048828;
static constexpr double delta_utc_solar_h = 0.32994445164998376;
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
    13.28301364430199, -8.8192907090465305, -3.4860963917107513, -0.21370024465892451, 0.62316947982431947, -0.19080376450046263, -0.2420325370567524
};
static constexpr int a1_coeffs_n_annual = 3;
static constexpr double a1_coeffs[] = {
    -3.2631303229061053, 1.6050204973586151, 0.17557549244838311, 0.43574187471347264, -0.15478841694214171, -0.033349336452207896, 0.018105251179331018
};
static constexpr int b1_coeffs_n_annual = 3;
static constexpr double b1_coeffs[] = {
    -1.1982258242629029, 0.057037227587536564, -0.26500698520622318, 0.19693422196462795, -0.26390336366048633, 0.15865885401764321, 0.17431868166107711
};
static constexpr int a2_coeffs_n_annual = 3;
static constexpr double a2_coeffs[] = {
    0.56932375183479011, 0.28304747434349731, -0.066658718756611945, -0.21334957700082308, 0.031250493821818721, -0.04969513987785746, -0.0024305999474802447
};
static constexpr int b2_coeffs_n_annual = 3;
static constexpr double b2_coeffs[] = {
    -0.085622651134906622, 0.3280356204029703, 0.089015012271124308, 0.10989690492823048, 0.082096149184013714, -0.064824947995709162, -0.026809026901235489
};
static constexpr int a3_coeffs_n_annual = 3;
static constexpr double a3_coeffs[] = {
    0.10683067777386725, -0.26365889565546524, 0.017950441496184254, -0.092336605277576492, 0.040371272404436359, 0.039550714656951438, -0.019584412244937319
};
static constexpr int b3_coeffs_n_annual = 3;
static constexpr double b3_coeffs[] = {
    -0.11834535818443513, 0.16990485373342273, -0.00063714729433017895, -0.049095404072806256, 0.012650553231207387, -0.050838814645886583, -0.023843449506614274
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
    0.0068835816938303809, -0.0025055690549882948, -0.0016310290344371664, 0.00012056709241654444, 0.00020735469974810072, -0.00011185981916897544, 0.0001066726758560658
};
static constexpr int a1_coeffs_n_annual = 3;
static constexpr double a1_coeffs[] = {
    -1.5401767269137267e-05, -0.00019345725425816209, -3.0022816784413476e-05, 3.6817812705447383e-05, 0.00010340380190000804, 2.9218959950204275e-05, -4.6609205021925232e-05
};
static constexpr int b1_coeffs_n_annual = 3;
static constexpr double b1_coeffs[] = {
    -4.0214701535592318e-05, -8.2186322579979847e-05, -2.0173232998510136e-05, -9.9204139748409449e-07, 4.5398804816579708e-05, 2.7602947701581806e-05, -2.6705761457380804e-05
};
static constexpr int a2_coeffs_n_annual = 3;
static constexpr double a2_coeffs[] = {
    -5.3091202941198735e-05, 0.00011426480082801052, 3.2601295720962185e-05, 1.6709279115117807e-05, -1.420685978729105e-05, -1.5998089643723314e-05, -1.18719779445469e-05
};
static constexpr int b2_coeffs_n_annual = 3;
static constexpr double b2_coeffs[] = {
    -8.3213448018624387e-05, 6.1786301282779968e-05, 1.2835178608788858e-05, 3.7558420979711982e-05, -1.2279028554903836e-05, -6.1376415392646634e-06, 6.2096826563426636e-07
};
static constexpr int a3_coeffs_n_annual = 3;
static constexpr double a3_coeffs[] = {
    1.5630931903143674e-05, -1.9298394982096729e-06, -1.3097183698165964e-05, -2.2840107427966033e-05, 5.7200948612096709e-07, -3.7214346848074309e-06, 7.8520620889335212e-06
};
static constexpr int b3_coeffs_n_annual = 3;
static constexpr double b3_coeffs[] = {
    3.511066044499562e-06, 2.9007247447648467e-05, -1.3983209471007659e-06, -7.6031429643119697e-07, 3.8337365617801209e-06, -9.9365449316767134e-06, -3.8486017547236331e-06
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
    993.74083953395143, 1.4920069798213564, -0.36269233882002472, 0.88134026977844837, 0.80201965045548806, 0.093573935774179967, 0.750382744346477
};
static constexpr int a1_coeffs_n_annual = 3;
static constexpr double a1_coeffs[] = {
    0.26787420271814677, -0.17106201988061148, 0.002517110665252596, -0.0095519486021595872, -0.0081906733222457742, -0.0020228682366138584, 0.010500208848250108
};
static constexpr int b1_coeffs_n_annual = 3;
static constexpr double b1_coeffs[] = {
    0.35467370817200849, -0.21959212117881086, 0.026405983101840853, -0.046562001770599049, 0.058357511426101856, -0.016975090736599138, -0.025518024398877862
};
static constexpr int a2_coeffs_n_annual = 3;
static constexpr double a2_coeffs[] = {
    -0.16381773146582743, 0.11686092894542222, 0.063371335489790542, 0.054048531398918104, 0.054671809251655185, -0.03869136899437859, -0.031244123852029512
};
static constexpr int b2_coeffs_n_annual = 3;
static constexpr double b2_coeffs[] = {
    -0.450777441848006, -0.02370972509962398, -0.025449787952654603, 0.042388835116361608, -0.021316303414354108, 0.017933810898099832, 0.015028075821974773
};
static constexpr int a3_coeffs_n_annual = 3;
static constexpr double a3_coeffs[] = {
    0.017559706770265131, 0.11790783324883586, -0.028881174494511223, -0.019650787896004048, -0.01549036152813092, -0.0091896584027859052, 0.00028376126180482245
};
static constexpr int b3_coeffs_n_annual = 3;
static constexpr double b3_coeffs[] = {
    0.048642661227656155, 0.1023161208460821, 0.018049533982452966, 0.018887682670210775, 0.019768065389956513, -0.0045492986494299536, 0.0085604214543883433
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