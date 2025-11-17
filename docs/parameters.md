# Model Parameter Reference

The HarmoClimate models are now expressed as explicit linear combinations of annual and diurnal harmonics. Three independent outputs are fitted:

- Temperature in degrees Celsius (`T`)
- Specific humidity in kilograms of water vapour per kilogram of moist air (`Q`)
- Station pressure in hectopascals (`P`)

Each exported JSON bundle contains everything needed to evaluate the corresponding target without additional dependencies.

---

## Metadata

Every model JSON exposes a `metadata` object. Key fields:

| Field                   | Description                                                                  |
| ----------------------- | ---------------------------------------------------------------------------- |
| `version`               | Model format version (`MODEL_VERSION`).                                      |
| `generated_at_utc`      | ISO-8601 timestamp of export.                                                |
| `country_code`          | ISO country code for the station.                                            |
| `author`                | Author string embedded in artefacts.                                         |
| `target_variable`       | One of `T`, `Q`, or `P`.                                                     |
| `target_unit`           | `degC`, `kg/kg`, or `hPa` respectively.                                      |
| `station_code`          | Meteo-France `NUM_POSTE` identifier (if available).                          |
| `station_usual_name`    | Name returned by the ingestion pipeline (`NOM_USUEL`).                       |
| `longitude_deg`         | Mean station longitude, used to convert UTC hour to local solar hour.        |
| `latitude_deg`          | Mean station latitude (metadata only).                                       |
| `altitude_m`            | Mean station altitude (metadata only).                                       |
| `delta_utc_solar_h`     | Offset (hours) from UTC time to local solar time.                            |
| `source_data_utc_start` | UTC timestamp of the first observation in the training dataset (or `null`).  |
| `source_data_utc_end`   | UTC timestamp of the last observation in the training dataset (or `null`).   |
| `error_envelope`        | Nested object describing training error statistics (see below).              |
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
| `name`   | Parameter identifier (`c0`, `a1`, `b1`, `a2`, `b2`, …).                                     |
| `role`   | Human-readable role (`offset`, `diurnal_cos_m`, `diurnal_sin_m`).                           |
| `n_annual` | Number of annual harmonics used for this parameter (≥ 0).                                 |
| `start`  | Index of the first coefficient for this parameter inside the flattened array.               |
| `length` | Number of coefficients associated with the parameter (equals `1 + 2 * n_annual`).           |
| `unit`   | Optional unit string (present for temperature/specific humidity parameters, omitted for offsets). |

The coefficient order for a parameter is always:

```
[ constant, cos(1⋅ωₐ day), sin(1⋅ωₐ day), … , cos(n⋅ωₐ day), sin(n⋅ωₐ day) ]
```


with ωₐ = 2π / 365.242189.

### Parameter glossary

| Name | Role            | Interpretation                                                                  |
| ---- | --------------- | ------------------------------------------------------------------------------- |
| `c0` | `offset`        | Annual envelope of the mean value for the target.                               |
| `aₘ` | `diurnal_cos_m` | Cosine component of the *m*-th diurnal harmonic, modulated by the annual cycle. |
| `bₘ` | `diurnal_sin_m` | Sine component of the *m*-th diurnal harmonic, modulated by the annual cycle.   |

---

## Mathematical model

**Time basis.** We work in **true solar time**. Let $`t`$ be the time in **hours**, corrected for longitude and equation of time using `delta_utc_solar_h`.
Define the angular frequencies:
```math
\omega_y=\frac{2\pi}{365.242189\times 24}\quad\text{(rad/hour)},\qquad
\omega_d=\frac{2\pi}{24}\quad\text{(rad/hour)}.
```

**Full formulation with seasonal modulation (default: 3 annual × 3 diurnal → 49 parameters per target).**  
The prediction $(\hat{y}(t)`$ is:
```math
\hat{y}(t)=c_0(t)+\sum_{m=1}^{n_{\text{diurnal}}}\Big(a_m(t)\cos(m\omega_d t)+b_m(t)\sin(m\omega_d t)\Big),
```
with the **annual envelope** and **annual modulations** defined as:
```math
c_0(t)=\beta_0+\sum_{k=1}^{3}\big(C_k\cos(k\omega_y t)+S_k\sin(k\omega_y t)\big),
```
```math
a_m(t)=\alpha_{m,0}+\sum_{k=1}^{3}\big(\alpha^{c}_{m,k}\cos(k\omega_y t)+\alpha^{s}_{m,k}\sin(k\omega_y t)\big),
```
```math
b_m(t)=\beta_{m,0}+\sum_{k=1}^{3}\big(\beta^{c}_{m,k}\cos(k\omega_y t)+\beta^{s}_{m,k}\sin(k\omega_y t)\big).
```

**Parameter count (per target).**
Annual envelope $`c_0(t)`$: $`1+3\times2=7`$.
For each diurnal harmonic $`m\in\{1,2,3\}`$: $`a_m(t)`$ has 7 and $`b_m(t)`$ has 7.
**Total:** $`7+3\times(7+7)=49`$.

**Interpretation.**
The term $`c_0(t)`$ captures the slow annual cycle. The pairs $`(a_m(t), b_m(t))`$ allow both **amplitude** and **phase** of the diurnal structure to vary smoothly with the season:
```math
A_m(t)=\sqrt{a_m(t)^2+b_m(t)^2},\qquad
\phi_m(t)=\mathrm{atan2}(b_m(t),\,a_m(t)).
```

**Correspondence with JSON layout.**
- `c0` coefficients map to $`\beta_0, C_k, S_k`$.
- `a1`,`a2`,`a3` map to $`\alpha_{m,0}, \alpha^c_{m,k}, \alpha^s_{m,k}`$.
- `b1`,`b2`,`b3` map to $`\beta_{m,0}, \beta^c_{m,k}, \beta^s_{m,k}`$.
The array order inside each block is `[constant, cos(1⋅ωₐ), sin(1⋅ωₐ), cos(2⋅ωₐ), sin(2⋅ωₐ), cos(3⋅ωₐ), sin(3⋅ωₐ)]`.

---

## Error envelope

`metadata.error_envelope` summarises the residual error on the training dataset:

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
