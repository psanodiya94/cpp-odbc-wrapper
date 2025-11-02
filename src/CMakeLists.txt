# Find ODBC package
find_package(ODBC REQUIRED)

# Find SQLite3 (optional, for creating test database)
find_package(SQLite3)
# Create SQLite3::SQLite3 target if it doesn't exist but variables are available
if(SQLite3_FOUND AND NOT TARGET SQLite3::SQLite3)
    add_library(SQLite3::SQLite3 UNKNOWN IMPORTED)
    set_target_properties(SQLite3::SQLite3 PROPERTIES
        IMPORTED_LOCATION "${SQLite3_LIBRARIES}"
        INTERFACE_INCLUDE_DIRECTORIES "${SQLite3_INCLUDE_DIRS}")
endif()

# Find spdlog and fmt for logging
find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)

# Define the library
add_library(odbccpp STATIC
    odbcexecutor.cpp
    odbcwrapper.cpp
)

# Add coverage flags for GCC/Clang if enabled
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(odbccpp PRIVATE --coverage -fprofile-arcs -ftest-coverage)
    target_link_options(odbccpp PRIVATE --coverage)
endif()

# Specify include directories for the library
target_include_directories(odbccpp PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/odbccpp>
)

# Link required libraries
target_link_libraries(odbccpp PRIVATE ${ODBC_LIBRARIES})
target_link_libraries(odbccpp PRIVATE spdlog::spdlog fmt::fmt)

# Link SQLite3 if found
if(SQLite3_FOUND)
    target_link_libraries(odbccpp PRIVATE SQLite3::SQLite3)
endif()
