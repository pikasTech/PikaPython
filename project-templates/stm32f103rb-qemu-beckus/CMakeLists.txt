cmake_minimum_required(VERSION 3.19)
project(pikapython_stm32f103rb_qemu C ASM)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXECUTABLE_SUFFIX ".elf")

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
        set(PIKA_SOURCE_PATH
            "${CMAKE_SOURCE_DIR}/${PIKA_RELATIVE_SOURCE}")
    else()
        set(PIKA_SOURCE_PATH
            "${PIKA_GENERATED_DIR}/${PIKA_RELATIVE_SOURCE}")
    endif()
    if(NOT EXISTS "${PIKA_SOURCE_PATH}")
        message(FATAL_ERROR
            "PikaPython source is missing: ${PIKA_RELATIVE_SOURCE}")
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

set(PIKA_FIRMWARE_DEFINITIONS
    PIKA_FRONTEND_CALL_DEPTH_LIMIT=4u
    PIKA_FRONTEND_MODULE_LIMIT=2u
    PIKA_FRONTEND_FUNCTION_LIMIT=8u
    PIKA_FRONTEND_CLASS_LIMIT=1u
    PIKA_FRONTEND_IDENTIFIER_BYTE_LIMIT=23u
    PIKA_FRONTEND_SYMBOL_LIMIT=16u
    PIKA_FRONTEND_PARAMETER_LIMIT=8u
    PIKA_FRONTEND_ARGUMENT_LIMIT=8u
    PIKA_FRONTEND_COLLECTION_ELEMENT_LIMIT=8u
    PIKA_FRONTEND_EXCEPTION_HANDLER_LIMIT=2u
    PIKA_RUNTIME_VALUE_LIMIT=16u
    PIKA_REPL_MAX_LINE_BYTES=128u
    PIKA_REPL_MAX_BLOCK_BYTES=256u
    PIKA_REPL_MAX_HISTORY_ENTRIES=4u
    PIKA_REPL_MAX_HISTORY_BYTES=128u
    PIKA_REPL_MAX_CODE_BYTES=4096u
    PIKA_REPL_MAX_OUTPUT_BYTES_PER_COMMAND=512u
    PIKA_REPL_MAX_TRACEBACK_FRAMES=4u
    PIKA_REPL_INTERRUPT_POLL_INTERVAL=128u
    PIKA_APPLICATION_REPL_BLOCK_BYTES=256u
    PIKA_APPLICATION_REPL_HISTORY_BYTES=128u
    PIKA_APPLICATION_REPL_HISTORY_ENTRIES=4u
)

function(configure_firmware target map_name)
    target_include_directories(${target} PRIVATE
        ${PIKA_INCLUDE_DIRECTORIES}
    )
    target_compile_options(${target} PRIVATE
        -mcpu=cortex-m3
        -mthumb
        -ffreestanding
        -ffunction-sections
        -fdata-sections
        -fno-common
        -fno-unwind-tables
        -fno-asynchronous-unwind-tables
        -Wall
        -Wextra
    )
    target_compile_definitions(${target} PRIVATE
        ${PIKA_FIRMWARE_DEFINITIONS}
    )
    target_link_options(${target} PRIVATE
        -mcpu=cortex-m3
        -mthumb
        -nostartfiles
        --specs=nano.specs
        --specs=nosys.specs
        -Wl,--build-id=none
        -Wl,--gc-sections
        -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/${map_name}
        -Wl,-T,${CMAKE_SOURCE_DIR}/firmware/linker.ld
    )
    target_link_libraries(${target} PRIVATE m)
endfunction()

add_executable(qemu-firmware
    firmware/startup.S
    firmware/main.c
    firmware/platform.c
    ${PIKA_SOURCES}
)
configure_firmware(
    qemu-firmware
    qemu-firmware.map
)
