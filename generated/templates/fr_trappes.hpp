// Auto-generated factorized model (annual + diurnal)
// Target city / station (France):
//   Station code : 78621001
//   Station name : TRAPPES
#pragma once
#include <cmath>
namespace harmoclimat {
static constexpr double longitude_deg = 2.0098330000000004;
static constexpr double latitude_deg  = 48.774332999999999;
static constexpr double delta_utc_solar_h = 0.13398886666666665;
static constexpr double annual_T_mu = 1.1637020111083984;
static constexpr double annual_T_A1 = 0.77975857257843018;
static constexpr double annual_T_ph1 = -2.8024852275848389;
static constexpr double annual_T_A2 = 0.05399041622877121;
static constexpr double annual_T_ph2 = 1.6115942001342773;
static constexpr double annual_RH_mu = 77.279960632324219;
static constexpr double annual_RH_A1 = 10.766310691833496;
static constexpr double annual_RH_ph1 = 6.0305871963500977;
static constexpr double annual_RH_A2 = 2.6754319667816162;
static constexpr double annual_RH_ph2 = -0.85954397916793823;
static constexpr double diurn_T_a0 = 0.30616593360900879;
static constexpr double diurn_T_a1 = -0.1594226211309433;
static constexpr double diurn_T_psiA = -0.045254651457071304;
static constexpr double diurn_T_ph0 = -2.7662782669067383;
static constexpr double diurn_T_k1 = 0.16200023889541626;
static constexpr double diurn_T_psiP = 0.32475465536117554;
static constexpr double diurn_T_alpha = 0.051687996834516525;
static constexpr double diurn_RH_beta_c = -9.8348808288574219;
static constexpr double diurn_RH_core_a0 = 9.4261255264282227;
static constexpr double diurn_RH_core_a1 = -5.0381383895874023;
static constexpr double diurn_RH_core_psiA = -0.042834147810935974;
static constexpr double diurn_RH_core_ph0 = 0.38014325499534607;
static constexpr double diurn_RH_core_k1 = 0.12091697007417679;
static constexpr double diurn_RH_core_psiP = 0.28386762738227844;
static constexpr double diurn_RH_core_alpha = -0.08283117413520813;
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