cmake_minimum_required(VERSION 3.0.0)
add_compile_options(-Wall -Wextra -Wno-unused-parameter -Wno-sign-conversion -Wno-write-strings -Wno-implicit-fallthrough -Wno-sign-compare -Wno-cast-function-type -Wno-unused-function -Wno-missing-braces -Wno-missing-field-initializers)
OPTION(PIKA_CONFIG_ENABLE "pika config enable" OFF)
IF(PIKA_CONFIG_ENABLE)
    ADD_DEFINITIONS(-DPIKA_CONFIG_ENABLE)
ENDIF(PIKA_CONFIG_ENABLE)
ADD_DEFINITIONS(-DUSE_GOOGLE_TEST=1)
ADD_DEFINITIONS(-DLV_CONF_INCLUDE_SIMPLE=1)
ADD_DEFINITIONS(-DLV_LVGL_H_INCLUDE_SIMPLE=1)
ADD_DEFINITIONS(-DPIKASCRIPT=1)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

# Enable Clang-Tidy
# set(CMAKE_C_CLANG_TIDY
#    clang-tidy;
#    -checks=-clang-analyzer-security.insecureAPI*,-clang-analyzer-deadcode*;
# )

# support lto
# IF(POLICY CMP0069)
    # CMAKE_POLICY(SET CMP0069 NEW)
# ENDIF()

project(pikascript VERSION 0.1.0)

SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage")
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage -lgcov")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fstack-usage")


include_directories(package/pikascript/pikascript-core)
include_directories(package/pikascript/pikascript-api)
include_directories(package/pikascript/pikascript-lib/PikaNN/TinyMaix/include)
include_directories(package/pikascript/pikascript-lib/PikaStdDevice)
include_directories(package/pikascript/pikascript-lib/pika_cjson)
# rbg/kcf add ---
include_directories(package/pikascript/pikascript-lib/flashdb)
include_directories(package/lvgl)
include_directories(package/lvgl/lvgl)


include_directories(test)
INCLUDE_DIRECTORIES(/usr/include/lua5.3)
include_directories(config)
# include_directories(boot/demo01-led)
add_subdirectory(package/googletest)
add_subdirectory(package/pikascript)
add_subdirectory(package/lvgl)
add_subdirectory(test)
# add_subdirectory(boot/demo01-led)
# add_subdirectory(boot/demo02-add)
# add_subdirectory(boot/demo03-usart)
# add_subdirectory(boot/demo04-usarts)
# add_subdirectory(boot/demo05-sysobject)
add_subdirectory(boot/demo06-pikamain)
add_subdirectory(boot/banchmark)
add_subdirectory(benchmark)
add_subdirectory(performance)

