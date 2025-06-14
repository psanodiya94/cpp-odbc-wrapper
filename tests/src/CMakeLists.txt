# Find GTest package
find_package(GTest REQUIRED)

# GMock may not have a separate find_package in older CMake versions
# Use GTest to locate GMock if necessary
find_path(GMOCK_INCLUDE_DIRS gmock/gmock.h)
find_library(GMOCK_LIBRARY NAMES gmock libgmock)
find_library(GMOCK_MAIN_LIBRARY NAMES gmock_main libgmock_main)

# Find SPDLOGspdlog and fmt for logging
find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)

# Define the test executable
add_executable(test_odbccpp test_odbcwrapper.cpp)

# Include directories
target_include_directories(test_odbccpp PRIVATE 
    ${ODBC_INCLUDE_DIRS} 
    ${SQLite3_INCLUDE_DIRS} 
    ${GTEST_INCLUDE_DIRS} 
    ${GMOCK_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/include 
    ${CMAKE_SOURCE_DIR}/tests/include
)

# Enable compiler flags for coverage
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(test_odbccpp PRIVATE --coverage -fprofile-arcs -ftest-coverage)
    target_link_options(test_odbccpp PRIVATE --coverage)
endif()

# Link the test executable to the library
target_link_libraries(test_odbccpp PRIVATE 
    odbccpp 
    ${ODBC_LIBRARIES} 
    ${SQLite3_LIBRARIES} 
    ${GTEST_LIBRARIES} 
    ${GTEST_MAIN_LIBRARIES} 
    ${GMOCK_LIBRARY} 
    ${GMOCK_MAIN_LIBRARY} 
    spdlog::spdlog 
    fmt::fmt
)

# Add test
enable_testing()
add_test(NAME OdbcWrapperTestSuite COMMAND test_odbccpp WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# Coverage target
find_program(LCOV lcov)
find_program(GENHTML genhtml)

if(LCOV AND GENHTML)
    add_custom_target(coverage
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/coverage
        COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_odbccpp
        COMMAND ${LCOV} --capture --directory ${CMAKE_BINARY_DIR} 
            --output-file ${CMAKE_BINARY_DIR}/coverage/coverage.info 
            --ignore-errors mismatch
        COMMAND ${LCOV} --remove ${CMAKE_BINARY_DIR}/coverage/coverage.info 
            '/usr/*' 
            '*/tests/*' 
            --output-file ${CMAKE_BINARY_DIR}/coverage/coverage_filtered.info
        COMMAND ${LCOV} --extract ${CMAKE_BINARY_DIR}/coverage/coverage_filtered.info 
            '${CMAKE_SOURCE_DIR}/src/*' 
            '${CMAKE_SOURCE_DIR}/include/*' 
            --output-file ${CMAKE_BINARY_DIR}/coverage/coverage_final.info
        COMMAND ${GENHTML} ${CMAKE_BINARY_DIR}/coverage/coverage_final.info 
            --output-directory ${CMAKE_BINARY_DIR}/coverage/html
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating coverage report for src/ and include/..."
    )
else()
    message(WARNING "lcov or genhtml not found, coverage target will not be available")
endif()
