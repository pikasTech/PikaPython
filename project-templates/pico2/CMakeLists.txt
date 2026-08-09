cmake_minimum_required(VERSION 3.19)

# Prefer an explicit environment/cache override, then the user's standard
# SDK location.
if(NOT PICO_SDK_PATH AND DEFINED ENV{PICO_SDK_PATH})
    set(PICO_SDK_PATH "$ENV{PICO_SDK_PATH}" CACHE PATH
        "Path to the Raspberry Pi Pico SDK")
endif()
if(NOT PICO_SDK_PATH AND EXISTS "$ENV{HOME}/pico-sdk")
    set(PICO_SDK_PATH "$ENV{HOME}/pico-sdk" CACHE PATH
        "Path to the Raspberry Pi Pico SDK")
endif()
if(NOT PICO_SDK_PATH)
    message(FATAL_ERROR
        "Pico SDK was not found; set PICO_SDK_PATH or PICO_SDK_PATH environment variable")
endif()

set(PICO_BOARD pico2 CACHE STRING "Pico board")
set(PICO_PLATFORM rp2350-arm-s CACHE STRING "Pico platform")
set(PICO_STDIO_USB_CONNECT_WAIT_TIMEOUT_MS 0 CACHE STRING
    "Do not block startup while waiting for a USB CDC host")

include("${PICO_SDK_PATH}/external/pico_sdk_import.cmake")

project(pikapython_pico2 C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
# The Pico SDK uses GNU inline assembly in its RP2350 runtime sources.
set(CMAKE_C_EXTENSIONS ON)

pico_sdk_init()

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
    if(PIKA_RELATIVE_SOURCE STREQUAL "pikapython-packages/socket/platform_socket.c")
        continue()
    endif()
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

add_executable(pico2-firmware
    firmware/main.c
    firmware/platform.c
    firmware/platform_socket.c
    ${PIKA_SOURCES}
)

target_include_directories(pico2-firmware PRIVATE
    firmware
    ${PIKA_INCLUDE_DIRECTORIES}
)

target_compile_definitions(pico2-firmware PRIVATE
    PIKA_FRONTEND_MODULE_LIMIT=32u
    PIKA_FRONTEND_FUNCTION_LIMIT=128u
    PIKA_FRONTEND_CLASS_LIMIT=16u
    PIKA_FRONTEND_IDENTIFIER_BYTE_LIMIT=63u
    PIKA_FRONTEND_SYMBOL_LIMIT=128u
    PIKA_FRONTEND_PARAMETER_LIMIT=16u
    PIKA_FRONTEND_ARGUMENT_LIMIT=16u
    PIKA_FRONTEND_COLLECTION_ELEMENT_LIMIT=64u
    PIKA_FRONTEND_EXCEPTION_HANDLER_LIMIT=16u
    PIKA_RUNTIME_VALUE_LIMIT=64u
    PIKA_REPL_MAX_LINE_BYTES=256u
    PIKA_REPL_MAX_BLOCK_BYTES=4096u
    PIKA_REPL_MAX_HISTORY_ENTRIES=8u
    PIKA_REPL_MAX_HISTORY_BYTES=1024u
    PIKA_REPL_MAX_CODE_BYTES=16384u
    PIKA_REPL_MAX_OUTPUT_BYTES_PER_COMMAND=2048u
    PIKA_REPL_MAX_TRACEBACK_FRAMES=8u
)

target_compile_options(pico2-firmware PRIVATE
    -Wall
    -Wextra
    -ffunction-sections
    -fdata-sections
)

target_link_libraries(pico2-firmware PRIVATE
    pico_stdlib
    hardware_adc
    hardware_pwm
)

pico_enable_stdio_usb(pico2-firmware 1)
pico_enable_stdio_uart(pico2-firmware 0)
pico_set_program_name(pico2-firmware "PikaPython Pico 2")
pico_set_program_description(pico2-firmware
    "PikaPython v2 on Raspberry Pi Pico 2")
pico_add_extra_outputs(pico2-firmware)
