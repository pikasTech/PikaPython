set(BINARY ${CMAKE_PROJECT_NAME}_benchmark)
file(GLOB_RECURSE BENCHMARK_SOURCES LIST_DIRECTORIES false *.h *.cpp *.c)
set(SOURCES ${BENCHMARK_SOURCES})
add_executable(${BINARY} ${BENCHMARK_SOURCES})
add_test(NAME ${BINARY} COMMAND ${BINARY})

find_package(benchmark REQUIRED)

# 链接src生成的lib库和gtest库
target_link_libraries(${BINARY} PUBLIC 
        -Wl,--whole-archive ${CMAKE_PROJECT_NAME}-core -Wl,--no-whole-archive
        gtest 
        benchmark::benchmark)
