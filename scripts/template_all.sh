#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
MODELS_DIR="${ROOT_DIR}/generated/models"

if command -v python >/dev/null 2>&1; then
  PYTHON_BIN="python"
elif command -v python3 >/dev/null 2>&1; then
  PYTHON_BIN="python3"
else
  echo "No Python interpreter found (expected 'python' or 'python3')." >&2
  exit 1
fi

if [[ ! -d "${MODELS_DIR}" ]]; then
  echo "No generated models directory found at ${MODELS_DIR}" >&2
  exit 1
fi

shopt -s nullglob
TEMPERATURE_MODELS=("${MODELS_DIR}"/*_temperature.json)
shopt -u nullglob

if [[ ${#TEMPERATURE_MODELS[@]} -eq 0 ]]; then
  echo "No temperature model JSON files found in ${MODELS_DIR}" >&2
  exit 0
fi

mapfile -t MODEL_BASENAMES < <(for model_path in "${TEMPERATURE_MODELS[@]}"; do
  basename="$(basename "${model_path}")"
  echo "${basename%_temperature.json}"
done | sort -u)

for model_basename in "${MODEL_BASENAMES[@]}"; do
  echo "[HarmoClimate] Generating template for ${model_basename}"
  "${PYTHON_BIN}" "${ROOT_DIR}/main.py" template "${model_basename}" cpp
done
