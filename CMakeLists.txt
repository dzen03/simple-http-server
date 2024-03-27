cmake_minimum_required(VERSION 3.22)
project(simple-http-server)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

add_subdirectory(libs)
include_directories(libs)

option(BUILD_TESTING "Enable tests" ON)
if(BUILD_TESTING)
    enable_testing()

    include(FetchContent)
    FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)


    add_subdirectory(tests)
endif()

add_executable(http-server main.cpp)
target_link_libraries(http-server PRIVATE simple-http-server-sources)
