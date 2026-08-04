/* SPEC: PJ2026-0501 V2 kernel v0.3; host script runner. */
#ifndef PIKA_SCRIPT_MAIN_ENABLE
#define PIKA_SCRIPT_MAIN_ENABLE 1
#endif

#if defined(__linux__) && PIKA_SCRIPT_MAIN_ENABLE

#include "pika_frontend.h"
#include "pika_kernel.h"

#ifdef PIKA_SCRIPT_BINDING_REGISTER
extern PikaStatus PIKA_SCRIPT_BINDING_REGISTER(
    PikaBindingRegistry* registry);
#endif

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

typedef struct {
    const char* entry_path;
    const char* entry_module;
    char* directory;
} ScriptSourceContext;

static PikaStatus write_stdout(void* context,
                               const char* bytes,
                               uint32_t length) {
    FILE* stream = (FILE*)context;
    return fwrite(bytes, 1u, length, stream) == length
               ? PIKA_STATUS_OK
               : PIKA_STATUS_OUTPUT_ERROR;
}

static char* resolve_script_path(const char* input, int* is_directory) {
    static const char suffix[] = "/main.py";
    struct stat information;
    size_t length;
    char* path;
    if (stat(input, &information) != 0) {
        fprintf(stderr, "io_error: %s: %s\n", input, strerror(errno));
        return NULL;
    }
    if (!S_ISDIR(information.st_mode)) {
        *is_directory = 0;
        length = strlen(input);
        if (length < 3u || strcmp(&input[length - 3u], ".py") != 0) {
            fprintf(stderr, "io_error: expected .py file: %s\n", input);
            return NULL;
        }
        path = (char*)malloc(length + 1u);
        if (path != NULL) {
            memcpy(path, input, length + 1u);
        }
        return path;
    }
    *is_directory = 1;
    length = strlen(input);
    if (length > SIZE_MAX - sizeof(suffix)) {
        return NULL;
    }
    path = (char*)malloc(length + sizeof(suffix));
    if (path == NULL) {
        return NULL;
    }
    memcpy(path, input, length);
    memcpy(&path[length], suffix, sizeof(suffix));
    if (stat(path, &information) != 0 || S_ISDIR(information.st_mode)) {
        fprintf(stderr, "io_error: directory requires main.py: %s\n",
                input);
        free(path);
        return NULL;
    }
    return path;
}

static PikaStatus read_module_source(const char* path,
                                     const char** result,
                                     size_t* length) {
    FILE* file = fopen(path, "rb");
    long end;
    char* source;
    size_t read_count;
    if (file == NULL) {
        return errno == ENOENT ? PIKA_STATUS_MODULE_NOT_FOUND
                               : PIKA_STATUS_IO_ERROR;
    }
    if (fseek(file, 0L, SEEK_END) != 0 ||
        (end = ftell(file)) < 0L ||
        fseek(file, 0L, SEEK_SET) != 0) {
        fclose(file);
        return PIKA_STATUS_IO_ERROR;
    }
    if ((unsigned long)end > SIZE_MAX - 1u) {
        fclose(file);
        return PIKA_STATUS_FRONTEND_LIMIT;
    }
    *length = (size_t)end;
    if (*length > PIKA_FRONTEND_SOURCE_BYTE_LIMIT) {
        fclose(file);
        return PIKA_STATUS_FRONTEND_LIMIT;
    }
    source = (char*)malloc(*length + 1u);
    if (source == NULL) {
        fclose(file);
        return PIKA_STATUS_FRONTEND_LIMIT;
    }
    read_count = fread(source, 1u, *length, file);
    if (read_count != *length || fclose(file) != 0) {
        free(source);
        return PIKA_STATUS_IO_ERROR;
    }
    source[*length] = '\0';
    *result = source;
    return PIKA_STATUS_OK;
}

static char* script_directory(const char* path) {
    const char* separator = strrchr(path, '/');
    size_t length;
    char* directory;
    if (separator == NULL) {
        directory = (char*)malloc(2u);
        if (directory != NULL) {
            directory[0] = '.';
            directory[1] = '\0';
        }
        return directory;
    }
    length = separator == path ? 1u : (size_t)(separator - path);
    directory = (char*)malloc(length + 1u);
    if (directory != NULL) {
        memcpy(directory, path, length);
        directory[length] = '\0';
    }
    return directory;
}

