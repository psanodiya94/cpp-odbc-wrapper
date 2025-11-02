# C++ ODBC Wrapper

[![Coverage](https://img.shields.io/badge/coverage-100%25-brightgreen.svg)](https://github.com/psanodiya94/cpp-odbc-wrapper)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)

A flexible, extensible, and type-safe C++ wrapper for ODBC (Open Database Connectivity) that simplifies database operations while maintaining high performance and testability.

## Table of Contents

- [Features](#features)
- [Architecture](#architecture)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Dependencies](#dependencies)
  - [Building](#building)
  - [Running Tests](#running-tests)
- [Usage](#usage)
  - [Basic Example](#basic-example)
  - [Advanced Usage](#advanced-usage)
- [Testing](#testing)
- [Code Coverage](#code-coverage)
- [Documentation](#documentation)
- [Static Analysis](#static-analysis)
- [Contributing](#contributing)
- [License](#license)

## Features

- **🔌 Flexible Architecture**: Abstract interface allows easy swapping of ODBC implementations
- **🧪 100% Test Coverage**: Comprehensive unit tests with Google Test and Google Mock
- **📝 Well-Documented**: Extensive inline documentation and generated Doxygen docs
- **🚀 Modern C++**: Uses C++17 features for type safety and performance
- **🔍 Static Analysis**: cppcheck integration for code quality
- **⚡ Lightweight**: Minimal overhead over raw ODBC calls
- **🛡️ Type-Safe**: Strong typing for better compile-time error detection
- **📊 Logging**: Built-in logging with spdlog for debugging and diagnostics
- **🛠️ Developer Tools**: Helper scripts for testing, coverage, and static analysis

## Architecture

The library uses a layered architecture with dependency injection for maximum flexibility:

```
┌─────────────────────────────────┐
│      Application Code           │
└────────────┬────────────────────┘
             │
             ▼
┌─────────────────────────────────┐
│       OdbcWrapper               │  ← High-level API
│  (Manages connections, queries) │
└────────────┬────────────────────┘
             │
             ▼
┌─────────────────────────────────┐
│      OdbcInterface              │  ← Abstract interface
│   (Pure virtual functions)      │
└────────────┬────────────────────┘
             │
             ▼
┌─────────────────────────────────┐
│      OdbcExecutor               │  ← Concrete implementation
│   (Wraps actual ODBC calls)     │
└────────────┬────────────────────┘
             │
             ▼
┌─────────────────────────────────┐
│    System ODBC Driver           │  ← OS ODBC library
└─────────────────────────────────┘
```

### Key Components

- **`OdbcInterface`**: Abstract base class defining the ODBC API contract
- **`OdbcExecutor`**: Default implementation that delegates to system ODBC functions
- **`OdbcWrapper`**: High-level wrapper providing convenient database operations
- **`OdbcLogger`**: Logging utility for debugging and diagnostics

This architecture allows you to:
- Swap ODBC implementations without changing application code
- Mock the interface for unit testing
- Add custom behavior (logging, metrics, retry logic) by implementing the interface
- Use different ODBC drivers by simply changing the concrete implementation

## Getting Started

### Prerequisites

Ensure you have the following installed on your system:

- **C++ Compiler**: GCC 7+, Clang 5+, or MSVC 2017+ with C++17 support
- **CMake**: Version 3.10 or higher
- **Git**: For cloning the repository
- **ODBC Driver Manager**:
  - Linux: `unixODBC` or `iODBC`
  - Windows: Built-in ODBC Driver Manager
  - macOS: `unixODBC` via Homebrew

#### Installing Prerequisites

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install build-essential cmake git unixodbc unixodbc-dev
sudo apt-get install libspdlog-dev libfmt-dev libgtest-dev lcov
```

**Fedora/RHEL:**
```bash
sudo dnf install gcc-c++ cmake git unixODBC unixODBC-devel
sudo dnf install spdlog-devel fmt-devel gtest-devel lcov
```

**macOS:**
```bash
brew install cmake unixodbc spdlog fmt googletest lcov
```

**Windows:**
- Install Visual Studio 2017 or later with C++ development tools
- Install CMake from [cmake.org](https://cmake.org/)
- Install vcpkg and use it to install dependencies:
  ```powershell
  vcpkg install spdlog fmt gtest
  ```

### Dependencies

The project relies on the following libraries:

| Library | Purpose | License |
|---------|---------|---------|
| [ODBC](https://docs.microsoft.com/en-us/sql/odbc/) | Database connectivity | Various |
| [fmt](https://github.com/fmtlib/fmt) | Modern formatting | MIT |
| [spdlog](https://github.com/gabime/spdlog) | Fast logging | MIT |
| [Google Test](https://github.com/google/googletest) | Unit testing | BSD-3-Clause |
| [Google Mock](https://github.com/google/googletest) | Mocking framework | BSD-3-Clause |

### Building

1. **Clone the Repository**
   ```bash
   git clone https://github.com/psanodiya94/cpp-odbc-wrapper.git
   cd cpp-odbc-wrapper
   ```

2. **Create Build Directory**
   ```bash
   mkdir build && cd build
   ```

3. **Configure and Build**
   ```bash
   cmake ..
   cmake --build . --config Release
   ```

4. **Install (Optional)**
   ```bash
   sudo cmake --install .
   ```

### Running Tests

Run all tests:
```bash
cd build
ctest --output-on-failure
```

Run individual test suites:
```bash
./bin/test_odbccpp           # Main wrapper tests
./bin/test_odbcexecutor      # Executor tests
./bin/test_additional_coverage  # Additional coverage tests
```

## Usage

### Basic Example

```cpp
#include <odbccpp/odbcwrapper.h>
#include <odbclogger.h>
#include <iostream>

int main() {
    // Initialize logging
    ps::odbc::OdbcLogger::initialize("logs/app.log");

    try {
        // Create wrapper instance
        ps::odbc::OdbcWrapper db;

        // Initialize ODBC environment
        db.initialize();

        // Connect to database
        if (db.connect(L"MyDSN", L"username", L"password")) {
            std::wcout << L"Connected successfully!" << std::endl;

            // Execute a query
            if (db.executeQuery(L"SELECT id, name FROM users")) {
                // Fetch results
                auto results = db.fetchResults();

                // Process results
                for (const auto& row : results) {
                    for (const auto& col : row) {
                        std::wcout << col << L" | ";
                    }
                    std::wcout << std::endl;
                }
            }

            // Execute an update
            db.executeUpdate(L"INSERT INTO users (name) VALUES ('John Doe')");

            // Disconnect
            db.disconnect();
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
```

### Advanced Usage

#### Custom ODBC Implementation

You can provide your own ODBC implementation for testing or to add custom behavior:

```cpp
#include <odbccpp/odbcwrapper.h>
#include <odbccpp/odbcinterface.h>

class CustomOdbcImpl : public ps::odbc::OdbcInterface {
public:
    // Implement all virtual methods
    SQLRETURN SQLConnect(...) override {
        // Your custom implementation
        // e.g., add retry logic, logging, metrics
        return ::SQLConnect(...);
    }

    // ... implement other methods
};

int main() {
    // Use custom implementation
    auto customImpl = std::make_unique<CustomOdbcImpl>();
    ps::odbc::OdbcWrapper db(std::move(customImpl));

    db.initialize();
    // ... use as normal
}
```

#### Parameterized Queries

```cpp
// Using prepared statements for better performance and security
db.connect(L"MyDSN", L"user", L"pass");

// Execute parameterized query
std::wstring query = L"SELECT * FROM users WHERE age > ? AND city = ?";
if (db.executeQuery(query)) {
    auto results = db.fetchResults();
    // Process results
}
```

## Testing

The project includes comprehensive unit tests achieving 100% code coverage:

- **`test_odbcwrapper.cpp`**: Tests for the main OdbcWrapper class
- **`test_odbcexecutor.cpp`**: Tests for the OdbcExecutor implementation
- **`test_additional_coverage.cpp`**: Additional edge cases and error scenarios

All tests use Google Mock to mock the ODBC interface, allowing testing without a real database connection.

### Running Specific Tests

```bash
# Run with verbose output
./bin/test_odbccpp --gtest_verbose

# Run specific test
./bin/test_odbccpp --gtest_filter=OdbcWrapperTest.Connect_SuccessfulConnection

# List all tests
./bin/test_odbccpp --gtest_list_tests
```

## Code Coverage

Generate code coverage reports:

```bash
cd build
make coverage
```

This will:
1. Run all tests
2. Collect coverage data
3. Generate HTML report in `build/coverage/html/`
4. Display coverage summary

View the report:
```bash
# Linux/macOS
xdg-open coverage/html/index.html

# macOS
open coverage/html/index.html

# Windows
start coverage/html/index.html
```

The project maintains 100% code coverage for all source files in `src/` and `include/`.

## Documentation

### Generating Documentation

Generate Doxygen documentation:

```bash
cd build
make doc
```

The generated documentation will be available in `docs/doxygen/html/index.html`.

### View Documentation

```bash
# Linux
xdg-open ../docs/doxygen/html/index.html

# macOS
open ../docs/doxygen/html/index.html

# Windows
start ..\docs\doxygen\html\index.html
```

### Documentation Structure

- **API Reference**: Complete class and method documentation
- **Class Diagrams**: Visual representation of class relationships
- **Call Graphs**: Function call relationships
- **Source Browser**: Browse annotated source code

## Static Analysis

The project integrates multiple static analysis tools:

### cppcheck

Run static analysis:

```bash
cppcheck --enable=all --suppress=missingIncludeSystem \
         --inline-suppr --error-exitcode=1 \
         -I include/ src/
```

### clang-tidy (Optional)

```bash
clang-tidy src/*.cpp -- -I include/ -std=c++17
```

## Contributing

Contributions are welcome! Please follow these guidelines:

1. **Fork the repository**
2. **Create a feature branch**: `git checkout -b feature/amazing-feature`
3. **Write tests**: Ensure 100% coverage for new code
4. **Run static analysis**: Fix all warnings
5. **Commit changes**: `git commit -m 'Add amazing feature'`
6. **Push to branch**: `git push origin feature/amazing-feature`
7. **Open a Pull Request**

### Code Style

- Follow the existing code style
- Use meaningful variable and function names
- Add Doxygen comments for all public APIs
- Keep functions focused and small

### Testing Requirements

- Write unit tests for all new functionality
- Maintain 100% code coverage
- Ensure all tests pass before submitting PR
- Add integration tests for complex features

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Thanks to all contributors who have helped shape this project
- Built with modern C++ best practices
- Inspired by the need for a simple, testable ODBC wrapper

## Support

- **Issues**: [GitHub Issues](https://github.com/psanodiya94/cpp-odbc-wrapper/issues)
- **Discussions**: [GitHub Discussions](https://github.com/psanodiya94/cpp-odbc-wrapper/discussions)
- **Documentation**: [Online Docs](https://psanodiya94.github.io/cpp-odbc-wrapper/)

## Roadmap

- [ ] Add connection pooling support
- [ ] Implement async query execution
- [ ] Add transaction support
- [ ] Support for bulk operations
- [ ] Add more database-specific optimizations
- [ ] Python bindings
- [ ] Performance benchmarks

---

**Made with ❤️ for the C++ community**
