"""Project-wide configuration constants and filesystem layout."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path

# ----------------------------- Project metadata -----------------------------
COUNTRY_CODE = "fr"
MODEL_VERSION = "1.0"
STATION_CODE = "18033001"
AUTHOR_NAME = "HarmoClimate"

CHUNK_SIZE = 200_000

# ----------------------------- Model configuration -----------------------------
N_DIURNAL_HARMONICS = 3
DEFAULT_ANNUAL_HARMONICS = 3
ANNUAL_HARMONICS_PER_PARAM: dict[str, int] = {}


def slugify_station_name(name: str) -> str:
    """Normalize a station usual name for filesystem usage."""

    lowered = name.strip().lower().replace(" ", "_").replace("-", "_")
    return "".join(ch for ch in lowered if ch.isalnum() or ch == "_")


def compute_output_basename(station_slug: str) -> str:
    """Build the shared basename used for generated artefacts."""

    return f"{COUNTRY_CODE.lower()}_{station_slug}"


@dataclass(frozen=True)
class ArtifactPaths:
    """Resolved filesystem paths for generated artefacts."""

    parquet: Path
    model_temperature_json: Path
    model_specific_humidity_json: Path
    model_pressure_json: Path
    cpp_header: Path


# ----------------------------- Derived paths -----------------------------
PROJECT_ROOT = Path(__file__).resolve().parents[2]
GENERATED_DIR = PROJECT_ROOT / "generated"
DATA_DIR = GENERATED_DIR / "data"
MODEL_DIR = GENERATED_DIR / "models"
TEMPLATE_DIR = GENERATED_DIR / "templates"
MEDIA_DIR = GENERATED_DIR / "media"


def build_artifact_paths(station_slug: str) -> ArtifactPaths:
    """Return the Parquet/JSON/C++ paths for a given station slug."""

    basename = compute_output_basename(station_slug)
    return ArtifactPaths(
        parquet=DATA_DIR / f"{basename}.parquet",
        model_temperature_json=MODEL_DIR / f"{basename}_temperature.json",
        model_specific_humidity_json=MODEL_DIR / f"{basename}_specific_humidity.json",
        model_pressure_json=MODEL_DIR / f"{basename}_pressure.json",
        cpp_header=TEMPLATE_DIR / f"{basename}.hpp",
    )


def department_code_from_station(station_code: str = STATION_CODE) -> str:
    """Extract the French department code from the Meteo-France station code."""

    code = str(station_code).strip()
    if len(code) < 2:
        raise ValueError("Station code must contain at least two digits.")
    return code[:2]


# ----------------------------- Remote data sources -----------------------------


def build_department_urls(department_code: str) -> list[str]:
    """Return archive URLs for a given department code."""

    code = department_code.strip()
    if not code:
        raise ValueError("Department code must be a non-empty string.")

    return [
        f"https://object.files.data.gouv.fr/meteofrance/data/synchro_ftp/BASE/HOR/H_{code}_2000-2009.csv.gz",
        f"https://object.files.data.gouv.fr/meteofrance/data/synchro_ftp/BASE/HOR/H_{code}_2010-2019.csv.gz",
        f"https://object.files.data.gouv.fr/meteofrance/data/synchro_ftp/BASE/HOR/H_{code}_previous-2020-2023.csv.gz",
        f"https://object.files.data.gouv.fr/meteofrance/data/synchro_ftp/BASE/HOR/H_{code}_latest-2024-2025.csv.gz",
    ]


def build_urls_for_station(station_code: str = STATION_CODE) -> list[str]:
    """Return archive URLs for the department associated with the station."""

    department_code = department_code_from_station(station_code)
    return build_department_urls(department_code)


URLS = build_urls_for_station()

# ----------------------------- Visualization defaults -----------------------------
SAMPLES_PER_DAY = 96

__all__ = [
    "ArtifactPaths",
    "AUTHOR_NAME",
    "ANNUAL_HARMONICS_PER_PARAM",
    "DEFAULT_ANNUAL_HARMONICS",
    "CHUNK_SIZE",
    "COUNTRY_CODE",
    "DATA_DIR",
    "GENERATED_DIR",
    "MEDIA_DIR",
    "MODEL_DIR",
    "MODEL_VERSION",
    "PROJECT_ROOT",
    "SAMPLES_PER_DAY",
    "STATION_CODE",
    "TEMPLATE_DIR",
    "N_DIURNAL_HARMONICS",
    "URLS",
    "build_artifact_paths",
    "build_department_urls",
    "compute_output_basename",
    "department_code_from_station",
    "slugify_station_name",
    "build_urls_for_station",
]
