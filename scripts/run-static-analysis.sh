#!/bin/bash
# Script to run static analysis tools locally

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

echo "========================================="
echo "Running Static Analysis"
echo "========================================="
echo ""

# Check if cppcheck is installed
if ! command -v cppcheck &> /dev/null; then
    echo "❌ cppcheck is not installed"
    echo "Please install it:"
    echo "  Ubuntu/Debian: sudo apt-get install cppcheck"
    echo "  macOS: brew install cppcheck"
    exit 1
fi

echo "✅ cppcheck version: $(cppcheck --version)"
echo ""

# Run cppcheck
echo "Running cppcheck..."
cd "$PROJECT_ROOT"

cppcheck \
    --enable=all \
    --suppressions-list=.cppcheck-suppressions.txt \
    --inline-suppr \
    --std=c++17 \
    --template='{file}:{line}:{severity}:{message}' \
    --quiet \
    -I include/ \
    src/ \
    2>&1 | tee cppcheck-report.txt

if [ ${PIPESTATUS[0]} -eq 0 ]; then
    echo "✅ cppcheck passed!"
else
    echo "❌ cppcheck found issues"
    exit 1
fi

echo ""
echo "Static analysis complete!"
echo "Report saved to: cppcheck-report.txt"
