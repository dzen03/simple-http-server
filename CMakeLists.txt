cmake_minimum_required(VERSION 3.25)
project(simple-http-server)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


if(CMAKE_CXX_COMPILER_ID STREQUAL GNU)
    set(CMAKE_CONFIGURATION_TYPES Debug Release ASan LSan UBSan)
elseif(CMAKE_CXX_COMPILER_ID MATCHES Clang)
    set(CMAKE_CONFIGURATION_TYPES Debug Release ASan LSan MSan UBSan Coverage)
elseif(MSVC)
    set(CMAKE_CONFIGURATION_TYPES Debug Release ASan)
endif()

if(CMAKE_BUILD_TYPE AND NOT CMAKE_BUILD_TYPE IN_LIST CMAKE_CONFIGURATION_TYPES)
    message(FATAL_ERROR "Unexpected build type ${CMAKE_BUILD_TYPE}, possible values: ${CMAKE_CONFIGURATION_TYPES}")
endif()

if(CMAKE_C_COMPILER_ID STREQUAL GNU OR CMAKE_C_COMPILER_ID MATCHES Clang)
    set(CMAKE_C_FLAGS       "-std=20 -pedantic -Wall -Werror -ggdb")
    set(CMAKE_C_FLAGS_ASAN  "${CMAKE_C_FLAGS_DEBUG} -fsanitize=address -fno-optimize-sibling-calls -fno-omit-frame-pointer")
    set(CMAKE_C_FLAGS_LSAN  "${CMAKE_C_FLAGS_DEBUG} -fsanitize=leak")
    set(CMAKE_C_FLAGS_MSAN  "${CMAKE_C_FLAGS_DEBUG} -fsanitize=memory -fno-optimize-sibling-calls -fno-omit-frame-pointer")
    set(CMAKE_C_FLAGS_UBSAN "${CMAKE_C_FLAGS_DEBUG} -fsanitize=undefined")
    set(CMAKE_C_FLAGS_COVERAGE "${CMAKE_C_FLAGS_DEBUG} --coverage")
elseif(MSVC)
    set(CMAKE_C_FLAGS       "/std:c20 /W4 /WX")
    set(CMAKE_C_FLAGS_ASAN  "${CMAKE_C_FLAGS_DEBUG} /fsanitize=address")
    set(CMAKE_EXE_LINKER_FLAGS_ASAN "/debug /INCREMENTAL:NO")
endif()


if(CMAKE_CXX_COMPILER_ID STREQUAL GNU OR CMAKE_CXX_COMPILER_ID MATCHES Clang)
    set(CMAKE_CXX_FLAGS       "-std=c++20 -pedantic -Wall -Werror -ggdb")
    set(CMAKE_CXX_FLAGS_ASAN  "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fno-optimize-sibling-calls -fno-omit-frame-pointer")
    set(CMAKE_CXX_FLAGS_LSAN  "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=leak")
    set(CMAKE_CXX_FLAGS_MSAN  "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=memory -fno-optimize-sibling-calls -fno-omit-frame-pointer")
    set(CMAKE_CXX_FLAGS_UBSAN "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=undefined")
    set(CMAKE_CXX_FLAGS_COVERAGE "${CMAKE_CXX_FLAGS_DEBUG} --coverage")
elseif(MSVC)
    set(CMAKE_CXX_FLAGS       "/std:c++20 /W4 /WX")
    set(CMAKE_CXX_FLAGS_ASAN  "${CMAKE_CXX_FLAGS_DEBUG} /fsanitize=address")
    set(CMAKE_EXE_LINKER_FLAGS_ASAN "/debug /INCREMENTAL:NO")
endif()

option(USE_CLANG_TIDY "Enable clang tidy" ON)
if(USE_CLANG_TIDY)
    find_program(CLANG_TIDY clang-tidy)
endif()

if(CLANG_TIDY)
    message(STATUS "Clang-tidy: ${CLANG_TIDY}")

    set_property(
            DIRECTORY
            APPEND
            PROPERTY CMAKE_CONFIGURE_DEPENDS clang-tidy-checks.txt
    )

    file(STRINGS clang-tidy-checks.txt clang_tidy_checks)
    list(JOIN clang_tidy_checks "," clang_tidy_checks_str)
    set(CMAKE_C_CLANG_TIDY
            ${CLANG_TIDY}
            -header-filter=${CMAKE_SOURCE_DIR}
            -checks=${clang_tidy_checks_str}
            -warnings-as-errors=*
    )
    set(CMAKE_CXX_CLANG_TIDY
            ${CLANG_TIDY}
            -header-filter=${CMAKE_SOURCE_DIR}
            -checks=${clang_tidy_checks_str}
            -warnings-as-errors=*
    )
endif()

if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

add_subdirectory(libs)
include_directories(libs)

# Function to disable clang-tidy on specified target
function(disable_clang_tidy target)
    if (TARGET ${target})
        set_target_properties(${target} PROPERTIES CXX_CLANG_TIDY "")
    else()
        message(WARNING "Target ${target} not found!")
    endif()
endfunction()

option(BUILD_TESTING "Enable tests" ON)
if(BUILD_TESTING)
    enable_testing()

    include(FetchContent)
    FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
            DOWNLOAD_EXTRACT_TIMESTAMP NEW
    )

    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    # Disable clang-tidy for GoogleTest targets
    disable_clang_tidy(gtest)
    disable_clang_tidy(gtest_main)
    disable_clang_tidy(gmock)
    disable_clang_tidy(gmock_main)

    add_subdirectory(tests)
endif()

if(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "-static")
    link_libraries(Ws2_32 Mswsock AdvApi32)
endif()

add_executable(http-server main.cpp)
target_link_libraries(http-server PRIVATE simple-http-server-sources)

configure_file(index.html ${CMAKE_BINARY_DIR} COPYONLY) # only for example
