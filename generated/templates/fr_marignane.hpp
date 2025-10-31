// Auto-generated factorized model (annual + diurnal)
// Target city / station (France):
//   Station code : 13054001
//   Station name : MARIGNANE
#pragma once
#include <cmath>
namespace harmoclimat {
static constexpr double longitude_deg = 5.2159999999999993;
static constexpr double latitude_deg  = 43.43766699999999;
static constexpr double delta_utc_solar_h = 0.34773333333333334;
static constexpr double annual_T_mu = 1.5981122255325317;
static constexpr double annual_T_A1 = 0.91375893354415894;
static constexpr double annual_T_ph1 = -2.7729072570800781;
static constexpr double annual_T_A2 = 0.064138591289520264;
static constexpr double annual_T_ph2 = 1.5008422136306763;
static constexpr double annual_RH_mu = 65.583518981933594;
static constexpr double annual_RH_A1 = 10.146945953369141;
static constexpr double annual_RH_ph1 = 6.2020077705383301;
static constexpr double annual_RH_A2 = 2.6064317226409912;
static constexpr double annual_RH_ph2 = -1.7433152198791504;
static constexpr double diurn_T_a0 = 0.37432193756103516;
static constexpr double diurn_T_a1 = 0.089207582175731659;
static constexpr double diurn_T_psiA = 3.0689411163330078;
static constexpr double diurn_T_ph0 = -2.9160933494567871;
static constexpr double diurn_T_k1 = -0.21941640973091125;
static constexpr double diurn_T_psiP = 3.2703111171722412;
static constexpr double diurn_T_alpha = 0.11768419295549393;
static constexpr double diurn_RH_beta_c = -6.1799430847167969;
static constexpr double diurn_RH_core_a0 = 10.077238082885742;
static constexpr double diurn_RH_core_a1 = -2.2447738647460938;
static constexpr double diurn_RH_core_psiA = -0.080589070916175842;
static constexpr double diurn_RH_core_ph0 = 0.18583247065544128;
static constexpr double diurn_RH_core_k1 = 0.25382354855537415;
static constexpr double diurn_RH_core_psiP = 0.34201058745384216;
static constexpr double diurn_RH_core_alpha = -0.17464341223239899;
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