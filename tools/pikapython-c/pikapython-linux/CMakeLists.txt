cmake_minimum_required(VERSION 3.10)

project(pikapython)

# 添加components文件夹中所有的.c文件及其子文件夹路径
file(GLOB_RECURSE COMPONENTS_FILES pikapython/*.c)

# add_compile_definitions(PIKA_CONFIG_ENABLE=1)

# 添加port/cmake/main.c文件
set(PORT_FILE main.c)

# 将所有的源文件组合在一起
set(SOURCE_FILES ${SRC_FILES} ${COMPONENTS_FILES} ${PORT_FILE})

# 添加头文件搜索路径
include_directories(${CMAKE_SOURCE_DIR}/pikapython/pikascript-core)
include_directories(${CMAKE_SOURCE_DIR}/pikapython/pikascript-api)

# 设置输出目标
add_executable(pikapython ${SOURCE_FILES})

# 添加pthread库链接
find_package(Threads REQUIRED)
target_link_libraries(pikapython Threads::Threads)
