# Contributing to C++ ODBC Wrapper

Thank you for your interest in contributing to the C++ ODBC Wrapper project! We welcome contributions from the community.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [Development Setup](#development-setup)
- [How to Contribute](#how-to-contribute)
- [Coding Standards](#coding-standards)
- [Testing Guidelines](#testing-guidelines)
- [Commit Messages](#commit-messages)
- [Pull Request Process](#pull-request-process)

## Code of Conduct

By participating in this project, you agree to maintain a respectful and inclusive environment for all contributors.

## Getting Started

1. Fork the repository
2. Clone your fork: `git clone https://github.com/YOUR_USERNAME/cpp-odbc-wrapper.git`
3. Add upstream remote: `git remote add upstream https://github.com/psanodiya94/cpp-odbc-wrapper.git`
4. Create a feature branch: `git checkout -b feature/your-feature-name`

## Development Setup

### Prerequisites

- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.10+
- ODBC driver manager (unixODBC, iODBC, or Windows ODBC)
- Development dependencies: spdlog, fmt, googletest

### Building

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

### Running Tests

```bash
cd build
ctest --output-on-failure
```

### Installing Pre-commit Hooks

We strongly recommend installing the pre-commit hooks:

```bash
./scripts/install-pre-commit-hook.sh
```

This will automatically run checks before each commit to catch issues early.

## How to Contribute

### Types of Contributions

- **Bug Reports**: Open an issue with detailed information
- **Feature Requests**: Discuss new features in issues first
- **Bug Fixes**: Submit a PR with tests
- **New Features**: Discuss design in an issue before implementing
- **Documentation**: Improve inline docs, README, or guides
- **Tests**: Add tests to improve coverage

### Finding Work

- Check the [Issues](https://github.com/psanodiya94/cpp-odbc-wrapper/issues) page
- Look for `good first issue` or `help wanted` labels
- Propose new features or improvements

## Coding Standards

### C++ Style Guide

- **C++ Version**: Use C++17 features
- **Naming**:
  - Classes: `PascalCase` (e.g., `OdbcWrapper`)
  - Functions/Methods: `camelCase` (e.g., `executeQuery`)
  - Variables: `snake_case` or `camelCase` (be consistent)
  - Constants: `UPPER_SNAKE_CASE`
  - Private members: `m_` prefix (e.g., `m_connected`)
- **Indentation**: 4 spaces (no tabs)
- **Line Length**: Prefer <100 characters, max 120
- **Braces**: Opening brace on same line for functions/classes

### Example Code Style

```cpp
namespace ps {
    namespace odbc {
        class OdbcWrapper {
        private:
            SQLHENV m_hEnv;
            bool m_connected;

        public:
            OdbcWrapper();
            ~OdbcWrapper();

            bool connect(const std::wstring& dsn,
                        const std::wstring& user,
                        const std::wstring& password);

            void disconnect();
        };
    }
}
```

### Documentation

- **All public APIs** must have Doxygen comments
- Include `@brief`, `@param`, and `@return` as applicable
- Add usage examples for complex functions
- Keep comments up-to-date with code changes

### Example Documentation

```cpp
/**
 * @brief Connects to a database using the specified credentials.
 *
 * @param dsn The Data Source Name (DSN) for the database.
 * @param user The username for authentication.
 * @param password The password for authentication.
 * @return True if the connection is successful, false otherwise.
 *
 * @throws std::runtime_error if connection fails and error handling is enabled.
 *
 * @example
 * @code
 * OdbcWrapper db;
 * db.initialize();
 * if (db.connect(L"MyDSN", L"user", L"password")) {
 *     // Connection successful
 * }
 * @endcode
 */
bool connect(const std::wstring& dsn,
            const std::wstring& user,
            const std::wstring& password);
```

## Testing Guidelines

### Test Coverage Requirements

- **All new code** must have unit tests
- **Maintain 100% coverage** for new features
- **Mock external dependencies** (ODBC calls)
- **Test edge cases** and error conditions

### Writing Tests

Use Google Test and Google Mock:

```cpp
TEST_F(OdbcWrapperTest, Connect_SuccessfulConnection) {
    EXPECT_CALL(*mock, SQLConnect(testing::_, testing::_, SQL_NTS,
                                  testing::_, SQL_NTS,
                                  testing::_, SQL_NTS))
        .WillOnce(testing::Return(SQL_SUCCESS));

    EXPECT_CALL(*mock, SQLAllocHandle(SQL_HANDLE_STMT, testing::_, testing::_))
        .WillOnce(testing::Return(SQL_SUCCESS));

    bool result = wrapper->connect(L"MyDSN", L"user", L"pass");
    EXPECT_TRUE(result);
}
```

### Running Tests

```bash
# Run all tests
./scripts/run-tests-with-coverage.sh

# Run specific test suite
./build/bin/test_odbccpp

# Run specific test
./build/bin/test_odbccpp --gtest_filter=OdbcWrapperTest.Connect_*
```

## Commit Messages

### Format

```
<type>(<scope>): <subject>

<body>

<footer>
```

### Types

- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation only
- `style`: Code style changes (formatting, no code change)
- `refactor`: Code refactoring (no feature change or bug fix)
- `test`: Adding or updating tests
- `chore`: Build process or auxiliary tool changes
- `perf`: Performance improvements

### Examples

```
feat(wrapper): add connection pooling support

Implement a connection pool to improve performance for applications
that need multiple concurrent database connections.

Closes #123
```

```
fix(executor): correct infinite recursion in SQLAllocHandle

Fixed bug where OdbcExecutor methods were calling themselves instead
of the system ODBC functions. Added :: prefix to delegate to global
namespace functions.

Fixes #456
```

## Pull Request Process

### Before Submitting

1. **Update from upstream**:
   ```bash
   git fetch upstream
   git rebase upstream/main
   ```

2. **Run all checks**:
   ```bash
   # Build and test
   ./scripts/run-tests-with-coverage.sh

   # Static analysis
   ./scripts/run-static-analysis.sh
   ```

3. **Verify coverage**: Ensure code coverage remains at 100%

4. **Update documentation**:
   - Update README if needed
   - Add/update Doxygen comments
   - Update CHANGELOG.md

### Submitting the PR

1. **Push to your fork**:
   ```bash
   git push origin feature/your-feature-name
   ```

2. **Create Pull Request** on GitHub

3. **Fill out the PR template** with:
   - Description of changes
   - Related issues
   - Test plan
   - Screenshots (if UI changes)

4. **Respond to review comments** promptly

### PR Checklist

- [ ] Code follows the project's style guidelines
- [ ] Self-review of code completed
- [ ] Comments added for hard-to-understand areas
- [ ] Documentation updated
- [ ] Tests added/updated
- [ ] All tests pass
- [ ] Code coverage maintained at 100%
- [ ] Static analysis passes with no errors
- [ ] Commits follow commit message convention
- [ ] Branch is up-to-date with main

### Review Process

- At least one maintainer must approve the PR
- All CI checks must pass
- No unresolved conversations
- Code coverage must not decrease

### After Approval

Once approved and merged:
- Delete your feature branch
- Update your fork:
  ```bash
  git checkout main
  git pull upstream main
  git push origin main
  ```

## Static Analysis

### Running Locally

```bash
./scripts/run-static-analysis.sh
```

### Fixing Issues

- **Critical errors**: Must be fixed before PR
- **Warnings**: Should be fixed or justified
- **Style issues**: Follow project conventions

### Suppressing False Positives

Add suppressions to `.cppcheck-suppressions.txt` with justification:

```
# Suppress false positive for intentional design
unmatchedSuppression:src/odbcwrapper.cpp
```

## Getting Help

- **Questions**: Open a [Discussion](https://github.com/psanodiya94/cpp-odbc-wrapper/discussions)
- **Bugs**: Open an [Issue](https://github.com/psanodiya94/cpp-odbc-wrapper/issues)
- **Security**: Email maintainers privately (see SECURITY.md)

## License

By contributing, you agree that your contributions will be licensed under the same license as the project (MIT License).

---

Thank you for contributing to C++ ODBC Wrapper! 🎉
