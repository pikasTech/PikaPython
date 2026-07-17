#设置 BINARY 为项目名IndexProject
set(BINARY ${CMAKE_PROJECT_NAME})
set(CMAKE_C_FLAGS "-Wno-error=deprecated-declarations -Wno-deprecated-declarations")
file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.h *.c)
IF(NOT PIKA_ENABLE_LVGL)
    file(GLOB_RECURSE LVGL_SOURCES LIST_DIRECTORIES true
         pikascript-lib/pika_lvgl/*.h
         pikascript-lib/pika_lvgl/*.c)
    list(REMOVE_ITEM SOURCES ${LVGL_SOURCES})
ENDIF()
set(SOURCES ${SOURCES})

# support <math.h>
link_libraries(m lua5.3 pthread mbedcrypto ${BINARY}-lvgl)

add_library(${BINARY}-core
            STATIC 
            ${SOURCES}
            )
