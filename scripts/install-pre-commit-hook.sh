#!/bin/bash
# Script to install pre-commit hooks

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
HOOK_DIR="$PROJECT_ROOT/.git/hooks"

echo "========================================="
echo "Installing Pre-commit Hooks"
echo "========================================="
echo ""

# Check if .git directory exists
if [ ! -d "$PROJECT_ROOT/.git" ]; then
    echo "❌ .git directory not found. Are you in a git repository?"
    exit 1
fi

# Create hooks directory if it doesn't exist
mkdir -p "$HOOK_DIR"

# Create pre-commit hook
cat > "$HOOK_DIR/pre-commit" << 'EOF'
#!/bin/bash
# Pre-commit hook for cpp-odbc-wrapper

set -e

echo "Running pre-commit checks..."

# Check for trailing whitespace
echo "Checking for trailing whitespace..."
if git diff --cached --name-only | grep -E '\.(cpp|h|hpp)$' | xargs grep -n '\s$' 2>/dev/null; then
    echo "❌ Found trailing whitespace in staged files"
    echo "Please remove trailing whitespace before committing"
    exit 1
fi

# Check for debug code
echo "Checking for debug code..."
if git diff --cached --name-only | grep -E '\.(cpp|h)$' | xargs grep -nE 'std::cout|printf\(' 2>/dev/null; then
    echo "⚠️ Warning: Found potential debug code (std::cout, printf)"
    echo "Consider using OdbcLogger instead"
    # Don't fail, just warn
fi

# Run cppcheck on staged files
if command -v cppcheck &> /dev/null; then
    echo "Running cppcheck on staged files..."
    STAGED_CPP_FILES=$(git diff --cached --name-only --diff-filter=ACM | grep -E '\.(cpp)$' || true)

    if [ ! -z "$STAGED_CPP_FILES" ]; then
        for file in $STAGED_CPP_FILES; do
            if [ -f "$file" ]; then
                cppcheck --enable=warning,style,performance --quiet --std=c++17 -I include/ "$file"
            fi
        done
    fi
fi

echo "✅ Pre-commit checks passed!"
exit 0
EOF

# Make the hook executable
chmod +x "$HOOK_DIR/pre-commit"

echo "✅ Pre-commit hook installed successfully!"
echo ""
echo "The hook will run automatically before each commit."
echo "To skip the hook (not recommended), use: git commit --no-verify"
