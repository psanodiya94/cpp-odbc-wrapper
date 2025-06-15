# Find ODBC package
find_package(ODBC REQUIRED)

# Find SQLite3 (optional, for creating test database)
find_package(SQLite3 REQUIRED)

# Find SPDLOGspdlog and fmt for logging
find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)

# Define the library
add_library(odbccpp STATIC
    odbcexecutor.cpp
    odbcwrapper.cpp
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(odbccpp PRIVATE --coverage -fprofile-arcs -ftest-coverage)
    target_link_options(odbccpp PRIVATE --coverage)
endif()

# Specify include directories for the library
target_include_directories(odbccpp PUBLIC ${CMAKE_SOURCE_DIR}/include/)
target_link_libraries(odbccpp PRIVATE ${ODBC_LIBRARIES} ${SQLite3_LIBRARIES})
target_link_libraries(odbccpp PRIVATE spdlog::spdlog fmt::fmt)
