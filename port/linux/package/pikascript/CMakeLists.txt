#设置 BINARY 为项目名IndexProject
set(BINARY ${CMAKE_PROJECT_NAME})
 
file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.h *.c)
set(SOURCES ${SOURCES})

# support <math.h>
link_libraries(m lua5.3)

add_library(${BINARY}-core
            STATIC 
            ${SOURCES}
            )
