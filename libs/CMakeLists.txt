file(GLOB_RECURSE sources CONFIGURE_DEPENDS
        *.cpp
        *.h
)

add_library(simple-http-server-sources SHARED ${sources})
target_include_directories(simple-http-server-sources PUBLIC .)
