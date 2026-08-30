/* SPEC: PJ2026-0501 V2内核 v0.47；解析与 IR lowering。 */
#include "frontend_internal.h"
#include "pika_text_method.h"

#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    uint32_t name_position;
    uint32_t default_start;
    uint32_t default_end;
    const char* stable_name;
    uint16_t stable_name_length;
    uint16_t default_constant;
} PikaParameterDeclaration;

typedef struct {
    uint32_t header_position;
    uint32_t name_position;
    uint32_t body_position;
    uint32_t after_body;
    const char* stable_name;
    uint16_t stable_name_length;
    uint16_t function_index;
    uint8_t parameter_count;
    uint8_t required_parameter_count;
    uint8_t variadic_parameter;
    uint16_t owner_class;
    PikaStaticShape return_shape;
    uint32_t first_return_element_shape;
    uint16_t return_element_shape_count;
    uint8_t has_return_shape;
    uint8_t is_lambda;
    PikaParameterDeclaration* parameters;
} PikaFunctionDeclaration;

typedef struct {
    const PikaBindingClass* binding_class;
    uint32_t name_position;
    const char* stable_name;
    uint16_t stable_name_length;
    uint16_t class_index;
    uint8_t kind;
    uint8_t binding_kind;
    uint8_t allows_none;
} PikaFieldDeclaration;

typedef struct {
    uint32_t header_position;
    uint32_t name_position;
    uint32_t after_body;
    const char* stable_name;
    uint16_t stable_name_length;
    PikaFieldDeclaration* fields;
    uint16_t class_index;
    uint16_t base_class;
    uint16_t field_count;
    uint16_t field_capacity;
    uint16_t member_count;
} PikaClassDeclaration;

typedef struct {
    const char* name_source;
    uint32_t offset;
    uint32_t length;
    const PikaBindingModule* module;
    uint8_t selected;
} PikaImportedModule;

typedef struct {
    int32_t break_head;
    int32_t continue_head;
} PikaLoopContext;

typedef struct {
    const PikaToken* first_control_transfer;
    int32_t return_jump_head;
    uint8_t return_slot;
    uint8_t has_return;
    uint8_t return_uses_binding;
} PikaTryContext;

typedef struct {
    const char* source;
    const PikaToken* tokens;
    uint32_t token_count;
    uint32_t position;
    PikaCompiledModule* module;
    PikaDiagnostic* diagnostic;
    PikaCapability enabled_capabilities;
    const PikaBindingRegistry* bindings;
    PikaSymbol* symbols;
    uint32_t symbol_capacity;
    uint32_t symbol_count;
    PikaSymbol* module_globals;
    uint32_t module_global_capacity;
    uint32_t module_global_count;
    uint16_t next_slot;
    PikaFunctionDeclaration* declarations;
    uint32_t declaration_capacity;
    uint32_t declaration_count;
    uint32_t base_declaration_count;
    PikaParameterDeclaration* parameter_declarations;
    uint32_t parameter_declaration_capacity;
    uint32_t parameter_declaration_count;
    PikaClassDeclaration* classes;
    uint32_t class_capacity;
    uint32_t class_count;
    uint32_t base_class_count;
    PikaLoopContext loops[PIKA_FRONTEND_LOOP_LIMIT];
    uint32_t loop_depth;
    PikaTryContext try_contexts[PIKA_FRONTEND_BLOCK_DEPTH_LIMIT];
    uint32_t try_depth;
    uint8_t caught_status_slots[PIKA_FRONTEND_BLOCK_DEPTH_LIMIT];
    uint32_t caught_depth;
    uint32_t finally_depth;
    PikaImportedModule* imports;
    uint32_t import_capacity;
    uint32_t import_count;
    uint32_t base_import_count;
    uint32_t base_global_count;
    uint32_t base_instruction_count;
    uint32_t base_binding_call_count;
    uint32_t base_builtin_call_count;
    uint32_t base_dynamic_method_call_count;
    uint32_t base_dynamic_method_candidate_count;
    uint32_t base_constant_count;
    uint32_t base_index_site_count;
    uint32_t expression_depth;
    uint32_t function_instruction_start;
    uint16_t current_function;
    int in_function;
    int interactive;
    int interactive_return;
    uint16_t live_slot_floor;
    uint16_t slot_high_water;
} PikaParser;

#define PIKA_FRONTEND_DYNAMIC_TEXT_BYTES 64u

static int is_text_value_kind(PikaValueKind kind) {
    return kind == PIKA_VALUE_STRING || kind == PIKA_VALUE_BYTES ||
           kind == PIKA_VALUE_BYTEARRAY;
}

static int token_matches_text(const PikaParser* parser,
                              const PikaToken* token,
                              const char* text,
                              uint32_t length);

static const PikaToken* token_at(const PikaParser* parser,
                                 uint32_t position) {
    if (position >= parser->token_count) {
        return &parser->tokens[parser->token_count - 1u];
    }
    return &parser->tokens[position];
}

static const PikaToken* current_token(const PikaParser* parser) {
    return token_at(parser, parser->position);
}

static const PikaToken* next_token(const PikaParser* parser) {
    return token_at(parser, parser->position + 1u);
}

static PikaStatus fail_at(PikaParser* parser,
                          PikaStatus status,
                          const PikaToken* token) {
    uint32_t line = 1u;
    uint32_t column = 1u;
    pika_frontend_source_location(
        parser->source, token->offset, &line, &column);
    pika_frontend_set_diagnostic(
        parser->diagnostic, status, line, column);
    return status;
}

static PikaStatus fail_current(PikaParser* parser, PikaStatus status) {
    return fail_at(parser, status, current_token(parser));
}

static PikaStatus fail_limit_at(PikaParser* parser,
                                PikaFrontendBound bound,
                                uint32_t configured,
                                uint32_t observed,
                                const PikaToken* token) {
    uint32_t line = 1u;
    uint32_t column = 1u;
    pika_frontend_source_location(
        parser->source, token->offset, &line, &column);
    pika_frontend_set_limit_diagnostic(
        parser->diagnostic, bound, configured, observed,
        line, column, token->offset);
    return PIKA_STATUS_FRONTEND_LIMIT;
}

static PikaStatus fail_limit_current(PikaParser* parser,
                                     PikaFrontendBound bound,
                                     uint32_t configured,
                                     uint32_t observed) {
    return fail_limit_at(
        parser, bound, configured, observed, current_token(parser));
}

static PikaStatus account_object_result(PikaParser* parser,
                                        uint32_t bytes) {
    if (parser->module->object_slot_estimate == UINT32_MAX ||
        bytes > UINT32_MAX - 16u ||
        parser->module->object_byte_estimate >
            UINT32_MAX - bytes - 16u) {
        return fail_current(parser, PIKA_STATUS_FRONTEND_LIMIT);
    }
    ++parser->module->object_slot_estimate;
    parser->module->object_byte_estimate += bytes + 16u;
    return PIKA_STATUS_OK;
}

static PikaStatus require_capability_at(PikaParser* parser,
                                        PikaCapability capability,
                                        const PikaToken* token) {
    if ((parser->enabled_capabilities & capability) == 0u) {
        return fail_at(
            parser, PIKA_STATUS_UNSUPPORTED_CAPABILITY, token);
    }
    return PIKA_STATUS_OK;
}

static int match(PikaParser* parser, PikaTokenKind kind) {
    if (current_token(parser)->kind != kind) {
        return 0;
    }
    ++parser->position;
    return 1;
}

static PikaStatus require(PikaParser* parser, PikaTokenKind kind) {
    if (!match(parser, kind)) {
        const PikaToken* token = current_token(parser);
        if (parser->interactive &&
            token->kind == PIKA_TOKEN_EOF &&
            kind == PIKA_TOKEN_INDENT) {
            uint32_t line = 1u;
            uint32_t column = 1u;
            pika_frontend_source_location(
                parser->source, token->offset, &line, &column);
            pika_frontend_set_reason_diagnostic(
                parser->diagnostic, PIKA_STATUS_SYNTAX_ERROR,
                PIKA_DIAGNOSTIC_REASON_INCOMPLETE_INPUT,
                line, column, token->offset);
            return PIKA_STATUS_SYNTAX_ERROR;
        }
        return fail_current(parser, PIKA_STATUS_SYNTAX_ERROR);
    }
    return PIKA_STATUS_OK;
}

static PikaStatus allocate_slot(PikaParser* parser, uint8_t* slot) {
    if (parser->next_slot > UINT8_MAX) {
        return fail_current(parser, PIKA_STATUS_FRONTEND_LIMIT);
    }
    *slot = (uint8_t)parser->next_slot++;
    if (parser->next_slot > parser->slot_high_water) {
        parser->slot_high_water = parser->next_slot;
    }
    return PIKA_STATUS_OK;
}

static uint16_t persistent_slot_limit(const PikaParser* parser) {
    uint16_t limit = 0u;
    uint32_t index;
    for (index = 0u; index < parser->symbol_count; ++index) {
        if (parser->symbols[index].is_global != 0u) {
            continue;
        }
        uint16_t end = (uint16_t)parser->symbols[index].slot + 1u;
        if (end > limit) limit = end;
    }
    return limit;
}

static void release_statement_slots(PikaParser* parser) {
    uint16_t limit = persistent_slot_limit(parser);
    if (parser->live_slot_floor > limit) {
        limit = parser->live_slot_floor;
    }
    parser->next_slot = limit;
}

static void* grow_parser_array(PikaParser* parser,
                               void* data,
                               uint32_t* capacity,
                               uint32_t required,
                               uint32_t limit,
                               size_t element_size,
                               PikaStatus* status) {
    uint32_t next;
    void* grown;
    if (required <= *capacity) {
        *status = PIKA_STATUS_OK;
        return data;
    }
    if (required > limit || element_size == 0u) {
        *status = fail_current(
            parser, PIKA_STATUS_FRONTEND_LIMIT);
        return data;
    }
    if (*capacity == 0u) {
        uint32_t estimate = parser->token_count / 128u + 8u;
        next = 8u;
        while (next < estimate && next < limit) {
            next = next > limit / 2u ? limit : next * 2u;
        }
    } else {
        next = *capacity;
    }
    while (next < required) {
        if (next > limit / 2u) {
            next = limit;
            break;
        }
        next *= 2u;
    }
    if ((size_t)next > SIZE_MAX / element_size) {
        *status = fail_current(
            parser, PIKA_STATUS_FRONTEND_LIMIT);
        return data;
    }
    grown = realloc(data, (size_t)next * element_size);
    if (grown == NULL) {
        *status = fail_current(
            parser, PIKA_STATUS_STORAGE_TOO_SMALL);
        return data;
    }
    memset(
        (uint8_t*)grown + (size_t)(*capacity) * element_size,
        0, (size_t)(next - *capacity) * element_size);
    *capacity = next;
    *status = PIKA_STATUS_OK;
    return grown;
}

static uint32_t parser_count_limit(uint32_t base,
                                   uint32_t additional) {
    return base > UINT32_MAX - additional
               ? UINT32_MAX
               : base + additional;
}

static int count_header_parameters(
    const PikaParser* parser,
    uint32_t start,
    uint32_t* parameter_count) {
    uint32_t cursor = start;
    uint32_t parenthesis_depth = 1u;
    uint32_t bracket_depth = 0u;
    uint32_t brace_depth = 0u;
    uint32_t count = 0u;
    int has_content = 0;
    while (cursor < parser->token_count) {
        PikaTokenKind kind = parser->tokens[cursor].kind;
        if (kind == PIKA_TOKEN_EOF) return 0;
        if (kind == PIKA_TOKEN_LEFT_PAREN) {
            ++parenthesis_depth;
            has_content = 1;
        } else if (kind == PIKA_TOKEN_RIGHT_PAREN) {
            if (parenthesis_depth == 1u &&
                bracket_depth == 0u && brace_depth == 0u) {
                if (has_content != 0) ++count;
                *parameter_count = count;
                return 1;
            }
            if (parenthesis_depth == 0u) return 0;
            --parenthesis_depth;
            has_content = 1;
        } else if (kind == PIKA_TOKEN_LEFT_BRACKET) {
            ++bracket_depth;
            has_content = 1;
        } else if (kind == PIKA_TOKEN_RIGHT_BRACKET) {
            if (bracket_depth == 0u) return 0;
            --bracket_depth;
            has_content = 1;
        } else if (kind == PIKA_TOKEN_LEFT_BRACE) {
            ++brace_depth;
            has_content = 1;
        } else if (kind == PIKA_TOKEN_RIGHT_BRACE) {
            if (brace_depth == 0u) return 0;
            --brace_depth;
            has_content = 1;
        } else if (kind == PIKA_TOKEN_COMMA &&
                   parenthesis_depth == 1u &&
                   bracket_depth == 0u && brace_depth == 0u) {
            if (has_content != 0) ++count;
            has_content = 0;
        } else {
            has_content = 1;
        }
        ++cursor;
    }
    return 0;
}

static uint32_t count_lambda_parameters(const PikaParser* parser,
                                        uint32_t start) {
    uint32_t count = 0u;
    uint32_t cursor = start + 1u;
    while (cursor < parser->token_count &&
           parser->tokens[cursor].kind != PIKA_TOKEN_COLON) {
        if (parser->tokens[cursor].kind == PIKA_TOKEN_NAME) ++count;
        ++cursor;
    }
    return count;
}

static PikaStatus allocate_parameter_declarations(
    PikaParser* parser) {
    uint32_t capacity = 0u;
    uint32_t index;
    const uint32_t limit =
        PIKA_FRONTEND_FUNCTION_LIMIT *
        PIKA_FRONTEND_PARAMETER_LIMIT;
    for (index = 0u; index + 2u < parser->token_count; ++index) {
        uint32_t count = 0u;
        if (parser->tokens[index].kind == PIKA_TOKEN_LAMBDA) {
            count = count_lambda_parameters(parser, index);
            if (count > limit - capacity) {
                capacity = limit;
                break;
            }
            capacity += count;
            continue;
        }
        if (parser->tokens[index].kind != PIKA_TOKEN_DEF ||
            parser->tokens[index + 2u].kind !=
                PIKA_TOKEN_LEFT_PAREN) {
            continue;
        }
        if (!count_header_parameters(parser, index + 3u, &count)) {
            count = PIKA_FRONTEND_PARAMETER_LIMIT;
        }
        if (count > limit - capacity) {
            capacity = limit;
            break;
        }
        capacity += count;
    }
    if (capacity == 0u) return PIKA_STATUS_OK;
    parser->parameter_declarations =
        (PikaParameterDeclaration*)calloc(
            capacity, sizeof(*parser->parameter_declarations));
    if (parser->parameter_declarations == NULL) {
        return fail_current(
            parser, PIKA_STATUS_STORAGE_TOO_SMALL);
    }
    parser->parameter_declaration_capacity = capacity;
    return PIKA_STATUS_OK;
}

static PikaStatus reserve_parameter_declarations(
    PikaParser* parser,
    uint32_t count,
    PikaParameterDeclaration** declarations) {
    if (parser->parameter_declaration_count >
            parser->parameter_declaration_capacity ||
        count > parser->parameter_declaration_capacity -
                    parser->parameter_declaration_count) {
        return fail_current(
            parser, PIKA_STATUS_INTERNAL_ERROR);
    }
    *declarations = count == 0u
                        ? NULL
                        : &parser->parameter_declarations[
                              parser->parameter_declaration_count];
    parser->parameter_declaration_count += count;
    return PIKA_STATUS_OK;
}

static PikaStatus reserve_class_fields(
    PikaParser* parser,
    PikaClassDeclaration* declaration,
    uint32_t required) {
    uint32_t capacity;
    PikaFieldDeclaration* fields;
    if (required <= declaration->field_capacity) {
        return PIKA_STATUS_OK;
    }
    if (required > PIKA_FRONTEND_FIELD_LIMIT) {
        return fail_current(
            parser, PIKA_STATUS_FRONTEND_LIMIT);
    }
    capacity = declaration->field_capacity == 0u
                   ? (PIKA_FRONTEND_FIELD_LIMIT < 4u
                          ? PIKA_FRONTEND_FIELD_LIMIT
                          : 4u)
                   : declaration->field_capacity;
    while (capacity < required) {
        capacity = capacity > PIKA_FRONTEND_FIELD_LIMIT / 2u
                       ? PIKA_FRONTEND_FIELD_LIMIT
                       : capacity * 2u;
    }
    fields = (PikaFieldDeclaration*)realloc(
        declaration->fields,
        (size_t)capacity * sizeof(*declaration->fields));
    if (fields == NULL) {
        return fail_current(
            parser, PIKA_STATUS_STORAGE_TOO_SMALL);
    }
    memset(
        &fields[declaration->field_capacity], 0,
        (size_t)(capacity - declaration->field_capacity) *
            sizeof(*fields));
    declaration->fields = fields;
    declaration->field_capacity = (uint16_t)capacity;
    return PIKA_STATUS_OK;
}

static PikaStatus emit_instruction(PikaParser* parser,
                                   PikaOpcode opcode,
                                   uint8_t a,
                                   uint8_t b,
                                   uint8_t c,
                                   int32_t immediate,
                                   uint32_t* index) {
    PikaCompiledModule* module = parser->module;
    PikaStatus status;
    uint32_t generated_limit = parser->token_count * 4u + 16u;
    uint32_t limit = parser_count_limit(
        parser->base_instruction_count, generated_limit);
    uint32_t storage_count;
    PikaInstruction* instruction;
    if (parser->in_function &&
        module->program.instruction_count -
                parser->function_instruction_start >=
            PIKA_FRONTEND_FUNCTION_INSTRUCTION_LIMIT) {
        return fail_limit_current(
            parser, PIKA_FRONTEND_BOUND_FUNCTION_INSTRUCTIONS,
            PIKA_FRONTEND_FUNCTION_INSTRUCTION_LIMIT,
            PIKA_FRONTEND_FUNCTION_INSTRUCTION_LIMIT + 1u);
    }
    if (module->program.instruction_count <
            module->instruction_storage_base ||
        module->program.instruction_count >= limit) {
        return fail_current(
            parser, PIKA_STATUS_FRONTEND_LIMIT);
    }
    storage_count =
        module->program.instruction_count -
        module->instruction_storage_base;
    module->instructions = (PikaInstruction*)grow_parser_array(
        parser, module->instructions,
        &module->instruction_capacity,
        storage_count + 1u,
        generated_limit, sizeof(*module->instructions), &status);
    if (status != PIKA_STATUS_OK) return status;
    if (index != NULL) {
        *index = module->program.instruction_count;
    }
    instruction = &module->instructions[storage_count];
    ++module->program.instruction_count;
    instruction->opcode = (uint8_t)opcode;
    instruction->a = a;
    instruction->b = b;
    instruction->c = c;
    instruction->immediate = immediate;
    return PIKA_STATUS_OK;
}

static PikaInstruction* mutable_instruction_at(
    PikaParser* parser,
    uint32_t index) {
    PikaCompiledModule* module = parser->module;
    uint32_t storage_index;
    if (index < module->instruction_storage_base ||
        index >= module->program.instruction_count) {
        return NULL;
    }
    storage_index =
        index - module->instruction_storage_base;
    if (module->instructions == NULL ||
        storage_index >= module->instruction_capacity) {
        return NULL;
    }
    return &module->instructions[storage_index];
}

static PikaStatus set_instruction_immediate(
    PikaParser* parser,
    uint32_t index,
    int32_t immediate) {
    PikaInstruction* instruction =
        mutable_instruction_at(parser, index);
    if (instruction == NULL) {
        return fail_current(
            parser, PIKA_STATUS_INTERNAL_ERROR);
    }
    instruction->immediate = immediate;
    return PIKA_STATUS_OK;
}

static int token_text_equal(const PikaParser* parser,
                            const PikaToken* token,
                            const PikaSymbol* symbol) {
    const char* source = symbol->name_source != NULL
                             ? symbol->name_source
                             : parser->source;
    return token->length == symbol->length &&
           memcmp(&parser->source[token->offset],
                  &source[symbol->offset],
                  token->length) == 0;
}

static int tokens_text_equal(const PikaParser* parser,
                             const PikaToken* left,
                             const PikaToken* right) {
    return left->length == right->length &&
           memcmp(&parser->source[left->offset],
                  &parser->source[right->offset],
                  left->length) == 0;
}

static int token_matches_stable(
    const PikaParser* parser,
    const PikaToken* token,
    const char* stable_name,
    uint16_t stable_length,
    uint32_t name_position) {
    if (stable_name != NULL) {
        return token->length == stable_length &&
               memcmp(&parser->source[token->offset],
                      stable_name, token->length) == 0;
    }
    return tokens_text_equal(
        parser, token, token_at(parser, name_position));
}

static int token_matches_method_stable(
    const PikaParser* parser,
    const PikaToken* token,
    const char* stable_name,
    uint16_t stable_length,
    uint32_t name_position) {
    if (stable_name == NULL) {
        return tokens_text_equal(
            parser, token, token_at(parser, name_position));
    }
    if (token->length == stable_length &&
        memcmp(&parser->source[token->offset], stable_name,
               token->length) == 0) {
        return 1;
    }
    return stable_length > token->length &&
           stable_name[stable_length - token->length - 1u] == '_' &&
           memcmp(
               &stable_name[stable_length - token->length],
               &parser->source[token->offset], token->length) == 0;
}

static int semantic_name_valid(const PikaCompiledModule* module,
                               uint32_t offset,
                               uint16_t length) {
    return module->semantic_names != NULL &&
           offset <= module->semantic_name_count &&
           (uint32_t)length <=
               module->semantic_name_count - offset;
}

static int semantic_shape_range_valid(
    const PikaCompiledModule* module,
    uint32_t first,
    uint16_t count) {
    return count == 0u ||
           (module->semantic_shapes != NULL &&
            first <= module->semantic_shape_count &&
            (uint32_t)count <=
                module->semantic_shape_count - first);
}

static PikaStatus seed_semantic_catalog(PikaParser* parser) {
    PikaCompiledModule* module = parser->module;
    uint32_t index;
    PikaStatus status;
    for (index = 0u;
         index < module->semantic_function_count;
         ++index) {
        const PikaSemanticFunction* semantic =
            &module->semantic_functions[index];
        PikaFunctionDeclaration* declaration =
            &parser->declarations[index];
        uint32_t parameter_index;
        if (!semantic_name_valid(
                module, semantic->name_offset,
                semantic->name_length) ||
            (uint32_t)semantic->first_parameter +
                    semantic->parameter_count >
                module->semantic_parameter_count ||
            (semantic->variadic_parameter != UINT8_MAX &&
             semantic->variadic_parameter >=
                 semantic->parameter_count) ||
            semantic->has_return_shape > 1u ||
            (semantic->has_return_shape != 0u &&
             !semantic_shape_range_valid(
                 module,
                 semantic->first_return_element_shape,
                 semantic->return_element_shape_count))) {
            return fail_current(
                parser, PIKA_STATUS_INTERNAL_ERROR);
        }
        memset(declaration, 0, sizeof(*declaration));
        declaration->header_position = UINT32_MAX;
        declaration->name_position = UINT32_MAX;
        declaration->after_body = UINT32_MAX;
        declaration->stable_name =
            &module->semantic_names[semantic->name_offset];
        declaration->stable_name_length =
            semantic->name_length;
        declaration->function_index = (uint16_t)index;
        declaration->parameter_count =
            semantic->parameter_count;
        declaration->required_parameter_count =
            semantic->required_parameter_count;
        declaration->variadic_parameter =
            semantic->variadic_parameter;
        declaration->owner_class = semantic->owner_class;
        declaration->return_shape =
            semantic->return_shape;
        declaration->first_return_element_shape =
            semantic->first_return_element_shape;
        declaration->return_element_shape_count =
            semantic->return_element_shape_count;
        declaration->has_return_shape =
            semantic->has_return_shape;
        declaration->parameters = NULL;
        for (parameter_index = 0u;
             parameter_index < semantic->parameter_count;
             ++parameter_index) {
            const PikaSemanticParameter* parameter =
                &module->semantic_parameters[
                    semantic->first_parameter + parameter_index];
            if (!semantic_name_valid(
                    module, parameter->name_offset,
                    parameter->name_length) ||
                (parameter->default_constant != UINT16_MAX &&
                 parameter->default_constant >=
                     module->constant_count)) {
                return fail_current(
                    parser, PIKA_STATUS_INTERNAL_ERROR);
            }
        }
    }
    parser->declaration_count =
        module->semantic_function_count;
    parser->base_declaration_count =
        parser->declaration_count;

    for (index = 0u;
         index < module->semantic_class_count;
         ++index) {
        const PikaSemanticClass* semantic =
            &module->semantic_classes[index];
        PikaClassDeclaration* declaration =
            &parser->classes[index];
        uint32_t field_index;
        if (!semantic_name_valid(
                module, semantic->name_offset,
                semantic->name_length) ||
            (uint32_t)semantic->first_field +
                    semantic->field_count >
                module->semantic_field_count) {
            return fail_current(
                parser, PIKA_STATUS_INTERNAL_ERROR);
        }
        memset(declaration, 0, sizeof(*declaration));
        declaration->header_position = UINT32_MAX;
        declaration->name_position = UINT32_MAX;
        declaration->after_body = UINT32_MAX;
        declaration->stable_name =
            &module->semantic_names[semantic->name_offset];
        declaration->stable_name_length =
            semantic->name_length;
        declaration->class_index = (uint16_t)index;
        declaration->base_class = semantic->base_class;
        status = reserve_class_fields(
            parser, declaration, semantic->field_count);
        if (status != PIKA_STATUS_OK) return status;
        declaration->field_count = semantic->field_count;
        declaration->member_count = semantic->member_count;
        for (field_index = 0u;
             field_index < semantic->field_count;
             ++field_index) {
            const PikaSemanticField* field =
                &module->semantic_fields[
                    semantic->first_field + field_index];
            PikaFieldDeclaration* target =
                &declaration->fields[field_index];
            if (!semantic_name_valid(
                    module, field->name_offset,
                    field->name_length)) {
                return fail_current(
                    parser, PIKA_STATUS_INTERNAL_ERROR);
            }
            target->name_position = UINT32_MAX;
            target->stable_name =
                &module->semantic_names[field->name_offset];
            target->stable_name_length =
                field->name_length;
            target->kind = field->kind;
            target->binding_kind = field->binding_kind;
            target->allows_none = field->allows_none;
            target->binding_class = field->binding_class;
            target->class_index = field->class_index;
        }
    }
    parser->class_count = module->semantic_class_count;
    parser->base_class_count = parser->class_count;

    for (index = 0u;
         index < module->semantic_global_count;
         ++index) {
        const PikaSemanticGlobal* semantic =
            &module->semantic_globals[index];
        PikaSymbol* global = &parser->module_globals[index];
        if (!semantic_name_valid(
                module, semantic->name_offset,
                semantic->name_length) ||
            semantic->slot != index ||
            !semantic_shape_range_valid(
                module, semantic->first_element_shape,
                semantic->element_shape_count)) {
            return fail_current(
                parser, PIKA_STATUS_INTERNAL_ERROR);
        }
        memset(global, 0, sizeof(*global));
        global->name_source = module->semantic_names;
        global->offset = semantic->name_offset;
        global->length = semantic->name_length;
        global->slot = semantic->slot;
        global->kind = (PikaValueKind)semantic->kind;
        global->binding_kind =
            (PikaBindingValueKind)semantic->binding_kind;
        global->binding_class = semantic->binding_class;
        global->class_index = semantic->class_index;
        global->first_element_shape =
            semantic->first_element_shape;
        global->element_shape_count =
            semantic->element_shape_count;
        global->initialized = 1u;
        global->global_slot = semantic->slot;
    }
    parser->module_global_count =
        module->semantic_global_count;
    parser->base_global_count =
        parser->module_global_count;

    for (index = 0u;
         index < module->semantic_import_count;
         ++index) {
        const PikaSemanticImport* semantic =
            &module->semantic_imports[index];
        PikaImportedModule* imported =
            &parser->imports[index];
        if (!semantic_name_valid(
                module, semantic->name_offset,
                semantic->name_length) ||
            semantic->module == NULL) {
            return fail_current(
                parser, PIKA_STATUS_INTERNAL_ERROR);
        }
        imported->name_source = module->semantic_names;
        imported->offset = semantic->name_offset;
        imported->length = semantic->name_length;
        imported->module = semantic->module;
        imported->selected = semantic->selected;
    }
    parser->import_count = module->semantic_import_count;
    parser->base_import_count = parser->import_count;
    for (index = 0u;
         index < module->semantic_python_import_count;
         ++index) {
        const PikaPythonModule* semantic =
            &module->semantic_python_imports[index];
        if (!semantic_name_valid(
                module, semantic->name_offset,
                semantic->name_length) ||
            semantic->target_module == 0u ||
            semantic->target_module >=
                PIKA_FRONTEND_MODULE_LIMIT) {
            return fail_current(
                parser, PIKA_STATUS_INTERNAL_ERROR);
        }
    }
    return PIKA_STATUS_OK;
}

