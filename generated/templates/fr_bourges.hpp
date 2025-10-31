// Auto-generated factorized model (annual + diurnal)
// Target city / station (France):
//   Station code : 18033001
//   Station name : BOURGES
#pragma once
#include <cmath>
namespace harmoclimat {
static constexpr double longitude_deg = 2.3598330000000014;
static constexpr double latitude_deg  = 47.059167000000009;
static constexpr double delta_utc_solar_h = 0.15732220000000002;
static constexpr double annual_T_mu = 1.234075665473938;
static constexpr double annual_T_A1 = 0.84003162384033203;
static constexpr double annual_T_ph1 = -2.8147890567779541;
static constexpr double annual_T_A2 = 0.064055845141410828;
static constexpr double annual_T_ph2 = 1.5921535491943359;
static constexpr double annual_RH_mu = 75.951850891113281;
static constexpr double annual_RH_A1 = 11.216187477111816;
static constexpr double annual_RH_ph1 = 6.1330733299255371;
static constexpr double annual_RH_A2 = 3.4240245819091797;
static constexpr double annual_RH_ph2 = -1.0895266532897949;
static constexpr double diurn_T_a0 = 0.35951367020606995;
static constexpr double diurn_T_a1 = -0.16208350658416748;
static constexpr double diurn_T_psiA = -0.0078283278271555901;
static constexpr double diurn_T_ph0 = -2.7689146995544434;
static constexpr double diurn_T_k1 = 0.15572310984134674;
static constexpr double diurn_T_psiP = 0.25752738118171692;
static constexpr double diurn_T_alpha = 0.05408826470375061;
static constexpr double diurn_RH_beta_c = -10.514922142028809;
static constexpr double diurn_RH_core_a0 = 9.911107063293457;
static constexpr double diurn_RH_core_a1 = -5.084780216217041;
static constexpr double diurn_RH_core_psiA = -0.014223044738173485;
static constexpr double diurn_RH_core_ph0 = 0.37203997373580933;
static constexpr double diurn_RH_core_k1 = 0.12073667347431183;
static constexpr double diurn_RH_core_psiP = 0.20717106759548187;
static constexpr double diurn_RH_core_alpha = -0.090574808418750763;
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