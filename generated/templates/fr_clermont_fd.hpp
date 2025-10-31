// Auto-generated factorized model (annual + diurnal)
// Target city / station (France):
//   Station code : 63113001
//   Station name : CLERMONT-FD
#pragma once
#include <cmath>
namespace harmoclimat {
static constexpr double longitude_deg = 3.149332999999999;
static constexpr double latitude_deg  = 45.786833000000001;
static constexpr double delta_utc_solar_h = 0.20995553333333336;
static constexpr double annual_T_mu = 1.2299336194992065;
static constexpr double annual_T_A1 = 0.84396421909332275;
static constexpr double annual_T_ph1 = -2.8006129264831543;
static constexpr double annual_T_A2 = 0.058590829372406006;
static constexpr double annual_T_ph2 = 1.6477454900741577;
static constexpr double annual_RH_mu = 70.7027587890625;
static constexpr double annual_RH_A1 = 7.0406589508056641;
static constexpr double annual_RH_ph1 = 5.9984221458435059;
static constexpr double annual_RH_A2 = 2.9993624687194824;
static constexpr double annual_RH_ph2 = -1.0405287742614746;
static constexpr double diurn_T_a0 = 0.3877892792224884;
static constexpr double diurn_T_a1 = -0.15021258592605591;
static constexpr double diurn_T_psiA = -0.0008557827677577734;
static constexpr double diurn_T_ph0 = -2.8620760440826416;
static constexpr double diurn_T_k1 = 0.16774426400661469;
static constexpr double diurn_T_psiP = 0.29257750511169434;
static constexpr double diurn_T_alpha = 0.099858872592449188;
static constexpr double diurn_RH_beta_c = -12.30494213104248;
static constexpr double diurn_RH_core_a0 = 8.6489400863647461;
static constexpr double diurn_RH_core_a1 = -3.3860366344451904;
static constexpr double diurn_RH_core_psiA = 0.065386168658733368;
static constexpr double diurn_RH_core_ph0 = 0.24315717816352844;
static constexpr double diurn_RH_core_k1 = 0.15083245933055878;
static constexpr double diurn_RH_core_psiP = 0.40612444281578064;
static constexpr double diurn_RH_core_alpha = -0.14035838842391968;
inline double wrap_day(double d){
    while (d > 365.0) d -= 365.0;
    while (d < 0.0)   d += 365.0;
    return d;
}
inline double wrap_hour(double h){
    while (h >= 24.0) h -= 24.0;
    while (h < 0.0)   h += 24.0;
    return h;
}
inline double predict_annual_T(double day_solar){
    const double wA = 6.2831853071795864769 / 365.0;
    double th = wA * day_solar;
    return annual_T_mu + annual_T_A1 * std::cos(th - annual_T_ph1)
                       + annual_T_A2 * std::cos(2.0*th - annual_T_ph2);
}
inline double predict_annual_RH(double day_solar){
    const double wA = 6.2831853071795864769 / 365.0;
    double th = wA * day_solar;
    return annual_RH_mu + annual_RH_A1 * std::cos(th - annual_RH_ph1)
                        + annual_RH_A2 * std::cos(2.0*th - annual_RH_ph2);
}
inline double predict_diurn_T(double day_solar, double hour_solar){
    const double two_pi = 6.2831853071795864769;
    const double wA = two_pi / 365.0;
    const double wD = two_pi / 24.0;
    double thA = wA * day_solar;
    double thD = wD * hour_solar;
    double Aday  = diurn_T_a0 + diurn_T_a1 * std::cos(thA - diurn_T_psiA);
    double phday = diurn_T_ph0 + diurn_T_k1 * std::cos(thA - diurn_T_psiP);
    double base  = std::cos(thD - phday);
    double sec   = std::cos(2.0 * (thD - phday));
    return Aday * (base + diurn_T_alpha * sec);
}
inline double predict_diurn_RH(double day_solar, double hour_solar, double temp_res){
    const double two_pi = 6.2831853071795864769;
    const double wA = two_pi / 365.0;
    const double wD = two_pi / 24.0;
    double thA = wA * day_solar;
    double thD = wD * hour_solar;
    double Aday  = diurn_RH_core_a0 + diurn_RH_core_a1 * std::cos(thA - diurn_RH_core_psiA);
    double phday = diurn_RH_core_ph0 + diurn_RH_core_k1 * std::cos(thA - diurn_RH_core_psiP);
    double base  = std::cos(thD - phday);
    double sec   = std::cos(2.0 * (thD - phday));
    double g = Aday * (base + diurn_RH_core_alpha * sec);
    return g + diurn_RH_beta_c * temp_res;
}
inline void predict(double day_utc, double hour_utc, double& T, double& RH){
    double hour_solar = wrap_hour(hour_utc + delta_utc_solar_h);
    double day_solar  = wrap_day(day_utc + (delta_utc_solar_h / 24.0));
    double T_ann = predict_annual_T(day_solar);
    double T_res = predict_diurn_T(day_solar, hour_solar);
    T = T_ann + T_res;
    double RH_ann = predict_annual_RH(day_solar);
    double RH_res = predict_diurn_RH(day_solar, hour_solar, T_res);
    RH = RH_ann + RH_res;
}
} // namespace harmoclimat