include(GoogleTest)

if (WIN32)
    link_libraries(Ws2_32 Mswsock AdvApi32)
endif ()

FILE(GLOB_RECURSE test_sources CONFIGURE_DEPENDS
        *.cpp
)

foreach(test_source IN LISTS test_sources)
    string(REPLACE "/" ";" name_components ${test_source})
    list(GET name_components -1 name)
    string(REGEX REPLACE "\\.cpp$" "" name ${name})
    list(APPEND test_targets test_${name})

    add_executable(test_${name} ${test_source})
    target_link_libraries(test_${name} PRIVATE simple-http-server-sources GTest::gtest_main)

    # gtest_discover_tests(test_${name})
    add_test(NAME ${name} COMMAND test_${name})
endforeach()

# Create a single test executable for all tests
add_executable(all_tests ${test_sources})
target_link_libraries(all_tests PRIVATE simple-http-server-sources GTest::gtest_main)

# Discover all tests in the single executable
# gtest_discover_tests(all_tests DISCOVERY_MODE PRE_TEST)
# add_test(NAME "tests" COMMAND all_tests)
