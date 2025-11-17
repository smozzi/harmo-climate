"""Data ingestion utilities for HarmoClimate."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Iterable, List, Tuple

import pandas as pd

try:
    import pyarrow as pa
    import pyarrow.parquet as pq
except ImportError as exc:  # pragma: no cover - execution stops before tests
    raise ImportError("pyarrow is required for HarmoClimate data ingestion.") from exc

from .config import CHUNK_SIZE, STATION_CODE, build_artifact_paths, slugify_station_name
from .core import DATASET_COLUMNS


@dataclass
class StationRecord:
    """Lightweight station-level record captured while streaming."""

    station_code: str | None
    station_name: str
    lon: float
    lat: float
    alti: float
    delta_utc_solar_h: float


@dataclass
class StreamResult:
    """Container bundling streamed station records and generated artefact info."""

    station_records: List[StationRecord]
    parquet_path: Path
    station_name: str
    station_slug: str


def choose_columns(colnames: Iterable[str]) -> Tuple[str, str, str, str, str, str, str, str, str]:
    """Validate the expected Meteo-France column names."""

    col_code = "NUM_POSTE"
    col_station = "NOM_USUEL"
    col_dt = "AAAAMMJJHH"
    col_T = "T"
    col_U = "U"
    col_lon = "LON"
    col_lat = "LAT"
    col_alti = "ALTI"
    col_p = "PSTAT"

    required = [col_code, col_station, col_dt, col_T, col_U, col_lon, col_lat, col_alti, col_p]
    missing = [c for c in required if c not in colnames]
    if missing:
        raise RuntimeError(f"Missing required columns: {missing} in {colnames}")

    return col_code, col_station, col_dt, col_T, col_U, col_lon, col_lat, col_alti, col_p


def parse_dt_aaaammjjhh(series: pd.Series) -> pd.Series:
    """Parse AAAAMMJJHH timestamps expressed in local French time."""

    s = series.astype("string").str.strip()
    s = s.str.replace(r"\\.0$", "", regex=True)
    s = s.str.pad(10, side="right", fillchar="0")
    s = s.str.slice(0, 10)
    return pd.to_datetime(s, format="%Y%m%d%H", errors="coerce")


def _normalize_station_code(value: object) -> str:
    """Coerce a station code value to the canonical string representation."""

    code = str(value).strip()
    if not code:
        return ""
    code = code.replace(".0", "")  # CSV exports frequently store codes as floats
    if code.lower() in {"nan", "na", "<na>", "none"}:
        return ""
    return code


def stream_filter_to_disk(
    urls: Iterable[str],
    station_code: str = STATION_CODE,
    chunk_size: int = CHUNK_SIZE,
) -> StreamResult:
    """Stream Meteo-France archives, filter rows, and persist the reduced dataset."""

    station_code_str = _normalize_station_code(station_code)

    writer: pq.ParquetWriter | None = None
    parquet_path: Path | None = None
    station_name: str | None = None
    station_slug: str | None = None

    total_kept = 0
    station_records: List[StationRecord] = []

    for url in urls:
        print(f"[Stream] {url}")
        reader = pd.read_csv(
            url,
            compression="gzip",
            sep=";",
            encoding="utf-8",
            chunksize=chunk_size,
            low_memory=True,
        )
        for i, chunk in enumerate(reader, 1):
            (
                col_code,
                col_station,
                col_dt,
                col_T,
                col_U,
                col_lon,
                col_lat,
                col_alti,
                col_p,
            ) = choose_columns(list(chunk.columns))

            code_series = chunk[col_code].astype("string").str.strip().str.replace(r"\\.0$", "", regex=True)
            mask = code_series == station_code_str
            if not mask.any():
                continue

            keep_cols = [col_dt, col_T, col_U, col_lon, col_station, col_lat, col_code, col_alti, col_p]
            sub = chunk.loc[mask, keep_cols].copy()
            if sub.empty:
                continue

            if station_name is None:
                names = sub[col_station].astype("string").str.strip()
                candidate = next((name for name in names if name), "")
                fallback_name = station_code_str if not candidate else candidate
                station_name = candidate or fallback_name
                station_slug = slugify_station_name(station_name) or slugify_station_name(fallback_name)
                parquet_path = build_artifact_paths(station_slug).parquet
                parquet_path.parent.mkdir(parents=True, exist_ok=True)
                parquet_path.unlink(missing_ok=True)

            sub["dt_local"] = parse_dt_aaaammjjhh(sub[col_dt])
            sub["T"] = pd.to_numeric(sub[col_T], errors="coerce")
            sub["RH"] = pd.to_numeric(sub[col_U], errors="coerce")
            sub["LON"] = pd.to_numeric(sub[col_lon], errors="coerce")
            sub["LAT"] = pd.to_numeric(sub[col_lat], errors="coerce")
            sub["ALTI"] = pd.to_numeric(sub[col_alti], errors="coerce")
            sub["P"] = pd.to_numeric(sub[col_p], errors="coerce")
            sub["STATION_CODE"] = code_series[mask].to_numpy()
            sub["STATION_NAME"] = sub[col_station].astype("string").str.strip()

            sub = sub.dropna(subset=["dt_local", "T", "RH", "LON", "LAT", "ALTI", "P"])

            sub["dt_local"] = sub["dt_local"].dt.tz_localize(
                "Europe/Paris",
                nonexistent="shift_forward",
                ambiguous="NaT",
            )
            sub = sub.dropna(subset=["dt_local"])
            sub["DT_UTC"] = sub["dt_local"].dt.tz_convert("UTC")
            sub["T"] = sub["T"].astype("float32")
            sub["RH"] = sub["RH"].clip(0, 100).astype("float32")
            sub["P"] = sub["P"].astype("float32")
            sub["LON"] = sub["LON"].astype("float32")
            sub["LAT"] = sub["LAT"].astype("float32")
            sub["ALTI"] = sub["ALTI"].astype("float32")
            sub["STATION_CODE"] = sub["STATION_CODE"].astype("string")
            sub["STATION_NAME"] = sub["STATION_NAME"].astype("string")

            for _, row in sub.iterrows():
                lon_value = float(row["LON"])
                name_value = row["STATION_NAME"]
                if pd.isna(name_value):
                    name_str = ""
                else:
                    name_str = str(name_value)
                station_code_value = _normalize_station_code(row["STATION_CODE"])
                station_records.append(
                    StationRecord(
                        station_code=station_code_value or None,
                        station_name=name_str,
                        lon=lon_value,
                        lat=float(row["LAT"]),
                        alti=float(row["ALTI"]),
                        delta_utc_solar_h=lon_value / 15.0,
                    )
                )

            sub = sub[list(DATASET_COLUMNS)]

            table = pa.Table.from_pandas(sub, preserve_index=False)
            if parquet_path is None or station_slug is None:
                raise RuntimeError("Parquet output path was not initialised.")
            if writer is None:
                writer = pq.ParquetWriter(str(parquet_path), table.schema)
            writer.write_table(table)

            total_kept += len(sub)
            if i % 50 == 0:
                print(f"  chunks: {i:4d} | kept rows: {total_kept:,}")

    if writer is not None:
        writer.close()

    if parquet_path is None or station_name is None or station_slug is None:
        raise RuntimeError(f"No data found for station code {station_code_str}")

    print(f"[OK] Wrote {total_kept:,} filtered rows -> {parquet_path}")
    return StreamResult(
        station_records=station_records,
        parquet_path=parquet_path,
        station_name=station_name,
        station_slug=station_slug,
    )


__all__ = [
    "StationRecord",
    "StreamResult",
    "choose_columns",
    "parse_dt_aaaammjjhh",
    "stream_filter_to_disk",
]