static PikaStatus script_source_load(void* context,
                                     const char* module_name,
                                     const char** source,
                                     size_t* length) {
    ScriptSourceContext* source_context =
        (ScriptSourceContext*)context;
    const char suffix[] = ".py";
    const char* path = source_context->entry_path;
    char* module_path = NULL;
    size_t directory_length;
    size_t name_length;
    PikaStatus status;
    if (strcmp(module_name, source_context->entry_module) != 0) {
        directory_length = strlen(source_context->directory);
        name_length = strlen(module_name);
        if (directory_length > SIZE_MAX - name_length -
                                   sizeof(suffix) - 1u) {
            return PIKA_STATUS_FRONTEND_LIMIT;
        }
        module_path = (char*)malloc(
            directory_length + name_length + sizeof(suffix) + 1u);
        if (module_path == NULL) return PIKA_STATUS_FRONTEND_LIMIT;
        memcpy(module_path, source_context->directory,
               directory_length);
        module_path[directory_length] = '/';
        memcpy(&module_path[directory_length + 1u], module_name,
               name_length);
        memcpy(&module_path[directory_length + name_length + 1u],
               suffix, sizeof(suffix));
        path = module_path;
    }
    status = read_module_source(path, source, length);
    free(module_path);
    return status;
}

static void script_source_release(void* context, const char* source) {
    (void)context;
    free((void*)source);
}

static void write_frontend_diagnostic(
    PikaStatus status,
    const PikaDiagnostic* diagnostic) {
    const char* status_name = pika_status_name(status);
    if (diagnostic->reason ==
            PIKA_DIAGNOSTIC_REASON_LIMIT_EXCEEDED &&
        diagnostic->bound != PIKA_FRONTEND_BOUND_NONE) {
        fprintf(
            stderr,
            "%s:%s:%s:configured=%u:observed=%u:"
            "line=%u:column=%u:offset=%u\n",
            status_name,
            pika_frontend_diagnostic_reason_name(
                diagnostic->reason),
            pika_frontend_bound_macro(diagnostic->bound),
            diagnostic->configured, diagnostic->observed,
            diagnostic->line, diagnostic->column,
            diagnostic->offset);
        return;
    }
    if (diagnostic->reason != PIKA_DIAGNOSTIC_REASON_NONE) {
        fprintf(
            stderr, "%s:%s:line=%u:column=%u:offset=%u\n",
            status_name,
            pika_frontend_diagnostic_reason_name(
                diagnostic->reason),
            diagnostic->line, diagnostic->column,
            diagnostic->offset);
        return;
    }
    fprintf(
        stderr, "%s:%u:%u\n", status_name,
        diagnostic->line, diagnostic->column);
}

