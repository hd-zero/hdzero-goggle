#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
X86_64_SETUP_SCRIPT="${SCRIPT_DIR}/.devcontainer/x86_64/setup.sh"

exec bash "${X86_64_SETUP_SCRIPT}"