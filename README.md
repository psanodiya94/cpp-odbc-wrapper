# cpp-odbc-wrapper

## Getting Started

Follow these instructions to get started with `cpp-odbc-wrapper`:

### Prerequisites

Ensure you have the following installed on your system:

- A C++ compiler that supports C++17 or later
- CMake (version 3.10 or higher)
- Git
- ODBC driver and driver manager

### Dependencies

The project relies on the following libraries:

- **ODBC**: For database connectivity.
- **fmt**: A modern formatting library for C++.
- **spdlog**: A fast logging library for C++.
- **googletest**: Used for unit testing.

Ensure these dependencies are available on your system or included in your build process.

### Installation

To set up the project, follow these steps:

1. **Clone the Repository**  
    Clone the repository to your local machine:

    ```sh
    git clone https://github.com/psanodiya94/cpp-odbc-wrapper.git
    cd cpp-odbc-wrapper
    ```

2. **Build the Project**  
    Create a build directory, generate build files, and compile the project:

    ```sh
    mkdir build
    cd build
    cmake ..
    cmake --build .
    ```

3. **Run Tests**  
    Execute the tests to verify the build:

    ```sh
    ctest
    ```

4. **Install the Library** (Optional)  
    To install the library system-wide:

    ```sh
    sudo cmake --install .
    ```

### Documentation

For detailed documentation, refer to the [Wiki](https://github.com/psanodiya94/cpp-odbc-wrapper/wiki) or the `docs` folder in the repository.
