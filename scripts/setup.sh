#!/usr/bin/env bash
#
# scripts/setup.sh
# Prepare a local development environment for the "harmoclimate" project.
# - create .venv
# - install the project (pyproject.toml) in editable mode
#
# Usage:
#   ./scripts/setup.sh
#
set -e

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
VENV_DIR="$PROJECT_ROOT/.venv"

# 1) Find python3
if command -v python3 >/dev/null 2>&1; then
  PYTHON=python3
else
  echo "❌ python3 not found in PATH."
  exit 1
fi

# 2) Create venv if missing
if [[ ! -d "$VENV_DIR" ]]; then
  echo "➡️  Creating virtual environment in $VENV_DIR"
  "$PYTHON" -m venv "$VENV_DIR"
else
  echo "ℹ️  Virtual environment already exists at $VENV_DIR"
fi

# 3) Activate venv
# shellcheck disable=SC1090
source "$VENV_DIR/bin/activate"

# 4) Upgrade pip
echo "➡️  Upgrading pip inside the virtual environment…"
pip install --upgrade pip

# 5) Install the project (editable)
echo "➡️  Installing the project in editable mode…"
pip install -e "$PROJECT_ROOT"

echo
echo "✅ Setup completed."
echo "To activate the environment later:"
echo "  source .venv/bin/activate"
