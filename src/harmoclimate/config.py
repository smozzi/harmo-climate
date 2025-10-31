"""Project-wide configuration constants and filesystem layout."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path

# ----------------------------- Project metadata -----------------------------
COUNTRY_CODE = "fr"
MODEL_VERSION = "1.0"
STATION_CODE = "18033001"

CHUNK_SIZE = 200_000
RANDOM_SEED = 42


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
    model_json: Path
    cpp_header: Path


# ----------------------------- Derived paths -----------------------------
PROJECT_ROOT = Path(__file__).resolve().parents[2]
GENERATED_DIR = PROJECT_ROOT / "generated"
DATA_DIR = GENERATED_DIR / "data"
MODEL_DIR = GENERATED_DIR / "models"
TEMPLATE_DIR = GENERATED_DIR / "templates"


def build_artifact_paths(station_slug: str) -> ArtifactPaths:
    """Return the Parquet/JSON/C++ paths for a given station slug."""

    basename = compute_output_basename(station_slug)
    return ArtifactPaths(
        parquet=DATA_DIR / f"{basename}.parquet",
        model_json=MODEL_DIR / f"{basename}.json",
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
    "CHUNK_SIZE",
    "COUNTRY_CODE",
    "DATA_DIR",
    "GENERATED_DIR",
    "MODEL_DIR",
    "MODEL_VERSION",
    "PROJECT_ROOT",
    "RANDOM_SEED",
    "SAMPLES_PER_DAY",
    "STATION_CODE",
    "TEMPLATE_DIR",
    "URLS",
    "build_artifact_paths",
    "build_department_urls",
    "compute_output_basename",
    "department_code_from_station",
    "slugify_station_name",
    "build_urls_for_station",
]
