#!/usr/bin/env bash
#
# scripts/activate.sh
# Activate the local virtual environment for this project.
#
# Usage:
#   ./scripts/activate.sh
#   source ./scripts/activate.sh   # preferred, to keep the venv active in current shell
#
set -e

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
VENV_DIR="$PROJECT_ROOT/.venv"
ACTIVATE_FILE="$VENV_DIR/bin/activate"

if [[ ! -d "$VENV_DIR" ]]; then
  echo "❌ Virtual environment not found at $VENV_DIR"
  echo "   Run: ./scripts/setup.sh"
  exit 1
fi

if [[ ! -f "$ACTIVATE_FILE" ]]; then
  echo "❌ Activate script not found: $ACTIVATE_FILE"
  exit 1
fi

# If the user runs "source ./scripts/activate.sh", we want to activate in the current shell.
# If the user runs "./scripts/activate.sh" directly, this will activate in a subshell.
# shellcheck disable=SC1090
source "$ACTIVATE_FILE"

echo "✅ Virtual environment activated."
echo "Current Python:"
python -V