static PikaStatus copy_token_text(PikaParser* parser,
                                  const PikaToken* token,
                                  char* buffer,
                                  uint32_t capacity) {
    if (token->length + 1u > capacity) {
        return fail_at(
            parser, PIKA_STATUS_FRONTEND_LIMIT, token);
    }
    memcpy(buffer, &parser->source[token->offset], token->length);
    buffer[token->length] = '\0';
    return PIKA_STATUS_OK;
}

static int integer_literal_is_embedded(
    const PikaParser* parser,
    const PikaToken* token) {
    uint32_t index;
    if (token->length > 2u &&
        parser->source[token->offset] == '0' &&
        (parser->source[token->offset + 1u] == 'x' ||
         parser->source[token->offset + 1u] == 'X' ||
         parser->source[token->offset + 1u] == 'b' ||
         parser->source[token->offset + 1u] == 'B' ||
         parser->source[token->offset + 1u] == 'o' ||
         parser->source[token->offset + 1u] == 'O')) {
        return 1;
    }
    for (index = 0u; index < token->length; ++index) {
        if (parser->source[token->offset + index] == '_') {
            return 1;
        }
    }
    return 0;
}

#if PIKA_BINDING_RUNTIME_ENABLE
static const PikaImportedModule* find_import(
    const PikaParser* parser,
    const PikaToken* token) {
    uint32_t index;
    for (index = 0u; index < parser->import_count; ++index) {
        const PikaImportedModule* imported = &parser->imports[index];
        const char* source = imported->name_source != NULL
                                 ? imported->name_source
                                 : parser->source;
        if (token->length == imported->length &&
            memcmp(&parser->source[token->offset],
                   &source[imported->offset],
                   token->length) == 0) {
            return imported;
        }
    }
    return NULL;
}

static PikaStatus append_binding_call(
    PikaParser* parser,
    const PikaBindingCallSite* call,
    uint16_t* index) {
    PikaCompiledModule* module = parser->module;
    PikaStatus status;
    if (module->binding_call_count >= UINT16_MAX) {
        return fail_current(parser, PIKA_STATUS_FRONTEND_LIMIT);
    }
    module->binding_calls = (PikaBindingCallSite*)grow_parser_array(
        parser, module->binding_calls,
        &module->binding_call_capacity,
        module->binding_call_count + 1u,
        parser_count_limit(
            parser->base_binding_call_count,
            parser->token_count),
        sizeof(*module->binding_calls), &status);
    if (status != PIKA_STATUS_OK) return status;
    *index = (uint16_t)module->binding_call_count;
    module->binding_calls[module->binding_call_count++] = *call;
    return PIKA_STATUS_OK;
}
#endif

#if PIKA_CAPABILITY_MODULE_IMPORT_ENABLE
static const PikaPythonModule* find_python_import(
    const PikaParser* parser,
    const PikaToken* token) {
    const PikaCompiledModule* module = parser->module;
    uint32_t index;
    for (index = 0u;
         index < module->semantic_python_import_count;
         ++index) {
        const PikaPythonModule* imported =
            &module->semantic_python_imports[index];
        if (semantic_name_valid(
                module, imported->name_offset,
                imported->name_length) &&
            token->length == imported->name_length &&
            memcmp(
                &parser->source[token->offset],
                &module->semantic_names[imported->name_offset],
                token->length) == 0) {
            return imported;
        }
    }
    return NULL;
}
#endif

static void mark_typed(PikaParser* parser) {
    parser->module->program.uses_typed_values = 1u;
}

static PikaStatus append_builtin_call(PikaParser* parser,
                                      PikaBuiltinId builtin,
                                      uint8_t argument_count,
                                      PikaValueKind result_kind,
                                      uint16_t* index) {
    PikaCompiledModule* module = parser->module;
    PikaBuiltinCallSite* site;
    PikaStatus status;
    if (module->builtin_call_count >= UINT16_MAX) {
        return fail_current(parser, PIKA_STATUS_FRONTEND_LIMIT);
    }
    module->builtin_calls = (PikaBuiltinCallSite*)grow_parser_array(
        parser, module->builtin_calls,
        &module->builtin_call_capacity,
        module->builtin_call_count + 1u,
        parser_count_limit(
            parser->base_builtin_call_count,
            parser->token_count),
        sizeof(*module->builtin_calls), &status);
    if (status != PIKA_STATUS_OK) return status;
    *index = (uint16_t)module->builtin_call_count;
    site = &module->builtin_calls[module->builtin_call_count++];
    site->builtin_id = (uint16_t)builtin;
    site->argument_count = argument_count;
    site->result_kind = (uint8_t)result_kind;
    return PIKA_STATUS_OK;
}

static PikaStatus append_constant(PikaParser* parser,
                                  const PikaConstant* constant,
                                  uint16_t* index) {
    PikaCompiledModule* module = parser->module;
    PikaStatus status;
    if (module->constant_count >= UINT16_MAX) {
        return fail_current(parser, PIKA_STATUS_FRONTEND_LIMIT);
    }
    module->constants = (PikaConstant*)grow_parser_array(
        parser, module->constants, &module->constant_capacity,
        module->constant_count + 1u,
        parser_count_limit(
            parser->base_constant_count,
            parser->token_count),
        sizeof(*module->constants), &status);
    if (status != PIKA_STATUS_OK) return status;
    *index = (uint16_t)module->constant_count;
    module->constants[module->constant_count++] = *constant;
    return PIKA_STATUS_OK;
}

static PikaStatus append_index_site(PikaParser* parser,
                                    const PikaIndexSite* site,
                                    uint16_t* index) {
    PikaCompiledModule* module = parser->module;
    PikaStatus status;
    if (module->index_site_count >= UINT16_MAX) {
        return fail_current(parser, PIKA_STATUS_FRONTEND_LIMIT);
    }
    module->index_sites = (PikaIndexSite*)grow_parser_array(
        parser, module->index_sites, &module->index_site_capacity,
        module->index_site_count + 1u,
        parser_count_limit(
            parser->base_index_site_count,
            parser->token_count),
        sizeof(*module->index_sites), &status);
    if (status != PIKA_STATUS_OK) return status;
    *index = (uint16_t)module->index_site_count;
    module->index_sites[module->index_site_count++] = *site;
    return PIKA_STATUS_OK;
}

static PikaStatus append_exception_handler(
    PikaParser* parser,
    uint8_t status_slot,
    PikaExceptionKind catch_kind,
    uint32_t first_instruction,
    uint32_t instruction_count,
    uint32_t handler_instruction) {
    PikaExceptionHandler* handler;
    PikaStatus status;
    if (instruction_count == 0u) {
        return PIKA_STATUS_OK;
    }
    if (parser->module->exception_handler_count >=
        PIKA_FRONTEND_EXCEPTION_HANDLER_LIMIT) {
        return fail_limit_current(
            parser, PIKA_FRONTEND_BOUND_EXCEPTION_HANDLERS,
            PIKA_FRONTEND_EXCEPTION_HANDLER_LIMIT,
            parser->module->exception_handler_count + 1u);
    }
    parser->module->exception_handlers =
        (PikaExceptionHandler*)grow_parser_array(
            parser, parser->module->exception_handlers,
            &parser->module->exception_handler_capacity,
            parser->module->exception_handler_count + 1u,
            PIKA_FRONTEND_EXCEPTION_HANDLER_LIMIT,
            sizeof(*parser->module->exception_handlers), &status);
    if (status != PIKA_STATUS_OK) return status;
    handler = &parser->module->exception_handlers[
        parser->module->exception_handler_count++];
    handler->function_index = parser->current_function;
    handler->status_slot = status_slot;
    handler->catch_kind = (uint8_t)catch_kind;
    handler->first_instruction = first_instruction;
    handler->instruction_count = instruction_count;
    handler->handler_instruction = handler_instruction;
    return PIKA_STATUS_OK;
}

static PikaStatus set_expression_kind(PikaExpression* expression,
                                      PikaValueKind kind) {
    expression->kind = kind;
    expression->binding_class = NULL;
    expression->class_index = UINT16_MAX;
    expression->first_element_shape = 0u;
    expression->element_shape_count = 0u;
    expression->binding_kind =
        kind == PIKA_VALUE_BOOLEAN ? PIKA_BINDING_VALUE_BOOLEAN
                                   : PIKA_BINDING_VALUE_INTEGER;
    if (kind == PIKA_VALUE_FLOAT) {
        expression->binding_kind = PIKA_BINDING_VALUE_FLOAT;
    } else if (kind == PIKA_VALUE_STRING) {
        expression->binding_kind = PIKA_BINDING_VALUE_STRING;
    } else if (kind == PIKA_VALUE_BYTES) {
        expression->binding_kind = PIKA_BINDING_VALUE_BYTES;
    } else if (kind == PIKA_VALUE_BYTEARRAY) {
        expression->binding_kind = PIKA_BINDING_VALUE_BYTEARRAY;
    } else if (kind == PIKA_VALUE_LIST) {
        expression->binding_kind = PIKA_BINDING_VALUE_LIST;
    } else if (kind == PIKA_VALUE_TUPLE) {
        expression->binding_kind = PIKA_BINDING_VALUE_TUPLE;
    } else if (kind == PIKA_VALUE_DICT) {
        expression->binding_kind = PIKA_BINDING_VALUE_DICT;
    } else if (kind != PIKA_VALUE_INTEGER &&
               kind != PIKA_VALUE_BOOLEAN) {
        expression->binding_kind = PIKA_BINDING_VALUE_ANY;
    }
    return PIKA_STATUS_OK;
}

static PikaStaticShape static_shape_from_expression(
    const PikaExpression* expression) {
    PikaStaticShape shape;
    shape.binding_class = expression->binding_class;
    shape.class_index = expression->class_index;
    shape.kind = (uint8_t)expression->kind;
    shape.binding_kind =
        (uint8_t)expression->binding_kind;
    return shape;
}

static void set_expression_from_static_shape(
    PikaExpression* expression,
    const PikaStaticShape* shape,
    uint32_t first_element_shape,
    uint16_t element_shape_count) {
    expression->kind = (PikaValueKind)shape->kind;
    expression->binding_kind =
        (PikaBindingValueKind)shape->binding_kind;
    expression->binding_class = shape->binding_class;
    expression->class_index = shape->class_index;
    expression->first_element_shape =
        first_element_shape;
    expression->element_shape_count =
        element_shape_count;
}

static void set_unknown_static_shape(PikaStaticShape* shape) {
    memset(shape, 0, sizeof(*shape));
    shape->kind = (uint8_t)PIKA_VALUE_UNKNOWN;
    shape->binding_kind =
        (uint8_t)PIKA_BINDING_VALUE_ANY;
    shape->class_index = UINT16_MAX;
}

static int static_shapes_equal(
    const PikaStaticShape* left,
    const PikaStaticShape* right) {
    return left->kind == right->kind &&
           left->binding_kind == right->binding_kind &&
           left->binding_class == right->binding_class &&
           left->class_index == right->class_index;
}

static PikaStatus append_expression_element_shapes(
    PikaParser* parser,
    const PikaExpression* elements,
    uint32_t count,
    uint32_t* first_shape) {
    PikaCompiledModule* module = parser->module;
    uint32_t required;
    uint32_t capacity;
    uint32_t index;
    if (first_shape == NULL ||
        count > UINT16_MAX ||
        count > UINT32_MAX - module->semantic_shape_count) {
        return fail_current(
            parser, PIKA_STATUS_FRONTEND_LIMIT);
    }
    *first_shape = module->semantic_shape_count;
    if (count == 0u) return PIKA_STATUS_OK;
    required = module->semantic_shape_count + count;
    if (required > module->semantic_shape_capacity) {
        PikaStaticShape* shapes;
        capacity =
            module->semantic_shape_capacity == 0u
                ? 16u
                : module->semantic_shape_capacity;
        while (capacity < required) {
            if (capacity > UINT32_MAX / 2u) {
                capacity = required;
                break;
            }
            capacity *= 2u;
        }
#if SIZE_MAX <= UINT32_MAX
        if (capacity >
            SIZE_MAX / sizeof(PikaStaticShape)) {
            return fail_current(
                parser, PIKA_STATUS_FRONTEND_LIMIT);
        }
#endif
        shapes = (PikaStaticShape*)realloc(
            module->semantic_shapes,
            (size_t)capacity * sizeof(PikaStaticShape));
        if (shapes == NULL) {
            return fail_current(
                parser, PIKA_STATUS_FRONTEND_LIMIT);
        }
        module->semantic_shapes = shapes;
        module->semantic_shape_capacity = capacity;
    }
    for (index = 0u; index < count; ++index) {
        module->semantic_shapes[
            module->semantic_shape_count++] =
            static_shape_from_expression(&elements[index]);
    }
    return PIKA_STATUS_OK;
}

static int expression_shape_matches_return(
    const PikaCompiledModule* module,
    const PikaExpression* expression,
    const PikaFunctionDeclaration* declaration) {
    PikaStaticShape shape =
        static_shape_from_expression(expression);
    uint32_t index;
    if (!static_shapes_equal(
            &shape, &declaration->return_shape) ||
        expression->element_shape_count !=
            declaration->return_element_shape_count ||
        !semantic_shape_range_valid(
            module, expression->first_element_shape,
            expression->element_shape_count) ||
        !semantic_shape_range_valid(
            module,
            declaration->first_return_element_shape,
            declaration->return_element_shape_count)) {
        return 0;
    }
    for (index = 0u;
         index < expression->element_shape_count;
         ++index) {
        if (!static_shapes_equal(
                &module->semantic_shapes[
                    expression->first_element_shape + index],
                &module->semantic_shapes[
                    declaration->first_return_element_shape +
                    index])) {
            return 0;
        }
    }
    return 1;
}

static PikaStatus record_return_shape(
    PikaParser* parser,
    const PikaExpression* expression) {
    PikaFunctionDeclaration* declaration;
    if (parser->current_function >=
        parser->declaration_count) {
        return fail_current(
            parser, PIKA_STATUS_INTERNAL_ERROR);
    }
    if (!semantic_shape_range_valid(
            parser->module,
            expression->first_element_shape,
            expression->element_shape_count)) {
        return fail_current(
            parser, PIKA_STATUS_INTERNAL_ERROR);
    }
    declaration =
        &parser->declarations[parser->current_function];
    if (declaration->has_return_shape == 0u) {
        declaration->return_shape =
            static_shape_from_expression(expression);
        declaration->first_return_element_shape =
            expression->first_element_shape;
        declaration->return_element_shape_count =
            expression->element_shape_count;
        declaration->has_return_shape = 1u;
        return PIKA_STATUS_OK;
    }
    if (!expression_shape_matches_return(
            parser->module, expression, declaration)) {
        set_unknown_static_shape(
            &declaration->return_shape);
        declaration->first_return_element_shape = 0u;
        declaration->return_element_shape_count = 0u;
    }
    return PIKA_STATUS_OK;
}

static void set_expression_from_return_shape(
    PikaExpression* expression,
    const PikaFunctionDeclaration* declaration) {
    if (declaration->has_return_shape == 0u) {
        set_expression_kind(
            expression, PIKA_VALUE_UNKNOWN);
        return;
    }
    set_expression_from_static_shape(
        expression, &declaration->return_shape,
        declaration->first_return_element_shape,
        declaration->return_element_shape_count);
}

static const PikaFunctionDeclaration* find_function(
    const PikaParser* parser,
    const PikaToken* name) {
    uint32_t index;
    for (index = parser->declaration_count; index > 0u;) {
        --index;
        const PikaFunctionDeclaration* declaration =
            &parser->declarations[index];
        if (declaration->owner_class == UINT16_MAX &&
            token_matches_stable(
                parser, name, declaration->stable_name,
                declaration->stable_name_length,
                declaration->name_position)) {
            return declaration;
        }
    }
    return NULL;
}

static const PikaClassDeclaration* find_class(
    const PikaParser* parser,
    const PikaToken* name) {
    uint32_t index;
    for (index = parser->class_count; index > 0u;) {
        --index;
        const PikaClassDeclaration* declaration = &parser->classes[index];
        if (token_matches_stable(
                parser, name, declaration->stable_name,
                declaration->stable_name_length,
                declaration->name_position)) {
            return declaration;
        }
        if (name->length > 4u &&
            (unsigned char)parser->source[name->offset] == 95u &&
            (unsigned char)parser->source[name->offset + 1u] == 95u &&
            (unsigned char)parser->source[name->offset + 2u] == 112u) {
            uint32_t prefix = 3u;
            while (prefix < name->length &&
                   (unsigned char)parser->source[name->offset + prefix] >= 48u &&
                   (unsigned char)parser->source[name->offset + prefix] <= 57u) {
                ++prefix;
            }
            if (prefix < name->length &&
                (unsigned char)parser->source[name->offset + prefix] == 95u) {
                PikaToken original = *name;
                ++prefix;
                original.offset += prefix;
                original.length = (uint16_t)(name->length - prefix);
                if (token_matches_stable(
                        parser, &original, declaration->stable_name,
                        declaration->stable_name_length,
                        declaration->name_position)) {
                    return declaration;
                }
            }
        }
    }
    return NULL;
}

#if PIKA_CAPABILITY_OBJECT_ATTRIBUTE_ENABLE
static const PikaFunctionDeclaration* find_stable_function(
    const PikaParser* parser,
    const char* name,
    uint16_t length) {
    uint32_t index;
    for (index = parser->declaration_count; index > 0u;) {
        const PikaFunctionDeclaration* declaration;
        --index;
        declaration = &parser->declarations[index];
        if (declaration->owner_class == UINT16_MAX &&
            declaration->stable_name != NULL &&
            declaration->stable_name_length == length &&
            memcmp(declaration->stable_name, name, length) == 0) {
            return declaration;
        }
    }
    return NULL;
}

static const PikaClassDeclaration* find_stable_class(
    const PikaParser* parser,
    const char* name,
    uint16_t length) {
    uint32_t index;
    for (index = parser->class_count; index > 0u;) {
        const PikaClassDeclaration* declaration;
        --index;
        declaration = &parser->classes[index];
        if (declaration->stable_name != NULL &&
            declaration->stable_name_length == length &&
            memcmp(declaration->stable_name, name, length) == 0) {
            return declaration;
        }
    }
    return NULL;
}
#endif

#if PIKA_CAPABILITY_OBJECT_ATTRIBUTE_ENABLE
static const PikaFunctionDeclaration* find_method(
    const PikaParser* parser,
    uint16_t class_index,
    const PikaToken* name) {
    while (class_index != UINT16_MAX) {
        uint32_t index;
        for (index = parser->declaration_count; index > 0u;) {
            --index;
            const PikaFunctionDeclaration* declaration =
                &parser->declarations[index];
            if (declaration->owner_class == class_index &&
                token_matches_method_stable(
                    parser, name, declaration->stable_name,
                    declaration->stable_name_length,
                    declaration->name_position)) {
                return declaration;
            }
        }
        class_index = parser->classes[class_index].base_class;
    }
    return NULL;
}

static const PikaFunctionDeclaration* find_unambiguous_method(
    const PikaParser* parser,
    const PikaToken* name,
    int* ambiguous) {
    const PikaFunctionDeclaration* candidate = NULL;
    uint32_t index;
    *ambiguous = 0;
    for (index = 0u; index < parser->declaration_count; ++index) {
        const PikaFunctionDeclaration* declaration =
            &parser->declarations[index];
        if (declaration->owner_class == UINT16_MAX ||
            !token_matches_method_stable(
                parser, name, declaration->stable_name,
                declaration->stable_name_length,
                declaration->name_position)) {
            continue;
        }
        if (candidate != NULL &&
            candidate->function_index != declaration->function_index) {
            *ambiguous = 1;
            return NULL;
        }
        candidate = declaration;
    }
    return candidate;
}

static uint32_t find_method_candidates(
    const PikaParser* parser,
    const PikaToken* name,
    const PikaFunctionDeclaration** candidates,
    uint32_t capacity) {
    uint32_t count = 0u;
    uint32_t index;
    for (index = 0u; index < parser->declaration_count; ++index) {
        const PikaFunctionDeclaration* declaration =
            &parser->declarations[index];
        if (declaration->owner_class == UINT16_MAX ||
            !token_matches_method_stable(
                parser, name, declaration->stable_name,
                declaration->stable_name_length,
                declaration->name_position)) {
            continue;
        }
        if (count < capacity) candidates[count] = declaration;
        ++count;
    }
    return count;
}

static int class_is_descendant(const PikaParser* parser,
                               uint16_t class_index,
                               uint16_t base_class) {
    while (class_index != UINT16_MAX) {
        if (class_index == base_class) return 1;
        class_index = parser->classes[class_index].base_class;
    }
    return 0;
}

static uint32_t find_inherited_method_candidates(
    const PikaParser* parser,
    uint16_t class_index,
    const PikaToken* name,
    const PikaFunctionDeclaration** candidates,
    uint32_t capacity) {
    uint32_t count = 0u;
    uint32_t index;
    for (index = 0u; index < parser->declaration_count; ++index) {
        const PikaFunctionDeclaration* declaration =
            &parser->declarations[index];
        if (declaration->owner_class == UINT16_MAX ||
            !class_is_descendant(
                parser, declaration->owner_class, class_index) ||
            !token_matches_method_stable(
                parser, name, declaration->stable_name,
                declaration->stable_name_length,
                declaration->name_position)) {
            continue;
        }
        if (count < capacity) candidates[count] = declaration;
        ++count;
    }
    return count;
}

static PikaStatus append_dynamic_method_call(
    PikaParser* parser,
    const PikaFunctionDeclaration* const* declarations,
    uint32_t declaration_count,
    uint8_t argument_count,
    uint16_t* call_index) {
    PikaCompiledModule* module = parser->module;
    PikaDynamicMethodCallSite* site;
    uint32_t candidate_start = module->dynamic_method_candidate_count;
    PikaStatus status;
    if (declaration_count == 0u || declaration_count > UINT8_MAX ||
        module->dynamic_method_call_count >= UINT16_MAX ||
        candidate_start > UINT16_MAX ||
        declaration_count > UINT16_MAX - candidate_start) {
        return fail_current(parser, PIKA_STATUS_FRONTEND_LIMIT);
    }
    module->dynamic_method_candidates =
        (PikaDynamicMethodCandidate*)grow_parser_array(
            parser, module->dynamic_method_candidates,
            &module->dynamic_method_candidate_capacity,
            candidate_start + declaration_count,
            parser_count_limit(
                parser->base_dynamic_method_candidate_count,
                parser->token_count),
            sizeof(*module->dynamic_method_candidates), &status);
    if (status != PIKA_STATUS_OK) return status;
    module->dynamic_method_calls =
        (PikaDynamicMethodCallSite*)grow_parser_array(
            parser, module->dynamic_method_calls,
            &module->dynamic_method_call_capacity,
            module->dynamic_method_call_count + 1u,
            parser_count_limit(
                parser->base_dynamic_method_call_count,
                parser->token_count),
            sizeof(*module->dynamic_method_calls), &status);
    if (status != PIKA_STATUS_OK) return status;
    for (uint32_t index = 0u; index < declaration_count; ++index) {
        PikaDynamicMethodCandidate* candidate =
            &module->dynamic_method_candidates[candidate_start + index];
        candidate->class_index = declarations[index]->owner_class;
        candidate->function_index = declarations[index]->function_index;
    }
    module->dynamic_method_candidate_count += declaration_count;
    *call_index = (uint16_t)module->dynamic_method_call_count;
    site = &module->dynamic_method_calls[
        module->dynamic_method_call_count++];
    site->first_candidate = (uint16_t)candidate_start;
    site->candidate_count = (uint8_t)declaration_count;
    site->argument_count = argument_count;
    return PIKA_STATUS_OK;
}
#endif

static const PikaFunctionDeclaration* find_named_method(
    const PikaParser* parser,
    uint16_t class_index,
    const char* name,
    uint32_t length) {
    while (class_index != UINT16_MAX) {
        uint32_t index;
        for (index = parser->declaration_count; index > 0u;) {
            const char* method_name;
            uint32_t method_length;
            --index;
            const PikaFunctionDeclaration* declaration =
                &parser->declarations[index];
            if (declaration->stable_name != NULL) {
                method_name = declaration->stable_name;
                method_length = declaration->stable_name_length;
            } else {
                const PikaToken* token =
                    token_at(parser, declaration->name_position);
                method_name = &parser->source[token->offset];
                method_length = token->length;
            }
            if (declaration->owner_class == class_index &&
                method_length == length &&
                memcmp(method_name, name, length) == 0) {
                return declaration;
            }
        }
        class_index = parser->classes[class_index].base_class;
    }
    return NULL;
}

static PikaFieldDeclaration* find_field(
    PikaParser* parser,
    uint16_t class_index,
    const PikaToken* name,
    uint16_t* field_index) {
    while (class_index != UINT16_MAX) {
        PikaClassDeclaration* declaration =
            &parser->classes[class_index];
        uint16_t inherited = declaration->base_class == UINT16_MAX
                                 ? 0u
                                 : parser->classes[declaration->base_class]
                                       .field_count;
        uint16_t index;
        for (index = inherited; index < declaration->field_count; ++index) {
            PikaFieldDeclaration* field =
                &declaration->fields[index];
            if (token_matches_stable(
                    parser, name, field->stable_name,
                    field->stable_name_length,
                    field->name_position)) {
                *field_index = index;
                return field;
            }
        }
        class_index = declaration->base_class;
    }
    return NULL;
}

