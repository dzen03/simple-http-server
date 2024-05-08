file(GLOB_RECURSE sources CONFIGURE_DEPENDS
        *.cpp
        *.h
)
if (WIN32)
    link_libraries(Ws2_32 Mswsock AdvApi32)
    add_library(simple-http-server-sources STATIC ${sources})
else ()
    add_library(simple-http-server-sources SHARED ${sources})
endif ()
target_include_directories(simple-http-server-sources PRIVATE .)
