"""HarmoClimate core package."""

from .config import (
    ANNUAL_HARMONICS_PER_PARAM,
    AUTHOR_NAME,
    CHUNK_SIZE,
    COUNTRY_CODE,
    DEFAULT_ANNUAL_HARMONICS,
    MODEL_VERSION,
    N_DIURNAL_HARMONICS,
    STATION_CODE,
    URLS,
    build_artifact_paths,
    build_department_urls,
    build_urls_for_station,
    compute_output_basename,
    department_code_from_station,
    slugify_station_name,
)

__all__ = [
    "ANNUAL_HARMONICS_PER_PARAM",
    "AUTHOR_NAME",
    "CHUNK_SIZE",
    "COUNTRY_CODE",
    "DEFAULT_ANNUAL_HARMONICS",
    "MODEL_VERSION",
    "N_DIURNAL_HARMONICS",
    "STATION_CODE",
    "URLS",
    "build_artifact_paths",
    "build_department_urls",
    "build_urls_for_station",
    "compute_output_basename",
    "department_code_from_station",
    "slugify_station_name",
]
