// Auto-generated factorized model (annual + diurnal)
// Target city / station (France):
//   Station code : 44020001
//   Station name : NANTES-BOUGUENAIS
#pragma once
#include <cmath>
namespace harmoclimat {
static constexpr double longitude_deg = -1.6088330000000006;
static constexpr double latitude_deg  = 47.149999999999984;
static constexpr double delta_utc_solar_h = -0.10725553333333336;
static constexpr double annual_T_mu = 1.2729642391204834;
static constexpr double annual_T_A1 = 0.69459259510040283;
static constexpr double annual_T_ph1 = -2.7675545215606689;
static constexpr double annual_T_A2 = 0.047960873693227768;
static constexpr double annual_T_ph2 = 1.6246868371963501;
static constexpr double annual_RH_mu = 78.986228942871094;
static constexpr double annual_RH_A1 = 8.4528179168701172;
static constexpr double annual_RH_ph1 = 6.0635595321655273;
static constexpr double annual_RH_A2 = 1.9103602170944214;
static constexpr double annual_RH_ph2 = -0.78313380479812622;
static constexpr double diurn_T_a0 = 0.34084683656692505;
static constexpr double diurn_T_a1 = -0.14729219675064087;
static constexpr double diurn_T_psiA = 0.07171427458524704;
static constexpr double diurn_T_ph0 = -2.9260993003845215;
static constexpr double diurn_T_k1 = 0.18610157072544098;
static constexpr double diurn_T_psiP = 0.33213391900062561;
static constexpr double diurn_T_alpha = 0.10230478644371033;
static constexpr double diurn_RH_beta_c = -8.6041641235351562;
static constexpr double diurn_RH_core_a0 = 10.225109100341797;
static constexpr double diurn_RH_core_a1 = -5.0009031295776367;
static constexpr double diurn_RH_core_psiA = 0.090884588658809662;
static constexpr double diurn_RH_core_ph0 = 0.23667904734611511;
static constexpr double diurn_RH_core_k1 = 0.16437990963459015;
static constexpr double diurn_RH_core_psiP = 0.37709301710128784;
static constexpr double diurn_RH_core_alpha = -0.13970839977264404;
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