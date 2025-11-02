cmake_minimum_required(VERSION 3.10)
project(ODBCCPPProject
    VERSION 1.0.0
    DESCRIPTION "A flexible and extensible C++ wrapper for ODBC database connectivity"
    LANGUAGES CXX
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set project version
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

# Set output directories for executables and libraries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Ensure output directories are used for all configurations (e.g., Debug/Release on Windows)
foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG_UPPER)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${CMAKE_BINARY_DIR}/lib)
endforeach()

# Add subdirectories
add_subdirectory(src)
add_subdirectory(tests/src)

# Add Test
enable_testing()

# Install the library
install(TARGETS odbccpp
    EXPORT odbccppTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

# Install the headers
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/
    DESTINATION include/odbccpp
    FILES_MATCHING PATTERN "*.h"
)

# Export the targets for use by other projects
install(EXPORT odbccppTargets
    FILE odbccppConfig.cmake
    NAMESPACE odbccpp::
    DESTINATION lib/cmake/odbccpp
)

# Ensure the include directory is added correctly
target_include_directories(odbccpp PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/odbccpp>
)

# Add Doxygen support
find_package(Doxygen OPTIONAL_COMPONENTS dot)

if(DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_SOURCE_DIR}/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_BINARY_DIR}/Doxyfile)

    # Configure the Doxyfile
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

    # Create docs directory if it doesn't exist
    file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/docs/doxygen)

    # Add custom target for documentation generation
    add_custom_target(doc
        COMMAND ${CMAKE_COMMAND} -E echo "Generating API documentation with Doxygen..."
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        COMMAND ${CMAKE_COMMAND} -E echo "Documentation generated in docs/doxygen/html/"
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
    )

    # Install documentation (optional)
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/docs/doxygen/html
        DESTINATION share/doc/odbccpp
        OPTIONAL
    )
else()
    message(WARNING "Doxygen not found. Documentation generation will not be available.")
endif()
