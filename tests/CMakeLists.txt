
cmake_minimum_required(VERSION 3.10)

include(GoogleTest)

file(GLOB_RECURSE TCP_TEST_SOURCES
        "${PROJECT_SOURCE_DIR}/tests/client_test.cpp"
        "${PROJECT_SOURCE_DIR}/tests/server_test.cpp")
#
foreach(test_source ${TCP_TEST_SOURCES})
    get_filename_component(test_filename ${test_source} NAME)
    string(REPLACE ".cpp" "" test_name ${test_filename})
    add_executable(${test_name} EXCLUDE_FROM_ALL ${test_source})
    target_link_libraries(${test_name} PUBLIC tcp gtest gmock_main)
endforeach()
