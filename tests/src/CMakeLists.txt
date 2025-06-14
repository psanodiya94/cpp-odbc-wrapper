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

# Enable compiler flags to detect stack smashing and undefined behavior
# target_compile_options(test_odbccpp PRIVATE -fstack-protector-all -fsanitize=address,undefined)
# target_link_options(test_odbccpp PRIVATE -fsanitize=address,undefined)

# Include directories
target_include_directories(test_odbccpp PRIVATE ${ODBC_INCLUDE_DIRS} ${SQLite3_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS})
target_include_directories(test_odbccpp PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/tests/include)

# Link the test executable to the library
target_link_libraries(test_odbccpp PRIVATE odbccpp ${ODBC_LIBRARIES} ${SQLite3_LIBRARIES} ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} ${GMOCK_LIBRARY} ${GMOCK_MAIN_LIBRARY} spdlog::spdlog fmt::fmt)

# Add test
enable_testing()
add_test(NAME OdbcWrapperTestSuite COMMAND test_odbccpp WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