static void set_expression_from_field(
    PikaExpression* expression,
    const PikaFieldDeclaration* field) {
    expression->kind = (PikaValueKind)field->kind;
    expression->binding_kind =
        (PikaBindingValueKind)field->binding_kind;
    expression->binding_class = field->binding_class;
    expression->class_index = field->class_index;
    expression->first_element_shape = 0u;
    expression->element_shape_count = 0u;
}

static PikaStatus update_field_type(
    PikaParser* parser,
    PikaFieldDeclaration* field,
    const PikaToken* member,
    const PikaExpression* value) {
    if (value->kind == PIKA_VALUE_NONE) {
        field->allows_none = 1u;
        return PIKA_STATUS_OK;
    }
    if (value->kind == PIKA_VALUE_UNKNOWN) {
        return PIKA_STATUS_OK;
    }
    if (field->kind == PIKA_VALUE_UNKNOWN ||
        field->kind == PIKA_VALUE_NONE) {
        field->kind = (uint8_t)value->kind;
        field->binding_kind =
            (uint8_t)value->binding_kind;
        field->binding_class = value->binding_class;
        field->class_index = value->class_index;
        return PIKA_STATUS_OK;
    }
    if (field->kind == value->kind &&
        field->binding_kind == value->binding_kind &&
        field->binding_class == value->binding_class &&
        field->class_index == value->class_index) {
        return PIKA_STATUS_OK;
    }
    return fail_at(parser, PIKA_STATUS_TYPE_MISMATCH, member);
}

static PikaSymbol* find_symbol(PikaParser* parser,
                               const PikaToken* token) {
    uint32_t index = parser->symbol_count;
    while (index > 0u) {
        --index;
        if (token_text_equal(parser, token, &parser->symbols[index])) {
            return &parser->symbols[index];
        }
    }
    return NULL;
}

static PikaSymbol* find_module_global(
    PikaParser* parser,
    const PikaToken* token) {
    uint32_t index;
    for (index = 0u;
         index < parser->module_global_count;
         ++index) {
        if (token_text_equal(
                parser, token,
                &parser->module_globals[index])) {
            return &parser->module_globals[index];
        }
    }
    return NULL;
}

static PikaSymbol* find_read_symbol(
    PikaParser* parser,
    const PikaToken* token,
    PikaSymbol* resolved_global) {
    PikaSymbol* symbol = find_symbol(parser, token);
    if (symbol != NULL || !parser->in_function) {
        return symbol;
    }
    symbol = find_module_global(parser, token);
    if (symbol == NULL) {
        return NULL;
    }
    *resolved_global = *symbol;
    resolved_global->is_global = 1u;
    return resolved_global;
}

static PikaStatus emit_symbol_receiver(
    PikaParser* parser,
    const PikaToken* owner,
    const PikaSymbol* symbol,
    PikaExpression* receiver) {
    PikaStatus status;
    memset(receiver, 0, sizeof(*receiver));
    receiver->slot = symbol->slot;
    receiver->kind = symbol->kind;
    receiver->binding_kind = symbol->binding_kind;
    receiver->binding_class = symbol->binding_class;
    receiver->class_index = symbol->class_index;
    receiver->first_element_shape = symbol->first_element_shape;
    receiver->element_shape_count = symbol->element_shape_count;
    if (symbol->is_global == 0u) return PIKA_STATUS_OK;
    status = require_capability_at(
        parser, PIKA_CAPABILITY_NAME_GLOBAL, owner);
    if (status == PIKA_STATUS_OK) {
        status = allocate_slot(parser, &receiver->slot);
    }
    if (status == PIKA_STATUS_OK) {
        mark_typed(parser);
        status = emit_instruction(
            parser, PIKA_OP_LOAD_GLOBAL, receiver->slot,
            0u, 0u, symbol->global_slot, NULL);
    }
    return status;
}

#if PIKA_CAPABILITY_OBJECT_ATTRIBUTE_ENABLE
static PikaSymbol* find_stable_module_global(
    PikaParser* parser,
    const char* name,
    uint16_t length) {
    uint32_t index;
    for (index = parser->module_global_count; index > 0u;) {
        PikaSymbol* global;
        const char* source;
        --index;
        global = &parser->module_globals[index];
        source = global->name_source != NULL
                     ? global->name_source
                     : parser->source;
        if (global->length == length &&
            memcmp(&source[global->offset], name, length) == 0) {
            return global;
        }
    }
    return NULL;
}

static PikaStatus build_python_member_name(
    PikaParser* parser,
    const PikaPythonModule* imported,
    const PikaToken* member,
    char* name,
    uint32_t capacity,
    uint16_t* length) {
    char digits[3];
    uint32_t digit_count = 0u;
    uint32_t module_index = imported->target_module;
    uint32_t position = 0u;
    uint32_t value = module_index;
    uint32_t name_length;
    do {
        digits[digit_count++] =
            (char)('0' + (value % 10u));
        value /= 10u;
    } while (value != 0u && digit_count < sizeof(digits));
    name_length = 4u + digit_count + member->length;
    if (module_index == 0u || value != 0u ||
        name_length > UINT16_MAX || name_length + 1u > capacity) {
        return fail_at(
            parser, PIKA_STATUS_FRONTEND_LIMIT, member);
    }
    name[position++] = '_';
    name[position++] = '_';
    name[position++] = 'p';
    while (digit_count > 0u) {
        name[position++] = digits[--digit_count];
    }
    name[position++] = '_';
    memcpy(&name[position],
           &parser->source[member->offset], member->length);
    position += member->length;
    name[position] = '\0';
    *length = (uint16_t)position;
    return PIKA_STATUS_OK;
}
#endif

static PikaStatus assign_symbol(PikaParser* parser,
                                const PikaToken* name,
                                PikaValueKind kind,
                                uint8_t* slot) {
    PikaSymbol* symbol = find_symbol(parser, name);
    if (symbol == NULL) {
        if (parser->symbol_count >= parser->symbol_capacity ||
            parser->symbol_count >= PIKA_FRONTEND_SYMBOL_LIMIT) {
            return fail_limit_at(
                parser, PIKA_FRONTEND_BOUND_SYMBOLS,
                PIKA_FRONTEND_SYMBOL_LIMIT,
                parser->symbol_count + 1u, name);
        }
        symbol = &parser->symbols[parser->symbol_count++];
        symbol->name_source = parser->source;
        symbol->offset = name->offset;
        symbol->length = name->length;
        if (allocate_slot(parser, &symbol->slot) != PIKA_STATUS_OK) {
            return PIKA_STATUS_FRONTEND_LIMIT;
        }
    }
    symbol->kind = kind;
    symbol->binding_kind = PIKA_BINDING_VALUE_ANY;
    if (kind == PIKA_VALUE_INTEGER) {
        symbol->binding_kind = PIKA_BINDING_VALUE_INTEGER;
    } else if (kind == PIKA_VALUE_BOOLEAN) {
        symbol->binding_kind = PIKA_BINDING_VALUE_BOOLEAN;
    } else if (kind == PIKA_VALUE_FLOAT) {
        symbol->binding_kind = PIKA_BINDING_VALUE_FLOAT;
    } else if (kind == PIKA_VALUE_STRING) {
        symbol->binding_kind = PIKA_BINDING_VALUE_STRING;
    } else if (kind == PIKA_VALUE_BYTES) {
        symbol->binding_kind = PIKA_BINDING_VALUE_BYTES;
    } else if (kind == PIKA_VALUE_BYTEARRAY) {
        symbol->binding_kind = PIKA_BINDING_VALUE_BYTEARRAY;
    } else if (kind == PIKA_VALUE_LIST) {
        symbol->binding_kind = PIKA_BINDING_VALUE_LIST;
    } else if (kind == PIKA_VALUE_TUPLE) {
        symbol->binding_kind = PIKA_BINDING_VALUE_TUPLE;
    } else if (kind == PIKA_VALUE_DICT) {
        symbol->binding_kind = PIKA_BINDING_VALUE_DICT;
    } else if (kind == PIKA_VALUE_NONE) {
        symbol->binding_kind = PIKA_BINDING_VALUE_NONE;
    } else if (kind == PIKA_VALUE_CALLABLE) {
        symbol->binding_kind = PIKA_BINDING_VALUE_CALLABLE;
    }
    symbol->binding_class = NULL;
    symbol->class_index = UINT16_MAX;
    symbol->first_element_shape = 0u;
    symbol->element_shape_count = 0u;
    symbol->initialized = 1u;
    symbol->is_global = 0u;
    symbol->global_slot = 0u;
    *slot = symbol->slot;
    return PIKA_STATUS_OK;
}

#include "frontend_expression.inc"

static PikaStatus parse_statement(PikaParser* parser);

static PikaStatus parse_suite(PikaParser* parser) {
    while (current_token(parser)->kind != PIKA_TOKEN_DEDENT &&
           current_token(parser)->kind != PIKA_TOKEN_EOF) {
        PikaStatus status;
        if (match(parser, PIKA_TOKEN_NEWLINE)) {
            continue;
        }
        status = parse_statement(parser);
        if (status != PIKA_STATUS_OK) {
            return status;
        }
        release_statement_slots(parser);
    }
    return require(parser, PIKA_TOKEN_DEDENT);
}

static PikaStatus parse_print(PikaParser* parser) {
    const PikaToken* token = current_token(parser);
    PikaExpression arguments[PIKA_FRONTEND_ARGUMENT_LIMIT];
    PikaExpression expression = {0};
    uint32_t count;
    uint8_t base = 0u;
    uint16_t call_index = 0u;
    PikaStatus status = require_capability_at(
        parser, PIKA_CAPABILITY_PRINT_BUILTIN, token);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    ++parser->position;
    status = parse_argument_list(parser, arguments, &count);
    if (status == PIKA_STATUS_OK) {
        status = require(parser, PIKA_TOKEN_NEWLINE);
    }
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    if (count == 1u &&
        (arguments[0].kind == PIKA_VALUE_INTEGER ||
         arguments[0].kind == PIKA_VALUE_BOOLEAN)) {
        return emit_instruction(
            parser, PIKA_OP_PRINT, arguments[0].slot,
            arguments[0].kind == PIKA_VALUE_BOOLEAN
                ? PIKA_PRINT_BOOLEAN
                : PIKA_PRINT_INTEGER,
            0u, 0, NULL);
    }
    status = allocate_slot(parser, &expression.slot);
    if (status == PIKA_STATUS_OK) {
        status = emit_argument_moves(parser, arguments, count, &base);
    }
    if (status == PIKA_STATUS_OK) {
        status = append_builtin_call(
            parser, PIKA_BUILTIN_PRINT, (uint8_t)count,
            PIKA_VALUE_NONE, &call_index);
    }
    if (status != PIKA_STATUS_OK) return status;
    set_expression_kind(&expression, PIKA_VALUE_NONE);
    mark_typed(parser);
    return emit_instruction(
        parser, PIKA_OP_BUILTIN_CALL, expression.slot, base,
        (uint8_t)count, call_index, NULL);
}

