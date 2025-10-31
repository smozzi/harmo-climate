// Auto-generated factorized model (annual + diurnal)
// Target city / station (France):
//   Station code : 75114001
//   Station name : PARIS-MONTSOURIS
#pragma once
#include <cmath>
namespace harmoclimat {
static constexpr double longitude_deg = 2.3378329999999994;
static constexpr double latitude_deg  = 48.821667000000012;
static constexpr double delta_utc_solar_h = 0.15585553333333332;
static constexpr double annual_T_mu = 1.2907721996307373;
static constexpr double annual_T_A1 = 0.79430735111236572;
static constexpr double annual_T_ph1 = -2.8239150047302246;
static constexpr double annual_T_A2 = 0.055742844939231873;
static constexpr double annual_T_ph2 = 1.6297981739044189;
static constexpr double annual_RH_mu = 71.328819274902344;
static constexpr double annual_RH_A1 = 10.676748275756836;
static constexpr double annual_RH_ph1 = 5.9722957611083984;
static constexpr double annual_RH_A2 = 2.4513974189758301;
static constexpr double annual_RH_ph2 = -0.96398979425430298;
static constexpr double diurn_T_a0 = 0.27690938115119934;
static constexpr double diurn_T_a1 = -0.15137551724910736;
static constexpr double diurn_T_psiA = -0.071259923279285431;
static constexpr double diurn_T_ph0 = -2.6798341274261475;
static constexpr double diurn_T_k1 = 0.22374609112739563;
static constexpr double diurn_T_psiP = 0.16485600173473358;
static constexpr double diurn_T_alpha = 0.037542227655649185;
static constexpr double diurn_RH_beta_c = -8.8173580169677734;
static constexpr double diurn_RH_core_a0 = 8.3296232223510742;
static constexpr double diurn_RH_core_a1 = -3.8833143711090088;
static constexpr double diurn_RH_core_psiA = -0.013717759400606155;
static constexpr double diurn_RH_core_ph0 = 0.45144635438919067;
static constexpr double diurn_RH_core_k1 = 0.11632639914751053;
static constexpr double diurn_RH_core_psiP = 0.20970985293388367;
static constexpr double diurn_RH_core_alpha = -0.040611635893583298;
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