static int execute_script(ScriptSourceContext* source_context) {
    PikaCompiledModule* module = NULL;
    PikaDiagnostic diagnostic = {0};
    PikaModuleProvider provider = {
        script_source_load,
        script_source_release,
        source_context,
    };
    PikaProgramRequirements requirements;
    PikaArguments arguments = {NULL, 0u};
    PikaStorage storage;
    PikaOutput output = {write_stdout, stdout};
    PikaResult result;
    PikaMetrics metrics;
    PikaStatus status;
    uint32_t runtime_instruction = UINT32_MAX;
    void* frames = NULL;
    int64_t* values = NULL;
    PikaRuntimeValue* typed_values = NULL;
    PikaObjectSlot* object_slots = NULL;
    uint8_t* object_arena = NULL;
    PikaObjectStorage object_storage;
    PikaObjectStorage* object_storage_pointer = NULL;
    PikaFrontendConfig frontend_config;
#ifdef PIKA_SCRIPT_BINDING_REGISTER
    PikaBindingRegistry registry;
    PikaBindingStorage binding_storage;
    PikaBindingStorage* binding_storage_pointer = NULL;
    PikaBindingValue* binding_values = NULL;
    PikaBindingObject* binding_objects = NULL;

    status = PIKA_SCRIPT_BINDING_REGISTER(&registry);
    if (status != PIKA_STATUS_OK) {
        fprintf(stderr, "%s\n", pika_status_name(status));
        return 1;
    }
    frontend_config.enabled =
        PIKA_CAPABILITY_BINDING_DEFAULT |
        (PIKA_CAPABILITY_OBJECT_CLASS & PIKA_CAPABILITY_PROVIDED);
    frontend_config.bindings = &registry;
#else
    frontend_config.enabled =
        PIKA_CAPABILITY_APPLICATION_DEFAULT |
        (PIKA_CAPABILITY_OBJECT_CLASS & PIKA_CAPABILITY_PROVIDED);
    frontend_config.bindings = NULL;
#endif
    status = pika_module_graph_compile(
        source_context->entry_module, &provider, &frontend_config,
        &module, &diagnostic);
    if (status != PIKA_STATUS_OK) {
        write_frontend_diagnostic(status, &diagnostic);
        return 1;
    }
    status = pika_program_storage_requirements(
        pika_compiled_module_program(module), &requirements);
    if (status == PIKA_STATUS_OK) {
        frames = malloc(requirements.frame_bytes);
        if (requirements.value_count > 0u) {
            values = (int64_t*)calloc(
                requirements.value_count, sizeof(int64_t));
        }
        if (requirements.typed_value_count > 0u) {
            typed_values = (PikaRuntimeValue*)calloc(
                requirements.typed_value_count, sizeof(PikaRuntimeValue));
        }
        if (requirements.object_slot_count > 0u) {
            object_slots = (PikaObjectSlot*)calloc(
                requirements.object_slot_count, sizeof(PikaObjectSlot));
            if (requirements.object_bytes > 0u) {
                object_arena = (uint8_t*)calloc(
                    requirements.object_bytes, sizeof(uint8_t));
            }
            if (object_slots != NULL &&
                (requirements.object_bytes == 0u || object_arena != NULL)) {
                memset(&object_storage, 0, sizeof(object_storage));
                object_storage.slots = object_slots;
                object_storage.capacity = requirements.object_slot_count;
                object_storage.arena = object_arena;
                object_storage.arena_capacity = requirements.object_bytes;
                object_storage_pointer = &object_storage;
            }
        }
        if (frames == NULL ||
            (requirements.value_count > 0u && values == NULL) ||
            (requirements.typed_value_count > 0u && typed_values == NULL) ||
            (requirements.object_slot_count > 0u &&
             object_storage_pointer == NULL)) {
            status = PIKA_STATUS_STORAGE_TOO_SMALL;
        }
#ifdef PIKA_SCRIPT_BINDING_REGISTER
        if (requirements.binding_value_count > 0u) {
            binding_values = (PikaBindingValue*)calloc(
                requirements.binding_value_count,
                sizeof(PikaBindingValue));
        }
        if (requirements.binding_object_count > 0u) {
            binding_objects = (PikaBindingObject*)calloc(
                requirements.binding_object_count,
                sizeof(PikaBindingObject));
        }
        if ((requirements.binding_value_count > 0u &&
             binding_values == NULL) ||
            (requirements.binding_object_count > 0u &&
             binding_objects == NULL)) {
            status = PIKA_STATUS_STORAGE_TOO_SMALL;
        } else if (requirements.binding_value_count > 0u ||
                   requirements.binding_object_count > 0u) {
            binding_storage.values = binding_values;
            binding_storage.value_capacity =
                requirements.binding_value_count;
            binding_storage.objects = binding_objects;
            binding_storage.object_capacity =
                requirements.binding_object_count;
            binding_storage_pointer = &binding_storage;
        }
#endif
    }
    if (status == PIKA_STATUS_OK) {
        storage.frames = frames;
        storage.frame_bytes = requirements.frame_bytes;
        storage.values = values;
        storage.value_capacity = requirements.value_count;
        storage.typed_values = typed_values;
        storage.typed_value_capacity = requirements.typed_value_count;
        storage.objects = object_storage_pointer;
#ifdef PIKA_SCRIPT_BINDING_REGISTER
        status = pika_program_execute_bound(
            pika_compiled_module_program(module), &arguments, &storage,
            binding_storage_pointer, &output, &result, &metrics);
#else
        status = pika_program_execute(
            pika_compiled_module_program(module), &arguments, &storage,
            &output, &result, &metrics);
#endif
        runtime_instruction = result.instruction_index;
    }
#ifdef PIKA_SCRIPT_BINDING_REGISTER
    free(binding_objects);
    free(binding_values);
#endif
    if (object_storage_pointer != NULL) {
        pika_object_storage_release(object_storage_pointer);
    }
    free(object_arena);
    free(object_slots);
    free(typed_values);
    free(values);
    free(frames);
    pika_compiled_module_destroy(module);
    if (status != PIKA_STATUS_OK) {
        if (runtime_instruction != UINT32_MAX) {
            fprintf(
                stderr, "%s:instruction=%u\n",
                pika_status_name(status), runtime_instruction);
        } else {
            fprintf(stderr, "%s\n", pika_status_name(status));
        }
        return 1;
    }
    return 0;
}

int main(int argc, char** argv) {
    char* path;
    ScriptSourceContext source_context;
    int is_directory = 0;
    int result;
    if (argc != 2) {
        fprintf(stderr, "usage: pika_script <file.py|directory>\n");
        return 2;
    }
    path = resolve_script_path(argv[1], &is_directory);
    if (path == NULL) {
        return 1;
    }
    source_context.entry_path = path;
    source_context.entry_module = is_directory ? "main" : "__entry";
    source_context.directory = script_directory(path);
    if (source_context.directory == NULL) {
        free(path);
        fprintf(stderr, "frontend_limit\n");
        return 1;
    }
    result = execute_script(&source_context);
    free(source_context.directory);
    free(path);
    return result;
}

#else

typedef unsigned char PikaScriptMainDisabledTranslationUnit;

#endif
