cmake_minimum_required(VERSION 3.0.0)
project(pikaByteCodeGen VERSION 0.1.0)

include(CTest)
enable_testing()

aux_source_directory(pikascript/pikascript-core pikascript-core)
include_directories(pikascript/pikascript-core)
include_directories(libpikabinder)
link_directories(libpikabinder)

add_executable(pikaByteCodeGen main.c
               ${pikascript-core}
)

target_link_libraries(pikaByteCodeGen
        bcrypt
        user32
        userenv
        ws2_32
        wsock32
        pikabinder
        )

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
