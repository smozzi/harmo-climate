// Auto-generated factorized model (annual + diurnal)
// Target city / station (France):
//   Station code : 59343001
//   Station name : LILLE-LESQUIN
#pragma once
#include <cmath>
namespace harmoclimat {
static constexpr double longitude_deg = 3.0974999999999997;
static constexpr double latitude_deg  = 50.570000000000007;
static constexpr double delta_utc_solar_h = 0.20650000000000004;
static constexpr double annual_T_mu = 1.141330361366272;
static constexpr double annual_T_A1 = 0.75758945941925049;
static constexpr double annual_T_ph1 = -2.787938117980957;
static constexpr double annual_T_A2 = 0.045817993581295013;
static constexpr double annual_T_ph2 = 1.8075602054595947;
static constexpr double annual_RH_mu = 78.558746337890625;
static constexpr double annual_RH_A1 = 9.6219043731689453;
static constexpr double annual_RH_ph1 = 5.9794697761535645;
static constexpr double annual_RH_A2 = 1.7152165174484253;
static constexpr double annual_RH_ph2 = -0.51395553350448608;
static constexpr double diurn_T_a0 = 0.29145476222038269;
static constexpr double diurn_T_a1 = -0.15798266232013702;
static constexpr double diurn_T_psiA = -0.066472768783569336;
static constexpr double diurn_T_ph0 = -2.8132083415985107;
static constexpr double diurn_T_k1 = 0.22822563350200653;
static constexpr double diurn_T_psiP = 0.2542911171913147;
static constexpr double diurn_T_alpha = 0.063255950808525085;
static constexpr double diurn_RH_beta_c = -9.4656314849853516;
static constexpr double diurn_RH_core_a0 = 9.0730075836181641;
static constexpr double diurn_RH_core_a1 = -5.7246007919311523;
static constexpr double diurn_RH_core_psiA = -0.058045834302902222;
static constexpr double diurn_RH_core_ph0 = 0.32582572102546692;
static constexpr double diurn_RH_core_k1 = 0.13131263852119446;
static constexpr double diurn_RH_core_psiP = 0.4038236141204834;
static constexpr double diurn_RH_core_alpha = -0.091635763645172119;
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