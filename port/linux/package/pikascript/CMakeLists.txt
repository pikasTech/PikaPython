#设置 BINARY 为项目名IndexProject
set(BINARY ${CMAKE_PROJECT_NAME})
set(CMAKE_C_FLAGS "-Wno-error=deprecated-declarations -Wno-deprecated-declarations")
file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.h *.c)
set(SOURCES ${SOURCES})

# support <math.h>
link_libraries(m lua5.3 pthread mbedcrypto)

add_library(${BINARY}-core
            STATIC 
            ${SOURCES}
            )
