#设置 BINARY 为项目名IndexProject
set(BINARY ${CMAKE_PROJECT_NAME})
 
file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.h *.c)
set(SOURCES ${SOURCES})

# support <math.h>
link_libraries(m)

add_library(${BINARY}-core
            STATIC 
            ${SOURCES}
            )
