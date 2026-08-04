cmake_minimum_required(VERSION 3.19)
project(pikapython_windows_x86_64 C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(PIKA_GENERATED_DIR "${CMAKE_SOURCE_DIR}/pikapython-generated")
set(PIKA_SOURCE_MANIFEST "${PIKA_GENERATED_DIR}/source-manifest.json")
if(NOT EXISTS "${PIKA_SOURCE_MANIFEST}")
    message(FATAL_ERROR
        "Missing pikapython-generated/source-manifest.json; run pikapython-cli build --prebuild-only first")
endif()

file(READ "${PIKA_SOURCE_MANIFEST}" PIKA_MANIFEST_JSON)
string(JSON PIKA_SOURCE_COUNT LENGTH "${PIKA_MANIFEST_JSON}" sources)
if(PIKA_SOURCE_COUNT EQUAL 0)
    message(FATAL_ERROR "PikaPython source manifest contains no sources")
endif()
math(EXPR PIKA_SOURCE_LAST "${PIKA_SOURCE_COUNT} - 1")
set(PIKA_SOURCES)
foreach(PIKA_INDEX RANGE 0 ${PIKA_SOURCE_LAST})
    string(JSON PIKA_RELATIVE_SOURCE GET
        "${PIKA_MANIFEST_JSON}" sources ${PIKA_INDEX})
    if(PIKA_RELATIVE_SOURCE MATCHES "/")
        set(PIKA_SOURCE_PATH "${CMAKE_SOURCE_DIR}/${PIKA_RELATIVE_SOURCE}")
    else()
        set(PIKA_SOURCE_PATH "${PIKA_GENERATED_DIR}/${PIKA_RELATIVE_SOURCE}")
    endif()
    if(NOT EXISTS "${PIKA_SOURCE_PATH}")
        message(FATAL_ERROR "PikaPython source is missing: ${PIKA_RELATIVE_SOURCE}")
    endif()
    list(APPEND PIKA_SOURCES "${PIKA_SOURCE_PATH}")
endforeach()

string(JSON PIKA_INCLUDE_COUNT LENGTH
    "${PIKA_MANIFEST_JSON}" includeDirectories)
set(PIKA_INCLUDE_DIRECTORIES)
if(PIKA_INCLUDE_COUNT GREATER 0)
    math(EXPR PIKA_INCLUDE_LAST "${PIKA_INCLUDE_COUNT} - 1")
    foreach(PIKA_INDEX RANGE 0 ${PIKA_INCLUDE_LAST})
        string(JSON PIKA_RELATIVE_INCLUDE GET
            "${PIKA_MANIFEST_JSON}" includeDirectories ${PIKA_INDEX})
        list(APPEND PIKA_INCLUDE_DIRECTORIES
            "${CMAKE_SOURCE_DIR}/${PIKA_RELATIVE_INCLUDE}")
    endforeach()
endif()

add_executable(pikapython-windows
    firmware/main.c
    firmware/platform.c
    ${PIKA_SOURCES}
)
target_include_directories(pikapython-windows PRIVATE
    ${PIKA_INCLUDE_DIRECTORIES}
)
target_compile_definitions(pikapython-windows PRIVATE
    PIKA_SCRIPT_MAIN_ENABLE=0
    PIKA_REPL_SHELL_MAIN_ENABLE=0
    PIKA_FRONTEND_MODULE_LIMIT=48u
    PIKA_FRONTEND_CLASS_LIMIT=8u
    PIKA_FRONTEND_IDENTIFIER_BYTE_LIMIT=63u
    PIKA_FRONTEND_SYMBOL_LIMIT=192u
    PIKA_FRONTEND_PARAMETER_LIMIT=16u
    PIKA_FRONTEND_ARGUMENT_LIMIT=16u
    PIKA_FRONTEND_COLLECTION_ELEMENT_LIMIT=32u
    PIKA_FRONTEND_EXCEPTION_HANDLER_LIMIT=16u
    PIKA_RUNTIME_VALUE_LIMIT=128u
)
target_compile_options(pikapython-windows PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -ffunction-sections
    -fdata-sections
)
target_link_options(pikapython-windows PRIVATE
    -Wl,--gc-sections
    -static
)
target_link_libraries(pikapython-windows PRIVATE m ws2_32)
