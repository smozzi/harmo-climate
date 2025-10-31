"""Project-wide configuration constants and filesystem layout."""

from pathlib import Path

# ----------------------------- Project metadata -----------------------------
DEPARTMENT_CODE = 18
TARGET_CITY_NAME = "Bourges"
STATION_KEYWORD = "BOURGES"
COUNTRY_CODE = "fr"
MODEL_VERSION = "1.0"

CHUNK_SIZE = 200_000
RANDOM_SEED = 42

# ----------------------------- Naming helpers -----------------------------
def _slugify_city_name(name: str) -> str:
    """Normalize the target city name for filesystem usage."""

    lowered = name.strip().lower().replace(" ", "_").replace("-", "_")
    return "".join(ch for ch in lowered if ch.isalnum() or ch == "_")


TARGET_CITY_SLUG = _slugify_city_name(TARGET_CITY_NAME)
OUTPUT_FILE_BASENAME = f"{COUNTRY_CODE.lower()}_{TARGET_CITY_SLUG}"

# ----------------------------- Derived paths -----------------------------
PROJECT_ROOT = Path(__file__).resolve().parents[2]
GENERATED_DIR = PROJECT_ROOT / "generated"
DATA_DIR = GENERATED_DIR / "data"
MODEL_DIR = GENERATED_DIR / "models"
TEMPLATE_DIR = GENERATED_DIR / "templates"

PARQUET_PATH = DATA_DIR / f"{OUTPUT_FILE_BASENAME}.parquet"
MODEL_JSON_PATH = MODEL_DIR / f"{OUTPUT_FILE_BASENAME}.json"
CPP_HEADER_PATH = TEMPLATE_DIR / f"{OUTPUT_FILE_BASENAME}.hpp"

# ----------------------------- Remote data sources -----------------------------
URLS = [
    f"https://object.files.data.gouv.fr/meteofrance/data/synchro_ftp/BASE/HOR/H_{DEPARTMENT_CODE}_2000-2009.csv.gz",
    f"https://object.files.data.gouv.fr/meteofrance/data/synchro_ftp/BASE/HOR/H_{DEPARTMENT_CODE}_2010-2019.csv.gz",
    f"https://object.files.data.gouv.fr/meteofrance/data/synchro_ftp/BASE/HOR/H_{DEPARTMENT_CODE}_previous-2020-2023.csv.gz",
    f"https://object.files.data.gouv.fr/meteofrance/data/synchro_ftp/BASE/HOR/H_{DEPARTMENT_CODE}_latest-2024-2025.csv.gz",
]

# ----------------------------- Visualization defaults -----------------------------
SAMPLES_PER_DAY = 96

__all__ = [
    "CHUNK_SIZE",
    "COUNTRY_CODE",
    "CPP_HEADER_PATH",
    "DATA_DIR",
    "DEPARTMENT_CODE",
    "GENERATED_DIR",
    "MODEL_DIR",
    "MODEL_JSON_PATH",
    "MODEL_VERSION",
    "OUTPUT_FILE_BASENAME",
    "PARQUET_PATH",
    "PROJECT_ROOT",
    "RANDOM_SEED",
    "SAMPLES_PER_DAY",
    "STATION_KEYWORD",
    "TARGET_CITY_NAME",
    "TARGET_CITY_SLUG",
    "TEMPLATE_DIR",
    "URLS",
]
