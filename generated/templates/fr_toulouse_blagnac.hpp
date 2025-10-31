// Auto-generated factorized model (annual + diurnal)
// Target city / station (France):
//   Station code : 31069001
//   Station name : TOULOUSE-BLAGNAC
#pragma once
#include <cmath>
namespace harmoclimat {
static constexpr double longitude_deg = 1.3788329999999998;
static constexpr double latitude_deg  = 43.620999999999988;
static constexpr double delta_utc_solar_h = 0.091922199999999968;
static constexpr double annual_T_mu = 1.4275072813034058;
static constexpr double annual_T_A1 = 0.85770183801651001;
static constexpr double annual_T_ph1 = -2.7672703266143799;
static constexpr double annual_T_A2 = 0.084161035716533661;
static constexpr double annual_T_ph2 = 1.831773042678833;
static constexpr double annual_RH_mu = 72.922294616699219;
static constexpr double annual_RH_A1 = 10.74421215057373;
static constexpr double annual_RH_ph1 = 0.013178610242903233;
static constexpr double annual_RH_A2 = 3.7346584796905518;
static constexpr double annual_RH_ph2 = -1.0282412767410278;
static constexpr double diurn_T_a0 = 0.37094736099243164;
static constexpr double diurn_T_a1 = -0.12817822396755219;
static constexpr double diurn_T_psiA = 0.12764556705951691;
static constexpr double diurn_T_ph0 = -2.7733442783355713;
static constexpr double diurn_T_k1 = 0.12003587186336517;
static constexpr double diurn_T_psiP = 0.29524773359298706;
static constexpr double diurn_T_alpha = 0.081524685025215149;
static constexpr double diurn_RH_beta_c = -10.658083915710449;
static constexpr double diurn_RH_core_a0 = 10.738776206970215;
static constexpr double diurn_RH_core_a1 = -3.899177074432373;
static constexpr double diurn_RH_core_psiA = 0.17299458384513855;
static constexpr double diurn_RH_core_ph0 = 0.43268305063247681;
static constexpr double diurn_RH_core_k1 = 0.088997691869735718;
static constexpr double diurn_RH_core_psiP = 0.59585064649581909;
static constexpr double diurn_RH_core_alpha = -0.060694634914398193;
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