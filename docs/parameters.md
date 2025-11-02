# Model Parameter Reference

The HarmoClimate models are now expressed as explicit linear combinations of annual and diurnal harmonics. Three independent outputs are fitted:

- Temperature in degrees Celsius (`T`)
- Specific humidity in kilograms of water vapour per kilogram of moist air (`Q`)
- Station pressure in hectopascals (`P`)

Each exported JSON bundle contains everything needed to evaluate the corresponding target without additional dependencies.

---

## Metadata

Every model JSON exposes a `metadata` object. Key fields:

| Field                   | Description                                                                 |
| ----------------------- | --------------------------------------------------------------------------- |
| `version`               | Model format version (from `MODEL_VERSION`).                                 |
| `generated_at_utc`      | ISO-8601 timestamp of export.                                                |
| `country_code`          | ISO country code for the station.                                            |
| `author`                | Author string embedded in artefacts.                                         |
| `station_usual_name`    | Name returned by the ingestion pipeline (`NOM_USUEL`).                       |
| `station_code`          | Meteo-France `NUM_POSTE` identifier (if available).                          |
| `longitude_deg`         | Mean station longitude, used to convert UTC hour to local solar hour.        |
| `latitude_deg`          | Mean station latitude (metadata only).                                       |
| `altitude_m`            | Mean station altitude (metadata only).                                       |
| `delta_utc_solar_h`     | Offset (hours) from UTC time to local solar time.                            |
| `target_variable`       | One of `T`, `Q`, or `P`.                                                   |
| `target_unit`           | `degC`, `kg/kg`, or `hPa` respectively.                                      |
| `time_basis`            | Fixed solar-time calendar descriptor (`{"type": "solar", "days": 365.242189, "calendar": "no-leap"}`). |

---

## Model description

The `model` object specifies how to read the coefficient vector:

- `n_diurnal`: number of diurnal harmonics included for the target (default: 3).
- `params_layout`: ordered list describing each block of coefficients.
- `coefficients`: flattened numeric array containing all fitted values.

### Layout entries

Each entry in `params_layout` carries:

| Field    | Meaning                                                                                     |
| -------- | ------------------------------------------------------------------------------------------- |
| `name`   | Parameter identifier (`c0`, `a1`, `b1`, `a2`, `b2`, …).                                      |
| `role`   | Human-readable role (`offset`, `diurnal_cos_m`, `diurnal_sin_m`).                           |
| `n_annual` | Number of annual harmonics used for this parameter (≥ 0).                                   |
| `start`  | Index of the first coefficient for this parameter inside the flattened array.               |
| `length` | Number of coefficients associated with the parameter (equals `1 + 2 * n_annual`).           |
| `unit`   | Optional unit string (present for temperature/specific humidity parameters, omitted for offsets). |

The coefficient order for a parameter is always:

```
[ constant, cos(1⋅ωₐ day), sin(1⋅ωₐ day), … , cos(n⋅ωₐ day), sin(n⋅ωₐ day) ]
```

with ωₐ = 2π / 365.242189.

### Parameter glossary

| Name | Role                  | Interpretation                                                                               |
| ---- | --------------------- | ------------------------------------------------------------------------------------------- |
| `c0` | `offset`              | Annual envelope of the mean value for the target.                                           |
| `aₘ` | `diurnal_cos_m`       | Cosine component of the *m*-th diurnal harmonic, modulated by the annual envelope.          |
| `bₘ` | `diurnal_sin_m`       | Sine component of the *m*-th diurnal harmonic, modulated by the annual envelope.            |

The diurnal frequency is ω_d = 2π / 24 (solar hours). During evaluation the library reconstructs:

```
target(day, hour) = envelope_c0(day)
                  + Σ_{m=1..n_diurnal} envelope_a_m(day) * cos(m * ω_d * hour)
                  + Σ_{m=1..n_diurnal} envelope_b_m(day) * sin(m * ω_d * hour)
```

where each `envelope_*` term is the annual series defined by its coefficient block.

---

## Metrics

The `metrics` object provides simple quality indicators for the fitted target:

| Field      | Meaning                                               |
| ---------- | ----------------------------------------------------- |
| `mae`      | Mean absolute error (°C, kg/kg, or hPa).              |
| `bias`     | Mean error (signed).                                  |
| `err_p05`  | 5th percentile of the error distribution.             |
| `err_p95`  | 95th percentile of the error distribution.            |

These metrics are computed on the training dataset and allow quick comparisons between model revisions.

---

## Summary

- All three exported models (temperature, specific humidity, pressure) share the same structure and differ only by their targets and units.
- Inspect `params_layout` to understand which slice of the coefficient vector affects a given harmonic.
- Modifying coefficients is as simple as editing the JSON array, provided the layout structure is preserved.
- The generated C++ header mirrors the exact layout shown here, enabling zero-dependency evaluation on embedded targets.
