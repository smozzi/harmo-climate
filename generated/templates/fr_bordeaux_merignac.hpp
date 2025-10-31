// Auto-generated factorized model (annual + diurnal)
// Target city / station (France):
//   Station code : 33281001
//   Station name : BORDEAUX-MERIGNAC
#pragma once
#include <cmath>
namespace harmoclimat {
static constexpr double longitude_deg = -0.6913330000000002;
static constexpr double latitude_deg  = 44.830666999999998;
static constexpr double delta_utc_solar_h = -0.046088866666666659;
static constexpr double annual_T_mu = 1.4092599153518677;
static constexpr double annual_T_A1 = 0.76857751607894897;
static constexpr double annual_T_ph1 = -2.7863636016845703;
static constexpr double annual_T_A2 = 0.064843803644180298;
static constexpr double annual_T_ph2 = 1.9396559000015259;
static constexpr double annual_RH_mu = 75.836647033691406;
static constexpr double annual_RH_A1 = 9.531926155090332;
static constexpr double annual_RH_ph1 = 6.0969529151916504;
static constexpr double annual_RH_A2 = 2.955230712890625;
static constexpr double annual_RH_ph2 = -0.76008868217468262;
static constexpr double diurn_T_a0 = 0.37703537940979004;
static constexpr double diurn_T_a1 = 0.1316574364900589;
static constexpr double diurn_T_psiA = 3.216270923614502;
static constexpr double diurn_T_ph0 = -2.8753180503845215;
static constexpr double diurn_T_k1 = 0.19669413566589355;
static constexpr double diurn_T_psiP = 0.33600491285324097;
static constexpr double diurn_T_alpha = 0.1077839732170105;
static constexpr double diurn_RH_beta_c = -10.514091491699219;
static constexpr double diurn_RH_core_a0 = 11.516726493835449;
static constexpr double diurn_RH_core_a1 = -4.6750736236572266;
static constexpr double diurn_RH_core_psiA = -0.01095321960747242;
static constexpr double diurn_RH_core_ph0 = 0.26021096110343933;
static constexpr double diurn_RH_core_k1 = 0.15294778347015381;
static constexpr double diurn_RH_core_psiP = 0.42316719889640808;
static constexpr double diurn_RH_core_alpha = -0.12535844743251801;
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