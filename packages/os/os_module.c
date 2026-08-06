/* SPEC: PJ2026-050114 runtime ecosystem packages v0.1; os binding. */
#include "_os_binding.h"
#include "os_platform.h"
#include "pika_capability_config.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#if PIKA_CAPABILITY_LIBRARY_OS_BASIC_ENABLE

static char pika_os_string_output[PIKA_BINDING_RESULT_BYTE_LIMIT];
static uint8_t pika_os_bytes_output[PIKA_BINDING_RESULT_BYTE_LIMIT];
static PikaOsDirectory pika_os_directory;
static PikaBindingValue pika_os_directory_values[PIKA_BINDING_CONTAINER_ELEMENT_LIMIT];

static PikaStatus os_validate(const PikaBindingCall* call, PikaBindingValue* result, uint8_t count) {
    if (call == NULL || result == NULL || call->argument_count != count ||
        (count > 0u && call->arguments == NULL)) return PIKA_STATUS_INVALID_ARGUMENT;
    return PIKA_STATUS_OK;
}

static PikaStatus os_path(const PikaBindingValue* value, char* output) {
    uint32_t index;
    if (value == NULL || output == NULL || value->kind != PIKA_BINDING_VALUE_STRING) return PIKA_STATUS_TYPE_MISMATCH;
    if (value->as.buffer.length == 0u || value->as.buffer.length >= PIKA_OS_PATH_LIMIT ||
        (value->as.buffer.length > 0u && value->as.buffer.data == NULL)) return PIKA_STATUS_VALUE_ERROR;
    for (index = 0u; index < value->as.buffer.length; ++index) {
        if (value->as.buffer.data[index] == 0u) return PIKA_STATUS_VALUE_ERROR;
    }
    memcpy(output, value->as.buffer.data, value->as.buffer.length);
    output[value->as.buffer.length] = '\0';
    return PIKA_STATUS_OK;
}

