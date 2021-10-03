set(BINARY ${CMAKE_PROJECT_NAME}_demo03-usart)
file(GLOB_RECURSE THIS_SOURCES LIST_DIRECTORIES false *.h *.cpp *.c)
set(SOURCES ${THIS_SOURCES})
add_executable(${BINARY} ${THIS_SOURCES})
# 链接src生成的lib库和gtest库
target_link_libraries(${BINARY} PUBLIC 
        ${CMAKE_PROJECT_NAME}-core
        )
