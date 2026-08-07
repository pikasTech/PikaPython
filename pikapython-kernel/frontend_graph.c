/* SPEC: PJ2026-0501 kernel v0.35; bounded reachable module graph. */
#include "frontend_internal.h"

#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PIKA_GRAPH_PROGRAM_SYMBOL_CAPACITY 48u

typedef struct {
    char text[PIKA_FRONTEND_IDENTIFIER_BYTE_LIMIT + 1u];
    uint32_t token_index;
} PikaGraphName;

typedef struct {
    PikaGraphName name;
    uint32_t body_first;
    uint32_t body_last;
    PikaGraphName* locals;
    uint32_t local_count;
    uint32_t local_capacity;
    PikaGraphName* declared_globals;
    uint32_t declared_global_count;
    uint32_t declared_global_capacity;
    uint64_t hidden_globals[PIKA_FRONTEND_MODULE_LIMIT];
} PikaGraphFunction;

typedef struct {
    PikaGraphName name;
    PikaGraphName* members;
    uint32_t member_count;
    uint32_t member_capacity;
    uint32_t last_token;
    uint8_t top_level;
    uint8_t is_python;
    uint8_t target_module;
    uint8_t from_import;
    uint8_t is_binding;
    uint8_t is_program_image;
    uint8_t from_base;
} PikaGraphImport;

typedef struct {
    char name[PIKA_FRONTEND_IDENTIFIER_BYTE_LIMIT + 1u];
    const char* source;
    size_t length;
    PikaToken* tokens;
    uint32_t token_count;
    PikaGraphImport* imports;
    uint32_t import_count;
    uint32_t import_capacity;
    PikaGraphFunction* functions;
    uint32_t function_count;
    uint32_t function_capacity;
    PikaGraphName* classes;
    uint32_t class_count;
    uint32_t class_capacity;
    PikaGraphName* globals;
    uint32_t global_count;
    uint32_t global_capacity;
    uint8_t state;
    uint8_t topology_order;
    uint8_t publicly_available;
    size_t generated_start;
    size_t generated_end;
} PikaGraphModule;

typedef struct {
    char* data;
    size_t length;
    size_t capacity;
    uint8_t count_only;
} PikaGraphBuffer;

typedef struct {
    uint32_t buffer_offset;
    uint16_t name_length;
    uint8_t target_module;
    uint8_t flags;
    uint8_t target_is_stable;
} PikaGraphRetainedImport;

typedef struct {
    const PikaModuleProvider* provider;
    const PikaFrontendConfig* config;
    const PikaCompiledModule* base;
    PikaDiagnostic* diagnostic;
    PikaGraphModule* modules[PIKA_FRONTEND_MODULE_LIMIT];
    uint32_t module_count;
    uint8_t topology[PIKA_FRONTEND_MODULE_LIMIT];
    uint32_t topology_count;
    uint8_t module_prefix_base;
    uint8_t build_program_image;
} PikaGraph;

typedef struct {
    const char* source;
    size_t length;
    const PikaModuleProvider* modules;
} PikaIncrementalGraphProvider;

static void graph_set_diagnostic_source(
    PikaGraph* graph, const PikaGraphModule* module) {
    size_t length;
    if (graph == NULL || graph->diagnostic == NULL || module == NULL) {
        return;
    }
    length = strlen(module->name);
    if (length >= sizeof(graph->diagnostic->source_name)) {
        length = sizeof(graph->diagnostic->source_name) - 1u;
    }
    memcpy(graph->diagnostic->source_name, module->name, length);
    graph->diagnostic->source_name[length] = '\0';
}

static int graph_assignment_token(PikaTokenKind kind) {
    return kind == PIKA_TOKEN_ASSIGN ||
           kind == PIKA_TOKEN_PLUS_ASSIGN ||
           kind == PIKA_TOKEN_MINUS_ASSIGN ||
           kind == PIKA_TOKEN_STAR_ASSIGN ||
           kind == PIKA_TOKEN_SLASH_ASSIGN ||
           kind == PIKA_TOKEN_DOUBLE_SLASH_ASSIGN ||
           kind == PIKA_TOKEN_PERCENT_ASSIGN ||
           kind == PIKA_TOKEN_DOUBLE_STAR_ASSIGN ||
           kind == PIKA_TOKEN_AMPERSAND_ASSIGN ||
           kind == PIKA_TOKEN_CARET_ASSIGN ||
           kind == PIKA_TOKEN_PIPE_ASSIGN ||
           kind == PIKA_TOKEN_LEFT_SHIFT_ASSIGN ||
           kind == PIKA_TOKEN_RIGHT_SHIFT_ASSIGN ||
           kind == PIKA_TOKEN_AT_ASSIGN;
}

static int graph_assignment_statement(
    const PikaGraphModule* module,
    uint32_t token_index) {
    PikaTokenKind previous;
    if (token_index + 1u >= module->token_count ||
        !graph_assignment_token(
            module->tokens[token_index + 1u].kind)) {
        return 0;
    }
    if (token_index == 0u) return 1;
    previous = module->tokens[token_index - 1u].kind;
    return previous == PIKA_TOKEN_NEWLINE ||
           previous == PIKA_TOKEN_INDENT ||
           previous == PIKA_TOKEN_DEDENT;
}

#if PIKA_CAPABILITY_ASSIGNMENT_UNPACK_ENABLE
static int graph_unpack_assignment_target(
    const PikaGraphModule* module,
    uint32_t token_index) {
    uint32_t start = token_index;
    uint32_t cursor;
    int target = 0;
    int saw_comma = 0;
    while (start > 0u) {
        PikaTokenKind previous =
            module->tokens[start - 1u].kind;
        if (previous == PIKA_TOKEN_NEWLINE ||
            previous == PIKA_TOKEN_INDENT ||
            previous == PIKA_TOKEN_DEDENT) {
            break;
        }
        --start;
    }
    cursor = start;
    while (cursor < module->token_count) {
        if (module->tokens[cursor].kind != PIKA_TOKEN_NAME) {
            return 0;
        }
        if (cursor == token_index) target = 1;
        ++cursor;
        if (cursor >= module->token_count) return 0;
        if (module->tokens[cursor].kind == PIKA_TOKEN_ASSIGN) {
            return target && saw_comma;
        }
        if (module->tokens[cursor].kind != PIKA_TOKEN_COMMA) {
            return 0;
        }
        saw_comma = 1;
        ++cursor;
        if (cursor >= module->token_count) return 0;
        if (module->tokens[cursor].kind == PIKA_TOKEN_ASSIGN) {
            return target;
        }
    }
    return 0;
}
#endif

static PikaStatus graph_fail(PikaGraph* graph,
                             PikaStatus status,
                             const PikaGraphModule* module,
                             uint32_t token_index) {
    uint32_t line = 1u;
    uint32_t column = 1u;
    if (module != NULL && module->tokens != NULL &&
        token_index < module->token_count) {
        pika_frontend_source_location(
            module->source, module->tokens[token_index].offset,
            &line, &column);
    }
    if (graph->diagnostic == NULL ||
        graph->diagnostic->status != status ||
        graph->diagnostic->reason ==
            PIKA_DIAGNOSTIC_REASON_NONE) {
        pika_frontend_set_diagnostic(
            graph->diagnostic, status, line, column);
        graph_set_diagnostic_source(graph, module);
    } else if (graph->diagnostic->source_name[0] == '\0') {
        graph_set_diagnostic_source(graph, module);
    }
    return status;
}

static PikaStatus graph_fail_limit(PikaGraph* graph,
                                   PikaFrontendBound bound,
                                   uint32_t configured,
                                   uint32_t observed) {
    pika_frontend_set_limit_diagnostic(
        graph->diagnostic, bound, configured, observed,
        1u, 1u, 0u);
    return PIKA_STATUS_FRONTEND_LIMIT;
}

static PikaStatus graph_fail_limit_at(
    PikaGraph* graph,
    const PikaGraphModule* module,
    uint32_t token_index,
    PikaFrontendBound bound,
    uint32_t configured,
    uint32_t observed) {
    uint32_t line = 1u;
    uint32_t column = 1u;
    uint32_t offset = 0u;
    if (module != NULL && module->tokens != NULL &&
        token_index < module->token_count) {
        offset = module->tokens[token_index].offset;
        pika_frontend_source_location(
            module->source, offset, &line, &column);
    }
    pika_frontend_set_limit_diagnostic(
        graph->diagnostic, bound, configured, observed,
        line, column, offset);
    graph_set_diagnostic_source(graph, module);
    return PIKA_STATUS_FRONTEND_LIMIT;
}

static int graph_identifier_valid(const char* name) {
    const unsigned char* value = (const unsigned char*)name;
    if (value == NULL ||
        !(value[0] == '_' ||
          (value[0] >= 'A' && value[0] <= 'Z') ||
          (value[0] >= 'a' && value[0] <= 'z'))) {
        return 0;
    }
    ++value;
    while (*value != '\0') {
        if (!(*value == '_' ||
              (*value >= 'A' && *value <= 'Z') ||
              (*value >= 'a' && *value <= 'z') ||
              (*value >= '0' && *value <= '9'))) {
            return 0;
        }
        ++value;
    }
    return 1;
}