static PikaStatus os_integer(const PikaBindingValue* value, int64_t* output) {
    if (value == NULL || output == NULL || value->kind != PIKA_BINDING_VALUE_INTEGER) return PIKA_STATUS_TYPE_MISMATCH;
    *output = value->as.integer;
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding__os_getcwd(const PikaBindingCall* call, PikaBindingValue* result) {
    size_t length = 0u;
    PikaStatus status = os_validate(call, result, 0u);
    if (status == PIKA_STATUS_OK) status = pika_os_platform_getcwd(pika_os_string_output, sizeof(pika_os_string_output), &length);
    if (status != PIKA_STATUS_OK) return status;
    result->kind = PIKA_BINDING_VALUE_STRING;
    result->as.buffer.data = (const uint8_t*)pika_os_string_output;
    result->as.buffer.length = (uint32_t)length;
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding__os_chdir(const PikaBindingCall* call, PikaBindingValue* result) {
    char path[PIKA_OS_PATH_LIMIT];
    PikaStatus status = os_validate(call, result, 1u);
    if (status == PIKA_STATUS_OK) status = os_path(&call->arguments[0], path);
    if (status == PIKA_STATUS_OK) status = pika_os_platform_chdir(path);
    if (status != PIKA_STATUS_OK) return status;
    result->kind = PIKA_BINDING_VALUE_NONE;
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding__os_mkdir(const PikaBindingCall* call, PikaBindingValue* result) {
    char path[PIKA_OS_PATH_LIMIT];
    int64_t mode;
    PikaStatus status = os_validate(call, result, 2u);
    if (status == PIKA_STATUS_OK) status = os_path(&call->arguments[0], path);
    if (status == PIKA_STATUS_OK) status = os_integer(&call->arguments[1], &mode);
    if (status == PIKA_STATUS_OK) status = pika_os_platform_mkdir(path, mode);
    if (status != PIKA_STATUS_OK) return status;
    result->kind = PIKA_BINDING_VALUE_NONE;
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding__os_rmdir(const PikaBindingCall* call, PikaBindingValue* result) {
    char path[PIKA_OS_PATH_LIMIT];
    PikaStatus status = os_validate(call, result, 1u);
    if (status == PIKA_STATUS_OK) status = os_path(&call->arguments[0], path);
    if (status == PIKA_STATUS_OK) status = pika_os_platform_rmdir(path);
    if (status != PIKA_STATUS_OK) return status;
    result->kind = PIKA_BINDING_VALUE_NONE;
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding__os_listdir(const PikaBindingCall* call, PikaBindingValue* result) {
    char path[PIKA_OS_PATH_LIMIT];
    size_t index;
    PikaStatus status = os_validate(call, result, 1u);
    if (status == PIKA_STATUS_OK) status = os_path(&call->arguments[0], path);
    if (status == PIKA_STATUS_OK) status = pika_os_platform_listdir(path, &pika_os_directory);
    if (status != PIKA_STATUS_OK) return status;
    if (pika_os_directory.count > PIKA_BINDING_CONTAINER_ELEMENT_LIMIT) return PIKA_STATUS_STORAGE_TOO_SMALL;
    for (index = 0u; index < pika_os_directory.count; ++index) {
        pika_os_directory_values[index].kind = PIKA_BINDING_VALUE_STRING;
        pika_os_directory_values[index].as.buffer.data = (const uint8_t*)pika_os_directory.names[index];
        pika_os_directory_values[index].as.buffer.length = (uint32_t)pika_os_directory.lengths[index];
    }
    result->kind = PIKA_BINDING_VALUE_LIST;
    result->as.sequence.values = pika_os_directory_values;
    result->as.sequence.length = (uint32_t)pika_os_directory.count;
    return PIKA_STATUS_OK;
}

static PikaStatus os_one_path_none(const PikaBindingCall* call, PikaBindingValue* result, PikaStatus (*operation)(const char*)) {
    char path[PIKA_OS_PATH_LIMIT];
    PikaStatus status = os_validate(call, result, 1u);
    if (status == PIKA_STATUS_OK) status = os_path(&call->arguments[0], path);
    if (status == PIKA_STATUS_OK) status = operation(path);
    if (status != PIKA_STATUS_OK) return status;
    result->kind = PIKA_BINDING_VALUE_NONE;
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding__os_remove(const PikaBindingCall* call, PikaBindingValue* result) {
    return os_one_path_none(call, result, pika_os_platform_remove);
}

PikaStatus pika_binding__os_rename(const PikaBindingCall* call, PikaBindingValue* result) {
    char source[PIKA_OS_PATH_LIMIT];
    char destination[PIKA_OS_PATH_LIMIT];
    PikaStatus status = os_validate(call, result, 2u);
    if (status == PIKA_STATUS_OK) status = os_path(&call->arguments[0], source);
    if (status == PIKA_STATUS_OK) status = os_path(&call->arguments[1], destination);
    if (status == PIKA_STATUS_OK) status = pika_os_platform_rename(source, destination);
    if (status != PIKA_STATUS_OK) return status;
    result->kind = PIKA_BINDING_VALUE_NONE;
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding__os_open(const PikaBindingCall* call, PikaBindingValue* result) {
    char path[PIKA_OS_PATH_LIMIT];
    int64_t flags;
    int64_t mode;
    int64_t descriptor;
    PikaStatus status = os_validate(call, result, 3u);
    if (status == PIKA_STATUS_OK) status = os_path(&call->arguments[0], path);
    if (status == PIKA_STATUS_OK) status = os_integer(&call->arguments[1], &flags);
    if (status == PIKA_STATUS_OK) status = os_integer(&call->arguments[2], &mode);
    if (status == PIKA_STATUS_OK) status = pika_os_platform_open(path, flags, mode, &descriptor);
    if (status != PIKA_STATUS_OK) return status;
    result->kind = PIKA_BINDING_VALUE_INTEGER;
    result->as.integer = descriptor;
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding__os_close(const PikaBindingCall* call, PikaBindingValue* result) {
    int64_t descriptor;
    PikaStatus status = os_validate(call, result, 1u);
    if (status == PIKA_STATUS_OK) status = os_integer(&call->arguments[0], &descriptor);
    if (status == PIKA_STATUS_OK) status = pika_os_platform_close(descriptor);
    if (status != PIKA_STATUS_OK) return status;
    result->kind = PIKA_BINDING_VALUE_NONE;
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding__os_read(const PikaBindingCall* call, PikaBindingValue* result) {
    int64_t descriptor;
    int64_t size;
    size_t length = 0u;
    PikaStatus status = os_validate(call, result, 2u);
    if (status == PIKA_STATUS_OK) status = os_integer(&call->arguments[0], &descriptor);
    if (status == PIKA_STATUS_OK) status = os_integer(&call->arguments[1], &size);
    if (status == PIKA_STATUS_OK && (size < 0 || (uint64_t)size > sizeof(pika_os_bytes_output))) status = PIKA_STATUS_VALUE_ERROR;
    if (status == PIKA_STATUS_OK) status = pika_os_platform_read(descriptor, pika_os_bytes_output, (size_t)size, &length);
    if (status != PIKA_STATUS_OK) return status;
    result->kind = PIKA_BINDING_VALUE_BYTES;
    result->as.buffer.data = pika_os_bytes_output;
    result->as.buffer.length = (uint32_t)length;
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding__os_write(const PikaBindingCall* call, PikaBindingValue* result) {
    int64_t descriptor;
    size_t written = 0u;
    PikaStatus status = os_validate(call, result, 2u);
    if (status == PIKA_STATUS_OK) status = os_integer(&call->arguments[0], &descriptor);
    if (status == PIKA_STATUS_OK && call->arguments[1].kind != PIKA_BINDING_VALUE_BYTES) status = PIKA_STATUS_TYPE_MISMATCH;
    if (status == PIKA_STATUS_OK) status = pika_os_platform_write(descriptor, call->arguments[1].as.buffer.data, call->arguments[1].as.buffer.length, &written);
    if (status != PIKA_STATUS_OK) return status;
    result->kind = PIKA_BINDING_VALUE_INTEGER;
    result->as.integer = (int64_t)written;
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding__os_lseek(const PikaBindingCall* call, PikaBindingValue* result) {
    int64_t descriptor;
    int64_t position;
    int64_t how;
    int64_t offset;
    PikaStatus status = os_validate(call, result, 3u);
    if (status == PIKA_STATUS_OK) status = os_integer(&call->arguments[0], &descriptor);
    if (status == PIKA_STATUS_OK) status = os_integer(&call->arguments[1], &position);
    if (status == PIKA_STATUS_OK) status = os_integer(&call->arguments[2], &how);
    if (status == PIKA_STATUS_OK) status = pika_os_platform_lseek(descriptor, position, how, &offset);
    if (status != PIKA_STATUS_OK) return status;
    result->kind = PIKA_BINDING_VALUE_INTEGER;
    result->as.integer = offset;
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding__os_stat_kind(const PikaBindingCall* call, PikaBindingValue* result) {
    char path[PIKA_OS_PATH_LIMIT];
    int64_t kind;
    PikaStatus status = os_validate(call, result, 1u);
    if (status == PIKA_STATUS_OK) status = os_path(&call->arguments[0], path);
    if (status == PIKA_STATUS_OK) status = pika_os_platform_stat_kind(path, &kind);
    if (status != PIKA_STATUS_OK) return status;
    result->kind = PIKA_BINDING_VALUE_INTEGER;
    result->as.integer = kind;
    return PIKA_STATUS_OK;
}

#endif
