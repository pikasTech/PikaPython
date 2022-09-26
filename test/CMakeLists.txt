set(BINARY ${CMAKE_PROJECT_NAME}_test)
file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false *.h *.cpp *.c)
set(SOURCES ${TEST_SOURCES})
add_executable(${BINARY} ${TEST_SOURCES})
add_test(NAME ${BINARY} COMMAND ${BINARY})


# 链接src生成的lib库和gtest库
target_link_libraries(${BINARY} PUBLIC 
        -Wl,--whole-archive ${CMAKE_PROJECT_NAME}-core -Wl,--no-whole-archive
        gtest)
