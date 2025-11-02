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
MODEL_FILES=("${MODELS_DIR}"/*.json)
shopt -u nullglob

if [[ ${#MODEL_FILES[@]} -eq 0 ]]; then
  echo "No model JSON files found in ${MODELS_DIR}" >&2
  exit 0
fi

for model_path in "${MODEL_FILES[@]}"; do
  model_name="$(basename "${model_path}")"
  echo "[HarmoClimate] Rendering plot for ${model_name}"
  "${PYTHON_BIN}" "${ROOT_DIR}/main.py" display "${model_path}"
done
