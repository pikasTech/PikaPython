#设置 BINARY 为项目名IndexProject
set(BINARY ${CMAKE_PROJECT_NAME})
set(CMAKE_C_FLAGS "-Wno-unused-variable")
file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.h *.c)
set(SOURCES ${SOURCES})

link_libraries(m)

add_library(${BINARY}-lvgl
            STATIC 
            ${SOURCES}
            )
