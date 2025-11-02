#!/bin/bash
# Script to build, test, and generate coverage report

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="$PROJECT_ROOT/build"

echo "========================================="
echo "Building, Testing, and Generating Coverage"
echo "========================================="
echo ""

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure
echo "Configuring CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build
echo "Building project..."
cmake --build . -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 2)

# Run tests
echo ""
echo "Running tests..."
ctest --output-on-failure

# Generate coverage
if command -v lcov &> /dev/null && command -v genhtml &> /dev/null; then
    echo ""
    echo "Generating coverage report..."
    make coverage

    echo ""
    echo "========================================="
    echo "Coverage Report Generated!"
    echo "========================================="
    echo ""
    echo "View the report:"
    echo "  Open: $BUILD_DIR/coverage/html/index.html"
    echo ""

    # Try to open the report automatically
    if command -v xdg-open &> /dev/null; then
        xdg-open "$BUILD_DIR/coverage/html/index.html" &
    elif command -v open &> /dev/null; then
        open "$BUILD_DIR/coverage/html/index.html" &
    fi
else
    echo ""
    echo "⚠️ lcov and/or genhtml not found. Skipping coverage report generation."
    echo "Install them to generate coverage reports:"
    echo "  Ubuntu/Debian: sudo apt-get install lcov"
    echo "  macOS: brew install lcov"
fi

echo ""
echo "✅ All tasks completed successfully!"
