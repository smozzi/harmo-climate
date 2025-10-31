// Auto-generated factorized model (annual + diurnal)
// Target city / station (France):
//   Station code : 69029001
//   Station name : LYON-BRON
#pragma once
#include <cmath>
namespace harmoclimat {
static constexpr double longitude_deg = 4.949167000000001;
static constexpr double latitude_deg  = 45.721333000000001;
static constexpr double delta_utc_solar_h = 0.32994446666666655;
static constexpr double annual_T_mu = 1.3284281492233276;
static constexpr double annual_T_A1 = 0.94844263792037964;
static constexpr double annual_T_ph1 = -2.840479850769043;
static constexpr double annual_T_A2 = 0.06583249568939209;
static constexpr double annual_T_ph2 = 1.7562462091445923;
static constexpr double annual_RH_mu = 70.028388977050781;
static constexpr double annual_RH_A1 = 11.997225761413574;
static constexpr double annual_RH_ph1 = 6.0695576667785645;
static constexpr double annual_RH_A2 = 3.0469720363616943;
static constexpr double annual_RH_ph2 = -1.1795890331268311;
static constexpr double diurn_T_a0 = 0.35416224598884583;
static constexpr double diurn_T_a1 = -0.14698800444602966;
static constexpr double diurn_T_psiA = -0.037024781107902527;
static constexpr double diurn_T_ph0 = -2.7674524784088135;
static constexpr double diurn_T_k1 = 0.172409787774086;
static constexpr double diurn_T_psiP = 0.31115862727165222;
static constexpr double diurn_T_alpha = 0.073461592197418213;
static constexpr double diurn_RH_beta_c = -12.137578010559082;
static constexpr double diurn_RH_core_a0 = 9.228022575378418;
static constexpr double diurn_RH_core_a1 = -3.471621036529541;
static constexpr double diurn_RH_core_psiA = -0.024624094367027283;
static constexpr double diurn_RH_core_ph0 = 0.34913229942321777;
static constexpr double diurn_RH_core_k1 = 0.16269476711750031;
static constexpr double diurn_RH_core_psiP = 0.29972541332244873;
static constexpr double diurn_RH_core_alpha = -0.09353163093328476;
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