static PikaStatus graph_copy_token(const PikaGraphModule* module,
                                   uint32_t token_index,
                                   char* output,
                                   size_t capacity) {
    const PikaToken* token;
    if (module == NULL || token_index >= module->token_count ||
        output == NULL || capacity == 0u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    token = &module->tokens[token_index];
    if ((size_t)token->length + 1u > capacity) {
        return PIKA_STATUS_FRONTEND_LIMIT;
    }
    memcpy(output, &module->source[token->offset], token->length);
    output[token->length] = '\0';
    return PIKA_STATUS_OK;
}

static int graph_token_equals(const PikaGraphModule* module,
                              uint32_t token_index,
                              const char* text) {
    const PikaToken* token;
    size_t length;
    if (module == NULL || token_index >= module->token_count ||
        text == NULL) {
        return 0;
    }
    token = &module->tokens[token_index];
    length = strlen(text);
    return length == token->length &&
           memcmp(&module->source[token->offset], text, length) == 0;
}

static int graph_name_index(const PikaGraphModule* module,
                            const PikaGraphName* names,
                            uint32_t count,
                            uint32_t token_index) {
    uint32_t index;
    for (index = 0u; index < count; ++index) {
        if (graph_token_equals(module, token_index, names[index].text)) {
            return (int)index;
        }
    }
    return -1;
}

static PikaStatus graph_reserve(void** values,
                                uint32_t* capacity,
                                uint32_t required,
                                uint32_t limit,
                                size_t element_size) {
    uint32_t next;
    void* resized;
    if (values == NULL || capacity == NULL || element_size == 0u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (required > limit) return PIKA_STATUS_FRONTEND_LIMIT;
    if (required <= *capacity) return PIKA_STATUS_OK;
    next = *capacity == 0u ? 1u : *capacity;
    while (next < required) {
        if (next > limit / 2u) {
            next = limit;
            break;
        }
        next *= 2u;
    }
    if ((size_t)next > SIZE_MAX / element_size) {
        return PIKA_STATUS_FRONTEND_LIMIT;
    }
    resized = realloc(*values, (size_t)next * element_size);
    if (resized == NULL) return PIKA_STATUS_STORAGE_TOO_SMALL;
    memset((uint8_t*)resized + (size_t)(*capacity) * element_size,
           0,
           (size_t)(next - *capacity) * element_size);
    *values = resized;
    *capacity = next;
    return PIKA_STATUS_OK;
}

static PikaStatus graph_reserve_configured(
    PikaGraph* graph,
    const PikaGraphModule* module,
    uint32_t token_index,
    void** values,
    uint32_t* capacity,
    uint32_t required,
    uint32_t limit,
    size_t element_size,
    PikaFrontendBound bound) {
    PikaStatus status = graph_reserve(
        values, capacity, required, limit, element_size);
    if (status == PIKA_STATUS_FRONTEND_LIMIT) {
        return graph_fail_limit_at(
            graph, module, token_index, bound, limit, required);
    }
    return status;
}

static PikaStatus graph_add_name(PikaGraph* graph,
                                 const PikaGraphModule* module,
                                 PikaGraphName** names,
                                 uint32_t* count,
                                 uint32_t* capacity,
                                 uint32_t limit,
                                 uint32_t token_index,
                                 PikaFrontendBound bound) {
    PikaStatus status;
    if (graph_name_index(module, *names, *count, token_index) >= 0) {
        return PIKA_STATUS_OK;
    }
    status = graph_reserve_configured(
        graph, module, token_index,
        (void**)names, capacity, *count + 1u, limit,
        sizeof(**names), bound);
    if (status != PIKA_STATUS_OK) return status;
    status = graph_copy_token(
        module, token_index, (*names)[*count].text,
        sizeof((*names)[*count].text));
    if (status != PIKA_STATUS_OK) return status;
    (*names)[*count].token_index = token_index;
    ++*count;
    return PIKA_STATUS_OK;
}

static PikaStatus graph_analyze_function(PikaGraph* graph,
                                         PikaGraphModule* module,
                                         uint32_t def_index) {
    PikaGraphFunction* function;
    uint32_t index;
    uint32_t right_parenthesis;
    uint32_t indent_index;
    uint32_t depth;
    PikaStatus status;
    if (def_index + 2u >= module->token_count ||
        module->tokens[def_index + 1u].kind != PIKA_TOKEN_NAME ||
        module->tokens[def_index + 2u].kind !=
            PIKA_TOKEN_LEFT_PAREN) {
        return PIKA_STATUS_SYNTAX_ERROR;
    }
    status = graph_reserve_configured(
        graph, module, def_index,
        (void**)&module->functions,
        &module->function_capacity,
        module->function_count + 1u,
        PIKA_FRONTEND_FUNCTION_LIMIT,
        sizeof(*module->functions),
        PIKA_FRONTEND_BOUND_FUNCTIONS);
    if (status != PIKA_STATUS_OK) return status;
    function = &module->functions[module->function_count];
    memset(function, 0, sizeof(*function));
    status = graph_copy_token(
        module, def_index + 1u, function->name.text,
        sizeof(function->name.text));
    if (status != PIKA_STATUS_OK) return status;
    function->name.token_index = def_index + 1u;
    for (index = 0u; index < module->function_count; ++index) {
        if (strcmp(module->functions[index].name.text,
                   function->name.text) == 0) {
            return PIKA_STATUS_SYNTAX_ERROR;
        }
    }
    ++module->function_count;
    right_parenthesis = def_index + 3u;
    while (right_parenthesis < module->token_count &&
           module->tokens[right_parenthesis].kind !=
               PIKA_TOKEN_RIGHT_PAREN) {
        if (module->tokens[right_parenthesis].kind == PIKA_TOKEN_NAME) {
            status = graph_add_name(
                graph, module, &function->locals,
                &function->local_count,
                &function->local_capacity,
                PIKA_FRONTEND_SYMBOL_LIMIT, right_parenthesis,
                PIKA_FRONTEND_BOUND_SYMBOLS);
            if (status != PIKA_STATUS_OK) return status;
        }
        ++right_parenthesis;
    }
    if (right_parenthesis >= module->token_count) {
        return PIKA_STATUS_SYNTAX_ERROR;
    }
    indent_index = right_parenthesis + 1u;
    while (indent_index < module->token_count &&
           module->tokens[indent_index].kind != PIKA_TOKEN_INDENT) {
        ++indent_index;
    }
    if (indent_index >= module->token_count) {
        return PIKA_STATUS_SYNTAX_ERROR;
    }
    function->body_first = indent_index + 1u;
    depth = 1u;
    index = function->body_first;
    while (index < module->token_count && depth > 0u) {
        PikaTokenKind kind = module->tokens[index].kind;
        if (kind == PIKA_TOKEN_INDENT) {
            ++depth;
        } else if (kind == PIKA_TOKEN_DEDENT) {
            --depth;
        }
        if (depth > 0u && kind == PIKA_TOKEN_GLOBAL) {
            uint32_t cursor = index + 1u;
            while (cursor < module->token_count &&
                   module->tokens[cursor].kind != PIKA_TOKEN_NEWLINE) {
                if (module->tokens[cursor].kind == PIKA_TOKEN_NAME) {
                    status = graph_add_name(
                        graph, module, &function->declared_globals,
                        &function->declared_global_count,
                        &function->declared_global_capacity,
                        PIKA_FRONTEND_SYMBOL_LIMIT, cursor,
                        PIKA_FRONTEND_BOUND_SYMBOLS);
                    if (status != PIKA_STATUS_OK) return status;
                }
                ++cursor;
            }
        }
        if (depth > 0u && kind == PIKA_TOKEN_NAME &&
            graph_name_index(
                module, function->declared_globals,
                function->declared_global_count, index) < 0 &&
            (graph_assignment_statement(module, index)
#if PIKA_CAPABILITY_ASSIGNMENT_UNPACK_ENABLE
             || graph_unpack_assignment_target(module, index)
#endif
            )) {
            status = graph_add_name(
                graph, module, &function->locals,
                &function->local_count,
                &function->local_capacity,
                PIKA_FRONTEND_SYMBOL_LIMIT, index,
                PIKA_FRONTEND_BOUND_SYMBOLS);
            if (status != PIKA_STATUS_OK) return status;
        }
        if (depth > 0u && kind == PIKA_TOKEN_FOR &&
            index + 1u < module->token_count &&
            module->tokens[index + 1u].kind == PIKA_TOKEN_NAME &&
            graph_name_index(
                module, function->declared_globals,
                function->declared_global_count, index + 1u) < 0) {
            status = graph_add_name(
                graph, module, &function->locals,
                &function->local_count,
                &function->local_capacity,
                PIKA_FRONTEND_SYMBOL_LIMIT, index + 1u,
                PIKA_FRONTEND_BOUND_SYMBOLS);
            if (status != PIKA_STATUS_OK) return status;
        }
        ++index;
    }
    if (depth != 0u) return PIKA_STATUS_SYNTAX_ERROR;
    function->body_last = index - 1u;
    return PIKA_STATUS_OK;
}

static PikaStatus graph_analyze_module(PikaGraph* graph,
                                       PikaGraphModule* module) {
    PikaTokenBuffer buffer;
    uint32_t depth = 0u;
    uint32_t index;
    PikaStatus status;
    if (module->length > PIKA_FRONTEND_SOURCE_BYTE_LIMIT) {
        return graph_fail_limit(
            graph, PIKA_FRONTEND_BOUND_SOURCE_BYTES,
            PIKA_FRONTEND_SOURCE_BYTE_LIMIT,
            module->length > UINT32_MAX
                ? UINT32_MAX
                : (uint32_t)module->length);
    }
    status = pika_frontend_tokenize_allocated(
        module->source, module->length, &buffer,
        graph->diagnostic);
    if (status != PIKA_STATUS_OK) return status;
    module->tokens = buffer.tokens;
    module->token_count = buffer.count;
    for (index = 0u; index < module->token_count; ++index) {
        PikaTokenKind kind = module->tokens[index].kind;
        if (kind == PIKA_TOKEN_INDENT) {
            ++depth;
            continue;
        }
        if (kind == PIKA_TOKEN_DEDENT) {
            if (depth > 0u) --depth;
            continue;
        }
        if (kind == PIKA_TOKEN_FROM) {
            PikaGraphImport* import_entry;
            uint32_t cursor = index + 3u;
            uint32_t member_cursor;
            if (cursor >= module->token_count ||
                module->tokens[index + 1u].kind != PIKA_TOKEN_NAME ||
                module->tokens[index + 2u].kind !=
                    PIKA_TOKEN_IMPORT) {
                return graph_fail(
                    graph, PIKA_STATUS_SYNTAX_ERROR, module, index);
            }
            for (;;) {
                if (cursor >= module->token_count ||
                    module->tokens[cursor].kind != PIKA_TOKEN_NAME) {
                    return graph_fail(
                        graph, PIKA_STATUS_SYNTAX_ERROR,
                        module, index);
                }
                ++cursor;
                if (cursor >= module->token_count) {
                    return graph_fail(
                        graph, PIKA_STATUS_SYNTAX_ERROR,
                        module, index);
                }
                if (module->tokens[cursor].kind ==
                    PIKA_TOKEN_NEWLINE) {
                    break;
                }
                if (module->tokens[cursor].kind !=
                    PIKA_TOKEN_COMMA) {
                    return graph_fail(
                        graph, PIKA_STATUS_SYNTAX_ERROR,
                        module, cursor);
                }
                ++cursor;
            }
            status = graph_reserve_configured(
                graph, module, index,
                (void**)&module->imports,
                &module->import_capacity,
                module->import_count + 1u,
                PIKA_FRONTEND_SYMBOL_LIMIT,
                sizeof(*module->imports),
                PIKA_FRONTEND_BOUND_SYMBOLS);
            if (status != PIKA_STATUS_OK) {
                return graph_fail(graph, status, module, index);
            }
            import_entry =
                &module->imports[module->import_count++];
            memset(import_entry, 0, sizeof(*import_entry));
            status = graph_copy_token(
                module, index + 1u, import_entry->name.text,
                sizeof(import_entry->name.text));
            if (status != PIKA_STATUS_OK) {
                return graph_fail(
                    graph, status, module, index + 1u);
            }
            import_entry->name.token_index = index;
            import_entry->last_token = cursor - 1u;
            import_entry->top_level = depth == 0u ? 1u : 0u;
            import_entry->from_import = 1u;
            member_cursor = index + 3u;
            while (member_cursor < cursor) {
                status = graph_add_name(
                    graph, module, &import_entry->members,
                    &import_entry->member_count,
                    &import_entry->member_capacity,
                    PIKA_FRONTEND_SYMBOL_LIMIT, member_cursor,
                    PIKA_FRONTEND_BOUND_SYMBOLS);
                if (status != PIKA_STATUS_OK) {
                    return graph_fail(
                        graph, status, module, member_cursor);
                }
                ++member_cursor;
                if (member_cursor < cursor &&
                    module->tokens[member_cursor].kind ==
                        PIKA_TOKEN_COMMA) {
                    ++member_cursor;
                }
            }
            index = cursor;
            continue;
        }
        if (kind == PIKA_TOKEN_IMPORT) {
            PikaGraphImport* import_entry;
            if (index + 2u >= module->token_count ||
                module->tokens[index + 1u].kind != PIKA_TOKEN_NAME ||
                module->tokens[index + 2u].kind != PIKA_TOKEN_NEWLINE) {
                return graph_fail(
                    graph, PIKA_STATUS_SYNTAX_ERROR, module, index);
            }
            status = graph_reserve_configured(
                graph, module, index,
                (void**)&module->imports,
                &module->import_capacity,
                module->import_count + 1u,
                PIKA_FRONTEND_SYMBOL_LIMIT,
                sizeof(*module->imports),
                PIKA_FRONTEND_BOUND_SYMBOLS);
            if (status != PIKA_STATUS_OK) {
                return graph_fail(graph, status, module, index);
            }
            import_entry = &module->imports[module->import_count++];
            memset(import_entry, 0, sizeof(*import_entry));
            status = graph_copy_token(
                module, index + 1u, import_entry->name.text,
                sizeof(import_entry->name.text));
            if (status != PIKA_STATUS_OK) {
                return graph_fail(graph, status, module, index + 1u);
            }
            import_entry->name.token_index = index;
            import_entry->last_token = index + 1u;
            import_entry->top_level = depth == 0u ? 1u : 0u;
        }
        if (depth == 0u && kind == PIKA_TOKEN_DEF) {
            status = graph_analyze_function(graph, module, index);
            if (status != PIKA_STATUS_OK) {
                return graph_fail(graph, status, module, index);
            }
        }
        if (depth == 0u && kind == PIKA_TOKEN_CLASS) {
            if (index + 1u >= module->token_count ||
                module->tokens[index + 1u].kind != PIKA_TOKEN_NAME) {
                return graph_fail(
                    graph, PIKA_STATUS_SYNTAX_ERROR, module, index);
            }
            status = graph_add_name(
                graph, module, &module->classes,
                &module->class_count,
                &module->class_capacity,
                PIKA_FRONTEND_CLASS_LIMIT, index + 1u,
                PIKA_FRONTEND_BOUND_CLASSES);
            if (status != PIKA_STATUS_OK) {
                return graph_fail(graph, status, module, index + 1u);
            }
        }
        if (depth == 0u && kind == PIKA_TOKEN_NAME &&
            (graph_assignment_statement(module, index)
#if PIKA_CAPABILITY_ASSIGNMENT_UNPACK_ENABLE
             || graph_unpack_assignment_target(module, index)
#endif
            )) {
            status = graph_add_name(
                graph, module, &module->globals,
                &module->global_count,
                &module->global_capacity,
                PIKA_FRONTEND_SYMBOL_LIMIT, index,
                PIKA_FRONTEND_BOUND_SYMBOLS);
            if (status != PIKA_STATUS_OK) {
                return graph_fail(graph, status, module, index);
            }
        }
    }
    for (index = 0u; index < module->global_count; ++index) {
        uint32_t function_index;
        for (function_index = 0u;
             function_index < module->function_count;
             ++function_index) {
            if (strcmp(module->globals[index].text,
                       module->functions[function_index].name.text) == 0) {
                return graph_fail(
                    graph, PIKA_STATUS_UNSUPPORTED_SYNTAX, module,
                    module->globals[index].token_index);
            }
        }
    }
    return PIKA_STATUS_OK;
}

static int graph_find_module(const PikaGraph* graph,
                             const char* name) {
    uint32_t index;
    for (index = 0u; index < graph->module_count; ++index) {
        if (strcmp(graph->modules[index]->name, name) == 0) {
            return (int)index;
        }
    }
    return -1;
}

static int graph_base_import(
    const PikaGraph* graph,
    const char* name,
    uint8_t* target_module) {
    uint16_t index;
    size_t length;
    if (graph->base == NULL || name == NULL) return 0;
    length = strlen(name);
    for (index = 0u;
         index < graph->base->semantic_python_import_count;
         ++index) {
        const PikaPythonModule* imported =
            &graph->base->semantic_python_imports[index];
        if (imported->name_length == length &&
            imported->name_offset <=
                graph->base->semantic_name_count &&
            imported->name_length <=
                graph->base->semantic_name_count -
                    imported->name_offset &&
            memcmp(
                &graph->base->semantic_names[
                    imported->name_offset],
                name, length) == 0) {
            if (target_module != NULL) {
                *target_module = imported->target_module;
            }
            return 1;
        }
    }
    return 0;
}

static int graph_base_program_module(
    const PikaGraph* graph, uint8_t module_index) {
    uint16_t index;
    if (graph->base == NULL) return 0;
    for (index = 0u;
         index < graph->base->program_module_count;
         ++index) {
        if (graph->base->program_modules[index].module_index ==
            module_index) {
            return 1;
        }
    }
    return 0;
}

static PikaStatus graph_load_module(PikaGraph* graph,
                                    const char* name,
                                    uint8_t* module_index) {
    int existing = graph_find_module(graph, name);
    PikaGraphModule* module;
    const char* source = NULL;
    size_t length = 0u;
    PikaStatus status;
    if (existing >= 0) {
        *module_index = (uint8_t)existing;
        return PIKA_STATUS_OK;
    }
    if (!graph_identifier_valid(name)) {
        return PIKA_STATUS_SYNTAX_ERROR;
    }
    status = graph->provider->load(
        graph->provider->context, name, &source, &length);
    if (status != PIKA_STATUS_OK) {
        if (status == PIKA_STATUS_FRONTEND_LIMIT &&
            length > PIKA_FRONTEND_SOURCE_BYTE_LIMIT) {
            return graph_fail_limit(
                graph, PIKA_FRONTEND_BOUND_SOURCE_BYTES,
                PIKA_FRONTEND_SOURCE_BYTE_LIMIT,
                length > UINT32_MAX
                    ? UINT32_MAX
                    : (uint32_t)length);
        }
        return status;
    }
    if (source == NULL) return PIKA_STATUS_IO_ERROR;
    if (graph->module_count >= PIKA_FRONTEND_MODULE_LIMIT ||
        (graph->module_count > 0u &&
         graph->module_prefix_base >
             PIKA_FRONTEND_MODULE_LIMIT - 1u -
                 graph->module_count)) {
        if (graph->provider->release != NULL) {
            graph->provider->release(
                graph->provider->context, source);
        }
        return graph_fail_limit(
            graph, PIKA_FRONTEND_BOUND_MODULES,
            PIKA_FRONTEND_MODULE_LIMIT,
            graph->module_count + 1u);
    }
    module = (PikaGraphModule*)calloc(1u, sizeof(*module));
    if (module == NULL) {
        if (graph->provider->release != NULL) {
            graph->provider->release(
                graph->provider->context, source);
        }
        return PIKA_STATUS_FRONTEND_LIMIT;
    }
    graph->modules[graph->module_count] = module;
    if (strlen(name) + 1u > sizeof(module->name)) {
        if (graph->provider->release != NULL) {
            graph->provider->release(
                graph->provider->context, source);
        }
        free(module);
        graph->modules[graph->module_count] = NULL;
        return PIKA_STATUS_FRONTEND_LIMIT;
    }
    strcpy(module->name, name);
    module->source = source;
    module->length = length;
    *module_index = (uint8_t)graph->module_count++;
    status = graph_analyze_module(graph, module);
    if (status != PIKA_STATUS_OK) {
        graph_set_diagnostic_source(graph, module);
    }
    PIKA_FRONTEND_MEMORY_CHECKPOINT(
        "frontend-graph-module-analyzed",
        graph->module_count);
    return status;
}

static PikaStatus graph_native_module(const PikaGraph* graph,
                                      const char* name,
                                      int* found) {
#if PIKA_BINDING_RUNTIME_ENABLE
    const PikaBindingModule* module = NULL;
    PikaStatus status;
#endif
    *found = 0;
#if PIKA_BINDING_RUNTIME_ENABLE
    if (graph->config->bindings == NULL) return PIKA_STATUS_OK;
    status = pika_binding_find_module(
        graph->config->bindings, name, &module);
    if (status == PIKA_STATUS_OK) {
        *found = 1;
        return PIKA_STATUS_OK;
    }
    if (status == PIKA_STATUS_MODULE_NOT_FOUND) return PIKA_STATUS_OK;
    return status;
#else
    (void)graph;
    (void)name;
    return PIKA_STATUS_OK;
#endif
}

static int graph_module_has_member(
    const PikaGraphModule* module,
    const char* name) {
    uint32_t index;
    for (index = 0u; index < module->function_count; ++index) {
        if (strcmp(module->functions[index].name.text, name) == 0) {
            return 1;
        }
    }
    for (index = 0u; index < module->class_count; ++index) {
        if (strcmp(module->classes[index].text, name) == 0) {
            return 1;
        }
    }
    for (index = 0u; index < module->global_count; ++index) {
        if (strcmp(module->globals[index].text, name) == 0) {
            return 1;
        }
    }
    return 0;
}

static PikaStatus graph_resolve_module(PikaGraph* graph,
                                       uint8_t module_index) {
    PikaGraphModule* module = graph->modules[module_index];
    uint32_t index;
    PikaStatus status;
    if (module->state == 2u) return PIKA_STATUS_OK;
    if (module->state == 1u) return PIKA_STATUS_OK;
    module->state = 1u;
    for (index = 0u; index < module->import_count; ++index) {
        PikaGraphImport* import_entry = &module->imports[index];
        uint8_t target = 0u;
        int native_found = 0;
        status = graph_native_module(
            graph, import_entry->name.text, &native_found);
        if (status != PIKA_STATUS_OK) {
            return graph_fail(
                graph, status, module,
                import_entry->name.token_index);
        }
        if (native_found && import_entry->name.text[0] == '_') {
            import_entry->is_binding = 1u;
            continue;
        }
        if (graph_base_import(
                graph, import_entry->name.text, &target)) {
            import_entry->is_python = 1u;
            import_entry->target_module = target;
            import_entry->from_base = 1u;
            import_entry->is_program_image =
                graph_base_program_module(graph, target) ? 1u : 0u;
            continue;
        }
        status = graph_load_module(
            graph, import_entry->name.text, &target);
        if (status == PIKA_STATUS_OK) {
            if (native_found) {
                return graph_fail(
                    graph, PIKA_STATUS_MODULE_CONFLICT, module,
                    import_entry->name.token_index);
            }
            if (import_entry->top_level == 0u &&
                graph->build_program_image == 0u) {
                return graph_fail(
                    graph, PIKA_STATUS_UNSUPPORTED_SYNTAX, module,
                    import_entry->name.token_index);
            }
            import_entry->is_python = 1u;
            import_entry->target_module = target;
            if (import_entry->from_import != 0u) {
                uint32_t member_index;
                for (member_index = 0u;
                     member_index < import_entry->member_count;
                     ++member_index) {
                    if (!graph_module_has_member(
                            graph->modules[target],
                            import_entry->members[member_index].text)) {
                        return graph_fail(
                            graph, PIKA_STATUS_MEMBER_NOT_FOUND,
                            module,
                            import_entry->members[member_index]
                                .token_index);
                    }
                }
            }
            status = graph_resolve_module(graph, target);
            if (status != PIKA_STATUS_OK) return status;
        } else if (status == PIKA_STATUS_MODULE_NOT_FOUND) {
            if (!native_found) {
                return graph_fail(
                    graph, PIKA_STATUS_MODULE_NOT_FOUND, module,
                    import_entry->name.token_index);
            }
            import_entry->is_binding = 1u;
        } else {
            return graph_fail(
                graph, status, module,
                import_entry->name.token_index);
        }
    }
    module->state = 2u;
    if (graph->topology_count >= PIKA_FRONTEND_MODULE_LIMIT) {
        return graph_fail_limit(
            graph, PIKA_FRONTEND_BOUND_MODULES,
            PIKA_FRONTEND_MODULE_LIMIT,
            graph->topology_count + 1u);
    }
    module->topology_order = (uint8_t)graph->topology_count;
    graph->topology[graph->topology_count++] = module_index;
    return PIKA_STATUS_OK;
}

static PikaStatus graph_buffer_reserve(PikaGraphBuffer* buffer,
                                       size_t additional) {
    size_t required;
    size_t capacity;
    char* data;
    if (additional > SIZE_MAX - buffer->length) {
        return PIKA_STATUS_FRONTEND_LIMIT;
    }
    required = buffer->length + additional;
    if (required <= buffer->capacity) return PIKA_STATUS_OK;
    capacity = buffer->capacity == 0u ? 256u : buffer->capacity;
    while (capacity < required) {
        if (capacity > SIZE_MAX / 2u) {
            capacity = required;
            break;
        }
        capacity *= 2u;
    }
    data = (char*)realloc(buffer->data, capacity);
    if (data == NULL) return PIKA_STATUS_FRONTEND_LIMIT;
    buffer->data = data;
    buffer->capacity = capacity;
    return PIKA_STATUS_OK;
}

static PikaStatus graph_buffer_append(PikaGraphBuffer* buffer,
                                      const char* data,
                                      size_t length) {
    if (buffer->count_only != 0u) {
        if (length > SIZE_MAX - buffer->length) {
            return PIKA_STATUS_FRONTEND_LIMIT;
        }
        buffer->length += length;
        return PIKA_STATUS_OK;
    }
    PikaStatus status = graph_buffer_reserve(buffer, length);
    if (status != PIKA_STATUS_OK) return status;
    if (length > 0u) {
        memcpy(&buffer->data[buffer->length], data, length);
        buffer->length += length;
    }
    return PIKA_STATUS_OK;
}

static PikaStatus graph_buffer_text(PikaGraphBuffer* buffer,
                                    const char* text) {
    return graph_buffer_append(buffer, text, strlen(text));
}

static PikaStatus graph_capture_python_modules(
    PikaGraph* graph,
    uint8_t entry_index,
    PikaGraphBuffer* buffer,
    PikaGraphRetainedImport* retained,
    uint16_t* retained_count) {
    const PikaGraphModule* entry = graph->modules[entry_index];
    uint32_t module_index;
    *retained_count = 0u;
    for (module_index = 0u;
         module_index < graph->module_count;
         ++module_index) {
        const PikaGraphModule* imported =
            graph->modules[module_index];
        size_t name_length;
        PikaStatus status;
        if (module_index == entry_index) continue;
        name_length = strlen(imported->name);
        if (*retained_count >= PIKA_FRONTEND_MODULE_LIMIT ||
            name_length > UINT16_MAX ||
            buffer->length > UINT32_MAX - name_length) {
            return graph_fail(
                graph, PIKA_STATUS_FRONTEND_LIMIT,
                entry, 0u);
        }
        retained[*retained_count].buffer_offset =
            (uint32_t)buffer->length;
        retained[*retained_count].name_length =
            (uint16_t)name_length;
        retained[*retained_count].target_module =
            (uint8_t)module_index;
        retained[*retained_count].flags =
            imported->publicly_available != 0u
                ? PIKA_PYTHON_MODULE_PUBLIC
                : 0u;
        retained[*retained_count].target_is_stable = 0u;
        status = graph_buffer_append(
            buffer, imported->name, name_length);
        if (status != PIKA_STATUS_OK) return status;
        ++*retained_count;
    }
    for (module_index = 0u;
         module_index < entry->import_count;
         ++module_index) {
        const PikaGraphImport* imported =
            &entry->imports[module_index];
        size_t name_length;
        PikaStatus status;
        if (imported->is_python == 0u ||
            imported->from_base == 0u) {
            continue;
        }
        name_length = strlen(imported->name.text);
        if (*retained_count >= PIKA_FRONTEND_MODULE_LIMIT ||
            name_length > UINT16_MAX ||
            buffer->length > UINT32_MAX - name_length) {
            return graph_fail(
                graph, PIKA_STATUS_FRONTEND_LIMIT,
                entry, imported->name.token_index);
        }
        retained[*retained_count].buffer_offset =
            (uint32_t)buffer->length;
        retained[*retained_count].name_length =
            (uint16_t)name_length;
        retained[*retained_count].target_module =
            imported->target_module;
        retained[*retained_count].flags =
            PIKA_PYTHON_MODULE_PUBLIC;
        retained[*retained_count].target_is_stable = 1u;
        status = graph_buffer_append(
            buffer, imported->name.text, name_length);
        if (status != PIKA_STATUS_OK) return status;
        ++*retained_count;
    }
    return PIKA_STATUS_OK;
}

static uint8_t graph_program_module_index(
    uint8_t module_prefix_base,
    uint8_t local_index) {
    return local_index == 0u
               ? 0u
               : (uint8_t)(
                     module_prefix_base + local_index);
}

static PikaStatus graph_program_initializer_name(
    uint8_t module_index, char* output, size_t capacity) {
    int written = snprintf(
        output, capacity,
        "__pika_program_image_module_initialize_%u",
        (unsigned)module_index);
    return written < 0 || (size_t)written >= capacity
               ? PIKA_STATUS_FRONTEND_LIMIT
               : PIKA_STATUS_OK;
}

static PikaStatus graph_retain_python_imports(
    uint8_t module_prefix_base,
    PikaCompiledModule* module,
    const PikaGraphBuffer* buffer,
    const PikaGraphRetainedImport* retained,
    uint16_t retained_count) {
    uint32_t required = module->semantic_name_count;
    uint8_t duplicate[PIKA_FRONTEND_MODULE_LIMIT];
    uint16_t output_count =
        module->semantic_python_import_count;
    uint16_t index;
    char* names;
    memset(duplicate, 0, sizeof(duplicate));
    for (index = 0u; index < retained_count; ++index) {
        uint16_t existing_index;
        uint8_t target = retained[index].target_is_stable != 0u
                             ? retained[index].target_module
                             : graph_program_module_index(
                                   module_prefix_base,
                                   retained[index].target_module);
        if (retained[index].name_length > UINT32_MAX - required ||
            retained[index].buffer_offset > buffer->length ||
            retained[index].name_length >
                buffer->length - retained[index].buffer_offset) {
            return PIKA_STATUS_FRONTEND_LIMIT;
        }
        for (existing_index = 0u;
             existing_index <
                 module->semantic_python_import_count;
             ++existing_index) {
            PikaPythonModule* existing =
                &module->semantic_python_imports[existing_index];
            if (existing->name_length ==
                    retained[index].name_length &&
                existing->name_offset <=
                    module->semantic_name_count &&
                existing->name_length <=
                    module->semantic_name_count -
                        existing->name_offset &&
                memcmp(
                    &module->semantic_names[
                        existing->name_offset],
                    &buffer->data[
                        retained[index].buffer_offset],
                    retained[index].name_length) == 0) {
                if (existing->target_module != target) {
                    return PIKA_STATUS_MODULE_CONFLICT;
                }
                existing->flags |= retained[index].flags;
                duplicate[index] = 1u;
                break;
            }
        }
        if (duplicate[index] != 0u) continue;
        if (output_count >= PIKA_FRONTEND_MODULE_LIMIT) {
            return PIKA_STATUS_FRONTEND_LIMIT;
        }
        ++output_count;
        required += retained[index].name_length;
    }
    if (required > module->semantic_name_capacity) {
        names = (char*)realloc(module->semantic_names, required);
        if (names == NULL) return PIKA_STATUS_FRONTEND_LIMIT;
        module->semantic_names = names;
        module->semantic_name_capacity = required;
    }
    if (output_count >
        module->semantic_python_import_capacity) {
        PikaPythonModule* imports =
            (PikaPythonModule*)realloc(
                module->semantic_python_imports,
                (size_t)output_count *
                    sizeof(*module->semantic_python_imports));
        if (imports == NULL) return PIKA_STATUS_FRONTEND_LIMIT;
        memset(
            &imports[module->semantic_python_import_capacity], 0,
            (size_t)(output_count -
                     module->semantic_python_import_capacity) *
                sizeof(*imports));
        module->semantic_python_imports = imports;
        module->semantic_python_import_capacity = output_count;
    }
    output_count = module->semantic_python_import_count;
    for (index = 0u; index < retained_count; ++index) {
        uint8_t target;
        PikaPythonModule* semantic;
        if (duplicate[index] != 0u) continue;
        semantic =
            &module->semantic_python_imports[output_count];
        target = retained[index].target_is_stable != 0u
                     ? retained[index].target_module
                     : graph_program_module_index(
                           module_prefix_base,
                           retained[index].target_module);
        semantic->name_offset = module->semantic_name_count;
        semantic->name_length = retained[index].name_length;
        semantic->target_module = target;
        semantic->flags = retained[index].flags;
        memcpy(
            &module->semantic_names[module->semantic_name_count],
            &buffer->data[retained[index].buffer_offset],
            retained[index].name_length);
        module->semantic_name_count += retained[index].name_length;
        ++output_count;
    }
    module->semantic_python_import_count = output_count;
    return PIKA_STATUS_OK;
}

typedef struct {
    uint8_t module_prefix_base;
    PikaGraphBuffer* buffer;
    const PikaGraphRetainedImport* retained;
    uint16_t retained_count;
} PikaGraphPrepareContext;

static PikaStatus graph_prepare_python_imports(
    void* context,
    PikaCompiledModule* module) {
    PikaGraphPrepareContext* prepare =
        (PikaGraphPrepareContext*)context;
    PikaStatus status = graph_retain_python_imports(
        prepare->module_prefix_base, module,
        prepare->buffer, prepare->retained,
        prepare->retained_count);
    free(prepare->buffer->data);
    prepare->buffer->data = NULL;
    prepare->buffer->length = 0u;
    prepare->buffer->capacity = 0u;
    PIKA_FRONTEND_MEMORY_CHECKPOINT(
        "frontend-rewrite-released",
        prepare->retained_count);
    return status;
}

static PikaStatus graph_seed_python_imports(
    void* context,
    PikaCompiledModule* module) {
    PikaGraphPrepareContext* prepare =
        (PikaGraphPrepareContext*)context;
    uint32_t additional = 0u;
    uint16_t index;
    char* names;
    for (index = 0u; index < prepare->retained_count; ++index) {
        if (prepare->retained[index].name_length >
            UINT32_MAX - additional) {
            return PIKA_STATUS_FRONTEND_LIMIT;
        }
        additional += prepare->retained[index].name_length;
    }
    if (additional >
        UINT32_MAX - module->semantic_name_capacity) {
        return PIKA_STATUS_FRONTEND_LIMIT;
    }
    names = (char*)realloc(
        module->semantic_names,
        module->semantic_name_capacity + additional);
    if (names == NULL) return PIKA_STATUS_FRONTEND_LIMIT;
    module->semantic_names = names;
    module->semantic_name_capacity += additional;
    return graph_retain_python_imports(
        prepare->module_prefix_base, module,
        prepare->buffer, prepare->retained,
        prepare->retained_count);
}

static PikaStatus graph_prefixed_name(const PikaGraph* graph,
                                      uint8_t module_index,
                                      const char* name,
                                      char* output,
                                      size_t capacity) {
    int written;
    if (module_index == 0u) {
        written = snprintf(output, capacity, "%s", name);
    } else {
        written = snprintf(
            output, capacity, "__p%u_%s",
            (unsigned int)graph_program_module_index(
                graph->module_prefix_base, module_index),
            name);
    }
    if (written < 0 || (size_t)written >= capacity) {
        return PIKA_STATUS_FRONTEND_LIMIT;
    }
    return PIKA_STATUS_OK;
}

static const PikaGraphFunction* graph_function_by_token(
    const PikaGraphModule* module,
    uint32_t token_index) {
    uint32_t index;
    for (index = 0u; index < module->function_count; ++index) {
        if (module->functions[index].name.token_index == token_index) {
            return &module->functions[index];
        }
    }
    return NULL;
}

static const PikaGraphFunction* graph_function_by_name(
    const PikaGraphModule* module,
    uint32_t token_index) {
    uint32_t index;
    for (index = 0u; index < module->function_count; ++index) {
        if (graph_token_equals(
                module, token_index,
                module->functions[index].name.text)) {
            return &module->functions[index];
        }
    }
    return NULL;
}

static const PikaGraphName* graph_class_by_name(
    const PikaGraphModule* module,
    uint32_t token_index) {
    uint32_t index;
    for (index = 0u; index < module->class_count; ++index) {
        if (graph_token_equals(
                module, token_index, module->classes[index].text)) {
            return &module->classes[index];
        }
    }
    return NULL;
}

static const PikaGraphFunction* graph_function_scope(
    const PikaGraphModule* module,
    uint32_t token_index) {
    uint32_t index;
    for (index = 0u; index < module->function_count; ++index) {
        const PikaGraphFunction* function = &module->functions[index];
        if (token_index >= function->body_first &&
            token_index < function->body_last) {
            return function;
        }
    }
    return NULL;
}

static int graph_local_name(const PikaGraphModule* module,
                            const PikaGraphFunction* function,
                            uint32_t token_index) {
    if (function == NULL) return 0;
    return graph_name_index(
               module, function->locals, function->local_count,
               token_index) >= 0;
}

static int graph_declared_global_name(
    const PikaGraphModule* module,
    const PikaGraphFunction* function,
    uint32_t token_index) {
    if (function == NULL) return 0;
    return graph_name_index(
               module, function->declared_globals,
               function->declared_global_count, token_index) >= 0;
}

static int graph_attribute_member(const PikaGraphModule* module,
                                  uint32_t token_index) {
    return token_index > 0u &&
           module->tokens[token_index - 1u].kind == PIKA_TOKEN_DOT;
}

static int graph_class_base_reference(const PikaGraphModule* module,
                                      uint32_t token_index) {
    if (token_index + 1u >= module->token_count ||
        module->tokens[token_index + 1u].kind !=
            PIKA_TOKEN_RIGHT_PAREN) {
        return 0;
    }
    if (token_index >= 3u &&
        module->tokens[token_index - 3u].kind == PIKA_TOKEN_CLASS &&
        module->tokens[token_index - 2u].kind == PIKA_TOKEN_NAME &&
        module->tokens[token_index - 1u].kind ==
            PIKA_TOKEN_LEFT_PAREN) {
        return 1;
    }
    return token_index >= 5u &&
           module->tokens[token_index - 5u].kind ==
               PIKA_TOKEN_CLASS &&
           module->tokens[token_index - 4u].kind == PIKA_TOKEN_NAME &&
           module->tokens[token_index - 3u].kind ==
               PIKA_TOKEN_LEFT_PAREN &&
           module->tokens[token_index - 2u].kind == PIKA_TOKEN_NAME &&
           module->tokens[token_index - 1u].kind == PIKA_TOKEN_DOT;
}

static const PikaGraphImport* graph_import_at(
    const PikaGraphModule* module,
    uint32_t token_index) {
    uint32_t index;
    for (index = 0u; index < module->import_count; ++index) {
        if (module->imports[index].name.token_index == token_index) {
            return &module->imports[index];
        }
    }
    return NULL;
}

static const PikaGraphImport* graph_python_import_name(
    const PikaGraphModule* module,
    uint32_t token_index) {
    uint32_t index;
    for (index = 0u; index < module->import_count; ++index) {
        const PikaGraphImport* import_entry = &module->imports[index];
        if ((import_entry->is_python != 0u ||
             import_entry->is_program_image != 0u) &&
            import_entry->from_import == 0u &&
            graph_token_equals(
                module, token_index, import_entry->name.text)) {
            return import_entry;
        }
    }
    return NULL;
}

static const PikaGraphImport* graph_python_selected_import(
    const PikaGraphModule* module,
    uint32_t token_index) {
    uint32_t import_index = module->import_count;
    while (import_index > 0u) {
        const PikaGraphImport* import_entry;
        uint32_t member_index;
        --import_index;
        import_entry = &module->imports[import_index];
        if ((import_entry->is_python == 0u &&
             import_entry->is_program_image == 0u) ||
            import_entry->from_import == 0u) {
            continue;
        }
        for (member_index = 0u;
             member_index < import_entry->member_count;
             ++member_index) {
            if (graph_token_equals(
                    module, token_index,
                    import_entry->members[member_index].text)) {
                return import_entry;
            }
        }
    }
    return NULL;
}

static const char* graph_binding_selected_member(
    const PikaGraphModule* source,
    uint32_t token_index,
    const PikaGraphModule* target) {
    uint32_t import_index;
    for (import_index = 0u;
         import_index < target->import_count;
         ++import_index) {
        const PikaGraphImport* import_entry =
            &target->imports[import_index];
        uint32_t member_index;
        if (import_entry->is_binding == 0u ||
            import_entry->from_import == 0u) {
            continue;
        }
        for (member_index = 0u;
             member_index < import_entry->member_count;
             ++member_index) {
            if (graph_token_equals(
                    source, token_index,
                    import_entry->members[member_index].text)) {
                return import_entry->members[member_index].text;
            }
        }
    }
    return NULL;
}

static const PikaGraphFunction* graph_member_function(
    const PikaGraphModule* source,
    uint32_t token_index,
    const PikaGraphModule* target) {
    uint32_t index;
    for (index = 0u; index < target->function_count; ++index) {
        if (graph_token_equals(
                source, token_index,
                target->functions[index].name.text)) {
            return &target->functions[index];
        }
    }
    return NULL;
}

static const PikaGraphName* graph_member_class(
    const PikaGraphModule* source,
    uint32_t token_index,
    const PikaGraphModule* target) {
    uint32_t index;
    for (index = 0u; index < target->class_count; ++index) {
        if (graph_token_equals(
                source, token_index, target->classes[index].text)) {
            return &target->classes[index];
        }
    }
    return NULL;
}

static int graph_member_global(const PikaGraphModule* source,
                               uint32_t token_index,
                               const PikaGraphModule* target) {
    return graph_name_index(
        source, target->globals, target->global_count, token_index);
}

static int graph_merge_hidden(PikaGraphFunction* destination,
                              const PikaGraphFunction* source) {
    uint32_t module_index;
    int changed = 0;
    for (module_index = 0u;
         module_index < PIKA_FRONTEND_MODULE_LIMIT;
         ++module_index) {
        uint64_t previous =
            destination->hidden_globals[module_index];
        destination->hidden_globals[module_index] |=
            source->hidden_globals[module_index];
        if (destination->hidden_globals[module_index] != previous) {
            changed = 1;
        }
    }
    return changed;
}

static int graph_collect_function_hidden(
    PikaGraph* graph,
    uint8_t module_index,
    PikaGraphFunction* function) {
    PikaGraphModule* module = graph->modules[module_index];
    uint32_t index;
    int changed = 0;
    for (index = function->body_first;
         index < function->body_last;
         ++index) {
        const PikaGraphImport* imported;
        const PikaGraphFunction* called;
        int global_index;
        if (module->tokens[index].kind != PIKA_TOKEN_NAME ||
            graph_local_name(module, function, index) ||
            graph_declared_global_name(module, function, index) ||
            graph_attribute_member(module, index)) {
            continue;
        }
        imported = graph_python_import_name(module, index);
        if (imported != NULL && index + 2u < module->token_count &&
            module->tokens[index + 1u].kind == PIKA_TOKEN_DOT &&
            module->tokens[index + 2u].kind == PIKA_TOKEN_NAME) {
            PikaGraphModule* target =
                graph->modules[imported->target_module];
            global_index = graph_member_global(
                module, index + 2u, target);
            if (global_index >= 0) {
                uint64_t mask = (uint64_t)1u << (uint32_t)global_index;
                uint64_t* globals =
                    &function->hidden_globals[imported->target_module];
                if ((*globals & mask) == 0u) {
                    *globals |= mask;
                    changed = 1;
                }
            }
            called = graph_member_function(
                module, index + 2u, target);
            if (called != NULL) {
                changed |= graph_merge_hidden(function, called);
            }
            index += 2u;
            continue;
        }
        imported = graph_python_selected_import(module, index);
        if (imported != NULL) {
            PikaGraphModule* target =
                graph->modules[imported->target_module];
            global_index = graph_member_global(
                module, index, target);
            if (global_index >= 0) {
                uint64_t mask =
                    (uint64_t)1u << (uint32_t)global_index;
                uint64_t* globals =
                    &function->hidden_globals[
                        imported->target_module];
                if ((*globals & mask) == 0u) {
                    *globals |= mask;
                    changed = 1;
                }
            }
            called = graph_member_function(module, index, target);
            if (called != NULL) {
                changed |= graph_merge_hidden(function, called);
            }
            continue;
        }
        if (index + 1u < module->token_count &&
            module->tokens[index + 1u].kind ==
                PIKA_TOKEN_LEFT_PAREN) {
            called = graph_function_by_name(module, index);
            if (called != NULL) {
                changed |= graph_merge_hidden(function, called);
                continue;
            }
        }
        global_index = graph_name_index(
            module, module->globals, module->global_count, index);
        if (global_index >= 0) {
            uint64_t mask = (uint64_t)1u << (uint32_t)global_index;
            uint64_t* globals =
                &function->hidden_globals[module_index];
            if ((*globals & mask) == 0u) {
                *globals |= mask;
                changed = 1;
            }
        }
    }
    return changed;
}

static void graph_resolve_function_hidden(PikaGraph* graph) {
    int changed;
    do {
        uint32_t module_index;
        changed = 0;
        for (module_index = 0u;
             module_index < graph->module_count;
             ++module_index) {
            PikaGraphModule* module = graph->modules[module_index];
            uint32_t function_index;
            for (function_index = 0u;
                 function_index < module->function_count;
                 ++function_index) {
                changed |= graph_collect_function_hidden(
                    graph, (uint8_t)module_index,
                    &module->functions[function_index]);
            }
        }
    } while (changed != 0);
}

static PikaStatus graph_append_hidden(PikaGraph* graph,
                                      PikaGraphBuffer* buffer,
                                      const PikaGraphFunction* function,
                                      int original_arguments) {
    uint32_t order;
    uint32_t written = 0u;
    PikaStatus status;
    for (order = 0u; order < graph->topology_count; ++order) {
        uint8_t current = graph->topology[order];
        const PikaGraphModule* module = graph->modules[current];
        uint32_t global_index;
        for (global_index = 0u;
             global_index < module->global_count;
             ++global_index) {
            char name[PIKA_FRONTEND_SYMBOL_LIMIT];
            if ((function->hidden_globals[current] &
                 ((uint64_t)1u << global_index)) == 0u) {
                continue;
            }
            if (written > 0u) {
                status = graph_buffer_text(buffer, ", ");
                if (status != PIKA_STATUS_OK) return status;
            }
            status = graph_prefixed_name(
                graph, current,
                module->globals[global_index].text,
                name, sizeof(name));
            if (status != PIKA_STATUS_OK) return status;
            status = graph_buffer_text(buffer, name);
            if (status != PIKA_STATUS_OK) return status;
            ++written;
        }
    }
    if (written > 0u && original_arguments) {
        return graph_buffer_text(buffer, ", ");
    }
    return PIKA_STATUS_OK;
}

static int graph_has_hidden(
    const PikaGraph* graph,
    const PikaGraphFunction* function) {
    uint32_t module_index;
    for (module_index = 0u;
         module_index < graph->module_count;
         ++module_index) {
        if (function->hidden_globals[module_index] != 0u) {
            return 1;
        }
    }
    return 0;
}

static PikaStatus graph_append_callable(
    PikaGraph* graph,
    PikaGraphBuffer* buffer,
    const PikaGraphModule* source_module,
    size_t* cursor,
    uint32_t start_token,
    uint32_t name_end_token,
    uint32_t left_parenthesis_token,
    uint8_t owner_module,
    const PikaGraphFunction* function) {
    const PikaToken* start = &source_module->tokens[start_token];
    const PikaToken* name_end =
        &source_module->tokens[name_end_token];
    const PikaToken* left_parenthesis =
        &source_module->tokens[left_parenthesis_token];
    char replacement[PIKA_FRONTEND_SYMBOL_LIMIT];
    size_t left_end =
        (size_t)left_parenthesis->offset + left_parenthesis->length;
    int original_arguments =
        left_parenthesis_token + 1u < source_module->token_count &&
        source_module->tokens[left_parenthesis_token + 1u].kind !=
            PIKA_TOKEN_RIGHT_PAREN;
    PikaStatus status = graph_buffer_append(
        buffer, &source_module->source[*cursor],
        (size_t)start->offset - *cursor);
    if (status != PIKA_STATUS_OK) return status;
    status = graph_prefixed_name(
        graph, owner_module, function->name.text,
        replacement, sizeof(replacement));
    if (status != PIKA_STATUS_OK) return status;
    status = graph_buffer_text(buffer, replacement);
    if (status != PIKA_STATUS_OK) return status;
    status = graph_buffer_append(
        buffer,
        &source_module->source[
            (size_t)name_end->offset + name_end->length],
        left_end -
            ((size_t)name_end->offset + name_end->length));
    if (status != PIKA_STATUS_OK) return status;
    if (graph->build_program_image == 0u) {
        status = graph_append_hidden(
            graph, buffer, function, original_arguments);
    }
    if (status != PIKA_STATUS_OK) return status;
    *cursor = left_end;
    return PIKA_STATUS_OK;
}

static PikaStatus graph_replace_name(PikaGraph* graph,
                                     PikaGraphBuffer* buffer,
                                     const PikaGraphModule* module,
                                     size_t* cursor,
                                     uint32_t start_token,
                                     uint32_t end_token,
                                     uint8_t owner_module,
                                     const char* name) {
    const PikaToken* start = &module->tokens[start_token];
    const PikaToken* end = &module->tokens[end_token];
    char replacement[PIKA_FRONTEND_SYMBOL_LIMIT];
    PikaStatus status = graph_buffer_append(
        buffer, &module->source[*cursor],
        (size_t)start->offset - *cursor);
    if (status != PIKA_STATUS_OK) return status;
    status = graph_prefixed_name(
        graph, owner_module, name,
        replacement, sizeof(replacement));
    if (status != PIKA_STATUS_OK) return status;
    status = graph_buffer_text(buffer, replacement);
    if (status != PIKA_STATUS_OK) return status;
    *cursor = (size_t)end->offset + end->length;
    return PIKA_STATUS_OK;
}

static PikaStatus graph_replace_text(
    PikaGraphBuffer* buffer,
    const PikaGraphModule* module,
    size_t* cursor,
    uint32_t start_token,
    uint32_t end_token,
    const char* replacement) {
    const PikaToken* start = &module->tokens[start_token];
    const PikaToken* end = &module->tokens[end_token];
    PikaStatus status = graph_buffer_append(
        buffer, &module->source[*cursor],
        (size_t)start->offset - *cursor);
    if (status == PIKA_STATUS_OK) {
        status = graph_buffer_text(buffer, replacement);
    }
    if (status != PIKA_STATUS_OK) return status;
    *cursor = (size_t)end->offset + end->length;
    return PIKA_STATUS_OK;
}

static PikaStatus graph_rewrite_module_flat(PikaGraph* graph,
                                            uint8_t module_index,
                                            PikaGraphBuffer* buffer) {
    const PikaGraphModule* module = graph->modules[module_index];
    size_t cursor = 0u;
    uint32_t index;
    PikaStatus status;
    for (index = 0u; index < module->token_count; ++index) {
        const PikaToken* token = &module->tokens[index];
        const PikaGraphImport* import_entry =
            graph_import_at(module, index);
        const PikaGraphFunction* declaration =
            graph_function_by_token(module, index);
        const PikaGraphFunction* scope =
            graph_function_scope(module, index);
        if (token->kind == PIKA_TOKEN_EOF) break;
        if (graph->build_program_image != 0u &&
            scope != NULL &&
            index == scope->body_first &&
            graph_has_hidden(graph, scope)) {
            size_t line_start = token->offset;
            while (line_start > 0u &&
                   module->source[line_start - 1u] != '\n') {
                --line_start;
            }
            status = graph_buffer_append(
                buffer, &module->source[cursor],
                (size_t)token->offset - cursor);
            if (status == PIKA_STATUS_OK) {
                status = graph_buffer_text(buffer, "global ");
            }
            if (status == PIKA_STATUS_OK) {
                status = graph_append_hidden(
                    graph, buffer, scope, 0);
            }
            if (status == PIKA_STATUS_OK) {
                status = graph_buffer_text(buffer, "\n");
            }
            if (status == PIKA_STATUS_OK) {
                status = graph_buffer_append(
                    buffer, &module->source[line_start],
                    (size_t)token->offset - line_start);
            }
            if (status != PIKA_STATUS_OK) return status;
            cursor = token->offset;
        }
        if (import_entry != NULL &&
            (import_entry->is_python != 0u ||
             import_entry->is_program_image != 0u ||
             (graph->build_program_image != 0u &&
              graph->module_count > 1u &&
              import_entry->is_binding != 0u &&
              import_entry->top_level != 0u))) {
            uint32_t import_end = import_entry->last_token;
            const PikaToken* name;
            if (import_entry->is_binding != 0u &&
                import_end + 1u < module->token_count &&
                module->tokens[import_end + 1u].kind ==
                    PIKA_TOKEN_NEWLINE) {
                ++import_end;
            }
            name = &module->tokens[import_end];
            status = graph_buffer_append(
                buffer, &module->source[cursor],
                (size_t)token->offset - cursor);
            if (status != PIKA_STATUS_OK) return status;
            if (import_entry->is_binding == 0u &&
                (graph->build_program_image != 0u ||
                 import_entry->is_program_image != 0u)) {
                char initializer[
                    PIKA_GRAPH_PROGRAM_SYMBOL_CAPACITY];
                uint8_t target = import_entry->is_program_image != 0u
                                     ? import_entry->target_module
                                     : graph_program_module_index(
                                           graph->module_prefix_base,
                                           import_entry->target_module);
                status = graph_program_initializer_name(
                    target, initializer, sizeof(initializer));
                if (status == PIKA_STATUS_OK) {
                    status = graph_buffer_text(buffer, initializer);
                }
                if (status == PIKA_STATUS_OK) {
                    status = graph_buffer_text(buffer, "()");
                }
                if (status != PIKA_STATUS_OK) return status;
            }
            cursor = (size_t)name->offset + name->length;
            index = import_end;
            continue;
        }
        if (declaration != NULL &&
            index + 1u < module->token_count &&
            module->tokens[index + 1u].kind ==
                PIKA_TOKEN_LEFT_PAREN) {
            status = graph_append_callable(
                graph, buffer, module, &cursor, index, index,
                index + 1u, module_index,
                declaration);
            if (status != PIKA_STATUS_OK) return status;
            ++index;
            continue;
        }
        if (token->kind == PIKA_TOKEN_NAME && index > 0u &&
            module->tokens[index - 1u].kind == PIKA_TOKEN_DEF) {
            continue;
        }
        if (token->kind == PIKA_TOKEN_NAME &&
            !graph_attribute_member(module, index) &&
            !graph_local_name(module, scope, index) &&
            graph_function_by_name(module, index) == NULL &&
            graph_class_by_name(module, index) == NULL &&
            graph_name_index(
                module, module->globals, module->global_count,
                index) < 0) {
            const PikaGraphImport* selected =
                graph_python_selected_import(module, index);
            if (selected != NULL) {
                const PikaGraphModule* target =
                    graph->modules[selected->target_module];
                const PikaGraphFunction* member_function =
                    graph_member_function(module, index, target);
                const PikaGraphName* member_class =
                    graph_member_class(module, index, target);
                int member_global =
                    graph_member_global(module, index, target);
                if (member_function != NULL) {
                    if (index + 1u >= module->token_count ||
                        module->tokens[index + 1u].kind !=
                            PIKA_TOKEN_LEFT_PAREN) {
                        return graph_fail(
                            graph, PIKA_STATUS_UNSUPPORTED_SYNTAX,
                            module, index);
                    }
                    status = graph_append_callable(
                        graph, buffer, module, &cursor, index,
                        index, index + 1u, selected->target_module,
                        member_function);
                    if (status != PIKA_STATUS_OK) return status;
                    ++index;
                    continue;
                }
                if (member_class != NULL) {
                    if (index + 1u >= module->token_count ||
                        (module->tokens[index + 1u].kind !=
                             PIKA_TOKEN_LEFT_PAREN &&
                         !graph_class_base_reference(module, index))) {
                        return graph_fail(
                            graph, PIKA_STATUS_UNSUPPORTED_SYNTAX,
                            module, index);
                    }
                    status = graph_replace_name(
                        graph, buffer, module, &cursor, index, index,
                        selected->target_module, member_class->text);
                    if (status != PIKA_STATUS_OK) return status;
                    continue;
                }
                if (member_global >= 0) {
                    status = graph_replace_name(
                        graph, buffer, module, &cursor, index, index,
                        selected->target_module,
                        target->globals[member_global].text);
                    if (status != PIKA_STATUS_OK) return status;
                    continue;
                }
                return graph_fail(
                    graph, PIKA_STATUS_MEMBER_NOT_FOUND,
                    module, index);
            }
        }
        if (token->kind == PIKA_TOKEN_NAME &&
            index + 2u < module->token_count &&
            module->tokens[index + 1u].kind == PIKA_TOKEN_DOT &&
            module->tokens[index + 2u].kind == PIKA_TOKEN_NAME &&
            !graph_local_name(module, scope, index)) {
            const PikaGraphImport* imported =
                graph_python_import_name(module, index);
            if (imported != NULL) {
                const PikaGraphModule* target =
                    graph->modules[imported->target_module];
                uint32_t member_index;
                const PikaGraphFunction* member_function =
                    graph_member_function(
                        module, index + 2u, target);
                const PikaGraphName* member_class =
                    graph_member_class(
                        module, index + 2u, target);
                const char* binding_member =
                    graph_binding_selected_member(
                        module, index + 2u, target);
                const PikaGraphName* member_global = NULL;
                for (member_index = 0u;
                     member_index < target->global_count;
                     ++member_index) {
                    if (graph_token_equals(
                            module, index + 2u,
                            target->globals[member_index].text)) {
                        member_global = &target->globals[member_index];
                        break;
                    }
                }
                if (member_class != NULL) {
                    if (index + 3u >= module->token_count ||
                        (module->tokens[index + 3u].kind !=
                             PIKA_TOKEN_LEFT_PAREN &&
                         !graph_class_base_reference(
                             module, index + 2u))) {
                        return graph_fail(
                            graph, PIKA_STATUS_UNSUPPORTED_SYNTAX,
                            module, index + 2u);
                    }
                    status = graph_replace_name(
                        graph, buffer, module, &cursor,
                        index, index + 2u,
                        imported->target_module, member_class->text);
                    if (status != PIKA_STATUS_OK) return status;
                    index += 2u;
                    continue;
                }
                if (binding_member != NULL) {
                    status = graph_replace_text(
                        buffer, module, &cursor,
                        index, index + 2u, binding_member);
                    if (status != PIKA_STATUS_OK) return status;
                    index += 2u;
                    continue;
                }
                if (member_function != NULL) {
                    if (index + 3u >= module->token_count ||
                        module->tokens[index + 3u].kind !=
                            PIKA_TOKEN_LEFT_PAREN) {
                        return graph_fail(
                            graph, PIKA_STATUS_UNSUPPORTED_SYNTAX,
                            module, index + 2u);
                    }
                    status = graph_append_callable(
                        graph, buffer, module, &cursor, index,
                        index + 2u, index + 3u,
                        imported->target_module,
                        member_function);
                    if (status != PIKA_STATUS_OK) return status;
                    index += 3u;
                    continue;
                }
                if (member_global != NULL) {
                    status = graph_replace_name(
                        graph, buffer, module, &cursor,
                        index, index + 2u,
                        imported->target_module,
                        member_global->text);
                    if (status != PIKA_STATUS_OK) return status;
                    index += 2u;
                    continue;
                }
                return graph_fail(
                    graph, PIKA_STATUS_MEMBER_NOT_FOUND,
                    module, index + 2u);
            }
        }
        if (token->kind == PIKA_TOKEN_NAME &&
            !graph_attribute_member(module, index) &&
            !graph_local_name(module, scope, index)) {
            const PikaGraphName* class_reference =
                graph_class_by_name(module, index);
            if (class_reference != NULL) {
                status = graph_replace_name(
                    graph, buffer, module, &cursor, index, index,
                    module_index, class_reference->text);
                if (status != PIKA_STATUS_OK) return status;
                continue;
            }
        }
        if (token->kind == PIKA_TOKEN_NAME &&
            index + 1u < module->token_count &&
            module->tokens[index + 1u].kind ==
                PIKA_TOKEN_LEFT_PAREN &&
            !graph_attribute_member(module, index) &&
            !graph_local_name(module, scope, index)) {
            const PikaGraphFunction* function =
                graph_function_by_name(module, index);
            if (function != NULL) {
                status = graph_append_callable(
                    graph, buffer, module, &cursor, index, index,
                    index + 1u, module_index,
                    function);
                if (status != PIKA_STATUS_OK) return status;
                ++index;
                continue;
            }
        }
        if (token->kind == PIKA_TOKEN_NAME &&
            !graph_attribute_member(module, index) &&
            !graph_local_name(module, scope, index)) {
            const PikaGraphFunction* function =
                graph_function_by_name(module, index);
            if (function != NULL) {
                status = graph_replace_name(
                    graph, buffer, module, &cursor, index, index,
                    module_index, function->name.text);
                if (status != PIKA_STATUS_OK) return status;
                continue;
            }
        }
        if (token->kind == PIKA_TOKEN_NAME &&
            !graph_attribute_member(module, index) &&
            !graph_local_name(module, scope, index)) {
            int global_index = graph_name_index(
                module, module->globals, module->global_count, index);
            if (global_index >= 0) {
                status = graph_replace_name(
                    graph, buffer, module, &cursor, index, index,
                    module_index,
                    module->globals[global_index].text);
                if (status != PIKA_STATUS_OK) return status;
            }
        }
    }
    status = graph_buffer_append(
        buffer, &module->source[cursor], module->length - cursor);
    if (status != PIKA_STATUS_OK) return status;
    if (buffer->count_only != 0u) {
        return graph_buffer_text(buffer, "\n");
    }
    if (buffer->length == 0u ||
        buffer->data[buffer->length - 1u] != '\n') {
        status = graph_buffer_text(buffer, "\n");
    }
    return status;
}

typedef struct {
    size_t start;
    size_t end;
} PikaGraphSourceRange;

static PikaStatus graph_declaration_ranges(
    const char* source,
    size_t length,
    PikaGraphSourceRange* ranges,
    uint32_t capacity,
    uint32_t* count,
    PikaDiagnostic* diagnostic) {
    PikaTokenBuffer tokens;
    uint32_t index;
    uint32_t depth = 0u;
    PikaStatus status = pika_frontend_tokenize_allocated(
        source, length, &tokens, diagnostic);
    if (status != PIKA_STATUS_OK) return status;
    *count = 0u;
    for (index = 0u; index < tokens.count; ++index) {
        PikaTokenKind kind = (PikaTokenKind)tokens.tokens[index].kind;
        if (depth == 0u &&
            (kind == PIKA_TOKEN_DEF || kind == PIKA_TOKEN_CLASS)) {
            uint32_t cursor = index + 1u;
            uint32_t nested = 0u;
            while (cursor < tokens.count &&
                   tokens.tokens[cursor].kind != PIKA_TOKEN_INDENT &&
                   tokens.tokens[cursor].kind != PIKA_TOKEN_EOF) {
                ++cursor;
            }
            if (cursor >= tokens.count ||
                tokens.tokens[cursor].kind != PIKA_TOKEN_INDENT ||
                *count >= capacity) {
                status = PIKA_STATUS_FRONTEND_LIMIT;
                break;
            }
            nested = 1u;
            ++cursor;
            while (cursor < tokens.count && nested > 0u) {
                if (tokens.tokens[cursor].kind == PIKA_TOKEN_INDENT) {
                    ++nested;
                } else if (
                    tokens.tokens[cursor].kind == PIKA_TOKEN_DEDENT) {
                    --nested;
                }
                ++cursor;
            }
            if (nested != 0u) {
                status = PIKA_STATUS_SYNTAX_ERROR;
                break;
            }
            ranges[*count].start = tokens.tokens[index].offset;
            ranges[*count].end =
                cursor < tokens.count
                    ? tokens.tokens[cursor].offset
                    : length;
            if (ranges[*count].end < ranges[*count].start ||
                ranges[*count].end > length) {
                status = PIKA_STATUS_INTERNAL_ERROR;
                break;
            }
            ++*count;
            index = cursor == 0u ? 0u : cursor - 1u;
            continue;
        }
        if (kind == PIKA_TOKEN_INDENT) {
            ++depth;
        } else if (kind == PIKA_TOKEN_DEDENT && depth > 0u) {
            --depth;
        }
    }
    free(tokens.tokens);
    return status;
}

static PikaStatus graph_append_declarations(
    PikaGraphBuffer* buffer,
    const char* source,
    const PikaGraphSourceRange* ranges,
    uint32_t count) {
    uint32_t index;
    for (index = 0u; index < count; ++index) {
        PikaStatus status = graph_buffer_append(
            buffer, &source[ranges[index].start],
            ranges[index].end - ranges[index].start);
        if (status != PIKA_STATUS_OK) return status;
        status = graph_buffer_text(buffer, "\n");
        if (status != PIKA_STATUS_OK) return status;
    }
    return PIKA_STATUS_OK;
}

static PikaStatus graph_build_executable_source(
    PikaGraphBuffer* executable,
    const char* source,
    size_t length,
    const PikaGraphSourceRange* ranges,
    uint32_t count) {
    uint32_t index;
    size_t cursor = 0u;
    for (index = 0u; index < count; ++index) {
        PikaStatus status = graph_buffer_append(
            executable, &source[cursor], ranges[index].start - cursor);
        if (status != PIKA_STATUS_OK) return status;
        cursor = ranges[index].end;
    }
    return graph_buffer_append(
        executable, &source[cursor], length - cursor);
}

static PikaStatus graph_append_indented(
    PikaGraphBuffer* buffer,
    const char* source,
    size_t length,
    const char* indentation) {
    size_t cursor = 0u;
    while (cursor < length) {
        size_t end = cursor;
        PikaStatus status;
        while (end < length && source[end] != '\n') ++end;
        if (end > cursor) {
            status = graph_buffer_text(buffer, indentation);
            if (status != PIKA_STATUS_OK) return status;
            status = graph_buffer_append(
                buffer, &source[cursor], end - cursor);
            if (status != PIKA_STATUS_OK) return status;
        }
        if (end < length) {
            status = graph_buffer_text(buffer, "\n");
            if (status != PIKA_STATUS_OK) return status;
            ++end;
        }
        cursor = end;
    }
    return PIKA_STATUS_OK;
}

static PikaStatus graph_program_state_name(
    uint8_t module_index, char* output, size_t capacity) {
    int written = snprintf(
        output, capacity,
        "__pika_program_image_module_state_%u",
        (unsigned)module_index);
    return written < 0 || (size_t)written >= capacity
               ? PIKA_STATUS_FRONTEND_LIMIT
               : PIKA_STATUS_OK;
}

static PikaStatus graph_append_program_binding_imports(
    const PikaGraphModule* module,
    PikaGraphBuffer* buffer) {
    uint32_t import_index;
    for (import_index = 0u;
         import_index < module->import_count;
         ++import_index) {
        const PikaGraphImport* imported =
            &module->imports[import_index];
        uint32_t member_index;
        PikaStatus status;
        if (imported->is_binding == 0u ||
            imported->top_level == 0u) {
            continue;
        }
        status = graph_buffer_text(
            buffer, imported->from_import != 0u
                        ? "from " : "import ");
        if (status == PIKA_STATUS_OK) {
            status = graph_buffer_text(buffer, imported->name.text);
        }
        if (status == PIKA_STATUS_OK &&
            imported->from_import != 0u) {
            status = graph_buffer_text(buffer, " import ");
        }
        for (member_index = 0u;
             status == PIKA_STATUS_OK &&
             member_index < imported->member_count;
             ++member_index) {
            if (member_index > 0u) {
                status = graph_buffer_text(buffer, ", ");
            }
            if (status == PIKA_STATUS_OK) {
                status = graph_buffer_text(
                    buffer, imported->members[member_index].text);
            }
        }
        if (status == PIKA_STATUS_OK) {
            status = graph_buffer_text(buffer, "\n");
        }
        if (status != PIKA_STATUS_OK) return status;
    }
    return PIKA_STATUS_OK;
}

static PikaStatus graph_append_program_module(
    PikaGraph* graph,
    uint8_t module_index,
    const PikaGraphBuffer* rewritten,
    PikaGraphBuffer* buffer) {
    const PikaGraphModule* module = graph->modules[module_index];
    PikaGraphSourceRange ranges[
        PIKA_FRONTEND_FUNCTION_LIMIT + PIKA_FRONTEND_CLASS_LIMIT];
    PikaGraphBuffer executable;
    char initializer[PIKA_GRAPH_PROGRAM_SYMBOL_CAPACITY];
    char state_name[PIKA_GRAPH_PROGRAM_SYMBOL_CAPACITY];
    uint8_t stable_index = graph_program_module_index(
        graph->module_prefix_base, module_index);
    uint32_t range_count = 0u;
    uint32_t global_index;
    PikaStatus status = graph_append_program_binding_imports(
        module, buffer);
    memset(&executable, 0, sizeof(executable));
    if (status == PIKA_STATUS_OK) {
        status = graph_declaration_ranges(
            rewritten->data, rewritten->length, ranges,
            (uint32_t)(sizeof(ranges) / sizeof(ranges[0])),
            &range_count, graph->diagnostic);
    }
    if (status == PIKA_STATUS_OK) {
        status = graph_build_executable_source(
            &executable, rewritten->data, rewritten->length,
            ranges, range_count);
    }
    if (status == PIKA_STATUS_OK) {
        status = graph_append_declarations(
            buffer, rewritten->data, ranges, range_count);
    }
    if (status == PIKA_STATUS_OK) {
        status = graph_program_initializer_name(
            stable_index, initializer, sizeof(initializer));
    }
    if (status == PIKA_STATUS_OK) {
        status = graph_program_state_name(
            stable_index, state_name, sizeof(state_name));
    }
    if (status == PIKA_STATUS_OK) {
        status = graph_buffer_text(buffer, state_name);
    }
    if (status == PIKA_STATUS_OK) {
        status = graph_buffer_text(buffer, " = 0\n");
    }
    for (global_index = 0u;
         status == PIKA_STATUS_OK &&
         global_index < module->global_count;
         ++global_index) {
        char name[PIKA_FRONTEND_SYMBOL_LIMIT];
        status = graph_prefixed_name(
            graph, module_index,
            module->globals[global_index].text,
            name, sizeof(name));
        if (status == PIKA_STATUS_OK) {
            status = graph_buffer_text(buffer, name);
        }
        if (status == PIKA_STATUS_OK) {
            status = graph_buffer_text(buffer, " = None\n");
        }
    }
    if (status == PIKA_STATUS_OK) {
        status = graph_buffer_text(buffer, "def ");
    }
    if (status == PIKA_STATUS_OK) {
        status = graph_buffer_text(buffer, initializer);
    }
    if (status == PIKA_STATUS_OK) {
        status = graph_buffer_text(buffer, "():\n    global ");
    }
    if (status == PIKA_STATUS_OK) {
        status = graph_buffer_text(buffer, state_name);
    }
    if (status == PIKA_STATUS_OK) {
        status = graph_buffer_text(buffer, "\n");
    }
    for (global_index = 0u;
         status == PIKA_STATUS_OK &&
         global_index < module->global_count;
         ++global_index) {
        char name[PIKA_FRONTEND_SYMBOL_LIMIT];
        status = graph_prefixed_name(
            graph, module_index,
            module->globals[global_index].text,
            name, sizeof(name));
        if (status == PIKA_STATUS_OK) {
            status = graph_buffer_text(buffer, "    global ");
        }
        if (status == PIKA_STATUS_OK) {
            status = graph_buffer_text(buffer, name);
        }
        if (status == PIKA_STATUS_OK) {
            status = graph_buffer_text(buffer, "\n");
        }
    }
    if (status == PIKA_STATUS_OK) {
        status = graph_buffer_text(buffer, "    if ");
    }
    if (status == PIKA_STATUS_OK) {
        status = graph_buffer_text(buffer, state_name);
    }
    if (status == PIKA_STATUS_OK) {
        status = graph_buffer_text(
            buffer, " == 2:\n        return None\n    if ");
    }
    if (status == PIKA_STATUS_OK) {
        status = graph_buffer_text(buffer, state_name);
    }
    if (status == PIKA_STATUS_OK) {
        status = graph_buffer_text(
            buffer, " == 1:\n        return None\n    ");
    }
    if (status == PIKA_STATUS_OK) {
        status = graph_buffer_text(buffer, state_name);
    }
    if (status == PIKA_STATUS_OK) {
        status = graph_buffer_text(buffer, " = 1\n    try:\n");
    }
    if (status == PIKA_STATUS_OK) {
        status = graph_append_indented(
            buffer, executable.data, executable.length, "        ");
    }
    if (status == PIKA_STATUS_OK) {
        status = graph_buffer_text(buffer, "        ");
    }
    if (status == PIKA_STATUS_OK) {
        status = graph_buffer_text(buffer, state_name);
    }
    if (status == PIKA_STATUS_OK) {
        status = graph_buffer_text(
            buffer, " = 2\n    except:\n        ");
    }
    if (status == PIKA_STATUS_OK) {
        status = graph_buffer_text(buffer, state_name);
    }
    if (status == PIKA_STATUS_OK) {
        status = graph_buffer_text(buffer, " = 0\n");
    }
    for (global_index = 0u;
         status == PIKA_STATUS_OK &&
         global_index < module->global_count;
         ++global_index) {
        char name[PIKA_FRONTEND_SYMBOL_LIMIT];
        status = graph_prefixed_name(
            graph, module_index,
            module->globals[global_index].text,
            name, sizeof(name));
        if (status == PIKA_STATUS_OK) {
            status = graph_buffer_text(buffer, "        ");
        }
        if (status == PIKA_STATUS_OK) {
            status = graph_buffer_text(buffer, name);
        }
        if (status == PIKA_STATUS_OK) {
            status = graph_buffer_text(buffer, " = None\n");
        }
    }
    if (status == PIKA_STATUS_OK) {
        status = graph_buffer_text(buffer, "        raise\n\n");
    }
    free(executable.data);
    return status;
}

static PikaStatus graph_rewrite_module(PikaGraph* graph,
                                       uint8_t module_index,
                                       PikaGraphBuffer* buffer) {
    PikaGraphBuffer rewritten;
    PikaStatus status;
    if (graph->build_program_image == 0u ||
        graph->module_count == 1u) {
        return graph_rewrite_module_flat(graph, module_index, buffer);
    }
    if (buffer->count_only == 0u) {
        graph->modules[module_index]->generated_start = buffer->length;
    }
    memset(&rewritten, 0, sizeof(rewritten));
    status = graph_rewrite_module_flat(
        graph, module_index, &rewritten);
    if (status == PIKA_STATUS_OK) {
        status = graph_append_program_module(
            graph, module_index, &rewritten, buffer);
    }
    if (status == PIKA_STATUS_OK && buffer->count_only == 0u) {
        graph->modules[module_index]->generated_end = buffer->length;
    }
    if (status != PIKA_STATUS_OK) {
        graph_set_diagnostic_source(
            graph, graph->modules[module_index]);
    }
    free(rewritten.data);
    return status;
}

static PikaStatus graph_append_entry_initializer(
    PikaGraph* graph,
    uint8_t entry_index,
    PikaGraphBuffer* buffer) {
    char initializer[PIKA_GRAPH_PROGRAM_SYMBOL_CAPACITY];
    PikaStatus status;
    if (graph->build_program_image == 0u ||
        graph->module_count == 1u) {
        return PIKA_STATUS_OK;
    }
    status = graph_program_initializer_name(
        graph_program_module_index(
            graph->module_prefix_base, entry_index),
        initializer, sizeof(initializer));
    if (status == PIKA_STATUS_OK) {
        status = graph_buffer_text(buffer, initializer);
    }
    if (status == PIKA_STATUS_OK) {
        status = graph_buffer_text(buffer, "()\n");
    }
    return status;
}

static int graph_semantic_name_equals(
    const PikaCompiledModule* module,
    uint32_t offset,
    uint16_t length,
    const char* name) {
    size_t name_length;
    if (module == NULL || name == NULL ||
        offset > module->semantic_name_count ||
        length > module->semantic_name_count - offset) {
        return 0;
    }
    name_length = strlen(name);
    return name_length == length &&
           memcmp(&module->semantic_names[offset], name, length) == 0;
}

static int graph_semantic_names_equal(
    const PikaCompiledModule* module,
    uint32_t left_offset,
    uint16_t left_length,
    uint32_t right_offset,
    uint16_t right_length) {
    if (module == NULL || left_length != right_length ||
        left_offset > module->semantic_name_count ||
        left_length > module->semantic_name_count - left_offset ||
        right_offset > module->semantic_name_count ||
        right_length > module->semantic_name_count - right_offset) {
        return 0;
    }
    return memcmp(
               &module->semantic_names[left_offset],
               &module->semantic_names[right_offset],
               left_length) == 0;
}

static PikaStatus graph_append_semantic_name(
    PikaCompiledModule* module,
    const char* name,
    uint32_t* offset,
    uint16_t* length) {
    size_t name_length;
    uint32_t required;
    char* names;
    if (module == NULL || name == NULL ||
        offset == NULL || length == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    name_length = strlen(name);
    if (name_length == 0u || name_length > UINT16_MAX ||
        name_length > UINT32_MAX - module->semantic_name_count) {
        return PIKA_STATUS_FRONTEND_LIMIT;
    }
    required = module->semantic_name_count + (uint32_t)name_length;
    if (required > module->semantic_name_capacity) {
        names = (char*)realloc(module->semantic_names, required);
        if (names == NULL) return PIKA_STATUS_FRONTEND_LIMIT;
        module->semantic_names = names;
        module->semantic_name_capacity = required;
    }
    *offset = module->semantic_name_count;
    *length = (uint16_t)name_length;
    memcpy(&module->semantic_names[*offset], name, name_length);
    module->semantic_name_count = required;
    return PIKA_STATUS_OK;
}

static PikaStatus graph_find_initializer_function(
    const PikaCompiledModule* module,
    uint8_t module_index,
    uint16_t* function_index) {
    char initializer[PIKA_GRAPH_PROGRAM_SYMBOL_CAPACITY];
    uint16_t index;
    PikaStatus status = graph_program_initializer_name(
        module_index, initializer, sizeof(initializer));
    if (status != PIKA_STATUS_OK) return status;
    for (index = 0u;
         index < module->semantic_function_count;
         ++index) {
        const PikaSemanticFunction* function =
            &module->semantic_functions[index];
        if (graph_semantic_name_equals(
                module, function->name_offset,
                function->name_length, initializer)) {
            *function_index = index;
            return PIKA_STATUS_OK;
        }
    }
    return PIKA_STATUS_INTERNAL_ERROR;
}

static int graph_dependency_exists(
    const PikaCompiledModule* module,
    uint32_t first,
    uint16_t count,
    uint8_t dependency) {
    uint16_t index;
    for (index = 0u; index < count; ++index) {
        if (module->program_module_dependencies[first + index] ==
            dependency) {
            return 1;
        }
    }
    return 0;
}

static int graph_binding_import_exists(
    const PikaCompiledModule* module,
    uint32_t first,
    uint16_t count,
    const char* name) {
    uint16_t index;
    for (index = 0u; index < count; ++index) {
        const PikaSemanticProgramBindingImport* imported =
            &module->program_binding_imports[first + index];
        if (graph_semantic_name_equals(
                module, imported->name_offset,
                imported->name_length, name)) {
            return 1;
        }
    }
    return 0;
}

static PikaStatus graph_append_program_python_import(
    PikaCompiledModule* module,
    const PikaSemanticProgramModule* program_module,
    uint8_t flags) {
    PikaPythonModule* imports;
    uint16_t index;
    for (index = 0u;
         index < module->semantic_python_import_count;
         ++index) {
        PikaPythonModule* imported =
            &module->semantic_python_imports[index];
        if (graph_semantic_names_equal(
                module, imported->name_offset,
                imported->name_length,
                program_module->name_offset,
                program_module->name_length)) {
            if (imported->target_module !=
                program_module->module_index) {
                return PIKA_STATUS_MODULE_CONFLICT;
            }
            imported->flags |= flags;
            return PIKA_STATUS_OK;
        }
    }
    if (module->semantic_python_import_count == UINT16_MAX) {
        return PIKA_STATUS_FRONTEND_LIMIT;
    }
    imports = (PikaPythonModule*)realloc(
        module->semantic_python_imports,
        (size_t)(module->semantic_python_import_count + 1u) *
            sizeof(*imports));
    if (imports == NULL) return PIKA_STATUS_FRONTEND_LIMIT;
    module->semantic_python_imports = imports;
    module->semantic_python_import_capacity =
        module->semantic_python_import_count + 1u;
    imports = &module->semantic_python_imports[
        module->semantic_python_import_count++];
    imports->name_offset = program_module->name_offset;
    imports->name_length = program_module->name_length;
    imports->target_module = program_module->module_index;
    imports->flags = flags;
    return PIKA_STATUS_OK;
}

static PikaStatus graph_attach_program_modules(
    const PikaGraph* graph,
    uint8_t entry_index,
    PikaCompiledModule* module) {
    uint32_t import_capacity = 0u;
    uint32_t module_index;
    PikaStatus status = PIKA_STATUS_OK;
    for (module_index = 0u;
         module_index < graph->module_count;
         ++module_index) {
        uint32_t import_index;
        if (graph->modules[module_index]->import_count >
            UINT32_MAX - import_capacity) {
            return PIKA_STATUS_FRONTEND_LIMIT;
        }
        import_capacity +=
            graph->modules[module_index]->import_count;
        for (import_index = 0u;
             import_index < graph->modules[module_index]->import_count;
             ++import_index) {
            const PikaGraphImport* imported =
                &graph->modules[module_index]->imports[import_index];
            uint32_t exported_count;
            if (imported->is_python == 0u) continue;
            exported_count = graph->modules[
                imported->target_module]->import_count;
            if (exported_count > UINT32_MAX - import_capacity) {
                return PIKA_STATUS_FRONTEND_LIMIT;
            }
            import_capacity += exported_count;
        }
    }
    module->program_modules = (PikaSemanticProgramModule*)calloc(
        graph->module_count, sizeof(*module->program_modules));
    if (import_capacity > 0u) {
        module->program_module_dependencies = (uint8_t*)calloc(
            import_capacity,
            sizeof(*module->program_module_dependencies));
        module->program_binding_imports =
            (PikaSemanticProgramBindingImport*)calloc(
                import_capacity,
                sizeof(*module->program_binding_imports));
    }
    if (module->program_modules == NULL ||
        (import_capacity > 0u &&
         (module->program_module_dependencies == NULL ||
          module->program_binding_imports == NULL))) {
        return PIKA_STATUS_FRONTEND_LIMIT;
    }
    module->program_module_capacity =
        (uint16_t)graph->module_count;
    module->program_module_dependency_capacity = import_capacity;
    module->program_binding_import_capacity = import_capacity;
    module->program_entry_module = graph_program_module_index(
        graph->module_prefix_base, entry_index);
    for (module_index = 0u;
         status == PIKA_STATUS_OK &&
         module_index < graph->module_count;
         ++module_index) {
        const PikaGraphModule* source =
            graph->modules[module_index];
        PikaSemanticProgramModule* retained =
            &module->program_modules[module_index];
        uint8_t stable_index = graph_program_module_index(
            graph->module_prefix_base, (uint8_t)module_index);
        uint32_t import_index;
        status = graph_append_semantic_name(
            module, source->name, &retained->name_offset,
            &retained->name_length);
        if (status == PIKA_STATUS_OK) {
            if (graph->module_count == 1u) {
                retained->entry_function =
                    module->program.entry_function;
            } else {
                status = graph_find_initializer_function(
                    module, stable_index,
                    &retained->entry_function);
            }
        }
        retained->first_dependency =
            module->program_module_dependency_count;
        retained->first_binding_import =
            (uint16_t)module->program_binding_import_count;
        retained->module_index = stable_index;
        for (import_index = 0u;
             status == PIKA_STATUS_OK &&
             import_index < source->import_count;
             ++import_index) {
            const PikaGraphImport* imported =
                &source->imports[import_index];
            if (imported->is_python != 0u) {
                uint8_t dependency = graph_program_module_index(
                    graph->module_prefix_base,
                    imported->target_module);
                if (!graph_dependency_exists(
                        module, retained->first_dependency,
                        retained->dependency_count, dependency)) {
                    if (module->program_module_dependency_count >=
                        module->program_module_dependency_capacity ||
                        retained->dependency_count == UINT16_MAX) {
                        status = PIKA_STATUS_FRONTEND_LIMIT;
                        break;
                    }
                    module->program_module_dependencies[
                        module->program_module_dependency_count++] =
                        dependency;
                    ++retained->dependency_count;
                }
                {
                    const PikaGraphModule* target =
                        graph->modules[imported->target_module];
                    uint32_t target_import_index;
                    for (target_import_index = 0u;
                         status == PIKA_STATUS_OK &&
                         target_import_index < target->import_count;
                         ++target_import_index) {
                        const PikaGraphImport* exported =
                            &target->imports[target_import_index];
                        PikaSemanticProgramBindingImport* binding_import;
                        if (exported->is_binding == 0u ||
                            graph_binding_import_exists(
                                module,
                                retained->first_binding_import,
                                retained->binding_import_count,
                                exported->name.text)) {
                            continue;
                        }
                        if (module->program_binding_import_count >=
                                module->program_binding_import_capacity ||
                            retained->binding_import_count == UINT16_MAX) {
                            status = PIKA_STATUS_FRONTEND_LIMIT;
                            break;
                        }
                        binding_import = &module->program_binding_imports[
                            module->program_binding_import_count++];
                        status = graph_append_semantic_name(
                            module, exported->name.text,
                            &binding_import->name_offset,
                            &binding_import->name_length);
                        binding_import->module_index = stable_index;
                        if (status == PIKA_STATUS_OK) {
                            ++retained->binding_import_count;
                        }
                    }
                }
            } else if (imported->is_binding != 0u &&
                       !graph_binding_import_exists(
                           module, retained->first_binding_import,
                           retained->binding_import_count,
                           imported->name.text)) {
                PikaSemanticProgramBindingImport* binding_import;
                if (module->program_binding_import_count >=
                        module->program_binding_import_capacity ||
                    retained->binding_import_count == UINT16_MAX) {
                    status = PIKA_STATUS_FRONTEND_LIMIT;
                    break;
                }
                binding_import = &module->program_binding_imports[
                    module->program_binding_import_count++];
                status = graph_append_semantic_name(
                    module, imported->name.text,
                    &binding_import->name_offset,
                    &binding_import->name_length);
                binding_import->module_index = stable_index;
                if (status == PIKA_STATUS_OK) {
                    ++retained->binding_import_count;
                }
            }
        }
        if (status == PIKA_STATUS_OK) {
            ++module->program_module_count;
        }
    }
    for (module_index = 0u;
         status == PIKA_STATUS_OK &&
         module_index < module->program_module_count;
         ++module_index) {
        const PikaSemanticProgramModule* program_module =
            &module->program_modules[module_index];
        if (program_module->module_index !=
            module->program_entry_module) {
            status = graph_append_program_python_import(
                module, program_module,
                graph->modules[module_index]->publicly_available != 0u
                    ? PIKA_PYTHON_MODULE_PUBLIC
                    : 0u);
        }
    }
    return status;
}

static void graph_set_compile_diagnostic_source(
    PikaGraph* graph,
    uint8_t entry_index) {
    uint32_t index;
    if (graph == NULL || graph->diagnostic == NULL) return;
    for (index = 0u; index < graph->module_count; ++index) {
        const PikaGraphModule* source = graph->modules[index];
        if (graph->diagnostic->offset >= source->generated_start &&
            graph->diagnostic->offset < source->generated_end) {
            graph_set_diagnostic_source(graph, source);
            return;
        }
    }
    graph_set_diagnostic_source(graph, graph->modules[entry_index]);
}

static void graph_release(PikaGraph* graph) {
    uint32_t index;
    for (index = 0u; index < graph->module_count; ++index) {
        PikaGraphModule* module = graph->modules[index];
        uint32_t child;
        for (child = 0u; child < module->function_count; ++child) {
            free(module->functions[child].locals);
            free(module->functions[child].declared_globals);
        }
        for (child = 0u; child < module->import_count; ++child) {
            free(module->imports[child].members);
        }
        free(module->globals);
        free(module->classes);
        free(module->functions);
        free(module->imports);
        free(module->tokens);
        if (module->source != NULL &&
            graph->provider->release != NULL) {
            graph->provider->release(
                graph->provider->context, module->source);
        }
        free(module);
        graph->modules[index] = NULL;
    }
}

static PikaStatus graph_compile_mode(
    const PikaCompiledModule* base,
    PikaCompiledModule* transferable_base,
    const char* entry_module,
    const char* const* module_names,
    size_t module_name_count,
    int build_program_image,
    const PikaModuleProvider* provider,
    const PikaFrontendConfig* config,
    PikaModuleCommitCheck commit_check,
    void* commit_context,
    PikaCompiledModule** module,
    PikaInteractiveCompileState* interactive_state,
    PikaDiagnostic* diagnostic) {
    PikaGraph* graph;
    PikaGraphBuffer buffer;
    PikaGraphPrepareContext prepare_context;
    PikaGraphRetainedImport
        retained_imports[PIKA_FRONTEND_MODULE_LIMIT];
    uint16_t retained_import_count = 0u;
    uint8_t entry_index = 0u;
    uint8_t module_prefix_base = 0u;
    uint32_t order;
    size_t source_length = 0u;
    PikaStatus status;
    int imports_prepared = 0;
    if (module == NULL) return PIKA_STATUS_INVALID_ARGUMENT;
    *module = NULL;
    if (entry_module == NULL || provider == NULL ||
        provider->load == NULL || config == NULL) {
        pika_frontend_set_diagnostic(
            diagnostic, PIKA_STATUS_INVALID_ARGUMENT, 1u, 1u);
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if ((module_name_count > 0u && module_names == NULL) ||
        module_name_count > PIKA_FRONTEND_MODULE_LIMIT) {
        pika_frontend_set_diagnostic(
            diagnostic, PIKA_STATUS_INVALID_ARGUMENT, 1u, 1u);
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    graph = (PikaGraph*)calloc(1u, sizeof(PikaGraph));
    if (graph == NULL) {
        pika_frontend_set_diagnostic(
            diagnostic, PIKA_STATUS_FRONTEND_LIMIT, 1u, 1u);
        return PIKA_STATUS_FRONTEND_LIMIT;
    }
    memset(&buffer, 0, sizeof(buffer));
    memset(&prepare_context, 0, sizeof(prepare_context));
    graph->provider = provider;
    graph->config = config;
    graph->base = base;
    graph->diagnostic = diagnostic;
    graph->build_program_image = build_program_image != 0 ? 1u : 0u;
    PIKA_FRONTEND_MEMORY_CHECKPOINT(
        "frontend-graph-start", 0u);
    if (base != NULL) {
        uint16_t import_index;
        for (import_index = 0u;
             import_index <
                 base->semantic_python_import_count;
             ++import_index) {
            uint8_t target =
                base->semantic_python_imports[
                    import_index].target_module;
            if (target > graph->module_prefix_base) {
                graph->module_prefix_base = target;
            }
        }
    }
    status = graph_load_module(graph, entry_module, &entry_index);
    if (status == PIKA_STATUS_OK) {
        status = graph_resolve_module(graph, entry_index);
    }
    if (status == PIKA_STATUS_OK) {
        const PikaGraphModule* entry = graph->modules[entry_index];
        uint32_t import_index;
        for (import_index = 0u;
             import_index < entry->import_count;
             ++import_index) {
            const PikaGraphImport* imported =
                &entry->imports[import_index];
            if (imported->is_python != 0u &&
                imported->from_base == 0u) {
                graph->modules[
                    imported->target_module]->publicly_available = 1u;
            }
        }
    }
    for (order = 0u;
         status == PIKA_STATUS_OK && order < module_name_count;
         ++order) {
        uint8_t root_index = 0u;
        if (module_names[order] == NULL) {
            status = PIKA_STATUS_INVALID_ARGUMENT;
            break;
        }
        status = graph_load_module(
            graph, module_names[order], &root_index);
        if (status == PIKA_STATUS_OK) {
            graph->modules[root_index]->publicly_available = 1u;
            status = graph_resolve_module(graph, root_index);
        }
    }
    if (status == PIKA_STATUS_OK &&
        (config->enabled & PIKA_CAPABILITY_PYTHON_MODULE) == 0u) {
        uint32_t module_index;
        int requires_python_module =
            build_program_image == 0 || graph->module_count > 1u;
        for (module_index = 0u;
             requires_python_module == 0 &&
             module_index < graph->module_count;
             ++module_index) {
            const PikaGraphModule* current =
                graph->modules[module_index];
            uint32_t import_index;
            for (import_index = 0u;
                 import_index < current->import_count;
                 ++import_index) {
                if (current->imports[import_index].is_python != 0u) {
                    requires_python_module = 1;
                    break;
                }
            }
        }
        if (requires_python_module != 0) {
            pika_frontend_set_diagnostic(
                diagnostic, PIKA_STATUS_UNSUPPORTED_CAPABILITY,
                1u, 1u);
            status = PIKA_STATUS_UNSUPPORTED_CAPABILITY;
        }
    }
    if (status == PIKA_STATUS_OK) {
        graph_resolve_function_hidden(graph);
        PIKA_FRONTEND_MEMORY_CHECKPOINT(
            "frontend-graph-resolved",
            graph->module_count);
    }
    buffer.count_only = 1u;
    for (order = 0u;
         status == PIKA_STATUS_OK && order < graph->topology_count;
         ++order) {
        status = graph_rewrite_module(
            graph, graph->topology[order], &buffer);
    }
    if (status == PIKA_STATUS_OK) {
        status = graph_append_entry_initializer(
            graph, entry_index, &buffer);
    }
    if (status == PIKA_STATUS_OK) {
        PIKA_FRONTEND_MEMORY_CHECKPOINT(
            "frontend-rewrite-counted",
            graph->topology_count);
    }
    if (status == PIKA_STATUS_OK) {
        size_t retained_limit = graph->module_count - 1u;
        size_t import_bytes;
        size_t capacity;
        const PikaGraphModule* entry = graph->modules[entry_index];
        uint32_t import_index;
        for (import_index = 0u;
             import_index < entry->import_count;
             ++import_index) {
            if (entry->imports[import_index].from_base != 0u) {
                ++retained_limit;
            }
        }
        import_bytes = retained_limit *
            (PIKA_FRONTEND_IDENTIFIER_BYTE_LIMIT + 1u);
        if (buffer.length > SIZE_MAX - 1u - import_bytes) {
            status = PIKA_STATUS_FRONTEND_LIMIT;
        } else {
            capacity = buffer.length + 1u + import_bytes;
            buffer.data = (char*)malloc(capacity);
            if (buffer.data == NULL) {
                status = PIKA_STATUS_FRONTEND_LIMIT;
            } else {
                buffer.capacity = capacity;
                buffer.length = 0u;
                buffer.count_only = 0u;
                PIKA_FRONTEND_MEMORY_CHECKPOINT(
                    "frontend-rewrite-allocated",
                    graph->topology_count);
            }
        }
    }
    for (order = 0u;
         status == PIKA_STATUS_OK && order < graph->topology_count;
         ++order) {
        status = graph_rewrite_module(
            graph, graph->topology[order], &buffer);
    }
    if (status == PIKA_STATUS_OK) {
        status = graph_append_entry_initializer(
            graph, entry_index, &buffer);
    }
    if (status == PIKA_STATUS_OK) {
        PIKA_FRONTEND_MEMORY_CHECKPOINT(
            "frontend-rewrite-emitted",
            graph->topology_count);
    }
    if (status == PIKA_STATUS_OK) {
        status = graph_buffer_reserve(&buffer, 1u);
    }
    if (status == PIKA_STATUS_OK) {
        source_length = buffer.length;
        module_prefix_base = graph->module_prefix_base;
        buffer.data[buffer.length++] = '\0';
        status = graph_capture_python_modules(
            graph, entry_index, &buffer,
            retained_imports, &retained_import_count);
        PIKA_FRONTEND_MEMORY_CHECKPOINT(
            "frontend-graph-metadata",
            retained_import_count);
    }
    if (status == PIKA_STATUS_OK) {
        if (graph->build_program_image == 0u) {
            graph_release(graph);
            free(graph);
            graph = NULL;
            PIKA_FRONTEND_MEMORY_CHECKPOINT(
                "frontend-graph-released", 0u);
        }
        if (interactive_state == NULL) {
            prepare_context.module_prefix_base =
                module_prefix_base;
            prepare_context.buffer = &buffer;
            prepare_context.retained = retained_imports;
            prepare_context.retained_count =
                retained_import_count;
            imports_prepared = 1;
            status = pika_module_compile_seeded_configured(
                buffer.data, source_length, config,
                graph_seed_python_imports,
                &prepare_context,
                module, diagnostic);
        } else {
            prepare_context.module_prefix_base =
                module_prefix_base;
            prepare_context.buffer = &buffer;
            prepare_context.retained = retained_imports;
            prepare_context.retained_count =
                retained_import_count;
            imports_prepared = 1;
            status =
                pika_module_compile_incremental_prepared_interactive(
                    base, transferable_base,
                    buffer.data, source_length, config,
                    graph_prepare_python_imports,
                    &prepare_context,
                    commit_check, commit_context,
                    module, interactive_state, diagnostic);
        }
    }
    if (status == PIKA_STATUS_OK && *module != NULL) {
        if (graph != NULL && graph->build_program_image != 0u) {
            status = graph_attach_program_modules(
                graph, entry_index, *module);
        } else if (imports_prepared == 0) {
            status = graph_retain_python_imports(
                module_prefix_base, *module, &buffer,
                retained_imports,
                retained_import_count);
        }
        if (status == PIKA_STATUS_OK) {
            (*module)->program.module_names =
                (*module)->semantic_names;
            (*module)->program.module_name_bytes =
                (*module)->semantic_name_count;
            (*module)->program.python_modules =
                (*module)->semantic_python_imports;
            (*module)->program.python_module_count =
                (*module)->semantic_python_import_count;
            PIKA_FRONTEND_MEMORY_CHECKPOINT(
                "frontend-graph-committed",
                retained_import_count);
        }
        if (status != PIKA_STATUS_OK) {
            pika_compiled_module_destroy(*module);
            *module = NULL;
            pika_frontend_set_diagnostic(
                diagnostic, status, 1u, 1u);
        }
    } else if (graph != NULL &&
               graph->build_program_image != 0u) {
        graph_set_compile_diagnostic_source(graph, entry_index);
    } else if (diagnostic != NULL && diagnostic->status == PIKA_STATUS_OK) {
        pika_frontend_set_diagnostic(diagnostic, status, 1u, 1u);
    }
    free(buffer.data);
    if (graph != NULL) {
        graph_release(graph);
        free(graph);
    }
    return status;
}

static PikaStatus incremental_graph_load(
    void* context,
    const char* module_name,
    const char** source,
    size_t* length) {
    PikaIncrementalGraphProvider* provider =
        (PikaIncrementalGraphProvider*)context;
    if (strcmp(module_name, "__repl__") == 0) {
        *source = provider->source;
        *length = provider->length;
        return PIKA_STATUS_OK;
    }
    return provider->modules->load(
        provider->modules->context, module_name,
        source, length);
}

static void incremental_graph_release(
    void* context,
    const char* source) {
    PikaIncrementalGraphProvider* provider =
        (PikaIncrementalGraphProvider*)context;
    if (source == provider->source ||
        provider->modules->release == NULL) {
        return;
    }
    provider->modules->release(
        provider->modules->context, source);
}

PikaStatus pika_module_graph_compile(
    const char* entry_module,
    const PikaModuleProvider* provider,
    const PikaFrontendConfig* config,
    PikaCompiledModule** module,
    PikaDiagnostic* diagnostic) {
    return graph_compile_mode(
        NULL, NULL, entry_module, NULL, 0u, 0,
        provider, config,
        NULL, NULL, module, NULL, diagnostic);
}

PikaStatus pika_module_graph_compile_image(
    const char* entry_module,
    const char* const* module_names,
    size_t module_count,
    const PikaModuleProvider* provider,
    const PikaFrontendConfig* config,
    PikaCompiledModule** module,
    PikaDiagnostic* diagnostic) {
    return graph_compile_mode(
        NULL, NULL, entry_module,
        module_names, module_count, 1,
        provider, config, NULL, NULL,
        module, NULL, diagnostic);
}

static PikaStatus compile_incremental_graph_mode(
    const PikaCompiledModule* base,
    PikaCompiledModule* transferable_base,
    const char* source,
    size_t length,
    const PikaModuleProvider* provider,
    const PikaFrontendConfig* config,
    PikaModuleCommitCheck commit_check,
    void* commit_context,
    PikaCompiledModule** module,
    PikaInteractiveCompileState* state,
    PikaDiagnostic* diagnostic) {
    PikaIncrementalGraphProvider incremental;
    PikaModuleProvider entry_provider;
    if (source == NULL || provider == NULL ||
        provider->load == NULL || state == NULL) {
        pika_frontend_set_diagnostic(
            diagnostic, PIKA_STATUS_INVALID_ARGUMENT, 1u, 1u);
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    incremental.source = source;
    incremental.length = length;
    incremental.modules = provider;
    entry_provider.load = incremental_graph_load;
    entry_provider.release = incremental_graph_release;
    entry_provider.context = &incremental;
    return graph_compile_mode(
        base, transferable_base, "__repl__",
        NULL, 0u, 0,
        &entry_provider, config,
        commit_check, commit_context,
        module, state, diagnostic);
}

PikaStatus pika_module_graph_compile_incremental_interactive(
    const PikaCompiledModule* base,
    const char* source,
    size_t length,
    const PikaModuleProvider* provider,
    const PikaFrontendConfig* config,
    PikaCompiledModule** module,
    PikaInteractiveCompileState* state,
    PikaDiagnostic* diagnostic) {
    return compile_incremental_graph_mode(
        base, NULL, source, length, provider, config,
        NULL, NULL, module, state, diagnostic);
}

PikaStatus pika_module_graph_compile_incremental_owned_interactive(
    PikaCompiledModule* base,
    const char* source,
    size_t length,
    const PikaModuleProvider* provider,
    const PikaFrontendConfig* config,
    PikaModuleCommitCheck commit_check,
    void* commit_context,
    PikaCompiledModule** module,
    PikaInteractiveCompileState* state,
    PikaDiagnostic* diagnostic) {
    return compile_incremental_graph_mode(
        base, base, source, length, provider, config,
        commit_check, commit_context,
        module, state, diagnostic);
}