static int is_assignment_token(PikaTokenKind kind) {
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

static PikaStatus require_assignment_operator(
    PikaParser* parser,
    PikaTokenKind kind,
    const PikaToken* token) {
    PikaCapability operation = PIKA_CAPABILITY_NUMERIC_OPERATOR;
    PikaStatus status = require_capability_at(
        parser, PIKA_CAPABILITY_AUGMENTED_OPERATOR, token);
    if (status != PIKA_STATUS_OK) return status;
    if (kind == PIKA_TOKEN_AMPERSAND_ASSIGN ||
        kind == PIKA_TOKEN_CARET_ASSIGN ||
        kind == PIKA_TOKEN_PIPE_ASSIGN ||
        kind == PIKA_TOKEN_LEFT_SHIFT_ASSIGN ||
        kind == PIKA_TOKEN_RIGHT_SHIFT_ASSIGN) {
        operation = PIKA_CAPABILITY_BITWISE_OPERATOR;
    } else if (kind == PIKA_TOKEN_AT_ASSIGN) {
        operation = PIKA_CAPABILITY_MATRIX_OPERATOR;
    }
    return require_capability_at(parser, operation, token);
}

static int has_subscript_assignment(const PikaParser* parser) {
    uint32_t position = parser->position + 1u;
    uint32_t depth = 0u;
    while (position < parser->token_count) {
        PikaTokenKind kind = parser->tokens[position].kind;
        if (kind == PIKA_TOKEN_LEFT_BRACKET) {
            ++depth;
        } else if (kind == PIKA_TOKEN_RIGHT_BRACKET) {
            if (depth == 0u) return 0;
            --depth;
            if (depth == 0u) {
                return is_assignment_token(
                    token_at(parser, position + 1u)->kind);
            }
        } else if (depth == 0u &&
                   (kind == PIKA_TOKEN_NEWLINE ||
                    kind == PIKA_TOKEN_EOF)) {
            return 0;
        }
        ++position;
    }
    return 0;
}

#if PIKA_CAPABILITY_ASSIGNMENT_UNPACK_ENABLE
static int has_unpack_assignment(const PikaParser* parser) {
    uint32_t position = parser->position;
    uint32_t depth = 0u;
    int saw_comma = 0;
    while (position < parser->token_count) {
        PikaTokenKind kind = parser->tokens[position].kind;
        if (kind == PIKA_TOKEN_LEFT_PAREN ||
            kind == PIKA_TOKEN_LEFT_BRACKET ||
            kind == PIKA_TOKEN_LEFT_BRACE) {
            ++depth;
        } else if (kind == PIKA_TOKEN_RIGHT_PAREN ||
                   kind == PIKA_TOKEN_RIGHT_BRACKET ||
                   kind == PIKA_TOKEN_RIGHT_BRACE) {
            if (depth == 0u) return 0;
            --depth;
        } else if (depth == 0u && kind == PIKA_TOKEN_COMMA) {
            saw_comma = 1;
        } else if (depth == 0u && kind == PIKA_TOKEN_ASSIGN) {
            return saw_comma;
        } else if (depth == 0u &&
                   (kind == PIKA_TOKEN_NEWLINE ||
                    kind == PIKA_TOKEN_EOF)) {
            return 0;
        }
        ++position;
    }
    return 0;
}

static int is_unpack_assignment_target(
    const PikaParser* parser,
    uint32_t token_index) {
    uint32_t start = token_index;
    uint32_t cursor;
    int target = 0;
    int saw_comma = 0;
    while (start > 0u) {
        PikaTokenKind previous =
            parser->tokens[start - 1u].kind;
        if (previous == PIKA_TOKEN_NEWLINE ||
            previous == PIKA_TOKEN_INDENT ||
            previous == PIKA_TOKEN_DEDENT) {
            break;
        }
        --start;
    }
    cursor = start;
    while (cursor < parser->token_count) {
        if (parser->tokens[cursor].kind != PIKA_TOKEN_NAME) {
            return 0;
        }
        if (cursor == token_index) target = 1;
        ++cursor;
        if (parser->tokens[cursor].kind == PIKA_TOKEN_ASSIGN) {
            return target && saw_comma;
        }
        if (parser->tokens[cursor].kind != PIKA_TOKEN_COMMA) {
            return 0;
        }
        saw_comma = 1;
        ++cursor;
        if (parser->tokens[cursor].kind == PIKA_TOKEN_ASSIGN) {
            return target;
        }
    }
    return 0;
}
#endif

static PikaStatus parse_subscript_assignment(PikaParser* parser) {
#if !PIKA_CAPABILITY_OP_SUBSCRIPT_ENABLE
    return require_capability_at(
        parser, PIKA_CAPABILITY_SUBSCRIPT_OPERATOR,
        current_token(parser));
#else
    const PikaToken* name = current_token(parser);
    PikaSymbol resolved_global;
    PikaSymbol* symbol = find_read_symbol(
        parser, name, &resolved_global);
    PikaExpression index;
    PikaExpression value;
    PikaExpression old_value;
    PikaExpression computed;
    PikaExpression receiver;
    PikaExpression special_arguments[2];
    PikaExpression ignored;
    PikaIndexSite site;
    const PikaFunctionDeclaration* getter;
    const PikaFunctionDeclaration* setter;
    PikaTokenKind assignment_kind;
    PikaOpcode opcode;
    uint16_t site_index = 0u;
    PikaStatus status = require_capability_at(
        parser,
        symbol != NULL && symbol->is_global != 0u
            ? PIKA_CAPABILITY_NAME_GLOBAL
            : PIKA_CAPABILITY_NAME_LOCAL,
        name);
    memset(&old_value, 0, sizeof(old_value));
    if (status != PIKA_STATUS_OK) return status;
    if (symbol == NULL) return fail_at(
        parser, PIKA_STATUS_UNDEFINED_NAME, name);
    status = require_capability_at(
        parser, PIKA_CAPABILITY_SUBSCRIPT_OPERATOR, name);
    if (status != PIKA_STATUS_OK) return status;
    ++parser->position;
    status = require(parser, PIKA_TOKEN_LEFT_BRACKET);
    if (status == PIKA_STATUS_OK) {
        status = parse_expression(parser, &index);
    }
    if (status == PIKA_STATUS_OK) {
        status = require(parser, PIKA_TOKEN_RIGHT_BRACKET);
    }
    if (status != PIKA_STATUS_OK) return status;
    assignment_kind = current_token(parser)->kind;
    if (!is_assignment_token(assignment_kind)) {
        return fail_current(parser, PIKA_STATUS_SYNTAX_ERROR);
    }
    if (assignment_kind != PIKA_TOKEN_ASSIGN) {
        status = require_assignment_operator(
            parser, assignment_kind, current_token(parser));
        if (status != PIKA_STATUS_OK) return status;
    }
    ++parser->position;
    status = emit_symbol_receiver(
        parser, name, symbol, &receiver);
    if (status != PIKA_STATUS_OK) return status;
    memset(&site, 0, sizeof(site));
    site.container_slot = receiver.slot;
    site.index_slot = index.slot;
    site.start_slot = UINT8_MAX;
    site.stop_slot = UINT8_MAX;
    site.step_slot = UINT8_MAX;
    getter = find_expression_special_method(
        parser, &receiver, "__getitem__", 11u);
    setter = find_expression_special_method(
        parser, &receiver, "__setitem__", 11u);
    if (assignment_kind != PIKA_TOKEN_ASSIGN) {
        if (getter != NULL) {
            special_arguments[0] = index;
            status = emit_special_method_call(
                parser, &receiver, getter,
                special_arguments, 1u, &old_value);
        } else {
            status = allocate_slot(parser, &old_value.slot);
            if (status != PIKA_STATUS_OK) return status;
            set_expression_kind(&old_value, PIKA_VALUE_UNKNOWN);
            mark_typed(parser);
            status = append_index_site(
                parser, &site, &site_index);
            if (status == PIKA_STATUS_OK) {
                status = emit_instruction(
                    parser, PIKA_OP_SUBSCRIPT, old_value.slot,
                    0u, 0u, site_index, NULL);
            }
        }
        if (status != PIKA_STATUS_OK) return status;
    }
    status = parse_expression(parser, &value);
    if (status == PIKA_STATUS_OK) {
        status = require(parser, PIKA_TOKEN_NEWLINE);
    }
    if (status != PIKA_STATUS_OK) return status;
    if (assignment_kind != PIKA_TOKEN_ASSIGN) {
        switch (assignment_kind) {
            case PIKA_TOKEN_PLUS_ASSIGN: opcode = PIKA_OP_ADD; break;
            case PIKA_TOKEN_MINUS_ASSIGN: opcode = PIKA_OP_SUBTRACT; break;
            case PIKA_TOKEN_STAR_ASSIGN: opcode = PIKA_OP_MULTIPLY; break;
            case PIKA_TOKEN_SLASH_ASSIGN: opcode = PIKA_OP_DIVIDE; break;
            case PIKA_TOKEN_DOUBLE_SLASH_ASSIGN: opcode = PIKA_OP_FLOOR_DIVIDE; break;
            case PIKA_TOKEN_PERCENT_ASSIGN: opcode = PIKA_OP_MODULO; break;
            case PIKA_TOKEN_DOUBLE_STAR_ASSIGN: opcode = PIKA_OP_POWER; break;
            case PIKA_TOKEN_AMPERSAND_ASSIGN: opcode = PIKA_OP_BITWISE_AND; break;
            case PIKA_TOKEN_CARET_ASSIGN: opcode = PIKA_OP_BITWISE_XOR; break;
            case PIKA_TOKEN_PIPE_ASSIGN: opcode = PIKA_OP_BITWISE_OR; break;
            case PIKA_TOKEN_LEFT_SHIFT_ASSIGN: opcode = PIKA_OP_SHIFT_LEFT; break;
            case PIKA_TOKEN_RIGHT_SHIFT_ASSIGN: opcode = PIKA_OP_SHIFT_RIGHT; break;
            case PIKA_TOKEN_AT_ASSIGN: opcode = PIKA_OP_MATRIX_MULTIPLY; break;
            default: return fail_current(parser, PIKA_STATUS_SYNTAX_ERROR);
        }
        status = allocate_slot(parser, &computed.slot);
        if (status != PIKA_STATUS_OK) return status;
        set_expression_kind(&computed, PIKA_VALUE_UNKNOWN);
        mark_typed(parser);
        status = emit_instruction(
            parser, opcode, computed.slot, old_value.slot,
            value.slot, 0, NULL);
        if (status != PIKA_STATUS_OK) return status;
        value = computed;
    }
    if (setter != NULL) {
        special_arguments[0] = index;
        special_arguments[1] = value;
        return emit_special_method_call(
            parser, &receiver, setter,
            special_arguments, 2u, &ignored);
    }
    return emit_instruction(
        parser, PIKA_OP_SET_SUBSCRIPT, receiver.slot, index.slot,
        value.slot, 0, NULL);
#endif
}

static PikaStatus parse_delete_subscript(PikaParser* parser) {
#if !PIKA_CAPABILITY_OP_SUBSCRIPT_ENABLE || \
    !PIKA_TYPED_MUTABLE_CONTAINER_RUNTIME_ENABLE
    return fail_current(
        parser, PIKA_STATUS_UNSUPPORTED_CAPABILITY);
#else
    const PikaToken* owner;
    PikaSymbol resolved_global;
    PikaSymbol* symbol;
    PikaExpression receiver;
    PikaExpression index;
    uint8_t result_slot = 0u;
    PikaStatus status = require_capability_at(
        parser, PIKA_CAPABILITY_VALUE_CONTAINER,
        current_token(parser));
    if (status != PIKA_STATUS_OK) return status;
    ++parser->position;
    owner = current_token(parser);
    if (owner->kind != PIKA_TOKEN_NAME) {
        return fail_at(parser, PIKA_STATUS_SYNTAX_ERROR, owner);
    }
    if (next_token(parser)->kind == PIKA_TOKEN_DOT) {
        status = parse_binding_access(parser, &receiver);
    } else {
        symbol = find_read_symbol(
            parser, owner, &resolved_global);
        if (symbol == NULL) {
            return fail_at(
                parser, PIKA_STATUS_UNDEFINED_NAME, owner);
        }
        ++parser->position;
        status = emit_symbol_receiver(
            parser, owner, symbol, &receiver);
    }
    if (status == PIKA_STATUS_OK) {
        status = require(parser, PIKA_TOKEN_LEFT_BRACKET);
    }
    if (status == PIKA_STATUS_OK) {
        status = parse_expression(parser, &index);
    }
    if (status == PIKA_STATUS_OK) {
        status = require(parser, PIKA_TOKEN_RIGHT_BRACKET);
    }
    if (status == PIKA_STATUS_OK) {
        status = require(parser, PIKA_TOKEN_NEWLINE);
    }
    if (status == PIKA_STATUS_OK) {
        status = allocate_slot(parser, &result_slot);
    }
    if (status != PIKA_STATUS_OK) return status;
    mark_typed(parser);
    return emit_instruction(
        parser, PIKA_OP_POP, result_slot, receiver.slot,
        index.slot, -1, NULL);
#endif
}

static PikaStatus parse_assignment(PikaParser* parser) {
    const PikaToken* name = current_token(parser);
    PikaExpression expression;
    PikaExpression left;
    uint8_t slot;
    PikaOpcode move_opcode;
    PikaTokenKind assignment_kind = next_token(parser)->kind;
    PikaSymbol* target = find_symbol(parser, name);
    int is_global = target != NULL &&
                    target->is_global != 0u;
    PikaStatus status = require_capability_at(
        parser,
        is_global ? PIKA_CAPABILITY_NAME_GLOBAL
                  : PIKA_CAPABILITY_NAME_LOCAL,
        name);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    memset(&left, 0, sizeof(left));
    if (assignment_kind != PIKA_TOKEN_ASSIGN) {
        status = require_assignment_operator(
            parser, assignment_kind, next_token(parser));
        if (status != PIKA_STATUS_OK) return status;
        PikaSymbol* symbol = target;
        if (symbol == NULL) return fail_at(parser, PIKA_STATUS_UNDEFINED_NAME, name);
        if (is_global) {
            status = allocate_slot(parser, &left.slot);
            if (status == PIKA_STATUS_OK) {
                status = emit_instruction(
                    parser, PIKA_OP_LOAD_GLOBAL, left.slot,
                    0u, 0u, symbol->global_slot, NULL);
            }
            if (status != PIKA_STATUS_OK) {
                return status;
            }
        } else {
            left.slot = symbol->slot;
        }
        left.kind = symbol->kind;
        left.binding_kind = symbol->binding_kind;
        left.binding_class = symbol->binding_class;
        left.class_index = symbol->class_index;
    }
    ++parser->position;
    ++parser->position;
    status = parse_expression(parser, &expression);
    if (status == PIKA_STATUS_OK) {
        status = require(parser, PIKA_TOKEN_NEWLINE);
    }
    if (status == PIKA_STATUS_OK && assignment_kind != PIKA_TOKEN_ASSIGN) {
        PikaExpression result;
        PikaOpcode opcode;
        switch (assignment_kind) {
            case PIKA_TOKEN_PLUS_ASSIGN: opcode = PIKA_OP_ADD; break;
            case PIKA_TOKEN_MINUS_ASSIGN: opcode = PIKA_OP_SUBTRACT; break;
            case PIKA_TOKEN_STAR_ASSIGN: opcode = PIKA_OP_MULTIPLY; break;
            case PIKA_TOKEN_SLASH_ASSIGN: opcode = PIKA_OP_DIVIDE; break;
            case PIKA_TOKEN_DOUBLE_SLASH_ASSIGN: opcode = PIKA_OP_FLOOR_DIVIDE; break;
            case PIKA_TOKEN_PERCENT_ASSIGN: opcode = PIKA_OP_MODULO; break;
            case PIKA_TOKEN_DOUBLE_STAR_ASSIGN: opcode = PIKA_OP_POWER; break;
            case PIKA_TOKEN_AMPERSAND_ASSIGN: opcode = PIKA_OP_BITWISE_AND; break;
            case PIKA_TOKEN_CARET_ASSIGN: opcode = PIKA_OP_BITWISE_XOR; break;
            case PIKA_TOKEN_PIPE_ASSIGN: opcode = PIKA_OP_BITWISE_OR; break;
            case PIKA_TOKEN_LEFT_SHIFT_ASSIGN: opcode = PIKA_OP_SHIFT_LEFT; break;
            case PIKA_TOKEN_RIGHT_SHIFT_ASSIGN: opcode = PIKA_OP_SHIFT_RIGHT; break;
            case PIKA_TOKEN_AT_ASSIGN: opcode = PIKA_OP_MATRIX_MULTIPLY; break;
            default: return fail_at(parser, PIKA_STATUS_SYNTAX_ERROR, name);
        }
        status = allocate_slot(parser, &result.slot);
        if (status == PIKA_STATUS_OK) {
            set_expression_kind(&result, PIKA_VALUE_UNKNOWN);
            mark_typed(parser);
            status = emit_instruction(
                parser, opcode, result.slot, left.slot, expression.slot,
                0, NULL);
        }
        if (status == PIKA_STATUS_OK) expression = result;
    }
    if (status == PIKA_STATUS_OK &&
        assignment_kind == PIKA_TOKEN_ASSIGN &&
        !is_global) {
        status = assign_symbol(
            parser, name, expression.kind, &slot);
    } else if (status == PIKA_STATUS_OK) {
        slot = is_global
                   ? target->global_slot
                   : left.slot;
    }
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    PikaSymbol* symbol = find_symbol(parser, name);
    if (symbol == NULL) {
        return fail_at(
            parser, PIKA_STATUS_INTERNAL_ERROR, name);
    }
    symbol->binding_kind = expression.binding_kind;
    symbol->binding_class = expression.binding_class;
    symbol->class_index = expression.class_index;
    symbol->kind = expression.kind;
    symbol->first_element_shape =
        expression.first_element_shape;
    symbol->element_shape_count =
        expression.element_shape_count;
    symbol->initialized = 1u;
    if (is_global) {
        mark_typed(parser);
        return emit_instruction(
            parser, PIKA_OP_STORE_GLOBAL, expression.slot,
            0u, 0u, symbol->global_slot, NULL);
    }
    move_opcode = expression.kind == PIKA_VALUE_BINDING ||
                          expression.kind == PIKA_VALUE_NATIVE_OBJECT
                      ? PIKA_OP_BIND_MOVE
                      : PIKA_OP_MOVE;
    return emit_instruction(
        parser, move_opcode, slot, expression.slot, 0u, 0, NULL);
}

#if PIKA_CAPABILITY_ASSIGNMENT_UNPACK_ENABLE
static PikaStatus parse_unpack_assignment(PikaParser* parser) {
    const PikaToken* names[PIKA_FRONTEND_COLLECTION_ELEMENT_LIMIT];
    PikaSymbol* targets[PIKA_FRONTEND_COLLECTION_ELEMENT_LIMIT];
    uint8_t target_slots[PIKA_FRONTEND_COLLECTION_ELEMENT_LIMIT];
    uint8_t global_targets[PIKA_FRONTEND_COLLECTION_ELEMENT_LIMIT];
    PikaExpression source = {0};
    PikaExpression element;
    uint32_t count = 0u;
    uint8_t source_slot = 0u;
    uint8_t element_slot = 0u;
    PikaStatus status = require_capability_at(
        parser, PIKA_CAPABILITY_ASSIGNMENT_UNPACK,
        current_token(parser));
    if (status != PIKA_STATUS_OK) return status;
    for (;;) {
        const PikaToken* name = current_token(parser);
        PikaSymbol* target;
        int is_global;
        uint8_t slot;
        if (name->kind != PIKA_TOKEN_NAME) {
            return fail_current(
                parser, PIKA_STATUS_UNSUPPORTED_SYNTAX);
        }
        if (count >= PIKA_FRONTEND_COLLECTION_ELEMENT_LIMIT) {
            return fail_limit_current(
                parser, PIKA_FRONTEND_BOUND_COLLECTION_ELEMENTS,
                PIKA_FRONTEND_COLLECTION_ELEMENT_LIMIT,
                count + 1u);
        }
        target = find_symbol(parser, name);
        is_global = target != NULL && target->is_global != 0u;
        status = require_capability_at(
            parser,
            is_global ? PIKA_CAPABILITY_NAME_GLOBAL
                      : PIKA_CAPABILITY_NAME_LOCAL,
            name);
        if (status != PIKA_STATUS_OK) return status;
        if (is_global) {
            slot = target->global_slot;
        } else if (target == NULL) {
            status = assign_symbol(
                parser, name, PIKA_VALUE_UNKNOWN, &slot);
            if (status != PIKA_STATUS_OK) return status;
            target = find_symbol(parser, name);
            if (target != NULL) target->initialized = 0u;
        } else {
            slot = target->slot;
        }
        if (target == NULL) {
            return fail_at(
                parser, PIKA_STATUS_INTERNAL_ERROR, name);
        }
        names[count] = name;
        targets[count] = target;
        target_slots[count] = slot;
        global_targets[count] = is_global ? 1u : 0u;
        ++count;
        ++parser->position;
        if (match(parser, PIKA_TOKEN_COMMA)) {
            if (current_token(parser)->kind == PIKA_TOKEN_ASSIGN) {
                break;
            }
            continue;
        }
        if (current_token(parser)->kind == PIKA_TOKEN_ASSIGN) {
            break;
        }
        return fail_current(
            parser, PIKA_STATUS_UNSUPPORTED_SYNTAX);
    }
    status = require(parser, PIKA_TOKEN_ASSIGN);
    if (status == PIKA_STATUS_OK) {
        status = parse_expression(parser, &source);
    }
    if (status == PIKA_STATUS_OK) {
        status = require(parser, PIKA_TOKEN_NEWLINE);
    }
    if (status == PIKA_STATUS_OK) {
        status = allocate_slot(parser, &source_slot);
    }
    if (status == PIKA_STATUS_OK) {
        status = emit_instruction(
            parser, PIKA_OP_MOVE, source_slot, source.slot,
            0u, 0, NULL);
    }
    if (status == PIKA_STATUS_OK) {
        status = allocate_slot(parser, &element_slot);
    }
    if (status != PIKA_STATUS_OK) return status;
    mark_typed(parser);
    for (uint32_t index = 0u; index < count; ++index) {
        PikaOpcode move_opcode;
        memset(&element, 0, sizeof(element));
        element.slot = element_slot;
        set_expression_kind(&element, PIKA_VALUE_UNKNOWN);
        if (source.element_shape_count == count &&
            semantic_shape_range_valid(
                parser->module, source.first_element_shape,
                source.element_shape_count)) {
            set_expression_from_static_shape(
                &element,
                &parser->module->semantic_shapes[
                    source.first_element_shape + index],
                0u, 0u);
        }
        status = emit_instruction(
            parser, PIKA_OP_UNPACK_SEQUENCE,
            element_slot, source_slot, (uint8_t)count,
            (int32_t)index, NULL);
        if (status != PIKA_STATUS_OK) return status;
        targets[index]->kind = element.kind;
        targets[index]->binding_kind = element.binding_kind;
        targets[index]->binding_class = element.binding_class;
        targets[index]->class_index = element.class_index;
        targets[index]->first_element_shape =
            element.first_element_shape;
        targets[index]->element_shape_count =
            element.element_shape_count;
        if (global_targets[index] != 0u) {
            status = emit_instruction(
                parser, PIKA_OP_STORE_GLOBAL,
                element_slot, 0u, 0u,
                target_slots[index], NULL);
        } else {
            move_opcode =
                element.kind == PIKA_VALUE_BINDING ||
                        element.kind == PIKA_VALUE_NATIVE_OBJECT
                    ? PIKA_OP_BIND_MOVE
                    : PIKA_OP_MOVE;
            status = emit_instruction(
                parser, move_opcode, target_slots[index],
                element_slot, 0u, 0, NULL);
        }
        if (status != PIKA_STATUS_OK) {
            return fail_at(parser, status, names[index]);
        }
        targets[index]->initialized = 1u;
    }
    return PIKA_STATUS_OK;
}
#endif

static PikaStatus parse_attribute_assignment(PikaParser* parser) {
    const PikaToken* owner = current_token(parser);
    PikaSymbol resolved_global;
    PikaSymbol* symbol = find_read_symbol(
        parser, owner, &resolved_global);
    const PikaToken* member;
    PikaTokenKind assignment_kind;
    PikaExpression receiver;
    PikaExpression value;
    PikaFieldDeclaration* field;
    uint16_t field_index;
    PikaStatus status = require_capability_at(
        parser, PIKA_CAPABILITY_OBJECT_ATTRIBUTE, owner);
    if (status != PIKA_STATUS_OK) return status;
    if (symbol == NULL) {
        return fail_at(parser, PIKA_STATUS_UNDEFINED_NAME, owner);
    }
    if (symbol->kind != PIKA_VALUE_INSTANCE ||
        symbol->class_index == UINT16_MAX) {
        return fail_at(parser, PIKA_STATUS_TYPE_MISMATCH, owner);
    }
    status = emit_symbol_receiver(
        parser, owner, symbol, &receiver);
    if (status != PIKA_STATUS_OK) return status;
    ++parser->position;
    status = require(parser, PIKA_TOKEN_DOT);
    member = current_token(parser);
    if (status == PIKA_STATUS_OK) status = require(parser, PIKA_TOKEN_NAME);
    if (status != PIKA_STATUS_OK) return status;
    field = find_field(
        parser, symbol->class_index, member, &field_index);
    if (field == NULL) {
        return fail_at(parser, PIKA_STATUS_MEMBER_NOT_FOUND, member);
    }
    assignment_kind = current_token(parser)->kind;
    if (!is_assignment_token(assignment_kind)) {
        return fail_current(parser, PIKA_STATUS_SYNTAX_ERROR);
    }
    ++parser->position;
    if (assignment_kind != PIKA_TOKEN_ASSIGN) {
        PikaExpression old_value = {0};
        status = allocate_slot(parser, &old_value.slot);
        if (status != PIKA_STATUS_OK) return status;
        set_expression_from_field(&old_value, field);
        mark_typed(parser);
        status = emit_instruction(
            parser, PIKA_OP_LOAD_ATTRIBUTE, old_value.slot,
            receiver.slot, 0u, field_index, NULL);
        if (status != PIKA_STATUS_OK) return status;
        status = parse_expression(parser, &value);
        if (status == PIKA_STATUS_OK) {
            PikaExpression computed;
            PikaOpcode opcode;
            switch (assignment_kind) {
                case PIKA_TOKEN_PLUS_ASSIGN: opcode = PIKA_OP_ADD; break;
                case PIKA_TOKEN_MINUS_ASSIGN: opcode = PIKA_OP_SUBTRACT; break;
                case PIKA_TOKEN_STAR_ASSIGN: opcode = PIKA_OP_MULTIPLY; break;
                case PIKA_TOKEN_SLASH_ASSIGN: opcode = PIKA_OP_DIVIDE; break;
                case PIKA_TOKEN_DOUBLE_SLASH_ASSIGN: opcode = PIKA_OP_FLOOR_DIVIDE; break;
                case PIKA_TOKEN_PERCENT_ASSIGN: opcode = PIKA_OP_MODULO; break;
                case PIKA_TOKEN_DOUBLE_STAR_ASSIGN: opcode = PIKA_OP_POWER; break;
                case PIKA_TOKEN_AMPERSAND_ASSIGN: opcode = PIKA_OP_BITWISE_AND; break;
                case PIKA_TOKEN_CARET_ASSIGN: opcode = PIKA_OP_BITWISE_XOR; break;
                case PIKA_TOKEN_PIPE_ASSIGN: opcode = PIKA_OP_BITWISE_OR; break;
                case PIKA_TOKEN_LEFT_SHIFT_ASSIGN: opcode = PIKA_OP_SHIFT_LEFT; break;
                case PIKA_TOKEN_RIGHT_SHIFT_ASSIGN: opcode = PIKA_OP_SHIFT_RIGHT; break;
                case PIKA_TOKEN_AT_ASSIGN: opcode = PIKA_OP_MATRIX_MULTIPLY; break;
                default: return fail_current(
                    parser, PIKA_STATUS_SYNTAX_ERROR);
            }
            status = allocate_slot(parser, &computed.slot);
            if (status == PIKA_STATUS_OK) {
                set_expression_kind(&computed, PIKA_VALUE_UNKNOWN);
                mark_typed(parser);
                status = emit_instruction(
                    parser, opcode, computed.slot, old_value.slot,
                    value.slot, 0, NULL);
            }
            if (status == PIKA_STATUS_OK) value = computed;
        }
    } else {
        status = parse_expression(parser, &value);
    }
    if (status == PIKA_STATUS_OK) {
        status = require(parser, PIKA_TOKEN_NEWLINE);
    }
    if (status == PIKA_STATUS_OK &&
        assignment_kind == PIKA_TOKEN_ASSIGN) {
        status = update_field_type(
            parser, field, member, &value);
    }
    if (status != PIKA_STATUS_OK) return status;
    return emit_instruction(
        parser, PIKA_OP_STORE_ATTRIBUTE, receiver.slot,
        value.slot, 0u, field_index, NULL);
}

static PikaStatus patch_jump_chain(PikaParser* parser,
                                   int32_t head,
                                   uint32_t target) {
    while (head >= 0) {
        uint32_t index = (uint32_t)head;
        int32_t next;
        PikaInstruction* instruction =
            mutable_instruction_at(parser, index);
        if (instruction == NULL) {
            return fail_current(parser, PIKA_STATUS_INTERNAL_ERROR);
        }
        next = instruction->immediate;
        instruction->immediate = (int32_t)target;
        head = next;
    }
    return PIKA_STATUS_OK;
}

static PikaStatus note_control_transfer(
    PikaParser* parser,
    const PikaToken* token) {
    uint32_t index;
    if (parser->finally_depth > 0u) {
        return fail_at(
            parser, PIKA_STATUS_UNSUPPORTED_SYNTAX, token);
    }
    for (index = 0u; index < parser->try_depth; ++index) {
        if (parser->try_contexts[index].first_control_transfer ==
            NULL) {
            parser->try_contexts[index].first_control_transfer =
                token;
        }
    }
    return PIKA_STATUS_OK;
}

static PikaStatus push_loop(PikaParser* parser) {
    PikaLoopContext* loop;
    if (parser->loop_depth >= PIKA_FRONTEND_LOOP_LIMIT) {
        return fail_limit_current(
            parser, PIKA_FRONTEND_BOUND_LOOPS,
            PIKA_FRONTEND_LOOP_LIMIT, parser->loop_depth + 1u);
    }
    loop = &parser->loops[parser->loop_depth++];
    loop->break_head = -1;
    loop->continue_head = -1;
    return PIKA_STATUS_OK;
}

static PikaStatus parse_loop_control(PikaParser* parser,
                                     int is_continue) {
    const PikaToken* token = current_token(parser);
    PikaLoopContext* loop;
    uint32_t index;
    int32_t head;
    PikaStatus status;
    status = require_capability_at(
        parser, PIKA_CAPABILITY_LOOP_FLOW, token);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    if (parser->loop_depth == 0u) {
        return fail_at(
            parser, PIKA_STATUS_UNSUPPORTED_SYNTAX, token);
    }
    status = note_control_transfer(parser, token);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    ++parser->position;
    status = require(parser, PIKA_TOKEN_NEWLINE);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    loop = &parser->loops[parser->loop_depth - 1u];
    head = is_continue ? loop->continue_head : loop->break_head;
    status = emit_instruction(
        parser, PIKA_OP_JUMP, 0u, 0u, 0u, head, &index);
    if (status == PIKA_STATUS_OK) {
        if (is_continue) {
            loop->continue_head = (int32_t)index;
        } else {
            loop->break_head = (int32_t)index;
        }
    }
    return status;
}

static PikaStatus parse_return_expression(
    PikaParser* parser,
    PikaExpression* expression) {
    PikaExpression elements[PIKA_FRONTEND_COLLECTION_ELEMENT_LIMIT];
    const PikaToken* comma;
    uint32_t count = 1u;
    PikaStatus status = parse_expression(parser, &elements[0]);
    uint8_t base;
    if (status != PIKA_STATUS_OK ||
        current_token(parser)->kind != PIKA_TOKEN_COMMA) {
        if (status == PIKA_STATUS_OK) *expression = elements[0];
        return status;
    }
    comma = current_token(parser);
    status = require_capability_at(
        parser, PIKA_CAPABILITY_CONTAINER_TUPLE, comma);
    while (status == PIKA_STATUS_OK &&
           match(parser, PIKA_TOKEN_COMMA)) {
        if (current_token(parser)->kind == PIKA_TOKEN_NEWLINE) {
            break;
        }
        if (count >= PIKA_FRONTEND_COLLECTION_ELEMENT_LIMIT) {
            return fail_limit_current(
                parser,
                PIKA_FRONTEND_BOUND_COLLECTION_ELEMENTS,
                PIKA_FRONTEND_COLLECTION_ELEMENT_LIMIT,
                count + 1u);
        }
        status = parse_expression(parser, &elements[count]);
        if (status == PIKA_STATUS_OK) ++count;
    }
    if (status == PIKA_STATUS_OK) {
        status = allocate_slot(parser, &expression->slot);
    }
    if (status == PIKA_STATUS_OK) {
        status = emit_argument_moves(parser, elements, count, &base);
    }
    if (status == PIKA_STATUS_OK) {
        set_expression_kind(expression, PIKA_VALUE_TUPLE);
        status = append_expression_element_shapes(
            parser, elements, count,
            &expression->first_element_shape);
    }
    if (status == PIKA_STATUS_OK) {
        expression->element_shape_count = (uint16_t)count;
        mark_typed(parser);
        parser->module->object_slot_estimate += 1u;
        parser->module->object_byte_estimate +=
            count * (uint32_t)sizeof(PikaRuntimeValue) + 32u;
        status = emit_instruction(
            parser, PIKA_OP_BUILD_TUPLE,
            expression->slot, base, (uint8_t)count, 0, NULL);
    }
    return status;
}

static PikaStatus parse_return(PikaParser* parser) {
    const PikaToken* token = current_token(parser);
    PikaExpression expression;
    PikaConstant constant;
    PikaStatus status = require_capability_at(
        parser, PIKA_CAPABILITY_POSITIONAL_CALL, token);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    if (!parser->in_function) {
        return fail_at(
            parser, PIKA_STATUS_UNSUPPORTED_SYNTAX, token);
    }
    if (parser->finally_depth > 0u) {
        return fail_at(
            parser, PIKA_STATUS_UNSUPPORTED_SYNTAX, token);
    }
    ++parser->position;
    if (current_token(parser)->kind == PIKA_TOKEN_NEWLINE) {
        status = require_capability_at(
            parser, PIKA_CAPABILITY_VALUE_NONE, token);
        if (status == PIKA_STATUS_OK) {
            memset(&constant, 0, sizeof(constant));
            constant.kind = PIKA_CONSTANT_NONE;
            status = emit_constant(parser, &constant, &expression);
        }
    } else {
        status = parse_return_expression(parser, &expression);
    }
    if (status == PIKA_STATUS_OK) {
        status = require(parser, PIKA_TOKEN_NEWLINE);
    }
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    status = record_return_shape(
        parser, &expression);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    if (parser->try_depth > 0u) {
        PikaTryContext* context =
            &parser->try_contexts[parser->try_depth - 1u];
        PikaOpcode move_opcode =
            expression.kind == PIKA_VALUE_BINDING ||
                    expression.kind == PIKA_VALUE_NATIVE_OBJECT
                ? PIKA_OP_BIND_MOVE
                : PIKA_OP_MOVE;
        uint32_t jump_index;
        if (context->has_return == 0u) {
            status = allocate_slot(
                parser, &context->return_slot);
            if (status != PIKA_STATUS_OK) return status;
            context->has_return = 1u;
            if (parser->next_slot > parser->live_slot_floor) {
                parser->live_slot_floor = parser->next_slot;
            }
        }
        if (move_opcode == PIKA_OP_BIND_MOVE) {
            context->return_uses_binding = 1u;
        }
        status = emit_instruction(
            parser, move_opcode, context->return_slot,
            expression.slot, 0u, 0, NULL);
        if (status == PIKA_STATUS_OK) {
            status = emit_instruction(
                parser, PIKA_OP_JUMP, 0u, 0u, 0u,
                context->return_jump_head, &jump_index);
        }
        if (status == PIKA_STATUS_OK) {
            context->return_jump_head =
                (int32_t)jump_index;
        }
        return status;
    }
    return emit_instruction(
        parser, PIKA_OP_RETURN, expression.slot, 0u, 0u, 0, NULL);
}

static PikaStatus parse_pass(PikaParser* parser) {
    PikaStatus status = require_capability_at(
        parser, PIKA_CAPABILITY_PASS_FLOW,
        current_token(parser));
    if (status != PIKA_STATUS_OK) return status;
    ++parser->position;
    return require(parser, PIKA_TOKEN_NEWLINE);
}

static PikaStatus parse_expression_statement(PikaParser* parser) {
    PikaExpression expression;
    PikaStatus status = parse_expression(parser, &expression);
    if (status == PIKA_STATUS_OK) {
        status = require(parser, PIKA_TOKEN_NEWLINE);
    }
    if (status == PIKA_STATUS_OK && parser->interactive &&
        !parser->in_function &&
        current_token(parser)->kind == PIKA_TOKEN_EOF) {
        status = emit_instruction(
            parser, PIKA_OP_RETURN, expression.slot,
            0u, 0u, 0, NULL);
        if (status == PIKA_STATUS_OK) {
            parser->interactive_return = 1;
            parser->module->has_interactive_result = 1u;
        }
    }
    return status;
}

#if PIKA_CAPABILITY_MODULE_IMPORT_ENABLE
static const PikaPythonModule* find_python_import(
    const PikaParser* parser,
    const PikaToken* token);

static PikaStatus parse_import(PikaParser* parser) {
    const PikaToken* token = current_token(parser);
    const PikaToken* name;
    const PikaToken* bound_name;
#if PIKA_CAPABILITY_BINDING_C_ENABLE
    const PikaBindingModule* module = NULL;
#endif
    char module_name[PIKA_FRONTEND_IDENTIFIER_BYTE_LIMIT + 1u];
    PikaStatus status = require_capability_at(
        parser, PIKA_CAPABILITY_MODULE_IMPORT, token);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    ++parser->position;
    name = current_token(parser);
    status = require(parser, PIKA_TOKEN_NAME);
    if (status == PIKA_STATUS_OK) {
        bound_name = name;
        if (current_token(parser)->kind == PIKA_TOKEN_NAME &&
            token_matches_text(
                parser, current_token(parser), "as", 2u)) {
            ++parser->position;
            bound_name = current_token(parser);
            status = require(parser, PIKA_TOKEN_NAME);
        }
    }
    if (status == PIKA_STATUS_OK) {
        status = require(parser, PIKA_TOKEN_NEWLINE);
    }
    if (status == PIKA_STATUS_OK) {
        status = copy_token_text(
            parser, name, module_name,
            (uint32_t)sizeof(module_name));
    }
    if (status != PIKA_STATUS_OK) {
        return status;
    }
#if PIKA_CAPABILITY_MODULE_PYTHON_ENABLE
    if (find_python_import(parser, bound_name) != NULL) {
        return PIKA_STATUS_OK;
    }
#endif
#if !PIKA_CAPABILITY_BINDING_C_ENABLE
    return fail_at(parser, PIKA_STATUS_MODULE_NOT_FOUND, name);
#else
    status = require_capability_at(
        parser, PIKA_CAPABILITY_C_BINDING, name);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    if (find_import(parser, bound_name) != NULL) {
        return PIKA_STATUS_OK;
    }
    if (parser->bindings == NULL) {
        return fail_at(
            parser, PIKA_STATUS_MODULE_NOT_FOUND, name);
    }
    status = pika_binding_find_module(
        parser->bindings, module_name, &module);
    if (status != PIKA_STATUS_OK) {
        return fail_at(
            parser, PIKA_STATUS_MODULE_NOT_FOUND, name);
    }
    if (parser->import_count >= parser->import_capacity ||
        parser->import_count >= PIKA_FRONTEND_SYMBOL_LIMIT) {
        return fail_at(
            parser, PIKA_STATUS_FRONTEND_LIMIT, name);
    }
    parser->imports[parser->import_count].name_source =
        parser->source;
    parser->imports[parser->import_count].offset =
        bound_name->offset;
    parser->imports[parser->import_count].length =
        bound_name->length;
    parser->imports[parser->import_count].module = module;
    parser->imports[parser->import_count].selected = 0u;
    ++parser->import_count;
    return PIKA_STATUS_OK;
#endif
}

#if PIKA_CAPABILITY_BINDING_C_ENABLE
static PikaStatus parse_from_import(PikaParser* parser) {
    const PikaToken* from_token = current_token(parser);
    const PikaToken* module_token;
    const PikaBindingModule* module = NULL;
    int parenthesized = 0;
    char module_name[PIKA_FRONTEND_IDENTIFIER_BYTE_LIMIT + 1u];
    PikaStatus status = require_capability_at(
        parser, PIKA_CAPABILITY_FROM_IMPORT, from_token);
    if (status == PIKA_STATUS_OK) {
        status = require_capability_at(
            parser, PIKA_CAPABILITY_C_BINDING, from_token);
    }
    if (status != PIKA_STATUS_OK) return status;
    ++parser->position;
    module_token = current_token(parser);
    status = require(parser, PIKA_TOKEN_NAME);
    if (status == PIKA_STATUS_OK) {
        status = require(parser, PIKA_TOKEN_IMPORT);
    }
    if (status == PIKA_STATUS_OK) {
        status = copy_token_text(
            parser, module_token, module_name,
            (uint32_t)sizeof(module_name));
    }
    if (status != PIKA_STATUS_OK) return status;
    parenthesized = match(parser, PIKA_TOKEN_LEFT_PAREN);
    if (parser->bindings == NULL ||
        pika_binding_find_module(
            parser->bindings, module_name, &module) !=
            PIKA_STATUS_OK) {
        return fail_at(
            parser, PIKA_STATUS_MODULE_NOT_FOUND,
            module_token);
    }
    for (;;) {
        const PikaToken* member = current_token(parser);
        const PikaImportedModule* existing;
        const PikaBindingCallable* callable = NULL;
        const PikaBindingClass* class_descriptor = NULL;
        const PikaBindingConstant* constant = NULL;
        char member_name[
            PIKA_FRONTEND_IDENTIFIER_BYTE_LIMIT + 1u];
        status = require(parser, PIKA_TOKEN_NAME);
        if (status == PIKA_STATUS_OK) {
            status = copy_token_text(
                parser, member, member_name,
                (uint32_t)sizeof(member_name));
        }
        if (status != PIKA_STATUS_OK) return status;
        if (pika_binding_find_function(
                module, member_name, &callable) != PIKA_STATUS_OK &&
            pika_binding_find_class(
                module, member_name,
                &class_descriptor) != PIKA_STATUS_OK &&
            pika_binding_find_module_constant(
                module, member_name, &constant) != PIKA_STATUS_OK) {
            return fail_at(
                parser, PIKA_STATUS_MEMBER_NOT_FOUND, member);
        }
        existing = find_import(parser, member);
        if (existing != NULL) {
            if (existing->module != module ||
                existing->selected == 0u) {
                return fail_at(
                    parser, PIKA_STATUS_MODULE_CONFLICT, member);
            }
        } else {
            if (parser->import_count >= parser->import_capacity ||
                parser->import_count >=
                    PIKA_FRONTEND_SYMBOL_LIMIT) {
                return fail_at(
                    parser, PIKA_STATUS_FRONTEND_LIMIT, member);
            }
            parser->imports[parser->import_count].name_source =
                parser->source;
            parser->imports[parser->import_count].offset =
                member->offset;
            parser->imports[parser->import_count].length =
                member->length;
            parser->imports[parser->import_count].module = module;
            parser->imports[parser->import_count].selected = 1u;
            ++parser->import_count;
        }
        if (!match(parser, PIKA_TOKEN_COMMA)) break;
        if (parenthesized &&
            current_token(parser)->kind == PIKA_TOKEN_RIGHT_PAREN) {
            break;
        }
    }
    if (parenthesized) {
        status = require(parser, PIKA_TOKEN_RIGHT_PAREN);
        if (status != PIKA_STATUS_OK) {
            return status;
        }
    }
    return require(parser, PIKA_TOKEN_NEWLINE);
}
#endif
#endif

static PikaStatus discover_imports(PikaParser* parser) {
    uint32_t depth = 0u;
    parser->position = 0u;
    while (current_token(parser)->kind != PIKA_TOKEN_EOF) {
        PikaTokenKind kind = current_token(parser)->kind;
#if PIKA_CAPABILITY_MODULE_IMPORT_ENABLE
        PikaStatus status;
        if (depth == 0u && kind == PIKA_TOKEN_IMPORT) {
            status = parse_import(parser);
            if (status != PIKA_STATUS_OK) return status;
            continue;
        }
#if PIKA_CAPABILITY_BINDING_C_ENABLE
        if (depth == 0u && kind == PIKA_TOKEN_FROM) {
            status = parse_from_import(parser);
            if (status != PIKA_STATUS_OK) return status;
            continue;
        }
#endif
#endif
        if (kind == PIKA_TOKEN_INDENT) {
            ++depth;
        } else if (kind == PIKA_TOKEN_DEDENT && depth > 0u) {
            --depth;
        }
        ++parser->position;
    }
    return PIKA_STATUS_OK;
}

static PikaStatus parse_while(PikaParser* parser) {
    PikaExpression condition;
    PikaLoopContext* loop;
    uint16_t saved_floor;
    uint32_t loop_start;
    uint32_t branch_index = 0u;
    uint32_t exit_target;
    PikaStatus status = require_capability_at(
        parser, PIKA_CAPABILITY_LOOP_FLOW, current_token(parser));
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    ++parser->position;
    loop_start = parser->module->program.instruction_count;
    status = parse_expression(parser, &condition);
    if (status == PIKA_STATUS_OK) {
        status = require(parser, PIKA_TOKEN_COLON);
    }
    if (status == PIKA_STATUS_OK) {
        status = require(parser, PIKA_TOKEN_NEWLINE);
    }
    if (status == PIKA_STATUS_OK) {
        status = require(parser, PIKA_TOKEN_INDENT);
    }
    if (status == PIKA_STATUS_OK) {
        status = emit_instruction(
            parser, PIKA_OP_BRANCH_FALSE, condition.slot,
            0u, 0u, 0, &branch_index);
    }
    if (status == PIKA_STATUS_OK) {
        status = push_loop(parser);
    }
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    loop = &parser->loops[parser->loop_depth - 1u];
    saved_floor = parser->live_slot_floor;
    if (parser->next_slot > parser->live_slot_floor) {
        parser->live_slot_floor = parser->next_slot;
    }
    status = parse_suite(parser);
    parser->live_slot_floor = saved_floor;
    release_statement_slots(parser);
    if (status == PIKA_STATUS_OK) {
        status = patch_jump_chain(
            parser, loop->continue_head, loop_start);
    }
    if (status == PIKA_STATUS_OK) {
        status = emit_instruction(
            parser, PIKA_OP_JUMP, 0u, 0u, 0u,
            (int32_t)loop_start, NULL);
    }
    exit_target = parser->module->program.instruction_count;
    if (status == PIKA_STATUS_OK) {
        status = set_instruction_immediate(
            parser, branch_index, (int32_t)exit_target);
    }
    if (status == PIKA_STATUS_OK) {
        status = patch_jump_chain(
            parser, loop->break_head, exit_target);
    }
    --parser->loop_depth;
    return status;
}

static int is_builtin_iterable_kind(PikaValueKind kind) {
    return kind == PIKA_VALUE_LIST ||
           kind == PIKA_VALUE_TUPLE ||
           kind == PIKA_VALUE_DICT ||
           kind == PIKA_VALUE_STRING ||
           kind == PIKA_VALUE_BYTES ||
           kind == PIKA_VALUE_BYTEARRAY ||
           kind == PIKA_VALUE_UNKNOWN;
}

static PikaValueKind iteration_value_kind(PikaValueKind kind) {
    if (kind == PIKA_VALUE_RANGE ||
        kind == PIKA_VALUE_BYTES ||
        kind == PIKA_VALUE_BYTEARRAY) {
        return PIKA_VALUE_INTEGER;
    }
    if (kind == PIKA_VALUE_STRING) {
        return PIKA_VALUE_STRING;
    }
    return PIKA_VALUE_UNKNOWN;
}

static PikaStatus parse_for_iteration(PikaParser* parser) {
    const PikaToken* for_token = current_token(parser);
    const PikaToken* name;
    const PikaToken* source_token;
    PikaExpression source = {0};
    PikaLoopContext* loop;
    uint16_t saved_floor;
    uint8_t source_slot = 0u;
    uint8_t index_slot = 0u;
    uint8_t loop_slot = 0u;
    uint32_t iterate_index = 0u;
    uint32_t condition_target;
    uint32_t exit_target;
    PikaStatus status;
    ++parser->position;
    name = current_token(parser);
    status = require(parser, PIKA_TOKEN_NAME);
    if (status == PIKA_STATUS_OK) {
        status = require(parser, PIKA_TOKEN_IN);
    }
    source_token = current_token(parser);
    if (status == PIKA_STATUS_OK) {
        status = parse_expression(parser, &source);
    }
    if (status == PIKA_STATUS_OK && source.kind == PIKA_VALUE_RANGE) {
        status = require_capability_at(
            parser, PIKA_CAPABILITY_RANGE_ITERATION, for_token);
    } else if (status == PIKA_STATUS_OK &&
               is_builtin_iterable_kind(source.kind)) {
        status = require_capability_at(
            parser, PIKA_CAPABILITY_BUILTIN_ITERATION, for_token);
    } else if (status == PIKA_STATUS_OK) {
        status = fail_at(
            parser, PIKA_STATUS_TYPE_MISMATCH, source_token);
    }
    if (status == PIKA_STATUS_OK) {
        status = require(parser, PIKA_TOKEN_COLON);
    }
    if (status == PIKA_STATUS_OK) {
        status = require(parser, PIKA_TOKEN_NEWLINE);
    }
    if (status == PIKA_STATUS_OK) {
        status = require(parser, PIKA_TOKEN_INDENT);
    }
    if (status == PIKA_STATUS_OK) {
        status = allocate_slot(parser, &source_slot);
    }
    if (status == PIKA_STATUS_OK) {
        status = allocate_slot(parser, &index_slot);
    }
    if (status == PIKA_STATUS_OK) {
        status = assign_symbol(
            parser, name, iteration_value_kind(source.kind),
            &loop_slot);
    }
    if (status == PIKA_STATUS_OK) {
        status = emit_instruction(
            parser, PIKA_OP_MOVE, source_slot, source.slot,
            0u, 0, NULL);
    }
    if (status == PIKA_STATUS_OK) {
        status = emit_instruction(
            parser, PIKA_OP_CONSTANT, index_slot,
            0u, 0u, 0, NULL);
    }
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    mark_typed(parser);
    condition_target = parser->module->program.instruction_count;
    status = emit_instruction(
        parser, PIKA_OP_ITERATE, loop_slot, source_slot,
        index_slot, 0, &iterate_index);
    if (status == PIKA_STATUS_OK) {
        status = push_loop(parser);
    }
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    loop = &parser->loops[parser->loop_depth - 1u];
    saved_floor = parser->live_slot_floor;
    if (parser->next_slot > parser->live_slot_floor) {
        parser->live_slot_floor = parser->next_slot;
    }
    status = parse_suite(parser);
    parser->live_slot_floor = saved_floor;
    release_statement_slots(parser);
    if (status == PIKA_STATUS_OK) {
        status = patch_jump_chain(
            parser, loop->continue_head, condition_target);
    }
    if (status == PIKA_STATUS_OK) {
        status = emit_instruction(
            parser, PIKA_OP_JUMP, 0u, 0u, 0u,
            (int32_t)condition_target, NULL);
    }
    exit_target = parser->module->program.instruction_count;
    if (status == PIKA_STATUS_OK) {
        status = set_instruction_immediate(
            parser, iterate_index, (int32_t)exit_target);
    }
    if (status == PIKA_STATUS_OK) {
        status = patch_jump_chain(
            parser, loop->break_head, exit_target);
    }
    --parser->loop_depth;
    return status;
}

static PikaStatus parse_if(PikaParser* parser) {
    int32_t end_jump_head = -1;
    PikaStatus status = PIKA_STATUS_OK;
    for (;;) {
        PikaExpression condition;
        uint32_t branch_index = 0u;
        uint32_t jump_index = 0u;
        status = require_capability_at(
            parser, PIKA_CAPABILITY_BRANCH_FLOW,
            current_token(parser));
        if (status != PIKA_STATUS_OK) return status;
        ++parser->position;
        status = parse_expression(parser, &condition);
        if (status == PIKA_STATUS_OK) {
            status = require(parser, PIKA_TOKEN_COLON);
        }
        if (status == PIKA_STATUS_OK) {
            status = require(parser, PIKA_TOKEN_NEWLINE);
        }
        if (status == PIKA_STATUS_OK) {
            status = require(parser, PIKA_TOKEN_INDENT);
        }
        if (status == PIKA_STATUS_OK) {
            status = emit_instruction(
                parser, PIKA_OP_BRANCH_FALSE, condition.slot,
                0u, 0u, 0, &branch_index);
        }
        if (status == PIKA_STATUS_OK) {
            status = parse_suite(parser);
        }
        if (status != PIKA_STATUS_OK) return status;
        if (current_token(parser)->kind != PIKA_TOKEN_ELIF &&
            current_token(parser)->kind != PIKA_TOKEN_ELSE) {
            status = set_instruction_immediate(
                parser, branch_index,
                (int32_t)parser->module->program.instruction_count);
            if (status != PIKA_STATUS_OK) return status;
            break;
        }
        status = emit_instruction(
            parser, PIKA_OP_JUMP, 0u, 0u, 0u,
            end_jump_head, &jump_index);
        if (status != PIKA_STATUS_OK) return status;
        end_jump_head = (int32_t)jump_index;
        status = set_instruction_immediate(
            parser, branch_index,
            (int32_t)parser->module->program.instruction_count);
        if (status != PIKA_STATUS_OK) return status;
        if (current_token(parser)->kind == PIKA_TOKEN_ELIF) {
            continue;
        }
        ++parser->position;
        status = require(parser, PIKA_TOKEN_COLON);
        if (status == PIKA_STATUS_OK) {
            status = require(parser, PIKA_TOKEN_NEWLINE);
        }
        if (status == PIKA_STATUS_OK) {
            status = require(parser, PIKA_TOKEN_INDENT);
        }
        if (status == PIKA_STATUS_OK) {
            status = parse_suite(parser);
        }
        if (status != PIKA_STATUS_OK) return status;
        break;
    }
    return patch_jump_chain(
        parser, end_jump_head,
        parser->module->program.instruction_count);
}

static int exception_for_token(
    const PikaParser* parser,
    const PikaToken* token,
    PikaExceptionKind* kind,
    PikaStatus* status) {
    if (token_matches_text(parser, token, "Exception", 9u)) {
        *kind = PIKA_EXCEPTION_EXCEPTION;
        *status = PIKA_STATUS_EXCEPTION;
    } else if (token_matches_text(
                   parser, token, "RuntimeError", 12u)) {
        *kind = PIKA_EXCEPTION_RUNTIME_ERROR;
        *status = PIKA_STATUS_RUNTIME_ERROR;
    } else if (token_matches_text(
                   parser, token, "ValueError", 10u)) {
        *kind = PIKA_EXCEPTION_VALUE_ERROR;
        *status = PIKA_STATUS_VALUE_ERROR;
    } else if (token_matches_text(
                   parser, token, "TypeError", 9u)) {
        *kind = PIKA_EXCEPTION_TYPE_ERROR;
        *status = PIKA_STATUS_TYPE_ERROR;
    } else if (token_matches_text(
                   parser, token, "ZeroDivisionError", 17u)) {
        *kind = PIKA_EXCEPTION_ZERO_DIVISION_ERROR;
        *status = PIKA_STATUS_DIVISION_BY_ZERO;
    } else if (token_matches_text(
                   parser, token, "OverflowError", 13u)) {
        *kind = PIKA_EXCEPTION_OVERFLOW_ERROR;
        *status = PIKA_STATUS_OVERFLOW_ERROR;
    } else if (token_matches_text(
                   parser, token, "AssertionError", 14u)) {
        *kind = PIKA_EXCEPTION_ASSERTION_ERROR;
        *status = PIKA_STATUS_ASSERTION_ERROR;
    } else if (token_matches_text(parser, token, "OSError", 7u)) {
        *kind = PIKA_EXCEPTION_OS_ERROR;
        *status = PIKA_STATUS_OS_ERROR;
    } else {
        return 0;
    }
    return 1;
}

static PikaStatus parse_raise(PikaParser* parser) {
    const PikaToken* raise_token = current_token(parser);
    const PikaToken* exception_token;
    PikaExceptionKind exception_kind;
    PikaStatus raised_status;
    PikaStatus status = require_capability_at(
        parser, PIKA_CAPABILITY_EXCEPTION_BASIC, raise_token);
    uint8_t status_slot = 0u;
    uint8_t message_slot = 0u;
    uint8_t has_message = 0u;
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    ++parser->position;
    if (current_token(parser)->kind == PIKA_TOKEN_NEWLINE) {
        if (parser->caught_depth == 0u) {
            return fail_at(
                parser, PIKA_STATUS_UNSUPPORTED_SYNTAX,
                raise_token);
        }
        status_slot =
            parser->caught_status_slots[parser->caught_depth - 1u];
        ++parser->position;
        return emit_instruction(
            parser, PIKA_OP_RAISE, status_slot, 0u, 0u, 0, NULL);
    }
    exception_token = current_token(parser);
    status = require(parser, PIKA_TOKEN_NAME);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    if (!exception_for_token(
            parser, exception_token, &exception_kind,
            &raised_status)) {
        return fail_at(
            parser, PIKA_STATUS_UNSUPPORTED_SYNTAX,
            exception_token);
    }
    if (match(parser, PIKA_TOKEN_LEFT_PAREN)) {
        if (current_token(parser)->kind !=
            PIKA_TOKEN_RIGHT_PAREN) {
            const PikaToken* message_token =
                current_token(parser);
            PikaExpression message;
            status = parse_expression(parser, &message);
            if (status != PIKA_STATUS_OK) {
                return status;
            }
            if (message.kind != PIKA_VALUE_STRING &&
                message.kind != PIKA_VALUE_UNKNOWN) {
                return fail_at(
                    parser, PIKA_STATUS_TYPE_MISMATCH,
                    message_token);
            }
            message_slot = message.slot;
            has_message = 1u;
            if (current_token(parser)->kind ==
                PIKA_TOKEN_COMMA) {
                return fail_current(
                    parser, PIKA_STATUS_UNSUPPORTED_SYNTAX);
            }
        }
        status = require(parser, PIKA_TOKEN_RIGHT_PAREN);
        if (status != PIKA_STATUS_OK) {
            return status;
        }
    }
    status = require(parser, PIKA_TOKEN_NEWLINE);
    if (status == PIKA_STATUS_OK) {
        status = allocate_slot(parser, &status_slot);
    }
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    return emit_instruction(
        parser, PIKA_OP_RAISE, status_slot, message_slot,
        has_message,
        (int32_t)raised_status, NULL);
}

static PikaStatus parse_assert(PikaParser* parser) {
    const PikaToken* assert_token = current_token(parser);
    PikaExpression condition;
    PikaStatus status = require_capability_at(
        parser, PIKA_CAPABILITY_BRANCH_FLOW, assert_token);
    uint8_t status_slot = 0u;
    uint8_t message_slot = 0u;
    uint8_t has_message = 0u;
    uint32_t branch_index = 0u;
    uint32_t jump_index = 0u;
    uint32_t raise_index;

    if (status == PIKA_STATUS_OK) {
        status = require_capability_at(
            parser, PIKA_CAPABILITY_EXCEPTION_BASIC, assert_token);
    }
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    ++parser->position;
    status = parse_expression(parser, &condition);
    if (status == PIKA_STATUS_OK &&
        match(parser, PIKA_TOKEN_COMMA)) {
        const PikaToken* message_token = current_token(parser);
        PikaExpression message;
        status = parse_expression(parser, &message);
        if (status == PIKA_STATUS_OK &&
            message.kind != PIKA_VALUE_STRING &&
            message.kind != PIKA_VALUE_UNKNOWN) {
            status = fail_at(
                parser, PIKA_STATUS_TYPE_MISMATCH, message_token);
        }
        if (status == PIKA_STATUS_OK) {
            message_slot = message.slot;
            has_message = 1u;
        }
    }
    if (status == PIKA_STATUS_OK) {
        status = require(parser, PIKA_TOKEN_NEWLINE);
    }
    if (status == PIKA_STATUS_OK) {
        status = emit_instruction(
            parser, PIKA_OP_BRANCH_FALSE, condition.slot,
            0u, 0u, 0, &branch_index);
    }
    if (status == PIKA_STATUS_OK) {
        status = emit_instruction(
            parser, PIKA_OP_JUMP, 0u, 0u, 0u, 0, &jump_index);
    }
    raise_index = parser->module->program.instruction_count;
    if (status == PIKA_STATUS_OK) {
        status = allocate_slot(parser, &status_slot);
    }
    if (status == PIKA_STATUS_OK) {
        status = emit_instruction(
            parser, PIKA_OP_RAISE, status_slot, message_slot,
            has_message, (int32_t)PIKA_STATUS_ASSERTION_ERROR, NULL);
    }
    if (status == PIKA_STATUS_OK) {
        status = set_instruction_immediate(
            parser, branch_index, (int32_t)raise_index);
    }
    if (status == PIKA_STATUS_OK) {
        status = set_instruction_immediate(
            parser, jump_index,
            (int32_t)parser->module->program.instruction_count);
    }
    return status;
}

static PikaStatus emit_normal_jump(PikaParser* parser,
                                   int32_t* head) {
    uint32_t index;
    PikaStatus status = emit_instruction(
        parser, PIKA_OP_JUMP, 0u, 0u, 0u, *head, &index);
    if (status == PIKA_STATUS_OK) {
        *head = (int32_t)index;
    }
    return status;
}

static PikaStatus emit_saved_return(
    PikaParser* parser,
    const PikaTryContext* context) {
    PikaExpression expression;
    PikaStatus status;
    memset(&expression, 0, sizeof(expression));
    expression.slot = context->return_slot;
    expression.kind = context->return_uses_binding != 0u
                          ? PIKA_VALUE_BINDING
                          : PIKA_VALUE_UNKNOWN;
    if (parser->try_depth == 0u) {
        return emit_instruction(
            parser, PIKA_OP_RETURN, expression.slot,
            0u, 0u, 0, NULL);
    }
    {
        PikaTryContext* outer =
            &parser->try_contexts[parser->try_depth - 1u];
        PikaOpcode move_opcode =
            context->return_uses_binding != 0u
                ? PIKA_OP_BIND_MOVE
                : PIKA_OP_MOVE;
        uint32_t jump_index;
        if (outer->has_return == 0u) {
            status = allocate_slot(
                parser, &outer->return_slot);
            if (status != PIKA_STATUS_OK) return status;
            outer->has_return = 1u;
            if (parser->next_slot > parser->live_slot_floor) {
                parser->live_slot_floor = parser->next_slot;
            }
        }
        if (move_opcode == PIKA_OP_BIND_MOVE) {
            outer->return_uses_binding = 1u;
        }
        status = emit_instruction(
            parser, move_opcode, outer->return_slot,
            expression.slot, 0u, 0, NULL);
        if (status == PIKA_STATUS_OK) {
            status = emit_instruction(
                parser, PIKA_OP_JUMP, 0u, 0u, 0u,
                outer->return_jump_head, &jump_index);
        }
        if (status == PIKA_STATUS_OK) {
            outer->return_jump_head =
                (int32_t)jump_index;
        }
    }
    return status;
}

static PikaStatus parse_try(PikaParser* parser) {
    const PikaToken* try_token = current_token(parser);
    PikaTryContext* try_context;
    uint16_t saved_floor = parser->live_slot_floor;
    uint8_t status_slot = 0u;
    uint32_t try_first = 0u;
    uint32_t try_end = 0u;
    int32_t normal_jump_head = -1;
    int has_except = 0;
    int has_finally = 0;
    int saw_bare_except = 0;
    int context_active = 0;
    PikaStatus status = require_capability_at(
        parser, PIKA_CAPABILITY_EXCEPTION_BASIC, try_token);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    ++parser->position;
    status = require(parser, PIKA_TOKEN_COLON);
    if (status == PIKA_STATUS_OK) {
        status = require(parser, PIKA_TOKEN_NEWLINE);
    }
    if (status == PIKA_STATUS_OK) {
        status = require(parser, PIKA_TOKEN_INDENT);
    }
    if (status == PIKA_STATUS_OK) {
        status = allocate_slot(parser, &status_slot);
    }
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    if (parser->try_depth >=
        PIKA_FRONTEND_BLOCK_DEPTH_LIMIT) {
        return fail_limit_at(
            parser, PIKA_FRONTEND_BOUND_BLOCK_DEPTH,
            PIKA_FRONTEND_BLOCK_DEPTH_LIMIT,
            parser->try_depth + 1u, try_token);
    }
    if (parser->next_slot > parser->live_slot_floor) {
        parser->live_slot_floor = parser->next_slot;
    }
    try_context = &parser->try_contexts[parser->try_depth++];
    try_context->first_control_transfer = NULL;
    try_context->return_jump_head = -1;
    try_context->return_slot = 0u;
    try_context->has_return = 0u;
    try_context->return_uses_binding = 0u;
    context_active = 1;
    try_first = parser->module->program.instruction_count;
    status = parse_suite(parser);
    try_end = parser->module->program.instruction_count;
    if (status == PIKA_STATUS_OK) {
        status = emit_normal_jump(parser, &normal_jump_head);
    }
    while (status == PIKA_STATUS_OK &&
           current_token(parser)->kind == PIKA_TOKEN_EXCEPT) {
        const PikaToken* except_token = current_token(parser);
        const PikaToken* exception_token = NULL;
        PikaExceptionKind catch_kind =
            PIKA_EXCEPTION_EXCEPTION;
        PikaStatus ignored_status = PIKA_STATUS_EXCEPTION;
        uint32_t handler_instruction;
        uint8_t alias_slot = 0u;
        int has_alias = 0;
        if (saw_bare_except) {
            status = fail_current(
                parser, PIKA_STATUS_SYNTAX_ERROR);
            break;
        }
        has_except = 1;
        ++parser->position;
        if (current_token(parser)->kind == PIKA_TOKEN_COLON) {
            saw_bare_except = 1;
        } else {
            exception_token = current_token(parser);
            status = require(parser, PIKA_TOKEN_NAME);
            if (status == PIKA_STATUS_OK &&
                !exception_for_token(
                    parser, exception_token, &catch_kind,
                    &ignored_status)) {
                status = fail_at(
                    parser, PIKA_STATUS_UNSUPPORTED_SYNTAX,
                    exception_token);
            }
            if (status == PIKA_STATUS_OK &&
                current_token(parser)->kind ==
                    PIKA_TOKEN_NAME &&
                token_matches_text(
                    parser, current_token(parser), "as", 2u)) {
                const PikaToken* alias;
                ++parser->position;
                alias = current_token(parser);
                status = require(parser, PIKA_TOKEN_NAME);
                if (status == PIKA_STATUS_OK) {
                    status = assign_symbol(
                        parser, alias,
                        PIKA_VALUE_EXCEPTION, &alias_slot);
                }
                if (status == PIKA_STATUS_OK) {
                    has_alias = 1;
                }
            }
        }
        if (status == PIKA_STATUS_OK) {
            status = require(parser, PIKA_TOKEN_COLON);
        }
        if (status == PIKA_STATUS_OK) {
            status = require(parser, PIKA_TOKEN_NEWLINE);
        }
        if (status == PIKA_STATUS_OK) {
            status = require(parser, PIKA_TOKEN_INDENT);
        }
        handler_instruction =
            parser->module->program.instruction_count;
        if (status == PIKA_STATUS_OK) {
            status = append_exception_handler(
                parser, status_slot, catch_kind, try_first,
                try_end - try_first, handler_instruction);
        }
        if (status == PIKA_STATUS_OK && has_alias) {
            status = emit_instruction(
                parser, PIKA_OP_MOVE, alias_slot,
                status_slot, 0u, 0, NULL);
        }
        if (status == PIKA_STATUS_OK &&
            parser->caught_depth >=
                PIKA_FRONTEND_BLOCK_DEPTH_LIMIT) {
            status = fail_limit_at(
                parser, PIKA_FRONTEND_BOUND_BLOCK_DEPTH,
                PIKA_FRONTEND_BLOCK_DEPTH_LIMIT,
                parser->caught_depth + 1u, except_token);
        }
        if (status == PIKA_STATUS_OK) {
            parser->caught_status_slots[
                parser->caught_depth++] = status_slot;
            status = parse_suite(parser);
            --parser->caught_depth;
        }
        if (status == PIKA_STATUS_OK) {
            status = emit_normal_jump(
                parser, &normal_jump_head);
        }
    }
    if (status == PIKA_STATUS_OK &&
        current_token(parser)->kind == PIKA_TOKEN_FINALLY) {
        const PikaToken* finally_token = current_token(parser);
        uint32_t finally_body_position = 0u;
        uint32_t after_finally_position = 0u;
        uint32_t protected_end;
        uint32_t normal_entry;
        uint32_t finally_entry;
        int32_t finalizer_exit_head = -1;
        has_finally = 1;
        status = require_capability_at(
            parser, PIKA_CAPABILITY_EXCEPTION_FINALLY,
            finally_token);
        if (status == PIKA_STATUS_OK &&
            try_context->first_control_transfer != NULL) {
            status = fail_at(
                parser, PIKA_STATUS_UNSUPPORTED_SYNTAX,
                try_context->first_control_transfer);
        }
        ++parser->position;
        if (status == PIKA_STATUS_OK) {
            status = require(parser, PIKA_TOKEN_COLON);
        }
        if (status == PIKA_STATUS_OK) {
            status = require(parser, PIKA_TOKEN_NEWLINE);
        }
        if (status == PIKA_STATUS_OK) {
            status = require(parser, PIKA_TOKEN_INDENT);
        }
        finally_body_position = parser->position;
        protected_end =
            parser->module->program.instruction_count;
        normal_entry = protected_end;
        if (status == PIKA_STATUS_OK) {
            status = emit_instruction(
                parser, PIKA_OP_CONSTANT, status_slot,
                0u, 0u, 0, NULL);
        }
        finally_entry =
            parser->module->program.instruction_count;
        if (status == PIKA_STATUS_OK) {
            status = patch_jump_chain(
                parser, normal_jump_head, normal_entry);
        }
        if (status == PIKA_STATUS_OK) {
            status = append_exception_handler(
                parser, status_slot, PIKA_EXCEPTION_ANY,
                try_first, protected_end - try_first,
                finally_entry);
        }
        if (context_active) {
            --parser->try_depth;
            context_active = 0;
        }
        if (status == PIKA_STATUS_OK) {
            ++parser->finally_depth;
            status = parse_suite(parser);
            --parser->finally_depth;
        }
        after_finally_position = parser->position;
        if (status == PIKA_STATUS_OK) {
            status = emit_instruction(
                parser, PIKA_OP_RAISE, status_slot,
                0u, 0u, 0, NULL);
        }
        if (status == PIKA_STATUS_OK &&
            try_context->has_return != 0u) {
            uint32_t return_entry;
            uint32_t after_return;
            status = emit_normal_jump(
                parser, &finalizer_exit_head);
            return_entry =
                parser->module->program.instruction_count;
            parser->position = finally_body_position;
            if (status == PIKA_STATUS_OK) {
                ++parser->finally_depth;
                status = parse_suite(parser);
                --parser->finally_depth;
            }
            if (status == PIKA_STATUS_OK &&
                parser->position != after_finally_position) {
                status = fail_current(
                    parser, PIKA_STATUS_INTERNAL_ERROR);
            }
            if (status == PIKA_STATUS_OK) {
                status = emit_saved_return(
                    parser, try_context);
            }
            after_return =
                parser->module->program.instruction_count;
            if (status == PIKA_STATUS_OK) {
                status = patch_jump_chain(
                    parser, try_context->return_jump_head,
                    return_entry);
            }
            if (status == PIKA_STATUS_OK) {
                status = patch_jump_chain(
                    parser, finalizer_exit_head,
                    after_return);
            }
            parser->position = after_finally_position;
        }
    }
    if (status == PIKA_STATUS_OK && !has_except && !has_finally) {
        status = fail_current(
            parser, PIKA_STATUS_SYNTAX_ERROR);
    }
    if (status == PIKA_STATUS_OK && !has_finally) {
        if (context_active) {
            --parser->try_depth;
            context_active = 0;
        }
        if (try_context->has_return != 0u) {
            uint32_t return_entry =
                parser->module->program.instruction_count;
            uint32_t after_return;
            status = emit_saved_return(
                parser, try_context);
            after_return =
                parser->module->program.instruction_count;
            if (status == PIKA_STATUS_OK) {
                status = patch_jump_chain(
                    parser, try_context->return_jump_head,
                    return_entry);
            }
            if (status == PIKA_STATUS_OK) {
                status = patch_jump_chain(
                    parser, normal_jump_head,
                    after_return);
            }
        } else {
            status = patch_jump_chain(
                parser, normal_jump_head,
                parser->module->program.instruction_count);
        }
    }
    if (context_active) {
        --parser->try_depth;
    }
    parser->live_slot_floor = saved_floor;
    release_statement_slots(parser);
    return status;
}

static PikaStatus parse_statement(PikaParser* parser) {
#if PIKA_CAPABILITY_MODULE_IMPORT_ENABLE
    if (current_token(parser)->kind == PIKA_TOKEN_IMPORT) {
        return parse_import(parser);
    }
#if PIKA_CAPABILITY_BINDING_C_ENABLE
    if (current_token(parser)->kind == PIKA_TOKEN_FROM) {
        return parse_from_import(parser);
    }
#endif
#else
    if (current_token(parser)->kind == PIKA_TOKEN_IMPORT ||
        current_token(parser)->kind == PIKA_TOKEN_FROM) {
        return fail_current(
            parser, PIKA_STATUS_UNSUPPORTED_CAPABILITY);
    }
#endif
    if (current_token(parser)->kind == PIKA_TOKEN_IF) {
        return parse_if(parser);
    }
    if (current_token(parser)->kind == PIKA_TOKEN_TRY) {
        return parse_try(parser);
    }
    if (current_token(parser)->kind == PIKA_TOKEN_RAISE) {
        return parse_raise(parser);
    }
    if (current_token(parser)->kind == PIKA_TOKEN_ASSERT) {
        return parse_assert(parser);
    }
    if (current_token(parser)->kind == PIKA_TOKEN_WHILE) {
        return parse_while(parser);
    }
    if (current_token(parser)->kind == PIKA_TOKEN_FOR) {
        return parse_for_iteration(parser);
    }
    if (current_token(parser)->kind == PIKA_TOKEN_RETURN) {
        return parse_return(parser);
    }
    if (current_token(parser)->kind == PIKA_TOKEN_PASS) {
        return parse_pass(parser);
    }
    if (current_token(parser)->kind == PIKA_TOKEN_BREAK) {
        return parse_loop_control(parser, 0);
    }
    if (current_token(parser)->kind == PIKA_TOKEN_CONTINUE) {
        return parse_loop_control(parser, 1);
    }
    if (current_token(parser)->kind == PIKA_TOKEN_PRINT) {
        return parse_print(parser);
    }
    if (current_token(parser)->kind == PIKA_TOKEN_DEL) {
        return parse_delete_subscript(parser);
    }
    if (current_token(parser)->kind == PIKA_TOKEN_GLOBAL) {
        uint32_t count = 0u;
        PikaStatus status;
        if (!parser->in_function) {
            return fail_current(
                parser, PIKA_STATUS_SYNTAX_ERROR);
        }
        status = require_capability_at(
            parser, PIKA_CAPABILITY_GLOBAL_STATEMENT,
            current_token(parser));
        ++parser->position;
        while (status == PIKA_STATUS_OK) {
            const PikaToken* name = current_token(parser);
            PikaSymbol* module_global;
            PikaSymbol* symbol;
            if (count >= PIKA_FRONTEND_SYMBOL_LIMIT) {
                return fail_limit_current(
                    parser, PIKA_FRONTEND_BOUND_SYMBOLS,
                    PIKA_FRONTEND_SYMBOL_LIMIT, count + 1u);
            }
            status = require(parser, PIKA_TOKEN_NAME);
            if (status != PIKA_STATUS_OK) {
                break;
            }
            module_global = find_module_global(parser, name);
            if (module_global == NULL) {
                return fail_at(
                    parser, PIKA_STATUS_UNDEFINED_NAME, name);
            }
            symbol = find_symbol(parser, name);
            if (symbol != NULL && symbol->is_global == 0u) {
                return fail_at(
                    parser, PIKA_STATUS_SYNTAX_ERROR, name);
            }
            if (symbol == NULL) {
                if (parser->symbol_count >= parser->symbol_capacity ||
                    parser->symbol_count >=
                        PIKA_FRONTEND_SYMBOL_LIMIT) {
                    return fail_limit_at(
                        parser, PIKA_FRONTEND_BOUND_SYMBOLS,
                        PIKA_FRONTEND_SYMBOL_LIMIT,
                        parser->symbol_count + 1u, name);
                }
                symbol =
                    &parser->symbols[parser->symbol_count++];
                *symbol = *module_global;
                symbol->name_source = parser->source;
                symbol->offset = name->offset;
                symbol->length = name->length;
                symbol->is_global = 1u;
            }
            ++count;
            if (!match(parser, PIKA_TOKEN_COMMA)) {
                break;
            }
        }
        if (status == PIKA_STATUS_OK) {
            status = require(parser, PIKA_TOKEN_NEWLINE);
        }
        if (status == PIKA_STATUS_OK) {
            mark_typed(parser);
        }
        return status;
    }
    if (current_token(parser)->kind == PIKA_TOKEN_NAME) {
#if PIKA_CAPABILITY_ASSIGNMENT_UNPACK_ENABLE
        if (has_unpack_assignment(parser)) {
            return parse_unpack_assignment(parser);
        }
#endif
        if (next_token(parser)->kind == PIKA_TOKEN_DOT &&
            token_at(parser, parser->position + 2u)->kind ==
                PIKA_TOKEN_NAME &&
            is_assignment_token(
                token_at(parser, parser->position + 3u)->kind)) {
            return parse_attribute_assignment(parser);
        }
        if (next_token(parser)->kind == PIKA_TOKEN_LEFT_BRACKET &&
            has_subscript_assignment(parser)) {
            return parse_subscript_assignment(parser);
        }
        PikaTokenKind assignment = next_token(parser)->kind;
        if (is_assignment_token(assignment)) {
            return parse_assignment(parser);
        }
    }
    return parse_expression_statement(parser);
}

static void reset_scope(PikaParser* parser, int in_function) {
    if (parser->symbol_capacity > 0u) {
        memset(parser->symbols, 0,
               parser->symbol_capacity * sizeof(*parser->symbols));
    }
    memset(parser->loops, 0, sizeof(parser->loops));
    memset(parser->try_contexts, 0, sizeof(parser->try_contexts));
    memset(parser->caught_status_slots, 0,
           sizeof(parser->caught_status_slots));
    parser->symbol_count = 0u;
    parser->next_slot = 0u;
    parser->loop_depth = 0u;
    parser->try_depth = 0u;
    parser->caught_depth = 0u;
    parser->finally_depth = 0u;
    parser->expression_depth = 0u;
    parser->function_instruction_start =
        parser->module->program.instruction_count;
    parser->in_function = in_function;
    parser->live_slot_floor = 0u;
    parser->slot_high_water = 0u;
    if (!in_function) {
        uint32_t index;
        for (index = 0u;
             index < parser->module_global_count;
             ++index) {
            parser->symbols[index] =
                parser->module_globals[index];
            parser->symbols[index].is_global = 0u;
        }
        parser->symbol_count =
            parser->module_global_count;
        parser->next_slot =
            (uint16_t)parser->module_global_count;
        parser->slot_high_water =
            (uint16_t)parser->module_global_count;
    }
}

static const PikaFunctionDeclaration* declaration_at(
    const PikaParser* parser,
    uint32_t header_position) {
    uint32_t index;
    for (index = 0u; index < parser->declaration_count; ++index) {
        if (parser->declarations[index].header_position ==
            header_position) {
            return &parser->declarations[index];
        }
    }
    return NULL;
}

static const PikaClassDeclaration* class_at(
    const PikaParser* parser,
    uint32_t header_position) {
    uint32_t index;
    for (index = 0u; index < parser->class_count; ++index) {
        if (parser->classes[index].header_position == header_position) {
            return &parser->classes[index];
        }
    }
    return NULL;
}

static uint32_t default_literal_width(
    const PikaParser* parser,
    uint32_t position) {
    PikaTokenKind kind = token_at(parser, position)->kind;
    if (is_string_constant_token(kind)) {
        uint32_t width = 1u;
        while (is_string_constant_token(
                   token_at(parser, position + width)->kind)) {
            ++width;
        }
        return width;
    }
    if (kind == PIKA_TOKEN_PLUS || kind == PIKA_TOKEN_MINUS) {
        PikaTokenKind operand =
            token_at(parser, position + 1u)->kind;
        return operand == PIKA_TOKEN_INTEGER ||
                       operand == PIKA_TOKEN_FLOAT
                   ? 2u
                   : 0u;
    }
    return kind == PIKA_TOKEN_INTEGER ||
                   kind == PIKA_TOKEN_FLOAT ||
                   kind == PIKA_TOKEN_TRUE ||
                   kind == PIKA_TOKEN_FALSE ||
                   kind == PIKA_TOKEN_NONE
               ? 1u
               : 0u;
}

static PikaStatus discover_declaration(PikaParser* parser,
                                       uint16_t owner_class) {
    PikaFunctionDeclaration* declaration;
    const PikaToken* name;
    uint32_t header_position = parser->position;
    uint32_t name_position;
    uint32_t parameter_count = 0u;
    uint32_t reserved_parameter_count = 0u;
    uint32_t required_parameter_count = 0u;
    uint32_t variadic_parameter = UINT8_MAX;
    uint32_t depth = 1u;
    uint32_t index;
    int saw_default = 0;
    PikaStatus status = require_capability_at(
        parser, PIKA_CAPABILITY_POSITIONAL_CALL,
        current_token(parser));
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    if (parser->declaration_count >= parser->declaration_capacity ||
        parser->declaration_count >= PIKA_FRONTEND_FUNCTION_LIMIT) {
        return fail_limit_current(
            parser, PIKA_FRONTEND_BOUND_FUNCTIONS,
            PIKA_FRONTEND_FUNCTION_LIMIT,
            parser->declaration_count + 1u);
    }
    declaration =
        &parser->declarations[parser->declaration_count];
    memset(declaration, 0, sizeof(*declaration));
    status = require(parser, PIKA_TOKEN_DEF);
    name_position = parser->position;
    name = current_token(parser);
    if (status == PIKA_STATUS_OK) {
        status = require(parser, PIKA_TOKEN_NAME);
    }
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    for (index = parser->base_declaration_count;
         index < parser->declaration_count;
         ++index) {
        const PikaFunctionDeclaration* existing =
            &parser->declarations[index];
        if (existing->owner_class == owner_class &&
            token_matches_stable(
                parser, name, existing->stable_name,
                existing->stable_name_length,
                existing->name_position)) {
            return fail_at(parser, PIKA_STATUS_SYNTAX_ERROR, name);
        }
    }
    status = require(parser, PIKA_TOKEN_LEFT_PAREN);
    if (status == PIKA_STATUS_OK) {
        if (!count_header_parameters(
                parser, parser->position,
                &reserved_parameter_count)) {
            reserved_parameter_count =
                PIKA_FRONTEND_PARAMETER_LIMIT;
        }
        status = reserve_parameter_declarations(
            parser, reserved_parameter_count,
            &declaration->parameters);
    }
    if (status == PIKA_STATUS_OK &&
        current_token(parser)->kind != PIKA_TOKEN_RIGHT_PAREN) {
        for (;;) {
            const PikaToken* parameter = current_token(parser);
            PikaParameterDeclaration* metadata;
            int is_variadic = 0;
            if (parameter_count >= PIKA_FRONTEND_PARAMETER_LIMIT) {
                return fail_limit_current(
                    parser, PIKA_FRONTEND_BOUND_PARAMETERS,
                    PIKA_FRONTEND_PARAMETER_LIMIT,
                    parameter_count + 1u);
            }
#if PIKA_CAPABILITY_CALL_VARIADIC_ENABLE
            if (current_token(parser)->kind ==
                PIKA_TOKEN_DOUBLE_STAR) {
                return fail_current(
                    parser, PIKA_STATUS_UNSUPPORTED_SYNTAX);
            }
            if (current_token(parser)->kind == PIKA_TOKEN_STAR) {
                status = require_capability_at(
                    parser, PIKA_CAPABILITY_VARIADIC_CALL,
                    current_token(parser));
                if (status != PIKA_STATUS_OK) return status;
                if (variadic_parameter != UINT8_MAX) {
                    return fail_current(
                        parser, PIKA_STATUS_SYNTAX_ERROR);
                }
                variadic_parameter = parameter_count;
                is_variadic = 1;
                ++parser->position;
                parameter = current_token(parser);
            }
#else
            if (current_token(parser)->kind == PIKA_TOKEN_STAR ||
                current_token(parser)->kind ==
                    PIKA_TOKEN_DOUBLE_STAR) {
                return fail_current(
                    parser, PIKA_STATUS_UNSUPPORTED_SYNTAX);
            }
#endif
            if (parameter_count >= reserved_parameter_count) {
                return fail_current(
                    parser, PIKA_STATUS_INTERNAL_ERROR);
            }
            metadata =
                &declaration->parameters[parameter_count];
            metadata->name_position = parser->position;
            metadata->default_start = UINT32_MAX;
            metadata->default_end = UINT32_MAX;
            metadata->default_constant = UINT16_MAX;
            status = require(parser, PIKA_TOKEN_NAME);
            if (status != PIKA_STATUS_OK) {
                return status;
            }
            if (is_variadic != 0 &&
                current_token(parser)->kind == PIKA_TOKEN_ASSIGN) {
                return fail_current(
                    parser, PIKA_STATUS_SYNTAX_ERROR);
            }
            if (match(parser, PIKA_TOKEN_ASSIGN)) {
                uint32_t width;
                if (owner_class != UINT16_MAX &&
                    parameter_count == 0u) {
                    return fail_at(
                        parser, PIKA_STATUS_SYNTAX_ERROR,
                        parameter);
                }
                status = require_capability_at(
                    parser, PIKA_CAPABILITY_DEFAULT_CALL,
                    token_at(parser, parser->position - 1u));
                if (status != PIKA_STATUS_OK) {
                    return status;
                }
                metadata->default_start = parser->position;
                width = default_literal_width(
                    parser, parser->position);
                if (width == 0u) {
                    return fail_current(
                        parser, PIKA_STATUS_UNSUPPORTED_SYNTAX);
                }
                if (token_at(parser, parser->position)->kind ==
                        PIKA_TOKEN_INTEGER &&
                    integer_literal_is_embedded(
                        parser,
                        token_at(parser, parser->position))) {
                    status = require_capability_at(
                        parser,
                        PIKA_CAPABILITY_EMBEDDED_INTEGER,
                        token_at(parser, parser->position));
                    if (status != PIKA_STATUS_OK) {
                        return status;
                    }
                }
                parser->position += width;
                metadata->default_end = parser->position;
                saw_default = 1;
            } else if (is_variadic == 0) {
                if (saw_default) {
                    return fail_current(
                        parser, PIKA_STATUS_SYNTAX_ERROR);
                }
                ++required_parameter_count;
            }
            ++parameter_count;
            if (!match(parser, PIKA_TOKEN_COMMA)) {
                break;
            }
            if (is_variadic != 0 &&
                current_token(parser)->kind !=
                    PIKA_TOKEN_RIGHT_PAREN) {
                return fail_current(
                    parser, PIKA_STATUS_UNSUPPORTED_SYNTAX);
            }
        }
    }
    if (status == PIKA_STATUS_OK) {
        status = require(parser, PIKA_TOKEN_RIGHT_PAREN);
    }
    if (status == PIKA_STATUS_OK) {
        status = require(parser, PIKA_TOKEN_COLON);
    }
    if (status == PIKA_STATUS_OK) {
        status = require(parser, PIKA_TOKEN_NEWLINE);
    }
    if (status == PIKA_STATUS_OK) {
        status = require(parser, PIKA_TOKEN_INDENT);
    }
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    while (depth > 0u) {
        PikaTokenKind kind = current_token(parser)->kind;
        if (kind == PIKA_TOKEN_EOF) {
            return fail_current(parser, PIKA_STATUS_SYNTAX_ERROR);
        }
        if (kind == PIKA_TOKEN_INDENT) {
            ++depth;
        } else if (kind == PIKA_TOKEN_DEDENT) {
            --depth;
        }
        ++parser->position;
    }
    declaration->header_position = header_position;
    declaration->name_position = name_position;
    declaration->after_body = parser->position;
    declaration->function_index =
        (uint16_t)parser->declaration_count;
    declaration->parameter_count = (uint8_t)parameter_count;
    declaration->required_parameter_count =
        (uint8_t)required_parameter_count;
    declaration->variadic_parameter =
        (uint8_t)variadic_parameter;
    declaration->owner_class = owner_class;
    ++parser->declaration_count;
    return PIKA_STATUS_OK;
}

static uint32_t lambda_body_end(const PikaParser* parser,
                                uint32_t position) {
    uint32_t paren = 0u;
    uint32_t bracket = 0u;
    uint32_t brace = 0u;
    while (position < parser->token_count) {
        PikaTokenKind kind = parser->tokens[position].kind;
        if (kind == PIKA_TOKEN_EOF ||
            (kind == PIKA_TOKEN_NEWLINE && paren == 0u && bracket == 0u && brace == 0u) ||
            (kind == PIKA_TOKEN_COMMA && paren == 0u && bracket == 0u && brace == 0u) ||
            (kind == PIKA_TOKEN_RIGHT_PAREN && paren == 0u && bracket == 0u && brace == 0u) ||
            (kind == PIKA_TOKEN_RIGHT_BRACKET && paren == 0u && bracket == 0u && brace == 0u) ||
            (kind == PIKA_TOKEN_RIGHT_BRACE && paren == 0u && bracket == 0u && brace == 0u)) {
            break;
        }
        if (kind == PIKA_TOKEN_LEFT_PAREN) ++paren;
        else if (kind == PIKA_TOKEN_RIGHT_PAREN && paren > 0u) --paren;
        else if (kind == PIKA_TOKEN_LEFT_BRACKET) ++bracket;
        else if (kind == PIKA_TOKEN_RIGHT_BRACKET && bracket > 0u) --bracket;
        else if (kind == PIKA_TOKEN_LEFT_BRACE) ++brace;
        else if (kind == PIKA_TOKEN_RIGHT_BRACE && brace > 0u) --brace;
        ++position;
    }
    return position;
}

static PikaStatus discover_lambda(PikaParser* parser) {
    PikaFunctionDeclaration* declaration;
    uint32_t header = parser->position;
    uint32_t parameter_count = 0u;
    uint32_t parameter_capacity;
    PikaStatus status = require_capability_at(
        parser, PIKA_CAPABILITY_CALL_REFERENCE, current_token(parser));
    if (status != PIKA_STATUS_OK) return status;
    if (parser->declaration_count >= parser->declaration_capacity ||
        parser->declaration_count >= PIKA_FRONTEND_FUNCTION_LIMIT) {
        return fail_limit_current(parser, PIKA_FRONTEND_BOUND_FUNCTIONS,
                                   PIKA_FRONTEND_FUNCTION_LIMIT,
                                   parser->declaration_count + 1u);
    }
    declaration = &parser->declarations[parser->declaration_count];
    memset(declaration, 0, sizeof(*declaration));
    declaration->header_position = header;
    declaration->name_position = UINT32_MAX;
    declaration->owner_class = UINT16_MAX;
    declaration->is_lambda = 1u;
    ++parser->position;
    parameter_capacity = count_lambda_parameters(parser, header);
    if (parameter_capacity > PIKA_FRONTEND_PARAMETER_LIMIT) {
        parameter_capacity = PIKA_FRONTEND_PARAMETER_LIMIT;
    }
    status = reserve_parameter_declarations(parser, parameter_capacity,
                                            &declaration->parameters);
    if (status != PIKA_STATUS_OK) return status;
    parameter_count = 0u;
    while (current_token(parser)->kind != PIKA_TOKEN_COLON) {
        const PikaToken* parameter = current_token(parser);
        PikaParameterDeclaration* metadata;
        if (parameter->kind == PIKA_TOKEN_STAR ||
            parameter->kind == PIKA_TOKEN_DOUBLE_STAR ||
            parameter->kind == PIKA_TOKEN_ASSIGN) {
            return fail_current(parser, PIKA_STATUS_UNSUPPORTED_SYNTAX);
        }
        if (parameter->kind != PIKA_TOKEN_NAME) {
            return fail_current(parser, PIKA_STATUS_SYNTAX_ERROR);
        }
        metadata = &declaration->parameters[parameter_count++];
        metadata->name_position = parser->position;
        metadata->default_start = UINT32_MAX;
        metadata->default_end = UINT32_MAX;
        metadata->default_constant = UINT16_MAX;
        ++parser->position;
        if (!match(parser, PIKA_TOKEN_COMMA)) break;
    }
    status = require(parser, PIKA_TOKEN_COLON);
    if (status != PIKA_STATUS_OK) return status;
    declaration->body_position = parser->position;
    declaration->after_body = lambda_body_end(parser, parser->position);
    if (declaration->body_position == declaration->after_body) {
        return fail_current(parser, PIKA_STATUS_SYNTAX_ERROR);
    }
    declaration->parameter_count = (uint8_t)parameter_count;
    declaration->required_parameter_count = (uint8_t)parameter_count;
    declaration->variadic_parameter = UINT8_MAX;
    declaration->function_index = (uint16_t)parser->declaration_count;
    ++parser->declaration_count;
    return PIKA_STATUS_OK;
}

static PikaStatus discover_lambdas(PikaParser* parser) {
    uint32_t position = 0u;
    while (position < parser->token_count) {
        if (parser->tokens[position].kind == PIKA_TOKEN_LAMBDA) {
            parser->position = position;
            if (discover_lambda(parser) != PIKA_STATUS_OK) {
                return parser->diagnostic->status;
            }
            position = parser->declarations[parser->declaration_count - 1u].after_body;
            continue;
        }
        ++position;
    }
    return PIKA_STATUS_OK;
}

static int token_matches_text(const PikaParser* parser,
                              const PikaToken* token,
                              const char* text,
                              uint32_t length) {
    return token->length == length &&
           memcmp(&parser->source[token->offset], text, length) == 0;
}

static int is_field_assignment_kind(PikaTokenKind kind) {
    return kind == PIKA_TOKEN_ASSIGN ||
           (kind >= PIKA_TOKEN_PLUS_ASSIGN &&
            kind <= PIKA_TOKEN_RIGHT_SHIFT_ASSIGN) ||
           kind == PIKA_TOKEN_AT_ASSIGN;
}

static PikaStatus add_discovered_field(
    PikaParser* parser,
    PikaClassDeclaration* declaration,
    uint32_t name_position) {
    const PikaToken* name = token_at(parser, name_position);
    PikaFieldDeclaration* field;
    uint16_t field_index;
    PikaStatus status;
    if (find_field(
            parser, declaration->class_index, name, &field_index)) {
        return PIKA_STATUS_OK;
    }
    if (declaration->field_count >= PIKA_FRONTEND_FIELD_LIMIT) {
        return fail_limit_at(
            parser, PIKA_FRONTEND_BOUND_FIELDS,
            PIKA_FRONTEND_FIELD_LIMIT,
            (uint32_t)declaration->field_count + 1u, name);
    }
    status = reserve_class_fields(
        parser, declaration,
        (uint32_t)declaration->field_count + 1u);
    if (status != PIKA_STATUS_OK) return status;
    field = &declaration->fields[declaration->field_count++];
    memset(field, 0, sizeof(*field));
    field->name_position = name_position;
    field->kind = PIKA_VALUE_UNKNOWN;
    field->binding_kind = PIKA_BINDING_VALUE_ANY;
    field->class_index = UINT16_MAX;
    return PIKA_STATUS_OK;
}

static PikaStatus discover_class(PikaParser* parser) {
    const PikaToken* class_token = current_token(parser);
    const PikaToken* name;
    const PikaClassDeclaration* existing_class;
    const PikaFunctionDeclaration* existing_function;
    PikaClassDeclaration* declaration;
    uint32_t header_position = parser->position;
    uint32_t name_position;
    uint32_t body_position;
    uint16_t base_class = UINT16_MAX;
    PikaStatus status = require_capability_at(
        parser, PIKA_CAPABILITY_OBJECT_CLASS, class_token);
    if (status != PIKA_STATUS_OK) return status;
    if (parser->class_count >= PIKA_FRONTEND_CLASS_LIMIT) {
        return fail_limit_current(
            parser, PIKA_FRONTEND_BOUND_CLASSES,
            PIKA_FRONTEND_CLASS_LIMIT, parser->class_count + 1u);
    }
    if (parser->class_count >= parser->class_capacity) {
        return fail_current(
            parser, PIKA_STATUS_INTERNAL_ERROR);
    }
    status = require(parser, PIKA_TOKEN_CLASS);
    name_position = parser->position;
    name = current_token(parser);
    if (status == PIKA_STATUS_OK) status = require(parser, PIKA_TOKEN_NAME);
    if (status != PIKA_STATUS_OK) return status;
    existing_class = find_class(parser, name);
    existing_function = find_function(parser, name);
    if ((existing_class != NULL &&
         existing_class->class_index >= parser->base_class_count) ||
        (existing_function != NULL &&
         existing_function->function_index >=
             parser->base_declaration_count)) {
        return fail_at(parser, PIKA_STATUS_SYNTAX_ERROR, name);
    }
    if (match(parser, PIKA_TOKEN_LEFT_PAREN)) {
        const PikaToken* base_name = current_token(parser);
        const PikaClassDeclaration* base;
        status = require(parser, PIKA_TOKEN_NAME);
        while (status == PIKA_STATUS_OK && match(parser, PIKA_TOKEN_DOT)) {
            base_name = current_token(parser);
            status = require(parser, PIKA_TOKEN_NAME);
        }
        if (status == PIKA_STATUS_OK) {
            base = find_class(parser, base_name);
            if (base == NULL) {
                return fail_at(parser, PIKA_STATUS_UNDEFINED_NAME, base_name);
            }
            base_class = base->class_index;
        }
        if (status == PIKA_STATUS_OK) {
            status = require(parser, PIKA_TOKEN_RIGHT_PAREN);
        }
        if (status != PIKA_STATUS_OK) return status;
    }
    status = require(parser, PIKA_TOKEN_COLON);
    if (status == PIKA_STATUS_OK) status = require(parser, PIKA_TOKEN_NEWLINE);
    if (status == PIKA_STATUS_OK) status = require(parser, PIKA_TOKEN_INDENT);
    if (status != PIKA_STATUS_OK) return status;

    declaration = &parser->classes[parser->class_count];
    memset(declaration, 0, sizeof(*declaration));
    declaration->header_position = header_position;
    declaration->name_position = name_position;
    declaration->class_index = (uint16_t)parser->class_count;
    declaration->base_class = base_class;
    declaration->field_count =
        base_class == UINT16_MAX
            ? 0u
            : parser->classes[base_class].field_count;
    status = reserve_class_fields(
        parser, declaration, declaration->field_count);
    if (status != PIKA_STATUS_OK) return status;
    ++parser->class_count;
    body_position = parser->position;

    while (current_token(parser)->kind != PIKA_TOKEN_DEDENT) {
        if (current_token(parser)->kind == PIKA_TOKEN_EOF) {
            return fail_current(parser, PIKA_STATUS_SYNTAX_ERROR);
        }
        if (match(parser, PIKA_TOKEN_NEWLINE)) continue;
        if (current_token(parser)->kind == PIKA_TOKEN_DEF) {
            if (declaration->member_count >=
                PIKA_FRONTEND_CLASS_MEMBER_LIMIT) {
                return fail_limit_current(
                    parser, PIKA_FRONTEND_BOUND_CLASS_MEMBERS,
                    PIKA_FRONTEND_CLASS_MEMBER_LIMIT,
                    (uint32_t)declaration->member_count + 1u);
            }
            status = discover_declaration(
                parser, declaration->class_index);
            if (status != PIKA_STATUS_OK) return status;
            ++declaration->member_count;
            continue;
        }
        if (match(parser, PIKA_TOKEN_PASS)) {
            status = require(parser, PIKA_TOKEN_NEWLINE);
            if (status != PIKA_STATUS_OK) return status;
            continue;
        }
        return fail_current(parser, PIKA_STATUS_UNSUPPORTED_SYNTAX);
    }
    ++parser->position;
    declaration->after_body = parser->position;

    while (body_position + 3u < declaration->after_body) {
        const PikaToken* owner = token_at(parser, body_position);
        const PikaToken* member = token_at(parser, body_position + 2u);
        PikaTokenKind assignment =
            token_at(parser, body_position + 3u)->kind;
        if (owner->kind == PIKA_TOKEN_NAME &&
            token_matches_text(parser, owner, "self", 4u) &&
            token_at(parser, body_position + 1u)->kind == PIKA_TOKEN_DOT &&
            member->kind == PIKA_TOKEN_NAME &&
            is_field_assignment_kind(assignment)) {
            status = add_discovered_field(
                parser, declaration, body_position + 2u);
            if (status != PIKA_STATUS_OK) return status;
        }
        ++body_position;
    }
    return PIKA_STATUS_OK;
}

static PikaStatus discover_functions(PikaParser* parser) {
    uint32_t depth = 0u;
    parser->position = 0u;
    while (current_token(parser)->kind != PIKA_TOKEN_EOF) {
        PikaTokenKind kind = current_token(parser)->kind;
        if (kind == PIKA_TOKEN_DEF && depth == 0u) {
            PikaStatus status = discover_declaration(parser, UINT16_MAX);
            if (status != PIKA_STATUS_OK) {
                return status;
            }
            continue;
        }
        if (kind == PIKA_TOKEN_CLASS && depth == 0u) {
            PikaStatus status = discover_class(parser);
            if (status != PIKA_STATUS_OK) {
                return status;
            }
            continue;
        }
        if (kind == PIKA_TOKEN_NAME && depth == 0u &&
            (is_assignment_token(next_token(parser)->kind)
#if PIKA_CAPABILITY_ASSIGNMENT_UNPACK_ENABLE
             || is_unpack_assignment_target(
                    parser, parser->position)
#endif
            )) {
            const PikaToken* name = current_token(parser);
            PikaSymbol* global = find_module_global(parser, name);
            if (global == NULL) {
                if (parser->module_global_count >=
                        parser->module_global_capacity ||
                    parser->module_global_count >=
                        PIKA_FRONTEND_SYMBOL_LIMIT) {
                    return fail_limit_current(
                        parser, PIKA_FRONTEND_BOUND_SYMBOLS,
                        PIKA_FRONTEND_SYMBOL_LIMIT,
                        parser->module_global_count + 1u);
                }
                global = &parser->module_globals[
                    parser->module_global_count];
                memset(global, 0, sizeof(*global));
                global->name_source = parser->source;
                global->offset = name->offset;
                global->length = name->length;
                global->slot =
                    (uint8_t)parser->module_global_count;
                global->global_slot = global->slot;
                global->kind = PIKA_VALUE_UNKNOWN;
                global->binding_kind =
                    PIKA_BINDING_VALUE_ANY;
                global->class_index = UINT16_MAX;
                ++parser->module_global_count;
            }
            if (next_token(parser)->kind == PIKA_TOKEN_ASSIGN) {
                const PikaToken* value =
                    token_at(parser, parser->position + 2u);
                const PikaClassDeclaration* declaration = NULL;
                global->kind = PIKA_VALUE_UNKNOWN;
                global->binding_kind = PIKA_BINDING_VALUE_ANY;
                global->binding_class = NULL;
                global->class_index = UINT16_MAX;
                global->first_element_shape = 0u;
                global->element_shape_count = 0u;
                if (value->kind == PIKA_TOKEN_NAME &&
                    token_at(parser, parser->position + 3u)->kind ==
                        PIKA_TOKEN_LEFT_PAREN) {
                    declaration = find_class(parser, value);
                }
                if (declaration != NULL) {
                    global->kind = PIKA_VALUE_INSTANCE;
                    global->class_index =
                        declaration->class_index;
                }
            }
        }
        if (kind == PIKA_TOKEN_CLASS && depth == 0u) {
            PikaStatus status = discover_class(parser);
            if (status != PIKA_STATUS_OK) return status;
            continue;
        }
        if (kind == PIKA_TOKEN_INDENT) {
            ++depth;
        } else if (kind == PIKA_TOKEN_DEDENT && depth > 0u) {
            --depth;
        }
        ++parser->position;
    }
    return PIKA_STATUS_OK;
}

static PikaStatus append_default_return(PikaParser* parser) {
    uint8_t slot = 0u;
    PikaStatus status = allocate_slot(parser, &slot);
    if (status == PIKA_STATUS_OK) {
        status = emit_instruction(
            parser, PIKA_OP_CONSTANT, slot, 0u, 0u, 0, NULL);
    }
    if (status == PIKA_STATUS_OK) {
        status = emit_instruction(
            parser, PIKA_OP_RETURN, slot, 0u, 0u, 0, NULL);
    }
    return status;
}

static void record_function(PikaParser* parser,
                            uint16_t function_index,
                            uint32_t first_instruction,
                            uint8_t parameter_count,
                            uint16_t owner_class) {
    PikaFunction* function =
        &parser->module->functions[function_index];
    function->first_instruction = first_instruction;
    function->instruction_count =
        parser->module->program.instruction_count - first_instruction;
    function->slot_count = parser->slot_high_water;
    function->parameter_count = parameter_count;
    function->reserved = owner_class == UINT16_MAX
                             ? 0u
                             : (uint8_t)(owner_class + 1u);
}

static PikaStatus prepare_function_scope(
    PikaParser* parser,
    const PikaFunctionDeclaration* declaration) {
    uint32_t parameter_count = 0u;
    PikaStatus status;
    parser->position = declaration->header_position;
    status = require(parser, PIKA_TOKEN_DEF);
    if (status == PIKA_STATUS_OK) {
        status = require(parser, PIKA_TOKEN_NAME);
    }
    if (status == PIKA_STATUS_OK) {
        status = require(parser, PIKA_TOKEN_LEFT_PAREN);
    }
    if (status == PIKA_STATUS_OK &&
        current_token(parser)->kind != PIKA_TOKEN_RIGHT_PAREN) {
        for (;;) {
            const PikaToken* parameter = current_token(parser);
            uint8_t slot;
            int is_variadic =
                declaration->variadic_parameter == parameter_count;
            PikaValueKind kind =
                declaration->owner_class != UINT16_MAX &&
                        parameter_count == 0u
                    ? PIKA_VALUE_INSTANCE
                    : is_variadic != 0
                          ? PIKA_VALUE_TUPLE
#if PIKA_TYPED_RUNTIME_ENABLE
                    : PIKA_VALUE_UNKNOWN;
#else
                    : PIKA_VALUE_INTEGER;
#endif
            if (find_symbol(parser, parameter) != NULL) {
                return fail_at(
                    parser, PIKA_STATUS_SYNTAX_ERROR, parameter);
            }
            if (kind == PIKA_VALUE_INSTANCE &&
                !token_matches_text(parser, parameter, "self", 4u)) {
                return fail_at(
                    parser, PIKA_STATUS_SYNTAX_ERROR, parameter);
            }
            if (is_variadic != 0) {
                status = require(parser, PIKA_TOKEN_STAR);
                parameter = current_token(parser);
            }
            if (status == PIKA_STATUS_OK) {
                status = require(parser, PIKA_TOKEN_NAME);
            }
            if (status == PIKA_STATUS_OK) {
                status = assign_symbol(
                    parser, parameter, kind, &slot);
            }
            if (status == PIKA_STATUS_OK &&
                match(parser, PIKA_TOKEN_ASSIGN)) {
                uint32_t width = default_literal_width(
                    parser, parser->position);
                if (width == 0u) {
                    return fail_current(
                        parser, PIKA_STATUS_INTERNAL_ERROR);
                }
                parser->position += width;
            }
            if (status != PIKA_STATUS_OK) {
                return status;
            }
            if (kind == PIKA_VALUE_INSTANCE) {
                PikaSymbol* symbol = find_symbol(parser, parameter);
                symbol->class_index = declaration->owner_class;
                mark_typed(parser);
            } else if (kind == PIKA_VALUE_UNKNOWN) {
                mark_typed(parser);
            }
            ++parameter_count;
            if (!match(parser, PIKA_TOKEN_COMMA)) {
                break;
            }
        }
    }
    if (status == PIKA_STATUS_OK) {
        status = require(parser, PIKA_TOKEN_RIGHT_PAREN);
    }
    if (status == PIKA_STATUS_OK) {
        status = require(parser, PIKA_TOKEN_COLON);
    }
    if (status == PIKA_STATUS_OK) {
        status = require(parser, PIKA_TOKEN_NEWLINE);
    }
    if (status == PIKA_STATUS_OK) {
        status = require(parser, PIKA_TOKEN_INDENT);
    }
    if (status == PIKA_STATUS_OK &&
        parameter_count != declaration->parameter_count) {
        status = fail_current(parser, PIKA_STATUS_INTERNAL_ERROR);
    }
    return status;
}

static PikaStatus compile_function(
    PikaParser* parser,
    const PikaFunctionDeclaration* declaration) {
    uint32_t first_instruction;
    PikaStatus status;
    reset_scope(parser, 1);
    parser->current_function = declaration->function_index;
    status = prepare_function_scope(parser, declaration);
    first_instruction = parser->module->program.instruction_count;
    parser->function_instruction_start = first_instruction;
    if (status == PIKA_STATUS_OK) {
        status = parse_suite(parser);
    }
    if (status == PIKA_STATUS_OK &&
        parser->position != declaration->after_body) {
        status = fail_current(parser, PIKA_STATUS_INTERNAL_ERROR);
    }
    if (status == PIKA_STATUS_OK) {
        status = append_default_return(parser);
    }
    if (status == PIKA_STATUS_OK) {
        record_function(
            parser, declaration->function_index, first_instruction,
            declaration->parameter_count, declaration->owner_class);
    }
    return status;
}

static PikaStatus compile_lambda(
    PikaParser* parser,
    const PikaFunctionDeclaration* declaration) {
    uint32_t first_instruction;
    uint32_t index;
    PikaStatus status = PIKA_STATUS_OK;
    PikaExpression expression;
    uint32_t global_index;
    reset_scope(parser, 1);
    parser->current_function = declaration->function_index;
    for (global_index = 0u;
         global_index < parser->module_global_count;
         ++global_index) {
        PikaSymbol* global = &parser->module_globals[global_index];
        if (parser->symbol_count >= parser->symbol_capacity) {
            return fail_current(parser, PIKA_STATUS_FRONTEND_LIMIT);
        }
        parser->symbols[parser->symbol_count] = *global;
        parser->symbols[parser->symbol_count].is_global = 1u;
        ++parser->symbol_count;
    }
    parser->position = declaration->header_position + 1u;
    index = 0u;
    while (current_token(parser)->kind != PIKA_TOKEN_COLON) {
        const PikaToken* parameter = current_token(parser);
        uint8_t slot = 0u;
        if (parameter->kind != PIKA_TOKEN_NAME ||
            index >= declaration->parameter_count) {
            return fail_current(parser, PIKA_STATUS_INTERNAL_ERROR);
        }
        status = assign_symbol(parser, parameter, PIKA_VALUE_UNKNOWN, &slot);
        if (status != PIKA_STATUS_OK) return status;
        ++index;
        ++parser->position;
        if (!match(parser, PIKA_TOKEN_COMMA)) break;
    }
    parser->position = declaration->body_position;
    first_instruction = parser->module->program.instruction_count;
    parser->function_instruction_start = first_instruction;
    memset(&expression, 0, sizeof(expression));
    status = parse_expression(parser, &expression);
    if (status == PIKA_STATUS_OK) status = record_return_shape(parser, &expression);
    if (status == PIKA_STATUS_OK) status = emit_instruction(
        parser, PIKA_OP_RETURN, expression.slot, 0u, 0u, 0, NULL);
    if (status == PIKA_STATUS_OK && parser->position != declaration->after_body) {
        status = fail_current(parser, PIKA_STATUS_INTERNAL_ERROR);
    }
    if (status == PIKA_STATUS_OK) record_function(
        parser, declaration->function_index, first_instruction,
        declaration->parameter_count, UINT16_MAX);
    return status;
}

static PikaStatus compile_module(PikaParser* parser) {
    uint16_t function_index =
        (uint16_t)parser->declaration_count;
    uint32_t first_instruction =
        parser->module->program.instruction_count;
    PikaStatus status = PIKA_STATUS_OK;
    reset_scope(parser, 0);
    parser->current_function = function_index;
    parser->position = 0u;
    while (current_token(parser)->kind != PIKA_TOKEN_EOF) {
        if (match(parser, PIKA_TOKEN_NEWLINE)) {
            continue;
        }
        if (current_token(parser)->kind == PIKA_TOKEN_DEF) {
            const PikaFunctionDeclaration* declaration =
                declaration_at(parser, parser->position);
            if (declaration == NULL) {
                return fail_current(
                    parser, PIKA_STATUS_UNSUPPORTED_SYNTAX);
            }
            parser->position = declaration->after_body;
            continue;
        }
        if (current_token(parser)->kind == PIKA_TOKEN_CLASS) {
            const PikaClassDeclaration* declaration =
                class_at(parser, parser->position);
            if (declaration == NULL) {
                return fail_current(
                    parser, PIKA_STATUS_UNSUPPORTED_SYNTAX);
            }
            parser->position = declaration->after_body;
            continue;
        }
        status = parse_statement(parser);
        if (status != PIKA_STATUS_OK) {
            return status;
        }
        release_statement_slots(parser);
    }
    if (!parser->interactive_return) {
        status = append_default_return(parser);
    }
    if (status == PIKA_STATUS_OK) {
        record_function(
            parser, function_index, first_instruction, 0u,
            UINT16_MAX);
    }
    return status;
}

static PikaStatus retain_semantic_name(
    PikaParser* parser,
    const char* stable_name,
    uint16_t stable_length,
    uint32_t name_position,
    uint32_t* offset,
    uint16_t* length) {
    PikaCompiledModule* module = parser->module;
    const PikaToken* token;
    if (stable_name != NULL) {
        *offset = (uint32_t)(
            stable_name - module->semantic_names);
        *length = stable_length;
        return semantic_name_valid(
                   module, *offset, *length)
                   ? PIKA_STATUS_OK
                   : fail_current(
                         parser, PIKA_STATUS_INTERNAL_ERROR);
    }
    token = token_at(parser, name_position);
    if (token->length >
            module->semantic_name_capacity -
                module->semantic_name_count) {
        return fail_at(
            parser, PIKA_STATUS_FRONTEND_LIMIT, token);
    }
    *offset = module->semantic_name_count;
    *length = (uint16_t)token->length;
    memcpy(
        &module->semantic_names[module->semantic_name_count],
        &parser->source[token->offset], token->length);
    module->semantic_name_count += token->length;
    return PIKA_STATUS_OK;
}

static PikaStatus retain_source_name(
    PikaParser* parser,
    const char* source,
    uint32_t source_offset,
    uint32_t source_length,
    uint32_t* offset,
    uint16_t* length) {
    PikaCompiledModule* module = parser->module;
    if (source == module->semantic_names) {
        if (source_length > UINT16_MAX ||
            !semantic_name_valid(
                module, source_offset,
                (uint16_t)source_length)) {
            return fail_current(
                parser, PIKA_STATUS_INTERNAL_ERROR);
        }
        *offset = source_offset;
        *length = (uint16_t)source_length;
        return PIKA_STATUS_OK;
    }
    if (source == NULL || source_length > UINT16_MAX ||
        source_length >
            module->semantic_name_capacity -
                module->semantic_name_count) {
        return fail_current(
            parser, PIKA_STATUS_FRONTEND_LIMIT);
    }
    *offset = module->semantic_name_count;
    *length = (uint16_t)source_length;
    memcpy(
        &module->semantic_names[module->semantic_name_count],
        &source[source_offset], source_length);
    module->semantic_name_count += source_length;
    return PIKA_STATUS_OK;
}

static PikaStatus append_semantic_parameter(
    PikaParser* parser,
    const PikaSemanticParameter* parameter) {
    PikaCompiledModule* module = parser->module;
    if (module->semantic_parameter_count ==
        module->semantic_parameter_capacity) {
        uint32_t capacity =
            module->semantic_parameter_capacity == 0u
                ? 8u
                : (uint32_t)
                      module->semantic_parameter_capacity * 2u;
        PikaSemanticParameter* parameters;
        if (capacity > UINT16_MAX) capacity = UINT16_MAX;
        if (capacity <= module->semantic_parameter_count) {
            return fail_current(
                parser, PIKA_STATUS_FRONTEND_LIMIT);
        }
        parameters = (PikaSemanticParameter*)realloc(
            module->semantic_parameters,
            capacity * sizeof(PikaSemanticParameter));
        if (parameters == NULL) {
            return fail_current(
                parser, PIKA_STATUS_FRONTEND_LIMIT);
        }
        module->semantic_parameters = parameters;
        module->semantic_parameter_capacity =
            (uint16_t)capacity;
    }
    module->semantic_parameters[
        module->semantic_parameter_count++] = *parameter;
    return PIKA_STATUS_OK;
}

static PikaStatus retain_default_constant(
    PikaParser* parser,
    const PikaParameterDeclaration* parameter,
    uint16_t* constant_index) {
    const PikaToken* token;
    int negative = 0;
    PikaConstant constant;
    PikaStatus status;
    uint32_t position = parameter->default_start;
    *constant_index = UINT16_MAX;
    if (position == UINT32_MAX) return PIKA_STATUS_OK;
    token = token_at(parser, position);
    if (token->kind == PIKA_TOKEN_PLUS ||
        token->kind == PIKA_TOKEN_MINUS) {
        negative = token->kind == PIKA_TOKEN_MINUS;
        token = token_at(parser, ++position);
    }
    memset(&constant, 0, sizeof(constant));
    if (token->kind == PIKA_TOKEN_INTEGER) {
        int64_t value =
            pika_token_integer_value(parser->source, token);
        constant.kind = PIKA_CONSTANT_INTEGER;
        constant.as.integer = negative ? -value : value;
    } else if (token->kind == PIKA_TOKEN_FLOAT) {
        constant.kind = PIKA_CONSTANT_FLOAT;
        status = pika_token_float_value(
            parser->source, token, &constant.as.floating);
        if (status != PIKA_STATUS_OK) {
            return fail_at(parser, status, token);
        }
        if (negative) {
            constant.as.floating = -constant.as.floating;
        }
    } else if (token->kind == PIKA_TOKEN_STRING ||
               token->kind == PIKA_TOKEN_BYTES) {
        if (negative) {
            return fail_at(
                parser, PIKA_STATUS_INTERNAL_ERROR, token);
        }
        status = make_string_constant_sequence(
            parser, position, parameter->default_end,
            &constant);
        if (status != PIKA_STATUS_OK) return status;
    } else if (token->kind == PIKA_TOKEN_TRUE ||
               token->kind == PIKA_TOKEN_FALSE) {
        constant.kind = PIKA_CONSTANT_BOOLEAN;
        constant.as.integer =
            token->kind == PIKA_TOKEN_TRUE ? 1 : 0;
    } else if (token->kind == PIKA_TOKEN_NONE) {
        constant.kind = PIKA_CONSTANT_NONE;
    } else {
        return fail_at(
            parser, PIKA_STATUS_INTERNAL_ERROR, token);
    }
    status = append_constant(
        parser, &constant, constant_index);
    if (status != PIKA_STATUS_OK &&
        (constant.kind == PIKA_CONSTANT_STRING ||
         constant.kind == PIKA_CONSTANT_BYTES)) {
        free((void*)constant.as.bytes);
    }
    return status;
}

static PikaStatus retain_semantic_functions(
    PikaParser* parser) {
    PikaCompiledModule* module = parser->module;
    uint32_t index;
    PikaStatus reserve_status = PIKA_STATUS_OK;
    module->semantic_functions =
        (PikaSemanticFunction*)grow_parser_array(
            parser, module->semantic_functions,
            &module->semantic_function_capacity,
            parser->declaration_count,
            PIKA_FRONTEND_FUNCTION_LIMIT,
            sizeof(*module->semantic_functions),
            &reserve_status);
    if (reserve_status != PIKA_STATUS_OK) {
        return reserve_status;
    }
    for (index = parser->base_declaration_count;
         index < parser->declaration_count;
         ++index) {
        const PikaFunctionDeclaration* declaration =
            &parser->declarations[index];
        PikaSemanticFunction* semantic;
        uint32_t parameter_index;
        PikaStatus status;
        if (module->semantic_function_count != index) {
            return fail_current(
                parser, PIKA_STATUS_INTERNAL_ERROR);
        }
        semantic = &module->semantic_functions[index];
        memset(semantic, 0, sizeof(*semantic));
        status = retain_semantic_name(
            parser, declaration->stable_name,
            declaration->stable_name_length,
            declaration->name_position,
            &semantic->name_offset,
            &semantic->name_length);
        if (status != PIKA_STATUS_OK) return status;
        semantic->first_parameter =
            module->semantic_parameter_count;
        semantic->parameter_count =
            declaration->parameter_count;
        semantic->required_parameter_count =
            declaration->required_parameter_count;
        semantic->variadic_parameter =
            declaration->variadic_parameter;
        semantic->owner_class = declaration->owner_class;
        semantic->return_shape =
            declaration->return_shape;
        semantic->first_return_element_shape =
            declaration->first_return_element_shape;
        semantic->return_element_shape_count =
            declaration->return_element_shape_count;
        semantic->has_return_shape =
            declaration->has_return_shape;
        for (parameter_index = 0u;
             parameter_index < declaration->parameter_count;
             ++parameter_index) {
            const PikaParameterDeclaration* parameter =
                &declaration->parameters[parameter_index];
            PikaSemanticParameter retained;
            memset(&retained, 0, sizeof(retained));
            status = retain_semantic_name(
                parser, parameter->stable_name,
                parameter->stable_name_length,
                parameter->name_position,
                &retained.name_offset,
                &retained.name_length);
            if (status == PIKA_STATUS_OK) {
                status = retain_default_constant(
                    parser, parameter,
                    &retained.default_constant);
            }
            if (status == PIKA_STATUS_OK) {
                status = append_semantic_parameter(
                    parser, &retained);
            }
            if (status != PIKA_STATUS_OK) return status;
        }
        ++module->semantic_function_count;
    }
    return PIKA_STATUS_OK;
}

static PikaStatus retain_semantic_classes(
    PikaParser* parser) {
    PikaCompiledModule* module = parser->module;
    uint32_t index;
    PikaStatus reserve_status = PIKA_STATUS_OK;
    module->semantic_classes =
        (PikaSemanticClass*)grow_parser_array(
            parser, module->semantic_classes,
            &module->semantic_class_capacity,
            parser->class_count,
            PIKA_FRONTEND_CLASS_LIMIT,
            sizeof(*module->semantic_classes),
            &reserve_status);
    if (reserve_status != PIKA_STATUS_OK) {
        return reserve_status;
    }
    for (index = parser->base_class_count;
         index < parser->class_count;
         ++index) {
        const PikaClassDeclaration* declaration =
            &parser->classes[index];
        PikaSemanticClass* semantic;
        uint32_t field_index;
        PikaStatus status;
        if (module->semantic_class_count != index) {
            return fail_current(
                parser, PIKA_STATUS_INTERNAL_ERROR);
        }
        semantic = &module->semantic_classes[index];
        memset(semantic, 0, sizeof(*semantic));
        status = retain_semantic_name(
            parser, declaration->stable_name,
            declaration->stable_name_length,
            declaration->name_position,
            &semantic->name_offset,
            &semantic->name_length);
        if (status != PIKA_STATUS_OK) return status;
        semantic->base_class = declaration->base_class;
        semantic->first_field =
            module->semantic_field_count;
        semantic->field_count = declaration->field_count;
        semantic->member_count = declaration->member_count;
        module->semantic_fields =
            (PikaSemanticField*)grow_parser_array(
                parser, module->semantic_fields,
                &module->semantic_field_capacity,
                (uint32_t)module->semantic_field_count +
                    declaration->field_count,
                PIKA_FRONTEND_CLASS_LIMIT *
                    PIKA_FRONTEND_FIELD_LIMIT,
                sizeof(*module->semantic_fields), &status);
        if (status != PIKA_STATUS_OK) return status;
        for (field_index = 0u;
             field_index < declaration->field_count;
             ++field_index) {
            const PikaFieldDeclaration* field =
                &declaration->fields[field_index];
            PikaSemanticField* retained;
            if (module->semantic_field_count >=
                PIKA_FRONTEND_CLASS_LIMIT *
                    PIKA_FRONTEND_FIELD_LIMIT) {
                return fail_current(
                    parser, PIKA_STATUS_FRONTEND_LIMIT);
            }
            retained = &module->semantic_fields[
                module->semantic_field_count];
            memset(retained, 0, sizeof(*retained));
            status = retain_semantic_name(
                parser, field->stable_name,
                field->stable_name_length,
                field->name_position,
                &retained->name_offset,
                &retained->name_length);
            if (status != PIKA_STATUS_OK) return status;
            retained->kind = field->kind;
            retained->binding_kind = field->binding_kind;
            retained->allows_none = field->allows_none;
            retained->binding_class = field->binding_class;
            retained->class_index = field->class_index;
            ++module->semantic_field_count;
        }
        ++module->semantic_class_count;
    }
    return PIKA_STATUS_OK;
}

static PikaStatus retain_semantic_globals(
    PikaParser* parser) {
    PikaCompiledModule* module = parser->module;
    uint32_t index;
    PikaStatus reserve_status = PIKA_STATUS_OK;
    module->semantic_globals =
        (PikaSemanticGlobal*)grow_parser_array(
            parser, module->semantic_globals,
            &module->semantic_global_capacity,
            parser->module_global_count,
            PIKA_FRONTEND_SYMBOL_LIMIT,
            sizeof(*module->semantic_globals),
            &reserve_status);
    if (reserve_status != PIKA_STATUS_OK) {
        return reserve_status;
    }
    for (index = 0u;
         index < parser->module_global_count;
         ++index) {
        const PikaSymbol* declared =
            &parser->module_globals[index];
        const PikaSymbol* global =
            &parser->symbols[index];
        PikaSemanticGlobal* semantic =
            &module->semantic_globals[index];
        PikaStatus status = PIKA_STATUS_OK;
        if (index >= module->semantic_global_count) {
            status = retain_source_name(
                parser, declared->name_source,
                declared->offset, declared->length,
                &semantic->name_offset,
                &semantic->name_length);
        }
        if (status != PIKA_STATUS_OK) return status;
        semantic->slot = global->slot;
        semantic->kind = (uint8_t)global->kind;
        semantic->binding_kind =
            (uint8_t)global->binding_kind;
        semantic->binding_class = global->binding_class;
        semantic->class_index = global->class_index;
        semantic->first_element_shape =
            global->first_element_shape;
        semantic->element_shape_count =
            global->element_shape_count;
    }
    module->semantic_global_count =
        (uint16_t)parser->module_global_count;
    return PIKA_STATUS_OK;
}

static PikaStatus retain_semantic_imports(
    PikaParser* parser) {
    PikaCompiledModule* module = parser->module;
    uint32_t index;
    PikaStatus reserve_status = PIKA_STATUS_OK;
    module->semantic_imports =
        (PikaSemanticImport*)grow_parser_array(
            parser, module->semantic_imports,
            &module->semantic_import_capacity,
            parser->import_count,
            PIKA_FRONTEND_SYMBOL_LIMIT,
            sizeof(*module->semantic_imports),
            &reserve_status);
    if (reserve_status != PIKA_STATUS_OK) {
        return reserve_status;
    }
    for (index = parser->base_import_count;
         index < parser->import_count;
         ++index) {
        const PikaImportedModule* imported =
            &parser->imports[index];
        PikaSemanticImport* semantic =
            &module->semantic_imports[index];
        PikaStatus status = retain_source_name(
            parser, imported->name_source,
            imported->offset, imported->length,
            &semantic->name_offset,
            &semantic->name_length);
        if (status != PIKA_STATUS_OK) return status;
        semantic->selected = imported->selected;
        semantic->module = imported->module;
    }
    module->semantic_import_count =
        (uint16_t)parser->import_count;
    return PIKA_STATUS_OK;
}

static void release_parameter_catalog(PikaParser* parser) {
    uint32_t index;
    free(parser->parameter_declarations);
    parser->parameter_declarations = NULL;
    parser->parameter_declaration_capacity = 0u;
    parser->parameter_declaration_count = 0u;
    if (parser->declarations != NULL) {
        for (index = 0u; index < parser->declaration_count; ++index) {
            parser->declarations[index].parameters = NULL;
        }
    }
}

static void release_function_catalog(PikaParser* parser) {
    release_parameter_catalog(parser);
    free(parser->declarations);
    parser->declarations = NULL;
    parser->declaration_capacity = 0u;
}

static void release_class_catalog(PikaParser* parser) {
    uint32_t index;
    for (index = 0u; index < parser->class_capacity; ++index) {
        free(parser->classes[index].fields);
        parser->classes[index].fields = NULL;
    }
    free(parser->classes);
    parser->classes = NULL;
    parser->class_capacity = 0u;
}

static void release_global_catalog(PikaParser* parser) {
    free(parser->module_globals);
    parser->module_globals = NULL;
    free(parser->symbols);
    parser->symbols = NULL;
}

static void release_parser(PikaParser* parser) {
    if (parser == NULL) return;
    free(parser->imports);
    parser->imports = NULL;
    release_function_catalog(parser);
    release_global_catalog(parser);
    release_class_catalog(parser);
    free(parser);
}

static uint32_t parser_compound_end(const PikaParser* parser,
                                    uint32_t start) {
    uint32_t cursor = start;
    uint32_t depth;
    while (cursor < parser->token_count &&
           parser->tokens[cursor].kind != PIKA_TOKEN_INDENT &&
           parser->tokens[cursor].kind != PIKA_TOKEN_EOF) {
        ++cursor;
    }
    if (cursor >= parser->token_count ||
        parser->tokens[cursor].kind != PIKA_TOKEN_INDENT) {
        return parser->token_count;
    }
    depth = 1u;
    ++cursor;
    while (cursor < parser->token_count && depth > 0u) {
        if (parser->tokens[cursor].kind == PIKA_TOKEN_INDENT) {
            ++depth;
        } else if (parser->tokens[cursor].kind == PIKA_TOKEN_DEDENT) {
            --depth;
        }
        ++cursor;
    }
    return cursor;
}

static int retained_global_matches_token(
    const PikaParser* parser,
    const PikaToken* token) {
    const PikaCompiledModule* module = parser->module;
    uint32_t index;
    if (module->semantic_globals == NULL) return 0;
    for (index = 0u; index < module->semantic_global_count; ++index) {
        const PikaSemanticGlobal* global =
            &module->semantic_globals[index];
        if (semantic_name_valid(
                module, global->name_offset, global->name_length) &&
            token->length == global->name_length &&
            memcmp(&parser->source[token->offset],
                   &module->semantic_names[global->name_offset],
                   token->length) == 0) {
            return 1;
        }
    }
    return 0;
}

static uint32_t unique_name_count(const PikaParser* parser,
                                  uint32_t start,
                                  uint32_t end,
                                  uint32_t limit) {
    uint32_t count = 0u;
    uint32_t cursor;
    for (cursor = start;
         cursor < end && cursor < parser->token_count;
         ++cursor) {
        uint32_t previous;
        int seen = 0;
        if (parser->tokens[cursor].kind != PIKA_TOKEN_NAME) continue;
        for (previous = start; previous < cursor; ++previous) {
            if (parser->tokens[previous].kind == PIKA_TOKEN_NAME &&
                tokens_text_equal(
                    parser, &parser->tokens[cursor],
                    &parser->tokens[previous])) {
                seen = 1;
                break;
            }
        }
        if (seen == 0 && ++count >= limit) return limit;
    }
    return count;
}

static int module_name_seen_before(const PikaParser* parser,
                                   uint32_t target) {
    uint32_t cursor = 0u;
    uint32_t depth = 0u;
    while (cursor < target) {
        PikaTokenKind kind = parser->tokens[cursor].kind;
        if (depth == 0u &&
            (kind == PIKA_TOKEN_DEF || kind == PIKA_TOKEN_CLASS)) {
            cursor = parser_compound_end(parser, cursor);
            continue;
        }
        if (kind == PIKA_TOKEN_NAME &&
            tokens_text_equal(
                parser, &parser->tokens[target],
                &parser->tokens[cursor])) {
            return 1;
        }
        if (kind == PIKA_TOKEN_INDENT) {
            ++depth;
        } else if (kind == PIKA_TOKEN_DEDENT && depth > 0u) {
            --depth;
        }
        ++cursor;
    }
    return 0;
}

static uint32_t module_scope_symbol_capacity(
    const PikaParser* parser) {
    uint32_t capacity = parser->module->semantic_global_count;
    uint32_t cursor = 0u;
    uint32_t depth = 0u;
    if (capacity >= PIKA_FRONTEND_SYMBOL_LIMIT) {
        return PIKA_FRONTEND_SYMBOL_LIMIT;
    }
    while (cursor < parser->token_count) {
        PikaTokenKind kind = parser->tokens[cursor].kind;
        if (depth == 0u &&
            (kind == PIKA_TOKEN_DEF || kind == PIKA_TOKEN_CLASS)) {
            cursor = parser_compound_end(parser, cursor);
            continue;
        }
        if (kind == PIKA_TOKEN_NAME &&
            !retained_global_matches_token(
                parser, &parser->tokens[cursor]) &&
            !module_name_seen_before(parser, cursor)) {
            if (++capacity >= PIKA_FRONTEND_SYMBOL_LIMIT) {
                return PIKA_FRONTEND_SYMBOL_LIMIT;
            }
        }
        if (kind == PIKA_TOKEN_INDENT) {
            ++depth;
        } else if (kind == PIKA_TOKEN_DEDENT && depth > 0u) {
            --depth;
        }
        ++cursor;
    }
    return capacity;
}

static uint32_t planned_symbol_capacity(const PikaParser* parser) {
    uint32_t capacity = module_scope_symbol_capacity(parser);
    uint32_t cursor;
    for (cursor = 0u; cursor < parser->token_count; ++cursor) {
        uint32_t end;
        uint32_t function_names;
        if (parser->tokens[cursor].kind == PIKA_TOKEN_LAMBDA) {
            end = lambda_body_end(parser, cursor + 1u);
        } else if (parser->tokens[cursor].kind == PIKA_TOKEN_DEF) {
            end = parser_compound_end(parser, cursor);
        } else {
            continue;
        }
        function_names = unique_name_count(
            parser, cursor, end, PIKA_FRONTEND_SYMBOL_LIMIT);
        if (function_names > capacity) capacity = function_names;
        if (capacity >= PIKA_FRONTEND_SYMBOL_LIMIT) {
            return PIKA_FRONTEND_SYMBOL_LIMIT;
        }
    }
    return capacity;
}

static int top_level_assignment_target(const PikaParser* parser,
                                       uint32_t index) {
    if (parser->tokens[index].kind != PIKA_TOKEN_NAME) return 0;
    if (is_assignment_token(token_at(parser, index + 1u)->kind)) {
        return 1;
    }
#if PIKA_CAPABILITY_ASSIGNMENT_UNPACK_ENABLE
    return is_unpack_assignment_target(parser, index);
#else
    return 0;
#endif
}

static int top_level_assignment_seen_before(
    const PikaParser* parser,
    uint32_t target) {
    uint32_t cursor;
    uint32_t depth = 0u;
    for (cursor = 0u; cursor < target; ++cursor) {
        PikaTokenKind kind = parser->tokens[cursor].kind;
        if (depth == 0u &&
            top_level_assignment_target(parser, cursor) &&
            tokens_text_equal(
                parser, &parser->tokens[target],
                &parser->tokens[cursor])) {
            return 1;
        }
        if (kind == PIKA_TOKEN_INDENT) {
            ++depth;
        } else if (kind == PIKA_TOKEN_DEDENT && depth > 0u) {
            --depth;
        }
    }
    return 0;
}

static uint32_t planned_module_global_capacity(
    const PikaParser* parser) {
    uint32_t capacity = parser->module->semantic_global_count;
    uint32_t cursor;
    uint32_t depth = 0u;
    if (capacity >= PIKA_FRONTEND_SYMBOL_LIMIT) {
        return PIKA_FRONTEND_SYMBOL_LIMIT;
    }
    for (cursor = 0u; cursor < parser->token_count; ++cursor) {
        PikaTokenKind kind = parser->tokens[cursor].kind;
        if (depth == 0u &&
            top_level_assignment_target(parser, cursor) &&
            !retained_global_matches_token(
                parser, &parser->tokens[cursor]) &&
            !top_level_assignment_seen_before(parser, cursor)) {
            if (++capacity >= PIKA_FRONTEND_SYMBOL_LIMIT) {
                return PIKA_FRONTEND_SYMBOL_LIMIT;
            }
        }
        if (kind == PIKA_TOKEN_INDENT) {
            ++depth;
        } else if (kind == PIKA_TOKEN_DEDENT && depth > 0u) {
            --depth;
        }
    }
    return capacity;
}

static uint32_t planned_declaration_capacity(
    const PikaParser* parser) {
    uint32_t capacity = parser->module->semantic_function_count;
    uint32_t cursor;
    if (capacity >= PIKA_FRONTEND_FUNCTION_LIMIT) {
        return PIKA_FRONTEND_FUNCTION_LIMIT;
    }
    for (cursor = 0u; cursor < parser->token_count; ++cursor) {
        if ((parser->tokens[cursor].kind == PIKA_TOKEN_DEF ||
             parser->tokens[cursor].kind == PIKA_TOKEN_LAMBDA) &&
            ++capacity >= PIKA_FRONTEND_FUNCTION_LIMIT) {
            return PIKA_FRONTEND_FUNCTION_LIMIT;
        }
    }
    return capacity;
}

static uint32_t planned_import_capacity(const PikaParser* parser) {
    uint32_t capacity = parser->module->semantic_import_count;
    uint32_t cursor = 0u;
    if (capacity >= PIKA_FRONTEND_SYMBOL_LIMIT) {
        return PIKA_FRONTEND_SYMBOL_LIMIT;
    }
    while (cursor < parser->token_count) {
        PikaTokenKind kind = parser->tokens[cursor].kind;
        if (kind == PIKA_TOKEN_IMPORT) {
            if (token_at(parser, cursor + 1u)->kind == PIKA_TOKEN_NAME &&
                ++capacity >= PIKA_FRONTEND_SYMBOL_LIMIT) {
                return PIKA_FRONTEND_SYMBOL_LIMIT;
            }
            while (cursor < parser->token_count &&
                   parser->tokens[cursor].kind != PIKA_TOKEN_NEWLINE &&
                   parser->tokens[cursor].kind != PIKA_TOKEN_EOF) {
                ++cursor;
            }
            continue;
        }
        if (kind == PIKA_TOKEN_FROM) {
            uint32_t member = cursor + 1u;
            while (member < parser->token_count &&
                   parser->tokens[member].kind != PIKA_TOKEN_IMPORT &&
                   parser->tokens[member].kind != PIKA_TOKEN_NEWLINE &&
                   parser->tokens[member].kind != PIKA_TOKEN_EOF) {
                ++member;
            }
            if (member < parser->token_count &&
                parser->tokens[member].kind == PIKA_TOKEN_IMPORT) {
                ++member;
                while (member < parser->token_count &&
                       parser->tokens[member].kind !=
                           PIKA_TOKEN_NEWLINE &&
                       parser->tokens[member].kind != PIKA_TOKEN_EOF) {
                    if (parser->tokens[member].kind == PIKA_TOKEN_NAME &&
                        ++capacity >= PIKA_FRONTEND_SYMBOL_LIMIT) {
                        return PIKA_FRONTEND_SYMBOL_LIMIT;
                    }
                    ++member;
                }
            }
            cursor = member;
            continue;
        }
        ++cursor;
    }
    return capacity;
}

static PikaStatus allocate_parser_catalogs(PikaParser* parser) {
    parser->symbol_capacity = planned_symbol_capacity(parser);
    parser->module_global_capacity =
        planned_module_global_capacity(parser);
    parser->declaration_capacity =
        planned_declaration_capacity(parser);
    parser->import_capacity = planned_import_capacity(parser);
    if (parser->symbol_capacity > 0u) {
        parser->symbols = (PikaSymbol*)calloc(
            parser->symbol_capacity, sizeof(*parser->symbols));
    }
    if (parser->module_global_capacity > 0u) {
        parser->module_globals = (PikaSymbol*)calloc(
            parser->module_global_capacity,
            sizeof(*parser->module_globals));
    }
    if (parser->declaration_capacity > 0u) {
        parser->declarations = (PikaFunctionDeclaration*)calloc(
            parser->declaration_capacity,
            sizeof(*parser->declarations));
    }
    if (parser->import_capacity > 0u) {
        parser->imports = (PikaImportedModule*)calloc(
            parser->import_capacity, sizeof(*parser->imports));
    }
    if ((parser->symbol_capacity > 0u && parser->symbols == NULL) ||
        (parser->module_global_capacity > 0u &&
         parser->module_globals == NULL) ||
        (parser->declaration_capacity > 0u &&
         parser->declarations == NULL) ||
        (parser->import_capacity > 0u && parser->imports == NULL)) {
        return fail_current(
            parser, PIKA_STATUS_STORAGE_TOO_SMALL);
    }
    return PIKA_STATUS_OK;
}

static PikaStatus allocate_class_declarations(
    PikaParser* parser) {
    uint32_t capacity = parser->module->semantic_class_count;
    uint32_t index;
    for (index = 0u; index < parser->token_count; ++index) {
        if (parser->tokens[index].kind != PIKA_TOKEN_CLASS) {
            continue;
        }
        if (capacity >= PIKA_FRONTEND_CLASS_LIMIT) {
            return fail_limit_current(
                parser, PIKA_FRONTEND_BOUND_CLASSES,
                PIKA_FRONTEND_CLASS_LIMIT, capacity + 1u);
        }
        ++capacity;
    }
    if (capacity == 0u) return PIKA_STATUS_OK;
    parser->classes = (PikaClassDeclaration*)calloc(
        capacity, sizeof(*parser->classes));
    if (parser->classes == NULL) {
        return fail_current(
            parser, PIKA_STATUS_STORAGE_TOO_SMALL);
    }
    parser->class_capacity = capacity;
    return PIKA_STATUS_OK;
}

PikaStatus pika_frontend_parse(const char* source,
                               const PikaToken* tokens,
                               uint32_t token_count,
                               PikaCapability enabled_capabilities,
                               const PikaBindingRegistry* bindings,
                               int interactive,
                               PikaCompiledModule* module,
                               PikaDiagnostic* diagnostic) {
    PikaParser* parser;
    PikaStatus status;
    uint32_t index;
    if (source == NULL || tokens == NULL || token_count == 0u ||
        module == NULL) {
        pika_frontend_set_diagnostic(
            diagnostic, PIKA_STATUS_INVALID_ARGUMENT, 1u, 1u);
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    parser = (PikaParser*)calloc(1u, sizeof(*parser));
    if (parser == NULL) {
        pika_frontend_set_diagnostic(
            diagnostic, PIKA_STATUS_STORAGE_TOO_SMALL, 1u, 1u);
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    PIKA_FRONTEND_MEMORY_CHECKPOINT(
        "frontend-parser-created", token_count);
    parser->source = source;
    parser->tokens = tokens;
    parser->token_count = token_count;
    parser->module = module;
    parser->diagnostic = diagnostic;
    parser->enabled_capabilities = enabled_capabilities;
    parser->bindings = bindings;
    parser->interactive = interactive;
    parser->base_instruction_count =
        module->program.instruction_count;
    parser->base_binding_call_count =
        module->binding_call_count;
    parser->base_builtin_call_count =
        module->builtin_call_count;
    parser->base_dynamic_method_call_count =
        module->dynamic_method_call_count;
    parser->base_dynamic_method_candidate_count =
        module->dynamic_method_candidate_count;
    parser->base_constant_count = module->constant_count;
    parser->base_index_site_count = module->index_site_count;
    module->has_interactive_result = 0u;
    status = allocate_parser_catalogs(parser);
    if (status != PIKA_STATUS_OK) {
        release_parser(parser);
        return status;
    }
    status = allocate_parameter_declarations(parser);
    if (status != PIKA_STATUS_OK) {
        release_parser(parser);
        return status;
    }
    status = allocate_class_declarations(parser);
    if (status != PIKA_STATUS_OK) {
        release_parser(parser);
        return status;
    }
    PIKA_FRONTEND_MEMORY_CHECKPOINT(
        "frontend-parser-catalogs", token_count);
    status = seed_semantic_catalog(parser);
    if (status != PIKA_STATUS_OK) {
        release_parser(parser);
        return status;
    }
    status = discover_functions(parser);
    if (status != PIKA_STATUS_OK) {
        release_parser(parser);
        return status;
    }
    status = discover_lambdas(parser);
    if (status != PIKA_STATUS_OK) {
        release_parser(parser);
        return status;
    }
    status = discover_imports(parser);
    if (status != PIKA_STATUS_OK) {
        release_parser(parser);
        return status;
    }
    PIKA_FRONTEND_MEMORY_CHECKPOINT(
        "frontend-parser-discovered",
        parser->declaration_count);
    if (parser->declaration_count + 1u >
        module->function_capacity) {
        status = fail_current(
            parser, PIKA_STATUS_FRONTEND_LIMIT);
        release_parser(parser);
        return status;
    }
    for (index = parser->base_declaration_count;
         index < parser->declaration_count;
         ++index) {
        status = parser->declarations[index].is_lambda != 0u
                     ? compile_lambda(parser, &parser->declarations[index])
                     : compile_function(parser, &parser->declarations[index]);
        if (status != PIKA_STATUS_OK) {
            release_parser(parser);
            return status;
        }
    }
    status = retain_semantic_functions(parser);
    if (status != PIKA_STATUS_OK) {
        release_parser(parser);
        return status;
    }
    module->persistent_binding_call_count =
        module->binding_call_count;
    module->persistent_builtin_call_count =
        module->builtin_call_count;
    module->persistent_constant_count =
        module->constant_count;
    module->persistent_index_site_count =
        module->index_site_count;
    module->persistent_exception_handler_count =
        module->exception_handler_count;
    PIKA_FRONTEND_MEMORY_CHECKPOINT(
        "frontend-functions-emitted",
        parser->declaration_count);
    if (parser->base_declaration_count ==
        parser->declaration_count) {
        release_parameter_catalog(parser);
        PIKA_FRONTEND_MEMORY_CHECKPOINT(
            "frontend-parameter-catalog-released",
            module->semantic_parameter_count);
    }
    status = compile_module(parser);
    if (status != PIKA_STATUS_OK) {
        release_parser(parser);
        return status;
    }
    PIKA_FRONTEND_MEMORY_CHECKPOINT(
        "frontend-module-emitted",
        module->program.instruction_count);
    release_function_catalog(parser);
    PIKA_FRONTEND_MEMORY_CHECKPOINT(
        "frontend-function-catalog-released",
        module->semantic_function_count);
    if (status == PIKA_STATUS_OK) {
        status = retain_semantic_classes(parser);
    }
    if (status == PIKA_STATUS_OK) {
        module->program.classes = module->classes;
        module->program.class_count =
            (uint16_t)parser->class_count;
        module->class_count = parser->class_count;
        for (index = 0u; index < parser->class_count; ++index) {
            module->classes[index].field_count =
                parser->classes[index].field_count;
            module->classes[index].reserved =
                parser->classes[index].base_class == UINT16_MAX
                    ? 0u
                    : (uint16_t)(
                          parser->classes[index].base_class + 1u);
        }
        release_class_catalog(parser);
        PIKA_FRONTEND_MEMORY_CHECKPOINT(
            "frontend-class-catalog-released",
            module->semantic_class_count);
    }
    if (status == PIKA_STATUS_OK) {
        status = retain_semantic_globals(parser);
    }
    if (status == PIKA_STATUS_OK) {
        release_global_catalog(parser);
        PIKA_FRONTEND_MEMORY_CHECKPOINT(
            "frontend-global-catalog-released",
            module->semantic_global_count);
    }
    if (status == PIKA_STATUS_OK) {
        status = retain_semantic_imports(parser);
    }
    if (status == PIKA_STATUS_OK) {
        free(parser->imports);
        parser->imports = NULL;
        PIKA_FRONTEND_MEMORY_CHECKPOINT(
            "frontend-import-catalog-released",
            module->semantic_import_count);
    }
    if (status != PIKA_STATUS_OK) {
        release_parser(parser);
        return status;
    }
    PIKA_FRONTEND_MEMORY_CHECKPOINT(
        "frontend-semantic-retained",
        module->semantic_name_count);
    module->program.format_version = PIKA_PROGRAM_FORMAT_VERSION;
    module->program.instructions = module->instructions;
    module->program.functions = module->functions;
    module->program.function_count =
        (uint16_t)(parser->declaration_count + 1u);
    module->program.entry_function =
        (uint16_t)parser->declaration_count;
    module->program.max_call_depth =
        parser->declaration_count == 0u
            ? 1u
            : PIKA_FRONTEND_CALL_DEPTH_LIMIT;
    module->program.binding_calls = module->binding_calls;
    module->program.binding_call_count =
        (uint16_t)module->binding_call_count;
    module->program.builtin_calls = module->builtin_calls;
    module->program.builtin_call_count =
        (uint16_t)module->builtin_call_count;
    module->program.constants = module->constants;
    module->program.constant_count = (uint16_t)module->constant_count;
    module->program.index_sites = module->index_sites;
    module->program.index_site_count = (uint16_t)module->index_site_count;
    module->program.exception_handlers =
        module->exception_handlers;
    module->program.exception_handler_count =
        (uint16_t)module->exception_handler_count;
    if (module->program.uses_typed_values != 0u) {
        uint32_t slots = module->object_slot_estimate + 8u;
        module->program.object_slot_count =
            slots > UINT16_MAX ? UINT16_MAX : (uint16_t)slots;
        module->program.object_bytes = module->object_byte_estimate + 256u;
    }
    pika_frontend_set_diagnostic(
        diagnostic, PIKA_STATUS_OK, 1u, 1u);
    release_parser(parser);
    PIKA_FRONTEND_MEMORY_CHECKPOINT(
        "frontend-parser-released", 0u);
    return PIKA_STATUS_OK;
}
