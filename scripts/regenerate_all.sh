#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
MODELS_DIR="${ROOT_DIR}/generated/models"

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
  echo "[HarmoClimate] Regenerating ${model_name}"
  python "${ROOT_DIR}/main.py" regenerate "${model_path}"
done
