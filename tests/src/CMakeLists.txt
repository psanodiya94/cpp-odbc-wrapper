# Find GTest package
find_package(GTest REQUIRED)

# GMock may not have a separate find_package in older CMake versions
# Use GTest to locate GMock if necessary
find_path(GMOCK_INCLUDE_DIRS gmock/gmock.h PATHS ${GTEST_INCLUDE_DIRS})
find_library(GMOCK_LIBRARY NAMES gmock libgmock)
find_library(GMOCK_MAIN_LIBRARY NAMES gmock_main libgmock_main)

# Find SQLite3 (optional)
find_package(SQLite3)

# Find spdlog and fmt for logging
find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)

# Define the test executables
add_executable(test_odbccpp test_odbcwrapper.cpp)
add_executable(test_odbcexecutor test_odbcexecutor.cpp)
add_executable(test_additional_coverage test_additional_coverage.cpp)

# Configure all test targets
set(TEST_TARGETS test_odbccpp test_odbcexecutor test_additional_coverage)
foreach(TEST_TARGET ${TEST_TARGETS})
    # Include directories
    target_include_directories(${TEST_TARGET} PRIVATE
        ${ODBC_INCLUDE_DIRS}
        ${GTEST_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/tests/include
    )

    # Add GMock include dirs if found
    if(GMOCK_INCLUDE_DIRS)
        target_include_directories(${TEST_TARGET} PRIVATE ${GMOCK_INCLUDE_DIRS})
    endif()

    # Add SQLite3 include dirs if found
    if(SQLite3_FOUND)
        target_include_directories(${TEST_TARGET} PRIVATE ${SQLite3_INCLUDE_DIRS})
    endif()

    # Enable compiler flags for coverage
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${TEST_TARGET} PRIVATE --coverage -fprofile-arcs -ftest-coverage)
        target_link_options(${TEST_TARGET} PRIVATE --coverage)
    endif()

    # Link the test executable to the library
    target_link_libraries(${TEST_TARGET} PRIVATE
        odbccpp
        ${ODBC_LIBRARIES}
        GTest::GTest
        GTest::Main
        spdlog::spdlog
        fmt::fmt
    )

    # Link GMock if found
    if(GMOCK_LIBRARY)
        target_link_libraries(${TEST_TARGET} PRIVATE ${GMOCK_LIBRARY})
    endif()
    if(GMOCK_MAIN_LIBRARY)
        target_link_libraries(${TEST_TARGET} PRIVATE ${GMOCK_MAIN_LIBRARY})
    endif()

    # Link SQLite3 if found
    if(SQLite3_FOUND)
        target_link_libraries(${TEST_TARGET} PRIVATE SQLite3::SQLite3)
    endif()
endforeach()

# Add tests
enable_testing()
add_test(NAME OdbcWrapperTestSuite COMMAND test_odbccpp WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
add_test(NAME OdbcExecutorTestSuite COMMAND test_odbcexecutor WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
add_test(NAME OdbcAdditionalCoverageTestSuite COMMAND test_additional_coverage WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# Coverage target
find_program(LCOV lcov)
find_program(GENHTML genhtml)

if(LCOV AND GENHTML)
    add_custom_target(coverage
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/coverage
        COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_odbccpp || true
        COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_odbcexecutor || true
        COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_additional_coverage || true
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
        COMMAND ${LCOV} --summary ${CMAKE_BINARY_DIR}/coverage/coverage_final.info
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating coverage report for src/ and include/..."
    )
else()
    message(WARNING "lcov or genhtml not found, coverage target will not be available")
endif()
