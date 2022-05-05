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

OPTION(CROSS_COMPILE "cross-compile" OFF)
IF(CROSS_COMPILE)
        target_link_libraries(pikaByteCodeGen  
                pikabinder
                ws2_32
                bcrypt
                user32
                userenv
                )
ELSE(CROSS_COMPILE)
        target_link_libraries(pikaByteCodeGen  
                pikabinder
                )
ENDIF(CROSS_COMPILE)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
