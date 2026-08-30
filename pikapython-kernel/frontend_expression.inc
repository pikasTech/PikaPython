/* SPEC: PJ2026-050109 network-ready v0.1; expression lowering. */

static PikaStatus parse_expression(PikaParser* parser,
                                   PikaExpression* expression);
static PikaStatus emit_constant(
    PikaParser* parser,
    const PikaConstant* constant,
    PikaExpression* expression);
static PikaStatus reject_comprehension(
    PikaParser* parser,
    PikaTokenKind close_kind);
static PikaStatus parse_argument_list(PikaParser* parser,
                                      PikaExpression* arguments,
                                      uint32_t* count);
static PikaStatus parse_declared_arguments(
    PikaParser* parser,
    const PikaFunctionDeclaration* declaration,
    uint32_t first_parameter,
    PikaExpression* arguments,
    uint32_t* count);
static PikaStatus emit_argument_moves(
    PikaParser* parser,
    const PikaExpression* arguments,
    uint32_t count,
    uint8_t* base);
static PikaStatus parse_declared_function_call(
    PikaParser* parser,
    const PikaFunctionDeclaration* declaration,
    PikaExpression* expression);
#if PIKA_CAPABILITY_OBJECT_ATTRIBUTE_ENABLE
static PikaStatus parse_python_module_function_call(
    PikaParser* parser,
    const PikaToken* member,
    const PikaFunctionDeclaration* declaration,
    PikaExpression* expression);
#endif
static PikaStatus parse_declared_class_call(
    PikaParser* parser,
    const PikaToken* name,
    const PikaClassDeclaration* declaration,
    PikaExpression* expression);
static PikaStatus emit_module_global_value(
    PikaParser* parser,
    const PikaToken* name,
    const PikaSymbol* global,
    PikaExpression* expression);

static PikaStatus enter_expression_depth(PikaParser* parser) {
    if (parser->expression_depth >=
        PIKA_FRONTEND_EXPRESSION_DEPTH_LIMIT) {
        return fail_limit_current(
            parser, PIKA_FRONTEND_BOUND_EXPRESSION_DEPTH,
            PIKA_FRONTEND_EXPRESSION_DEPTH_LIMIT,
            parser->expression_depth + 1u);
    }
    ++parser->expression_depth;
    return PIKA_STATUS_OK;
}

static void leave_expression_depth(PikaParser* parser) {
    --parser->expression_depth;
}

#if 0
static PikaStatus parse_call(PikaParser* parser,
                             PikaExpression* expression) {
    const PikaToken* name = current_token(parser);
    const PikaFunctionDeclaration* declaration =
        find_function(parser, name);
    uint8_t argument_base = 0u;
    uint8_t argument_slots[PIKA_FRONTEND_SYMBOL_LIMIT];
    uint32_t index;
    PikaStatus status;
    status = require_capability_at(
        parser, PIKA_CAPABILITY_POSITIONAL_CALL, name);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    if (declaration == NULL) {
        return fail_at(parser, PIKA_STATUS_UNDEFINED_NAME, name);
    }
    parser->position += 2u;
    status = allocate_slot(parser, &expression->slot);
    for (index = 0u;
         status == PIKA_STATUS_OK &&
         index < declaration->parameter_count;
         ++index) {
        status = allocate_slot(parser, &argument_slots[index]);
        if (index == 0u && status == PIKA_STATUS_OK) {
            argument_base = argument_slots[index];
        }
    }
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    for (index = 0u; index < declaration->parameter_count; ++index) {
        PikaExpression argument;
        if (current_token(parser)->kind == PIKA_TOKEN_RIGHT_PAREN) {
            return fail_current(parser, PIKA_STATUS_ARGUMENT_COUNT);
        }
        status = parse_expression(parser, &argument);
        if (status != PIKA_STATUS_OK) {
            return status;
        }
        status = emit_instruction(
            parser, PIKA_OP_MOVE, argument_slots[index],
            argument.slot, 0u, 0, NULL);
        if (status != PIKA_STATUS_OK) {
            return status;
        }
        if (index + 1u < declaration->parameter_count &&
            !match(parser, PIKA_TOKEN_COMMA)) {
            return fail_current(parser, PIKA_STATUS_ARGUMENT_COUNT);
        }
    }
    if (!match(parser, PIKA_TOKEN_RIGHT_PAREN)) {
        return fail_current(parser, PIKA_STATUS_ARGUMENT_COUNT);
    }
    expression->kind = PIKA_VALUE_INTEGER;
    expression->binding_kind = PIKA_BINDING_VALUE_INTEGER;
    expression->binding_class = NULL;
    return emit_instruction(
        parser, PIKA_OP_CALL, expression->slot, argument_base,
        declaration->parameter_count, declaration->function_index,
        NULL);
}
#endif

#if PIKA_BINDING_RUNTIME_ENABLE
static int expression_is_numeric(const PikaExpression* expression) {
    return expression->kind == PIKA_VALUE_INTEGER ||
           expression->kind == PIKA_VALUE_BOOLEAN;
}
#endif

static PikaValueKind numeric_result_kind(
    const PikaExpression* left,
    const PikaExpression* right) {
    int left_integer =
        left->kind == PIKA_VALUE_INTEGER ||
        left->kind == PIKA_VALUE_BOOLEAN;
    int right_integer =
        right->kind == PIKA_VALUE_INTEGER ||
        right->kind == PIKA_VALUE_BOOLEAN;
    if (left_integer && right_integer) {
        return PIKA_VALUE_INTEGER;
    }
    if ((left_integer || left->kind == PIKA_VALUE_FLOAT) &&
        (right_integer || right->kind == PIKA_VALUE_FLOAT)) {
        return PIKA_VALUE_FLOAT;
    }
    return PIKA_VALUE_UNKNOWN;
}

#if PIKA_BINDING_RUNTIME_ENABLE
static int binding_argument_is_compatible(
    const PikaExpression* expression,
    PikaBindingValueKind expected) {
    return expected == PIKA_BINDING_VALUE_ANY ||
           expression->kind == PIKA_VALUE_UNKNOWN ||
           expression->binding_kind == expected;
}

static void set_binding_result(
    PikaExpression* expression,
    const PikaBindingCallable* callable,
    const PikaBindingClass* class_descriptor) {
    expression->binding_class = NULL;
    expression->class_index = UINT16_MAX;
    expression->first_element_shape = 0u;
    expression->element_shape_count = 0u;
    if (callable->kind == PIKA_BINDING_CALL_CONSTRUCTOR
#if PIKA_CAPABILITY_BINDING_OBJECT_RESULT_ENABLE
        || callable->result_kind == PIKA_BINDING_VALUE_OBJECT
#endif
    ) {
        expression->kind = PIKA_VALUE_NATIVE_OBJECT;
        expression->binding_kind = PIKA_BINDING_VALUE_OPAQUE;
        expression->binding_class = class_descriptor;
    } else if (callable->result_kind ==
               PIKA_BINDING_VALUE_INTEGER) {
        expression->kind = PIKA_VALUE_INTEGER;
        expression->binding_kind = PIKA_BINDING_VALUE_INTEGER;
    } else if (callable->result_kind ==
               PIKA_BINDING_VALUE_BOOLEAN) {
        expression->kind = PIKA_VALUE_BOOLEAN;
        expression->binding_kind = PIKA_BINDING_VALUE_BOOLEAN;
    } else if (callable->result_kind ==
               PIKA_BINDING_VALUE_FLOAT) {
        expression->kind = PIKA_VALUE_FLOAT;
        expression->binding_kind = PIKA_BINDING_VALUE_FLOAT;
    } else if (callable->result_kind ==
               PIKA_BINDING_VALUE_STRING) {
        expression->kind = PIKA_VALUE_STRING;
        expression->binding_kind = PIKA_BINDING_VALUE_STRING;
    } else if (callable->result_kind ==
               PIKA_BINDING_VALUE_BYTES) {
        expression->kind = PIKA_VALUE_BYTES;
        expression->binding_kind = PIKA_BINDING_VALUE_BYTES;
    } else if (callable->result_kind ==
               PIKA_BINDING_VALUE_BYTEARRAY) {
        expression->kind = PIKA_VALUE_BYTEARRAY;
        expression->binding_kind = PIKA_BINDING_VALUE_BYTEARRAY;
    } else if (callable->result_kind ==
               PIKA_BINDING_VALUE_LIST) {
        expression->kind = PIKA_VALUE_LIST;
        expression->binding_kind = PIKA_BINDING_VALUE_LIST;
    } else if (callable->result_kind ==
               PIKA_BINDING_VALUE_TUPLE) {
        expression->kind = PIKA_VALUE_TUPLE;
        expression->binding_kind = PIKA_BINDING_VALUE_TUPLE;
    } else if (callable->result_kind ==
               PIKA_BINDING_VALUE_DICT) {
        expression->kind = PIKA_VALUE_DICT;
        expression->binding_kind = PIKA_BINDING_VALUE_DICT;
    } else if (callable->result_kind ==
               PIKA_BINDING_VALUE_NONE) {
        expression->kind = PIKA_VALUE_NONE;
        expression->binding_kind = PIKA_BINDING_VALUE_NONE;
    } else {
        expression->kind = PIKA_VALUE_BINDING;
        expression->binding_kind = callable->result_kind;
    }
}

static PikaStatus emit_binding_value(
    PikaParser* parser,
    const PikaBindingValue* value,
    PikaExpression* expression) {
    PikaConstant constant;
    char* bytes = NULL;
    uint32_t constant_count_before;
    PikaStatus status;
    memset(&constant, 0, sizeof(constant));
    if (value->kind == PIKA_BINDING_VALUE_INTEGER) {
        constant.kind = PIKA_CONSTANT_INTEGER;
        constant.as.integer = value->as.integer;
    } else if (value->kind == PIKA_BINDING_VALUE_BOOLEAN) {
        constant.kind = PIKA_CONSTANT_BOOLEAN;
        constant.as.integer = value->as.boolean != 0u ? 1 : 0;
    } else if (value->kind == PIKA_BINDING_VALUE_FLOAT) {
        constant.kind = PIKA_CONSTANT_FLOAT;
        constant.as.floating = value->as.floating;
    } else if (value->kind == PIKA_BINDING_VALUE_NONE) {
        constant.kind = PIKA_CONSTANT_NONE;
    } else if (value->kind == PIKA_BINDING_VALUE_STRING ||
               value->kind == PIKA_BINDING_VALUE_BYTES) {
        if (value->as.buffer.length > UINT16_MAX) {
            return fail_current(
                parser, PIKA_STATUS_FRONTEND_LIMIT);
        }
        if (value->as.buffer.length > 0u) {
            bytes = (char*)malloc(value->as.buffer.length);
            if (bytes == NULL) {
                return fail_current(
                    parser, PIKA_STATUS_FRONTEND_LIMIT);
            }
            memcpy(bytes, value->as.buffer.data,
                   value->as.buffer.length);
        }
        constant.kind =
            value->kind == PIKA_BINDING_VALUE_STRING
                ? PIKA_CONSTANT_STRING
                : PIKA_CONSTANT_BYTES;
        constant.length =
            (uint16_t)value->as.buffer.length;
        constant.as.bytes = bytes;
        status = account_object_result(
            parser, value->as.buffer.length);
        if (status != PIKA_STATUS_OK) {
            free(bytes);
            return status;
        }
    } else {
        return fail_current(
            parser, PIKA_STATUS_BINDING_SCHEMA);
    }
    constant_count_before = parser->module->constant_count;
    status = emit_constant(parser, &constant, expression);
    if (status != PIKA_STATUS_OK &&
        parser->module->constant_count == constant_count_before) {
        free(bytes);
    }
    return status;
}

static int binding_parameter_matches(
    const PikaParser* parser,
    const PikaToken* token,
    const char* name) {
    size_t length = strlen(name);
    return length == token->length &&
           memcmp(&parser->source[token->offset],
                  name, length) == 0;
}

static PikaStatus parse_binding_arguments(
    PikaParser* parser,
    const PikaBindingCallable* callable,
    PikaExpression* arguments) {
    uint8_t provided[PIKA_BINDING_ARGUMENT_LIMIT];
    uint32_t positional_index = 0u;
    uint32_t supplied_count = 0u;
    uint32_t required_count =
        callable->parameter_names == NULL
            ? callable->parameter_count
            : callable->required_parameter_count;
    uint32_t index;
    int saw_keyword = 0;
    PikaStatus status = require(
        parser, PIKA_TOKEN_LEFT_PAREN);
    memset(provided, 0, sizeof(provided));
    if (status != PIKA_STATUS_OK) return status;
    status = reject_comprehension(
        parser, PIKA_TOKEN_RIGHT_PAREN);
    if (status != PIKA_STATUS_OK) return status;
    while (!match(parser, PIKA_TOKEN_RIGHT_PAREN)) {
        uint32_t parameter_index;
        if (supplied_count >= PIKA_BINDING_ARGUMENT_LIMIT) {
            return fail_limit_current(
                parser, PIKA_FRONTEND_BOUND_ARGUMENTS,
                PIKA_BINDING_ARGUMENT_LIMIT,
                supplied_count + 1u);
        }
        if (current_token(parser)->kind == PIKA_TOKEN_NAME &&
            next_token(parser)->kind == PIKA_TOKEN_ASSIGN) {
            const PikaToken* keyword = current_token(parser);
            status = require_capability_at(
                parser, PIKA_CAPABILITY_KEYWORD_CALL, keyword);
            if (status != PIKA_STATUS_OK) return status;
            saw_keyword = 1;
            parser->position += 2u;
            parameter_index = callable->parameter_count;
            if (callable->parameter_names != NULL) {
                for (index = 0u;
                     index < callable->parameter_count;
                     ++index) {
                    if (binding_parameter_matches(
                            parser, keyword,
                            callable->parameter_names[index])) {
                        parameter_index = index;
                        break;
                    }
                }
            }
            if (parameter_index == callable->parameter_count) {
                return fail_at(
                    parser, PIKA_STATUS_UNKNOWN_KEYWORD, keyword);
            }
        } else {
            if (saw_keyword) {
                return fail_current(
                    parser,
                    PIKA_STATUS_POSITIONAL_AFTER_KEYWORD);
            }
            parameter_index = positional_index++;
            if (parameter_index >= callable->parameter_count) {
                return fail_current(
                    parser, PIKA_STATUS_ARGUMENT_COUNT);
            }
        }
        if (provided[parameter_index] != 0u) {
            return fail_current(
                parser, PIKA_STATUS_DUPLICATE_ARGUMENT);
        }
        status = parse_expression(
            parser, &arguments[parameter_index]);
        if (status != PIKA_STATUS_OK) return status;
        provided[parameter_index] = 1u;
        ++supplied_count;
        if (match(parser, PIKA_TOKEN_RIGHT_PAREN)) break;
        status = require(parser, PIKA_TOKEN_COMMA);
        if (status != PIKA_STATUS_OK) return status;
        if (match(parser, PIKA_TOKEN_RIGHT_PAREN)) break;
    }
    for (index = 0u;
         index < callable->parameter_count;
         ++index) {
        if (provided[index] != 0u) continue;
        if (index < required_count) {
            return fail_current(
                parser, PIKA_STATUS_ARGUMENT_COUNT);
        }
        status = require_capability_at(
            parser, PIKA_CAPABILITY_DEFAULT_CALL,
            current_token(parser));
        if (status != PIKA_STATUS_OK) return status;
        status = emit_binding_value(
            parser,
            &callable->defaults[index - required_count],
            &arguments[index]);
        if (status != PIKA_STATUS_OK) return status;
    }
    return PIKA_STATUS_OK;
}

static PikaStatus parse_binding_call(
    PikaParser* parser,
    const PikaBindingModule* module,
    const PikaBindingClass* class_descriptor,
    const PikaBindingCallable* callable,
    uint8_t receiver_slot,
    PikaExpression* expression) {
    PikaBindingCallSite site;
    const PikaBindingClass* result_class = class_descriptor;
    PikaExpression arguments[PIKA_BINDING_ARGUMENT_LIMIT];
    uint8_t argument_slots[PIKA_BINDING_ARGUMENT_LIMIT];
    uint8_t argument_base = 0u;
    uint16_t call_index = 0u;
    uint32_t index;
    PikaStatus status;
    status = require_capability_at(
        parser, PIKA_CAPABILITY_C_BINDING,
        current_token(parser));
    if (status == PIKA_STATUS_OK &&
        callable->kind != PIKA_BINDING_CALL_FUNCTION) {
        status = require_capability_at(
            parser, PIKA_CAPABILITY_C_OBJECT_BINDING,
            current_token(parser));
    }
#if PIKA_CAPABILITY_BINDING_OBJECT_RESULT_ENABLE
    if (status == PIKA_STATUS_OK &&
        callable->result_kind == PIKA_BINDING_VALUE_OBJECT) {
        status = require_capability_at(
            parser, PIKA_CAPABILITY_BINDING_OBJECT_RESULT,
            current_token(parser));
        if (status == PIKA_STATUS_OK &&
            (module == NULL || module->classes == NULL ||
             callable->result_class_index >=
                 module->class_count)) {
            status = fail_current(
                parser, PIKA_STATUS_BINDING_SCHEMA);
        }
        if (status == PIKA_STATUS_OK) {
            result_class =
                &module->classes[
                    callable->result_class_index];
        }
    }
#endif
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    status = parse_binding_arguments(
        parser, callable, arguments);
    if (status == PIKA_STATUS_OK) {
        status = allocate_slot(parser, &expression->slot);
    }
    for (index = 0u;
         status == PIKA_STATUS_OK &&
         index < callable->parameter_count;
         ++index) {
        status = allocate_slot(parser, &argument_slots[index]);
        if (index == 0u && status == PIKA_STATUS_OK) {
            argument_base = argument_slots[index];
        }
    }
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    memset(&site, 0, sizeof(site));
    site.kind = (uint8_t)callable->kind;
    site.receiver_slot = receiver_slot;
    site.argument_count = callable->parameter_count;
    site.flags = PIKA_BINDING_CALL_SITE_PREVALIDATED_ARGUMENTS;
    site.module = module;
    site.class_descriptor = result_class;
    site.callable = callable;
    for (index = 0u; index < callable->parameter_count; ++index) {
        PikaOpcode move_opcode;
        if (!binding_argument_is_compatible(
                &arguments[index],
                callable->parameters[index])) {
            return fail_current(parser, PIKA_STATUS_TYPE_MISMATCH);
        }
        site.argument_kinds[index] =
            (uint8_t)(
                arguments[index].kind == PIKA_VALUE_UNKNOWN
                    ? callable->parameters[index]
                    : arguments[index].binding_kind);
        if (site.argument_kinds[index] == PIKA_BINDING_VALUE_ANY ||
            site.argument_kinds[index] == PIKA_BINDING_VALUE_LIST ||
            site.argument_kinds[index] == PIKA_BINDING_VALUE_TUPLE ||
            site.argument_kinds[index] == PIKA_BINDING_VALUE_DICT) {
            site.flags |=
                PIKA_BINDING_CALL_SITE_CONTAINER_ARGUMENT;
        }
        if (arguments[index].binding_kind ==
            PIKA_BINDING_VALUE_CALLABLE) {
            parser->module->program.allows_binding_callback = 1u;
        }
        move_opcode = expression_is_numeric(&arguments[index])
                          ? PIKA_OP_MOVE
                          : PIKA_OP_BIND_MOVE;
        status = emit_instruction(
            parser, move_opcode, argument_slots[index],
            arguments[index].slot, 0u, 0, NULL);
        if (status != PIKA_STATUS_OK) {
            return status;
        }
    }
    set_binding_result(expression, callable, result_class);
    if (expression->kind == PIKA_VALUE_FLOAT ||
        expression->kind == PIKA_VALUE_NONE) {
        mark_typed(parser);
    } else if (expression->kind == PIKA_VALUE_STRING ||
               expression->kind == PIKA_VALUE_BYTES ||
               expression->kind == PIKA_VALUE_BYTEARRAY) {
        mark_typed(parser);
        status = account_object_result(
            parser, PIKA_BINDING_RESULT_BYTE_LIMIT);
        if (status != PIKA_STATUS_OK) {
            return status;
        }
    } else if (expression->kind == PIKA_VALUE_LIST ||
               expression->kind == PIKA_VALUE_TUPLE) {
        mark_typed(parser);
        status = account_object_result(
            parser,
            PIKA_BINDING_CONTAINER_ELEMENT_LIMIT *
                (uint32_t)sizeof(PikaRuntimeValue));
        if (status != PIKA_STATUS_OK) {
            return status;
        }
    } else if (expression->kind == PIKA_VALUE_DICT) {
        mark_typed(parser);
        status = account_object_result(
            parser,
            PIKA_BINDING_CONTAINER_ELEMENT_LIMIT * 2u *
                (uint32_t)sizeof(PikaRuntimeValue));
        if (status != PIKA_STATUS_OK) {
            return status;
        }
    }
    site.result_kind =
        callable->kind == PIKA_BINDING_CALL_CONSTRUCTOR
            ? (uint8_t)PIKA_BINDING_VALUE_OPAQUE
            : (uint8_t)callable->result_kind;
    status = append_binding_call(parser, &site, &call_index);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    if (callable->kind == PIKA_BINDING_CALL_CONSTRUCTOR
#if PIKA_CAPABILITY_BINDING_OBJECT_RESULT_ENABLE
        || callable->result_kind == PIKA_BINDING_VALUE_OBJECT
#endif
    ) {
        parser->module->program.binding_object_count =
            PIKA_BINDING_RUNTIME_OBJECT_LIMIT;
    }
    return emit_instruction(
        parser, PIKA_OP_BIND_CALL, expression->slot,
        argument_base, callable->parameter_count,
        call_index, NULL);
}

static PikaStatus emit_binding_constant(
    PikaParser* parser,
    const PikaBindingConstant* constant,
    const PikaToken* token,
    PikaExpression* expression) {
    PikaStatus status = require_capability_at(
        parser, PIKA_CAPABILITY_BINDING_CONSTANT, token);
    if (status != PIKA_STATUS_OK) return status;
    return emit_binding_value(parser, &constant->value, expression);
}

static PikaStatus parse_selected_binding_call(
    PikaParser* parser,
    const PikaToken* owner,
    const PikaImportedModule* imported,
    PikaExpression* expression) {
    const PikaBindingCallable* callable = NULL;
    const PikaBindingClass* class_descriptor = NULL;
    char owner_name[PIKA_FRONTEND_IDENTIFIER_BYTE_LIMIT + 1u];
    PikaStatus status = copy_token_text(
        parser, owner, owner_name,
        (uint32_t)sizeof(owner_name));
    if (status != PIKA_STATUS_OK) return status;
    status = pika_binding_find_function(
        imported->module, owner_name, &callable);
    if (status == PIKA_STATUS_OK) {
        ++parser->position;
        return parse_binding_call(
            parser, imported->module, NULL, callable,
            0u, expression);
    }
    status = pika_binding_find_class(
        imported->module, owner_name, &class_descriptor);
    if (status != PIKA_STATUS_OK) {
        return fail_at(
            parser, PIKA_STATUS_MEMBER_NOT_FOUND, owner);
    }
    ++parser->position;
    return parse_binding_call(
        parser, imported->module, class_descriptor,
        class_descriptor->constructor, 0u, expression);
}

static PikaStatus emit_selected_binding_constant(
    PikaParser* parser,
    const PikaToken* owner,
    const PikaImportedModule* imported,
    PikaExpression* expression) {
    const PikaBindingConstant* constant = NULL;
    const PikaBindingCallable* callable = NULL;
    const PikaBindingClass* class_descriptor = NULL;
    char owner_name[PIKA_FRONTEND_IDENTIFIER_BYTE_LIMIT + 1u];
    PikaStatus status = copy_token_text(
        parser, owner, owner_name,
        (uint32_t)sizeof(owner_name));
    if (status != PIKA_STATUS_OK) return status;
    status = pika_binding_find_module_constant(
        imported->module, owner_name, &constant);
    if (status == PIKA_STATUS_OK) {
        ++parser->position;
        return emit_binding_constant(
            parser, constant, owner, expression);
    }
    if (pika_binding_find_function(
            imported->module, owner_name, &callable) ==
            PIKA_STATUS_OK ||
        pika_binding_find_class(
            imported->module, owner_name, &class_descriptor) ==
            PIKA_STATUS_OK) {
        return fail_at(
            parser, PIKA_STATUS_UNSUPPORTED_SYNTAX, owner);
    }
    return fail_at(
        parser, PIKA_STATUS_MEMBER_NOT_FOUND, owner);
}
#endif

#if PIKA_CAPABILITY_OBJECT_ATTRIBUTE_ENABLE || PIKA_CONTAINER_RUNTIME_ENABLE || PIKA_BINDING_RUNTIME_ENABLE
#if PIKA_CAPABILITY_PROTOCOL_TEXT_NETWORK_ENABLE
static PikaValueKind text_method_result_kind(
    uint16_t method_id,
    PikaValueKind receiver_kind) {
    if (method_id == PIKA_TEXT_METHOD_ENCODE) return PIKA_VALUE_BYTES;
    if (method_id == PIKA_TEXT_METHOD_DECODE ||
        method_id == PIKA_TEXT_METHOD_LOWER) {
        return method_id == PIKA_TEXT_METHOD_LOWER
                   ? receiver_kind
                   : PIKA_VALUE_STRING;
    }
    if (method_id == PIKA_TEXT_METHOD_FIND) return PIKA_VALUE_INTEGER;
    if (method_id == PIKA_TEXT_METHOD_SPLIT) return PIKA_VALUE_LIST;
    if (method_id == PIKA_TEXT_METHOD_STARTSWITH ||
        method_id == PIKA_TEXT_METHOD_ENDSWITH) {
        return PIKA_VALUE_BOOLEAN;
    }
    return PIKA_VALUE_UNKNOWN;
}

static PikaStatus parse_text_method_call(
    PikaParser* parser,
    const PikaExpression* receiver,
    const PikaToken* member,
    uint16_t method_id,
    PikaExpression* expression) {
    const PikaTextMethodDescriptor* descriptor =
        pika_text_method_descriptor(method_id);
    PikaExpression arguments[PIKA_FRONTEND_ARGUMENT_LIMIT];
    PikaExpression call_arguments[
        PIKA_FRONTEND_ARGUMENT_LIMIT + 1u];
    PikaValueKind result_kind;
    uint32_t count = 0u;
    uint8_t base = 0u;
    uint16_t call_index = 0u;
    PikaStatus status = require_capability_at(
        parser, PIKA_CAPABILITY_TEXT_NETWORK, member);
    if (status != PIKA_STATUS_OK) return status;
    if (descriptor == NULL) {
        return fail_at(parser, PIKA_STATUS_MEMBER_NOT_FOUND, member);
    }
    status = parse_argument_list(parser, arguments, &count);
    if (status != PIKA_STATUS_OK) return status;
    if (count + 1u < descriptor->minimum_arguments ||
        count + 1u > descriptor->maximum_arguments) {
        return fail_at(parser, PIKA_STATUS_ARGUMENT_COUNT, member);
    }
    call_arguments[0] = *receiver;
    for (uint32_t index = 0u; index < count; ++index) {
        call_arguments[index + 1u] = arguments[index];
    }
    status = allocate_slot(parser, &expression->slot);
    if (status == PIKA_STATUS_OK) {
        status = emit_argument_moves(
            parser, call_arguments, count + 1u, &base);
    }
    result_kind = text_method_result_kind(method_id, receiver->kind);
    if (status == PIKA_STATUS_OK) {
        status = append_builtin_call(
            parser, (PikaBuiltinId)method_id,
            (uint8_t)(count + 1u), result_kind, &call_index);
    }
    if (status != PIKA_STATUS_OK) return status;
    set_expression_kind(expression, result_kind);
    mark_typed(parser);
    if (method_id == PIKA_TEXT_METHOD_SPLIT) {
        parser->module->object_slot_estimate +=
            PIKA_TEXT_METHOD_PART_LIMIT + 1u;
        parser->module->object_byte_estimate +=
            PIKA_TEXT_METHOD_PART_LIMIT *
                (PIKA_FRONTEND_DYNAMIC_TEXT_BYTES +
                 (uint32_t)sizeof(PikaRuntimeValue)) +
            32u;
    } else if (result_kind == PIKA_VALUE_STRING ||
               result_kind == PIKA_VALUE_BYTES) {
        status = account_object_result(
            parser, PIKA_FRONTEND_DYNAMIC_TEXT_BYTES);
        if (status != PIKA_STATUS_OK) return status;
    }
    return emit_instruction(
        parser, PIKA_OP_BUILTIN_CALL, expression->slot, base,
        (uint8_t)(count + 1u), call_index, NULL);
}
#endif

static PikaStatus parse_user_method_call(
    PikaParser* parser,
    const PikaExpression* receiver,
    const PikaFunctionDeclaration* method,
    PikaExpression* expression) {
    PikaExpression arguments[PIKA_FRONTEND_ARGUMENT_LIMIT];
    PikaExpression call_arguments[
        PIKA_FRONTEND_ARGUMENT_LIMIT + 1u];
    uint32_t count;
    uint8_t base = 0u;
    PikaStatus status = parse_declared_arguments(
        parser, method, 1u, arguments, &count);
    if (status != PIKA_STATUS_OK) return status;
    call_arguments[0] = *receiver;
    for (uint32_t index = 0u; index < count; ++index) {
        call_arguments[index + 1u] = arguments[index];
    }
    status = allocate_slot(parser, &expression->slot);
    if (status == PIKA_STATUS_OK) {
        status = emit_argument_moves(
            parser, call_arguments, count + 1u, &base);
    }
    if (status != PIKA_STATUS_OK) return status;
    set_expression_from_return_shape(expression, method);
    mark_typed(parser);
    return emit_instruction(
        parser, PIKA_OP_CALL, expression->slot, base,
        method->parameter_count, method->function_index, NULL);
}

static PikaStatus parse_dynamic_user_method_call(
    PikaParser* parser,
    const PikaExpression* receiver,
    const PikaFunctionDeclaration* const* methods,
    uint32_t method_count,
    PikaExpression* expression) {
    PikaExpression arguments[PIKA_FRONTEND_ARGUMENT_LIMIT];
    PikaExpression call_arguments[PIKA_FRONTEND_ARGUMENT_LIMIT + 1u];
    const PikaFunctionDeclaration* contract = methods[0];
    uint32_t count;
    uint8_t base = 0u;
    uint16_t call_index = 0u;
    PikaStatus status;
    for (uint32_t index = 1u; index < method_count; ++index) {
        if (methods[index]->parameter_count !=
            contract->parameter_count) {
            return fail_current(
                parser, PIKA_STATUS_UNSUPPORTED_SYNTAX);
        }
    }
    status = parse_declared_arguments(
        parser, contract, 1u, arguments, &count);
    if (status != PIKA_STATUS_OK) return status;
    call_arguments[0] = *receiver;
    for (uint32_t index = 0u; index < count; ++index) {
        call_arguments[index + 1u] = arguments[index];
    }
    status = allocate_slot(parser, &expression->slot);
    if (status == PIKA_STATUS_OK) {
        status = emit_argument_moves(
            parser, call_arguments, count + 1u, &base);
    }
    if (status == PIKA_STATUS_OK) {
        status = append_dynamic_method_call(
            parser, methods, method_count,
            contract->parameter_count, &call_index);
    }
    if (status != PIKA_STATUS_OK) return status;
    set_expression_kind(expression, PIKA_VALUE_UNKNOWN);
    mark_typed(parser);
    return emit_instruction(
        parser, PIKA_OP_CALL_DYNAMIC_METHOD,
        expression->slot, base, contract->parameter_count,
        call_index, NULL);
}

static PikaStatus parse_instance_member_access(
    PikaParser* parser,
    const PikaExpression* receiver,
    const PikaToken* member,
    PikaExpression* expression) {
    PikaStatus status = require_capability_at(
        parser, PIKA_CAPABILITY_OBJECT_ATTRIBUTE, member);
    if (status != PIKA_STATUS_OK) return status;
#if PIKA_CAPABILITY_OBJECT_ATTRIBUTE_ENABLE
    if (current_token(parser)->kind == PIKA_TOKEN_LEFT_PAREN) {
        const PikaFunctionDeclaration* method = find_method(
            parser, receiver->class_index, member);
        if (method == NULL) {
            return fail_at(
                parser, PIKA_STATUS_MEMBER_NOT_FOUND, member);
        }
        {
            const PikaFunctionDeclaration* candidates[
                PIKA_FRONTEND_CLASS_LIMIT];
            uint32_t candidate_count = find_inherited_method_candidates(
                parser, receiver->class_index, member, candidates,
                PIKA_FRONTEND_CLASS_LIMIT);
            if (candidate_count > 1u &&
                candidate_count <= PIKA_FRONTEND_CLASS_LIMIT) {
                return parse_dynamic_user_method_call(
                    parser, receiver, candidates, candidate_count,
                    expression);
            }
        }
        return parse_user_method_call(
            parser, receiver, method, expression);
    }
    {
        PikaFieldDeclaration* field;
        uint16_t field_index;
        field = find_field(
            parser, receiver->class_index, member, &field_index);
        if (field == NULL) {
            return fail_at(
                parser, PIKA_STATUS_MEMBER_NOT_FOUND, member);
        }
        status = allocate_slot(parser, &expression->slot);
        if (status != PIKA_STATUS_OK) return status;
        set_expression_from_field(expression, field);
        mark_typed(parser);
        return emit_instruction(
            parser, PIKA_OP_LOAD_ATTRIBUTE, expression->slot,
            receiver->slot, 0u, field_index, NULL);
    }
#else
    (void)receiver;
    (void)expression;
    return fail_at(
        parser, PIKA_STATUS_UNSUPPORTED_CAPABILITY, member);
#endif
}

#if PIKA_BINDING_RUNTIME_ENABLE
static PikaStatus parse_native_member_call(
    PikaParser* parser,
    const PikaExpression* receiver,
    const PikaToken* member,
    const char* member_name,
    PikaExpression* expression) {
    const PikaBindingCallable* callable = NULL;
    PikaStatus status = require_capability_at(
        parser, PIKA_CAPABILITY_OBJECT_ATTRIBUTE, member);
    if (status != PIKA_STATUS_OK) return status;
    status = pika_binding_find_method(
        receiver->binding_class, member_name, &callable);
    if (status != PIKA_STATUS_OK) {
        return fail_at(
            parser, PIKA_STATUS_MEMBER_NOT_FOUND, member);
    }
    for (uint32_t index = 0u;
         index < parser->import_count;
         ++index) {
        const PikaBindingModule* module =
            parser->imports[index].module;
        for (uint32_t class_index = 0u;
             class_index < module->class_count;
             ++class_index) {
            if (&module->classes[class_index] ==
                receiver->binding_class) {
                return parse_binding_call(
                    parser, module, receiver->binding_class,
                    callable, receiver->slot, expression);
            }
        }
    }
    return fail_at(parser, PIKA_STATUS_BINDING_SCHEMA, member);
}

static PikaStatus find_unambiguous_binding_method(
    const PikaParser* parser,
    const char* member_name,
    const PikaBindingModule** result_module,
    const PikaBindingClass** result_class,
    const PikaBindingCallable** result_callable) {
    uint32_t import_index;
    *result_module = NULL;
    *result_class = NULL;
    *result_callable = NULL;
    for (import_index = 0u;
         import_index < parser->import_count;
         ++import_index) {
        const PikaBindingModule* module =
            parser->imports[import_index].module;
        uint32_t class_index;
        for (class_index = 0u;
             class_index < module->class_count;
             ++class_index) {
            const PikaBindingClass* class_descriptor =
                &module->classes[class_index];
            const PikaBindingCallable* callable = NULL;
            if (pika_binding_find_method(
                    class_descriptor, member_name,
                    &callable) != PIKA_STATUS_OK) {
                continue;
            }
            if (*result_callable != NULL &&
                *result_callable != callable) {
                return PIKA_STATUS_UNSUPPORTED_SYNTAX;
            }
            *result_module = module;
            *result_class = class_descriptor;
            *result_callable = callable;
        }
    }
    return *result_callable == NULL
               ? PIKA_STATUS_MEMBER_NOT_FOUND
               : PIKA_STATUS_OK;
}
#endif

#if PIKA_CAPABILITY_PROTOCOL_TEXT_NETWORK_ENABLE
static int token_names_text_method(
    PikaParser* parser,
    const PikaToken* member) {
    char member_name[PIKA_FRONTEND_IDENTIFIER_BYTE_LIMIT + 1u];
    uint16_t method_id;
    if (copy_token_text(
            parser, member, member_name,
            (uint32_t)sizeof(member_name)) != PIKA_STATUS_OK) {
        return 0;
    }
    return pika_text_method_find(member_name, &method_id) ==
           PIKA_STATUS_OK;
}
#endif

static PikaStatus parse_field_member_chain(
    PikaParser* parser,
    PikaExpression* expression) {
    while (current_token(parser)->kind == PIKA_TOKEN_DOT &&
           next_token(parser)->kind == PIKA_TOKEN_NAME) {
        const PikaToken* member = next_token(parser);
        PikaExpression result = {0};
        PikaStatus status;
        if (expression->kind == PIKA_VALUE_TYPE) {
            status = require_capability_at(
                parser, PIKA_CAPABILITY_OBJECT_ATTRIBUTE,
                member);
            if (status != PIKA_STATUS_OK) return status;
            if (!token_matches_text(
                    parser, member, "__name__", 8u)) {
                return fail_at(
                    parser, PIKA_STATUS_MEMBER_NOT_FOUND,
                    member);
            }
            parser->position += 2u;
            status = allocate_slot(parser, &result.slot);
            if (status != PIKA_STATUS_OK) return status;
            set_expression_kind(&result, PIKA_VALUE_STRING);
            mark_typed(parser);
            status = emit_instruction(
                parser, PIKA_OP_LOAD_ATTRIBUTE, result.slot,
                expression->slot, 0u,
                PIKA_TYPE_NAME_ATTRIBUTE, NULL);
        } else if (expression->kind == PIKA_VALUE_INSTANCE &&
            expression->class_index != UINT16_MAX) {
            parser->position += 2u;
            status = parse_instance_member_access(
                parser, expression, member, &result);
        }
#if PIKA_BINDING_RUNTIME_ENABLE
        else if (expression->kind == PIKA_VALUE_NATIVE_OBJECT &&
                 expression->binding_class != NULL) {
            char member_name[
                PIKA_FRONTEND_IDENTIFIER_BYTE_LIMIT + 1u];
            status = copy_token_text(
                parser, member, member_name,
                (uint32_t)sizeof(member_name));
            if (status != PIKA_STATUS_OK) return status;
            parser->position += 2u;
            status = parse_native_member_call(
                parser, expression, member, member_name, &result);
        }
#endif
#if PIKA_CAPABILITY_PROTOCOL_TEXT_NETWORK_ENABLE
        else if ((expression->kind == PIKA_VALUE_STRING ||
                  expression->kind == PIKA_VALUE_BYTES ||
                  (expression->kind == PIKA_VALUE_UNKNOWN &&
                   token_names_text_method(parser, member))) &&
                 token_at(parser, parser->position + 2u)->kind ==
                     PIKA_TOKEN_LEFT_PAREN) {
            uint16_t method_id;
            char member_name[
                PIKA_FRONTEND_IDENTIFIER_BYTE_LIMIT + 1u];
            status = copy_token_text(
                parser, member, member_name, sizeof(member_name));
            if (status == PIKA_STATUS_OK) {
                status = pika_text_method_find(
                    member_name, &method_id);
            }
            if (status == PIKA_STATUS_OK) {
                parser->position += 2u;
                status = parse_text_method_call(
                    parser, expression, member, method_id, &result);
            } else if (expression->kind != PIKA_VALUE_UNKNOWN) {
                return fail_at(
                    parser, PIKA_STATUS_MEMBER_NOT_FOUND, member);
            } else {
                status = PIKA_STATUS_OK;
            }
        }
#endif
        else if ((expression->kind == PIKA_VALUE_LIST ||
                  expression->kind == PIKA_VALUE_DICT ||
                  expression->kind == PIKA_VALUE_BYTES ||
                  expression->kind == PIKA_VALUE_BYTEARRAY ||
                  expression->kind == PIKA_VALUE_NONE ||
                  expression->kind == PIKA_VALUE_UNKNOWN) &&
                 token_at(parser, parser->position + 2u)->kind ==
                     PIKA_TOKEN_LEFT_PAREN) {
#if PIKA_CONTAINER_RUNTIME_ENABLE
            if (token_matches_text(parser, member, "append", 6u) ||
                token_matches_text(parser, member, "pop", 3u)) {
                PikaExpression arguments[2];
                uint32_t count = 0u;
                uint8_t argument_slot = UINT8_MAX;
                int32_t default_slot = -1;
                status = require_capability_at(
                    parser, PIKA_CAPABILITY_VALUE_CONTAINER, member);
                if (status != PIKA_STATUS_OK) return status;
                parser->position += 2u;
                status = parse_argument_list(
                    parser, arguments, &count);
                if (status != PIKA_STATUS_OK) return status;
                if (token_matches_text(parser, member, "append", 6u)) {
                    if (count != 1u) {
                        return fail_at(
                            parser, PIKA_STATUS_ARGUMENT_COUNT, member);
                    }
                    argument_slot = arguments[0].slot;
                    status = allocate_slot(parser, &result.slot);
                    if (status != PIKA_STATUS_OK) return status;
                    set_expression_kind(&result, PIKA_VALUE_NONE);
                    mark_typed(parser);
                    status = emit_instruction(
                        parser, PIKA_OP_APPEND, result.slot,
                        expression->slot, argument_slot, 0, NULL);
                } else {
                    if (count > 2u ||
                        (expression->kind != PIKA_VALUE_DICT &&
                         count > 1u) ||
                        (expression->kind == PIKA_VALUE_DICT &&
                         count == 0u)) {
                        return fail_at(
                            parser, PIKA_STATUS_ARGUMENT_COUNT, member);
                    }
                    if (count > 0u) argument_slot = arguments[0].slot;
                    status = allocate_slot(parser, &result.slot);
                    if (status != PIKA_STATUS_OK) return status;
                    set_expression_kind(&result, PIKA_VALUE_UNKNOWN);
                    mark_typed(parser);
                    status = emit_instruction(
                        parser, PIKA_OP_POP, result.slot,
                        expression->slot, argument_slot, default_slot,
                        NULL);
                }
                if (status != PIKA_STATUS_OK) return status;
                *expression = result;
                continue;
            }
#endif
            if (expression->kind != PIKA_VALUE_NONE &&
                expression->kind != PIKA_VALUE_UNKNOWN &&
                !(expression->kind == PIKA_VALUE_INSTANCE &&
                  expression->class_index == UINT16_MAX)) {
                return fail_at(
                    parser, PIKA_STATUS_MEMBER_NOT_FOUND, member);
            }
#if PIKA_CAPABILITY_OBJECT_ATTRIBUTE_ENABLE
            const PikaFunctionDeclaration* method;
            const PikaFunctionDeclaration* methods[
                PIKA_FRONTEND_CLASS_LIMIT];
            uint32_t method_count;
            int ambiguous = 0;
            method = find_unambiguous_method(
                parser, member, &ambiguous);
            if (ambiguous) {
                method_count = find_method_candidates(
                    parser, member, methods,
                    PIKA_FRONTEND_CLASS_LIMIT);
                if (method_count == 0u ||
                    method_count > PIKA_FRONTEND_CLASS_LIMIT) {
                    return fail_at(
                        parser, PIKA_STATUS_FRONTEND_LIMIT, member);
                }
                parser->position += 2u;
                status = parse_dynamic_user_method_call(
                    parser, expression, methods,
                    method_count, &result);
                method = NULL;
            }
            if (!ambiguous && method != NULL) {
                parser->position += 2u;
                status = parse_user_method_call(
                    parser, expression, method, &result);
            } else if (!ambiguous)
#endif
#if PIKA_BINDING_RUNTIME_ENABLE
            {
                const PikaBindingModule* method_module = NULL;
                const PikaBindingClass* method_class = NULL;
                const PikaBindingCallable* binding_method = NULL;
                char member_name[
                    PIKA_FRONTEND_IDENTIFIER_BYTE_LIMIT + 1u];
                status = copy_token_text(
                    parser, member, member_name,
                    (uint32_t)sizeof(member_name));
                if (status != PIKA_STATUS_OK) return status;
                status = find_unambiguous_binding_method(
                    parser, member_name, &method_module,
                    &method_class, &binding_method);
                if (status == PIKA_STATUS_OK) {
                    parser->position += 2u;
                    status = parse_binding_call(
                        parser, method_module, method_class,
                        binding_method, expression->slot, &result);
                } else if (status ==
                           PIKA_STATUS_UNSUPPORTED_SYNTAX) {
                    return fail_at(parser, status, member);
                } else {
                    break;
                }
            }
#else
            {
                break;
            }
#endif
        } else {
            break;
        }
        if (status != PIKA_STATUS_OK) return status;
        *expression = result;
    }
    return PIKA_STATUS_OK;
}

static PikaStatus parse_binding_access(
    PikaParser* parser,
    PikaExpression* expression) {
    const PikaToken* owner = current_token(parser);
    int has_binding_import = 0;
#if PIKA_CAPABILITY_OBJECT_ATTRIBUTE_ENABLE
    const PikaPythonModule* python_imported =
        find_python_import(parser, owner);
#endif
#if PIKA_BINDING_RUNTIME_ENABLE
    const PikaImportedModule* imported =
        find_import(parser, owner);
#endif
    PikaSymbol resolved_global;
    PikaSymbol* symbol = find_read_symbol(
        parser, owner, &resolved_global);
#if PIKA_BINDING_RUNTIME_ENABLE
    if (symbol != NULL && symbol->initialized != 0u) {
        imported = NULL;
    }
#endif
    const PikaToken* member;
#if PIKA_BINDING_RUNTIME_ENABLE
    const PikaBindingCallable* callable = NULL;
    const PikaBindingClass* class_descriptor = NULL;
#endif
    char member_name[PIKA_FRONTEND_IDENTIFIER_BYTE_LIMIT + 1u];
    PikaStatus status;
    ++parser->position;
    status = require(parser, PIKA_TOKEN_DOT);
    member = current_token(parser);
    if (status == PIKA_STATUS_OK) {
        status = require(parser, PIKA_TOKEN_NAME);
    }
    if (status == PIKA_STATUS_OK) {
        status = copy_token_text(
            parser, member, member_name,
            (uint32_t)sizeof(member_name));
    }
    if (status != PIKA_STATUS_OK) {
        return status;
    }
#if PIKA_BINDING_RUNTIME_ENABLE
    has_binding_import = imported != NULL;
#endif
    if (symbol != NULL && symbol->kind == PIKA_VALUE_INSTANCE &&
        symbol->class_index != UINT16_MAX) {
        PikaExpression receiver;
        int field_access =
            current_token(parser)->kind != PIKA_TOKEN_LEFT_PAREN;
        status = emit_symbol_receiver(
            parser, owner, symbol, &receiver);
        if (status != PIKA_STATUS_OK) return status;
        status = parse_instance_member_access(
            parser, &receiver, member, expression);
        if (status != PIKA_STATUS_OK || !field_access) return status;
        return parse_field_member_chain(parser, expression);
    }
#if PIKA_CAPABILITY_PROTOCOL_TEXT_NETWORK_ENABLE
    if (!has_binding_import && symbol != NULL &&
        (symbol->kind == PIKA_VALUE_STRING ||
         symbol->kind == PIKA_VALUE_BYTES ||
         symbol->kind == PIKA_VALUE_UNKNOWN) &&
        current_token(parser)->kind == PIKA_TOKEN_LEFT_PAREN) {
        PikaExpression receiver;
        uint16_t method_id;
        if (pika_text_method_find(member_name, &method_id) ==
            PIKA_STATUS_OK) {
            status = emit_symbol_receiver(
                parser, owner, symbol, &receiver);
            if (status != PIKA_STATUS_OK) return status;
            return parse_text_method_call(
                parser, &receiver, member, method_id, expression);
        }
    }
#endif
    if (!has_binding_import && symbol != NULL &&
        (symbol->kind == PIKA_VALUE_LIST ||
         symbol->kind == PIKA_VALUE_DICT ||
         symbol->kind == PIKA_VALUE_BYTES ||
         symbol->kind == PIKA_VALUE_BYTEARRAY ||
         symbol->kind == PIKA_VALUE_UNKNOWN) &&
        (strcmp(member_name, "append") == 0 ||
         strcmp(member_name, "pop") == 0)) {
        PikaExpression arguments[2];
        PikaExpression receiver;
        uint32_t count = 0u;
        uint8_t argument_slot = UINT8_MAX;
        int32_t default_slot = -1;
        status = require_capability_at(
            parser, PIKA_CAPABILITY_VALUE_CONTAINER, member);
        if (status != PIKA_STATUS_OK) return status;
        status = emit_symbol_receiver(
            parser, owner, symbol, &receiver);
        if (status != PIKA_STATUS_OK) return status;
        status = parse_argument_list(parser, arguments, &count);
        if (status != PIKA_STATUS_OK) return status;
        if (strcmp(member_name, "append") == 0) {
            if (count != 1u) {
                return fail_at(parser, PIKA_STATUS_ARGUMENT_COUNT, member);
            }
            argument_slot = arguments[0].slot;
            status = allocate_slot(parser, &expression->slot);
            if (status != PIKA_STATUS_OK) return status;
            set_expression_kind(expression, PIKA_VALUE_NONE);
            mark_typed(parser);
            return emit_instruction(
                parser, PIKA_OP_APPEND, expression->slot,
                receiver.slot, argument_slot, 0, NULL);
        }
        if (count > 2u ||
            (symbol->kind != PIKA_VALUE_DICT && count > 1u) ||
            (symbol->kind == PIKA_VALUE_DICT && count == 0u)) {
            return fail_at(parser, PIKA_STATUS_ARGUMENT_COUNT, member);
        }
        if (count > 0u) argument_slot = arguments[0].slot;
        if (count > 1u) default_slot = (int32_t)arguments[1].slot;
        status = allocate_slot(parser, &expression->slot);
        if (status != PIKA_STATUS_OK) return status;
        set_expression_kind(expression, PIKA_VALUE_UNKNOWN);
        mark_typed(parser);
        return emit_instruction(
            parser, PIKA_OP_POP, expression->slot, receiver.slot,
            argument_slot, default_slot, NULL);
    }
#if PIKA_CAPABILITY_OBJECT_ATTRIBUTE_ENABLE
    if (!has_binding_import && symbol != NULL &&
        symbol->kind == PIKA_VALUE_UNKNOWN &&
        current_token(parser)->kind == PIKA_TOKEN_LEFT_PAREN) {
        const PikaFunctionDeclaration* method;
        PikaExpression receiver;
        int ambiguous = 0;
        method = find_unambiguous_method(
            parser, member, &ambiguous);
        if (ambiguous) {
            const PikaFunctionDeclaration* methods[
                PIKA_FRONTEND_CLASS_LIMIT];
            uint32_t method_count = find_method_candidates(
                parser, member, methods,
                PIKA_FRONTEND_CLASS_LIMIT);
            if (method_count == 0u ||
                method_count > PIKA_FRONTEND_CLASS_LIMIT) {
                return fail_at(
                    parser, PIKA_STATUS_FRONTEND_LIMIT, member);
            }
            status = emit_symbol_receiver(
                parser, owner, symbol, &receiver);
            if (status != PIKA_STATUS_OK) return status;
            return parse_dynamic_user_method_call(
                parser, &receiver, methods,
                method_count, expression);
        }
        if (method != NULL) {
            status = emit_symbol_receiver(
                parser, owner, symbol, &receiver);
            if (status != PIKA_STATUS_OK) return status;
            return parse_user_method_call(
                parser, &receiver, method, expression);
        }
    }
#endif
#if PIKA_CAPABILITY_OBJECT_ATTRIBUTE_ENABLE
    if (python_imported != NULL) {
        char stable_name[
            PIKA_FRONTEND_IDENTIFIER_BYTE_LIMIT + 12u];
        uint16_t stable_length = 0u;
        const PikaFunctionDeclaration* function;
        const PikaClassDeclaration* class_declaration;
        PikaSymbol* global;
        status = build_python_member_name(
            parser, python_imported, member,
            stable_name, (uint32_t)sizeof(stable_name),
            &stable_length);
        if (status != PIKA_STATUS_OK) return status;
        function = find_stable_function(
            parser, stable_name, stable_length);
        class_declaration = find_stable_class(
            parser, stable_name, stable_length);
        global = find_stable_module_global(
            parser, stable_name, stable_length);
        if (current_token(parser)->kind ==
            PIKA_TOKEN_LEFT_PAREN) {
            if (function != NULL) {
                return parse_python_module_function_call(
                    parser, member, function, expression);
            }
            if (class_declaration != NULL) {
                return parse_declared_class_call(
                    parser, member, class_declaration,
                    expression);
            }
        } else if (global != NULL) {
            return emit_module_global_value(
                parser, member, global, expression);
        } else if (function != NULL ||
                   class_declaration != NULL) {
            return fail_at(
                parser, PIKA_STATUS_UNSUPPORTED_SYNTAX,
                member);
        }
        return fail_at(
            parser, PIKA_STATUS_MEMBER_NOT_FOUND, member);
    }
#endif
#if PIKA_BINDING_RUNTIME_ENABLE
    if (imported != NULL && imported->selected != 0u) {
        const PikaBindingConstant* constant = NULL;
        char owner_name[
            PIKA_FRONTEND_IDENTIFIER_BYTE_LIMIT + 1u];
        status = copy_token_text(
            parser, owner, owner_name,
            (uint32_t)sizeof(owner_name));
        if (status == PIKA_STATUS_OK) {
            status = pika_binding_find_class(
                imported->module, owner_name,
                &class_descriptor);
        }
        if (status == PIKA_STATUS_OK) {
            status = pika_binding_find_class_constant(
                class_descriptor, member_name, &constant);
        }
        if (status != PIKA_STATUS_OK) {
            return fail_at(
                parser, PIKA_STATUS_MEMBER_NOT_FOUND, member);
        }
        return emit_binding_constant(
            parser, constant, member, expression);
    }
    if (imported != NULL) {
        const PikaBindingConstant* constant = NULL;
        status = pika_binding_find_function(
            imported->module, member_name, &callable);
        if (status == PIKA_STATUS_OK) {
            if (current_token(parser)->kind !=
                PIKA_TOKEN_LEFT_PAREN) {
                return fail_at(
                    parser, PIKA_STATUS_UNSUPPORTED_SYNTAX,
                    member);
            }
            return parse_binding_call(
                parser, imported->module, NULL, callable,
                0u, expression);
        }
        status = pika_binding_find_class(
            imported->module, member_name, &class_descriptor);
        if (status == PIKA_STATUS_OK) {
            if (current_token(parser)->kind ==
                PIKA_TOKEN_LEFT_PAREN) {
                return parse_binding_call(
                    parser, imported->module,
                    class_descriptor,
                    class_descriptor->constructor,
                    0u, expression);
            }
            if (match(parser, PIKA_TOKEN_DOT)) {
                const PikaToken* constant_token =
                    current_token(parser);
                char constant_name[
                    PIKA_FRONTEND_IDENTIFIER_BYTE_LIMIT + 1u];
                status = require(parser, PIKA_TOKEN_NAME);
                if (status == PIKA_STATUS_OK) {
                    status = copy_token_text(
                        parser, constant_token, constant_name,
                        (uint32_t)sizeof(constant_name));
                }
                if (status == PIKA_STATUS_OK) {
                    status = pika_binding_find_class_constant(
                        class_descriptor, constant_name,
                        &constant);
                }
                if (status != PIKA_STATUS_OK) {
                    return fail_at(
                        parser, PIKA_STATUS_MEMBER_NOT_FOUND,
                        constant_token);
                }
                return emit_binding_constant(
                    parser, constant, constant_token,
                    expression);
            }
            return fail_at(
                parser, PIKA_STATUS_UNSUPPORTED_SYNTAX, member);
        }
        status = pika_binding_find_module_constant(
            imported->module, member_name, &constant);
        if (status == PIKA_STATUS_OK) {
            return emit_binding_constant(
                parser, constant, member, expression);
        }
        return fail_at(
            parser, PIKA_STATUS_MEMBER_NOT_FOUND, member);
    }
    if (symbol != NULL && symbol->kind == PIKA_VALUE_UNKNOWN &&
        current_token(parser)->kind == PIKA_TOKEN_LEFT_PAREN) {
        const PikaBindingModule* method_module = NULL;
        const PikaBindingClass* method_class = NULL;
        const PikaBindingCallable* method = NULL;
        PikaExpression receiver;
        status = find_unambiguous_binding_method(
            parser, member_name, &method_module,
            &method_class, &method);
        if (status == PIKA_STATUS_OK) {
            status = emit_symbol_receiver(
                parser, owner, symbol, &receiver);
            if (status != PIKA_STATUS_OK) return status;
            return parse_binding_call(
                parser, method_module, method_class, method,
                receiver.slot, expression);
        }
        if (status == PIKA_STATUS_UNSUPPORTED_SYNTAX) {
            return fail_at(parser, status, member);
        }
    }
    if (symbol == NULL ||
        symbol->kind != PIKA_VALUE_NATIVE_OBJECT ||
        symbol->binding_class == NULL) {
        return fail_at(
            parser, PIKA_STATUS_UNDEFINED_NAME, owner);
    }
    {
        PikaExpression receiver;
        status = emit_symbol_receiver(
            parser, owner, symbol, &receiver);
        if (status != PIKA_STATUS_OK) return status;
        return parse_native_member_call(
            parser, &receiver, member, member_name, expression);
    }
#else
    return fail_at(
        parser, PIKA_STATUS_UNSUPPORTED_CAPABILITY, owner);
#endif
}
#endif

#if 0
static PikaStatus parse_primary(PikaParser* parser,
                                PikaExpression* expression) {
    const PikaToken* token = current_token(parser);
    PikaStatus status;
    if (match(parser, PIKA_TOKEN_INTEGER)) {
        status = require_capability_at(
            parser, PIKA_CAPABILITY_VALUE_INTEGER, token);
        if (status != PIKA_STATUS_OK) {
            return status;
        }
        if (pika_token_integer_value(parser->source, token) >
            INT32_MAX) {
            return fail_at(
                parser, PIKA_STATUS_FRONTEND_LIMIT, token);
        }
        status = allocate_slot(parser, &expression->slot);
        if (status != PIKA_STATUS_OK) {
            return status;
        }
        expression->kind = PIKA_VALUE_INTEGER;
        expression->binding_kind = PIKA_BINDING_VALUE_INTEGER;
        expression->binding_class = NULL;
        return emit_instruction(
            parser, PIKA_OP_CONSTANT, expression->slot, 0u, 0u,
            (int32_t)pika_token_integer_value(
                parser->source, token), NULL);
    }
    if (token->kind == PIKA_TOKEN_NAME &&
        next_token(parser)->kind == PIKA_TOKEN_DOT) {
        return parse_binding_access(parser, expression);
    }
    if (token->kind == PIKA_TOKEN_NAME &&
        next_token(parser)->kind == PIKA_TOKEN_LEFT_PAREN) {
        return parse_call(parser, expression);
    }
    if (match(parser, PIKA_TOKEN_NAME)) {
        status = require_capability_at(
            parser, PIKA_CAPABILITY_NAME_LOCAL, token);
        if (status != PIKA_STATUS_OK) {
            return status;
        }
        PikaSymbol* symbol = find_symbol(parser, token);
        if (symbol == NULL) {
            return fail_at(
                parser, PIKA_STATUS_UNDEFINED_NAME, token);
        }
        expression->slot = symbol->slot;
        expression->kind = symbol->kind;
        expression->binding_kind = symbol->binding_kind;
        expression->binding_class = symbol->binding_class;
        expression->class_index = symbol->class_index;
        return PIKA_STATUS_OK;
    }
    if (match(parser, PIKA_TOKEN_LEFT_PAREN)) {
        status = parse_expression(parser, expression);
        if (status != PIKA_STATUS_OK) {
            return status;
        }
        return require(parser, PIKA_TOKEN_RIGHT_PAREN);
    }
    return fail_current(parser, PIKA_STATUS_SYNTAX_ERROR);
}

static PikaStatus parse_unary(PikaParser* parser,
                              PikaExpression* expression) {
    if (current_token(parser)->kind == PIKA_TOKEN_MINUS) {
        const PikaToken* token = current_token(parser);
        PikaExpression operand;
        uint8_t zero;
        PikaStatus status = require_capability_at(
            parser, PIKA_CAPABILITY_INTEGER_OPERATOR, token);
        if (status != PIKA_STATUS_OK) {
            return status;
        }
        ++parser->position;
        status = parse_unary(parser, &operand);
        if (status != PIKA_STATUS_OK) {
            return status;
        }
        if (operand.kind != PIKA_VALUE_INTEGER) {
            return fail_current(parser, PIKA_STATUS_SYNTAX_ERROR);
        }
        status = allocate_slot(parser, &zero);
        if (status == PIKA_STATUS_OK) {
            status = allocate_slot(parser, &expression->slot);
        }
        if (status == PIKA_STATUS_OK) {
            status = emit_instruction(
                parser, PIKA_OP_CONSTANT, zero, 0u, 0u, 0, NULL);
        }
        if (status != PIKA_STATUS_OK) {
            return status;
        }
        expression->kind = PIKA_VALUE_INTEGER;
        expression->binding_kind = PIKA_BINDING_VALUE_INTEGER;
        expression->binding_class = NULL;
        return emit_instruction(
            parser, PIKA_OP_SUBTRACT, expression->slot, zero,
            operand.slot, 0, NULL);
    }
    return parse_primary(parser, expression);
}

static PikaStatus parse_multiplicative(
    PikaParser* parser,
    PikaExpression* expression) {
    PikaStatus status = parse_unary(parser, expression);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    while (current_token(parser)->kind == PIKA_TOKEN_STAR ||
           current_token(parser)->kind == PIKA_TOKEN_PERCENT) {
        PikaTokenKind operator_kind = current_token(parser)->kind;
        PikaExpression right;
        PikaExpression result;
        status = require_capability_at(
            parser, PIKA_CAPABILITY_INTEGER_OPERATOR,
            current_token(parser));
        if (status != PIKA_STATUS_OK) {
            return status;
        }
        ++parser->position;
        status = parse_unary(parser, &right);
        if (status != PIKA_STATUS_OK) {
            return status;
        }
        if (expression->kind != PIKA_VALUE_INTEGER ||
            right.kind != PIKA_VALUE_INTEGER) {
            return fail_current(parser, PIKA_STATUS_SYNTAX_ERROR);
        }
        status = allocate_slot(parser, &result.slot);
        if (status != PIKA_STATUS_OK) {
            return status;
        }
        result.kind = PIKA_VALUE_INTEGER;
        result.binding_kind = PIKA_BINDING_VALUE_INTEGER;
        result.binding_class = NULL;
        status = emit_instruction(
            parser,
            operator_kind == PIKA_TOKEN_STAR
                ? PIKA_OP_MULTIPLY
                : PIKA_OP_MODULO,
            result.slot, expression->slot, right.slot, 0, NULL);
        if (status != PIKA_STATUS_OK) {
            return status;
        }
        *expression = result;
    }
    return PIKA_STATUS_OK;
}

static PikaStatus parse_additive(PikaParser* parser,
                                 PikaExpression* expression) {
    PikaStatus status =
        parse_multiplicative(parser, expression);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    while (current_token(parser)->kind == PIKA_TOKEN_PLUS ||
           current_token(parser)->kind == PIKA_TOKEN_MINUS) {
        PikaTokenKind operator_kind = current_token(parser)->kind;
        PikaExpression right;
        PikaExpression result;
        status = require_capability_at(
            parser, PIKA_CAPABILITY_INTEGER_OPERATOR,
            current_token(parser));
        if (status != PIKA_STATUS_OK) {
            return status;
        }
        ++parser->position;
        status = parse_multiplicative(parser, &right);
        if (status != PIKA_STATUS_OK) {
            return status;
        }
        if (expression->kind != PIKA_VALUE_INTEGER ||
            right.kind != PIKA_VALUE_INTEGER) {
            return fail_current(parser, PIKA_STATUS_SYNTAX_ERROR);
        }
        status = allocate_slot(parser, &result.slot);
        if (status != PIKA_STATUS_OK) {
            return status;
        }
        result.kind = PIKA_VALUE_INTEGER;
        result.binding_kind = PIKA_BINDING_VALUE_INTEGER;
        result.binding_class = NULL;
        status = emit_instruction(
            parser,
            operator_kind == PIKA_TOKEN_PLUS
                ? PIKA_OP_ADD
                : PIKA_OP_SUBTRACT,
            result.slot, expression->slot, right.slot, 0, NULL);
        if (status != PIKA_STATUS_OK) {
            return status;
        }
        *expression = result;
    }
    return PIKA_STATUS_OK;
}

static PikaStatus parse_expression(PikaParser* parser,
                                   PikaExpression* expression) {
    PikaStatus status = parse_additive(parser, expression);
    PikaTokenKind operator_kind;
    PikaExpression right;
    PikaExpression result;
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    operator_kind = current_token(parser)->kind;
    if (operator_kind != PIKA_TOKEN_LESS &&
        operator_kind != PIKA_TOKEN_EQUAL_EQUAL) {
        return PIKA_STATUS_OK;
    }
    status = require_capability_at(
        parser, PIKA_CAPABILITY_INTEGER_OPERATOR,
        current_token(parser));
    if (status == PIKA_STATUS_OK) {
        status = require_capability_at(
            parser, PIKA_CAPABILITY_VALUE_BOOLEAN,
            current_token(parser));
    }
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    ++parser->position;
    status = parse_additive(parser, &right);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    if (expression->kind != PIKA_VALUE_INTEGER ||
        right.kind != PIKA_VALUE_INTEGER) {
        return fail_current(parser, PIKA_STATUS_SYNTAX_ERROR);
    }
    status = allocate_slot(parser, &result.slot);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    result.kind = PIKA_VALUE_BOOLEAN;
    result.binding_kind = PIKA_BINDING_VALUE_BOOLEAN;
    result.binding_class = NULL;
    status = emit_instruction(
        parser,
        operator_kind == PIKA_TOKEN_LESS
            ? PIKA_OP_LESS_THAN
            : PIKA_OP_EQUAL,
        result.slot, expression->slot, right.slot, 0, NULL);
    if (status == PIKA_STATUS_OK) {
        *expression = result;
    }
    return status;
}
#endif

static PikaStatus parse_expression(PikaParser* parser,
                                   PikaExpression* expression);

static int is_numeric_kind(PikaValueKind kind) {
    return kind == PIKA_VALUE_INTEGER || kind == PIKA_VALUE_BOOLEAN ||
           kind == PIKA_VALUE_FLOAT || kind == PIKA_VALUE_UNKNOWN;
}

static int is_hex_digit(char value) {
    return (value >= '0' && value <= '9') ||
           (value >= 'a' && value <= 'f') ||
           (value >= 'A' && value <= 'F');
}

static unsigned int hex_value(char value) {
    if (value >= '0' && value <= '9') return (unsigned int)(value - '0');
    if (value >= 'a' && value <= 'f') return (unsigned int)(value - 'a' + 10);
    return (unsigned int)(value - 'A' + 10);
}

static PikaStatus emit_constant(PikaParser* parser,
                                const PikaConstant* constant,
                                PikaExpression* expression) {
    uint16_t index;
    PikaStatus status;
    status = allocate_slot(parser, &expression->slot);
    if (status != PIKA_STATUS_OK) return status;
    if (constant->kind == PIKA_CONSTANT_INTEGER &&
        constant->as.integer <= INT32_MAX &&
        constant->as.integer >= INT32_MIN) {
        status = emit_instruction(
            parser, PIKA_OP_CONSTANT, expression->slot, 0u, 0u,
            (int32_t)constant->as.integer, NULL);
    } else {
        status = append_constant(parser, constant, &index);
        if (status == PIKA_STATUS_OK) {
            status = emit_instruction(
                parser, PIKA_OP_LOAD_CONSTANT, expression->slot,
                0u, 0u, (int32_t)index, NULL);
        }
        if (status == PIKA_STATUS_OK) {
            mark_typed(parser);
        }
    }
    if (status != PIKA_STATUS_OK) return status;
    if (constant->kind == PIKA_CONSTANT_INTEGER) {
        set_expression_kind(expression, PIKA_VALUE_INTEGER);
    } else if (constant->kind == PIKA_CONSTANT_FLOAT) {
        set_expression_kind(expression, PIKA_VALUE_FLOAT);
        mark_typed(parser);
    } else if (constant->kind == PIKA_CONSTANT_STRING) {
        set_expression_kind(expression, PIKA_VALUE_STRING);
        mark_typed(parser);
    } else if (constant->kind == PIKA_CONSTANT_BYTES) {
        set_expression_kind(expression, PIKA_VALUE_BYTES);
        mark_typed(parser);
    } else if (constant->kind == PIKA_CONSTANT_BOOLEAN) {
        set_expression_kind(expression, PIKA_VALUE_BOOLEAN);
        mark_typed(parser);
    } else if (constant->kind == PIKA_CONSTANT_TYPE) {
        set_expression_kind(expression, PIKA_VALUE_TYPE);
        mark_typed(parser);
    } else if (constant->kind == PIKA_CONSTANT_MODULE) {
        set_expression_kind(expression, PIKA_VALUE_MODULE);
        mark_typed(parser);
    } else {
        set_expression_kind(expression, PIKA_VALUE_NONE);
        mark_typed(parser);
    }
    return PIKA_STATUS_OK;
}

static PikaStatus emit_semantic_constant(
    PikaParser* parser,
    uint16_t constant_index,
    PikaExpression* expression) {
    const PikaConstant* constant;
    PikaStatus status;
    if (constant_index >= parser->module->constant_count) {
        return fail_current(
            parser, PIKA_STATUS_INTERNAL_ERROR);
    }
    constant = &parser->module->constants[constant_index];
    if (constant->kind == PIKA_CONSTANT_STRING ||
        constant->kind == PIKA_CONSTANT_BYTES ||
        constant->kind == PIKA_CONSTANT_MODULE) {
        status = account_object_result(
            parser, constant->length);
        if (status != PIKA_STATUS_OK) return status;
    }
    status = allocate_slot(parser, &expression->slot);
    if (status == PIKA_STATUS_OK) {
        status = emit_instruction(
            parser, PIKA_OP_LOAD_CONSTANT, expression->slot,
            0u, 0u, (int32_t)constant_index, NULL);
    }
    if (status != PIKA_STATUS_OK) return status;
    if (constant->kind == PIKA_CONSTANT_INTEGER) {
        set_expression_kind(expression, PIKA_VALUE_INTEGER);
    } else if (constant->kind == PIKA_CONSTANT_FLOAT) {
        set_expression_kind(expression, PIKA_VALUE_FLOAT);
    } else if (constant->kind == PIKA_CONSTANT_STRING) {
        set_expression_kind(expression, PIKA_VALUE_STRING);
    } else if (constant->kind == PIKA_CONSTANT_BYTES) {
        set_expression_kind(expression, PIKA_VALUE_BYTES);
    } else if (constant->kind == PIKA_CONSTANT_BOOLEAN) {
        set_expression_kind(expression, PIKA_VALUE_BOOLEAN);
    } else if (constant->kind == PIKA_CONSTANT_TYPE) {
        set_expression_kind(expression, PIKA_VALUE_TYPE);
    } else if (constant->kind == PIKA_CONSTANT_MODULE) {
        set_expression_kind(expression, PIKA_VALUE_MODULE);
    } else {
        set_expression_kind(expression, PIKA_VALUE_NONE);
    }
    mark_typed(parser);
    return PIKA_STATUS_OK;
}

static PikaStatus emit_module_constant(PikaParser* parser,
                                       const char* source,
                                       uint32_t offset,
                                       uint16_t length,
                                       PikaExpression* expression) {
    PikaConstant constant;
    char* name;
    PikaStatus status;
    if (source == NULL) {
        return fail_current(parser, PIKA_STATUS_INTERNAL_ERROR);
    }
    name = (char*)malloc((size_t)length + 1u);
    if (name == NULL) {
        return fail_current(parser, PIKA_STATUS_FRONTEND_LIMIT);
    }
    if (length > 0u) {
        memcpy(name, &source[offset], length);
    }
    name[length] = '\0';
    status = account_object_result(parser, length);
    if (status != PIKA_STATUS_OK) {
        free(name);
        return status;
    }
    memset(&constant, 0, sizeof(constant));
    constant.kind = PIKA_CONSTANT_MODULE;
    constant.length = length;
    constant.as.bytes = name;
    return emit_constant(parser, &constant, expression);
}

static PikaStatus make_string_constant(PikaParser* parser,
                                       const PikaToken* token,
                                       PikaConstant* constant) {
    uint32_t source_end = token->offset + token->length;
    uint32_t prefix = token->kind == PIKA_TOKEN_BYTES ? 2u : 1u;
    uint32_t quote_offset = token->offset +
                            (token->kind == PIKA_TOKEN_BYTES ? 1u : 0u);
    int triple = quote_offset + 2u < source_end &&
                 parser->source[quote_offset] == parser->source[quote_offset + 1u] &&
                 parser->source[quote_offset] == parser->source[quote_offset + 2u];
    uint32_t position;
    uint32_t end;
    if (triple) prefix += 2u;
    position = token->offset + prefix;
    end = source_end - (triple ? 3u : 1u);
    uint32_t length = 0u;
    char* bytes;
    PikaStatus status = require_capability_at(
        parser,
        token->kind == PIKA_TOKEN_BYTES
            ? PIKA_CAPABILITY_VALUE_BYTES
            : PIKA_CAPABILITY_VALUE_STRING,
        token);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    if (token->length < prefix + 1u || source_end <= token->offset ||
        end >= source_end) {
        return fail_at(parser, PIKA_STATUS_LEX_ERROR, token);
    }
    bytes = (char*)malloc((size_t)token->length + 1u);
    if (bytes == NULL) return fail_at(parser, PIKA_STATUS_FRONTEND_LIMIT, token);
    while (position < end) {
        char value = parser->source[position++];
        if (value != '\\') {
            bytes[length++] = value;
            continue;
        }
        if (position >= end) {
            free(bytes);
            return fail_at(parser, PIKA_STATUS_LEX_ERROR, token);
        }
        value = parser->source[position++];
        switch (value) {
            case 'n': bytes[length++] = '\n'; break;
            case 'r': bytes[length++] = '\r'; break;
            case 't': bytes[length++] = '\t'; break;
            case 'f': bytes[length++] = '\f'; break;
            case 'v': bytes[length++] = '\v'; break;
            case '\\': bytes[length++] = '\\'; break;
            case '\'': bytes[length++] = '\''; break;
            case '"': bytes[length++] = '"'; break;
            case '0': bytes[length++] = '\0'; break;
            case 'x':
                if (position + 1u >= end ||
                    !is_hex_digit(parser->source[position]) ||
                    !is_hex_digit(parser->source[position + 1u])) {
                    free(bytes);
                    return fail_at(parser, PIKA_STATUS_LEX_ERROR, token);
                }
                bytes[length++] = (char)((hex_value(parser->source[position]) << 4u) |
                                         hex_value(parser->source[position + 1u]));
                position += 2u;
                break;
            default:
                bytes[length++] = value;
                break;
        }
    }
    memset(constant, 0, sizeof(*constant));
    constant->kind = token->kind == PIKA_TOKEN_BYTES
                         ? PIKA_CONSTANT_BYTES
                         : PIKA_CONSTANT_STRING;
    if (length > UINT16_MAX) {
        free(bytes);
        return fail_at(parser, PIKA_STATUS_FRONTEND_LIMIT, token);
    }
    constant->length = (uint16_t)length;
    bytes[length] = '\0';
    constant->as.bytes = bytes;
    return PIKA_STATUS_OK;
}

static int is_string_constant_token(PikaTokenKind kind) {
    return kind == PIKA_TOKEN_STRING ||
           kind == PIKA_TOKEN_BYTES;
}

static PikaStatus make_string_constant_sequence(
    PikaParser* parser,
    uint32_t start,
    uint32_t end,
    PikaConstant* constant) {
    uint32_t position;
    PikaStatus status;
    const PikaToken* first = token_at(parser, start);
    if (start >= end ||
        !is_string_constant_token(first->kind)) {
        return fail_at(
            parser, PIKA_STATUS_INTERNAL_ERROR, first);
    }
    status = make_string_constant(
        parser, first, constant);
    if (status != PIKA_STATUS_OK) return status;
    for (position = start + 1u; position < end; ++position) {
        const PikaToken* token = token_at(parser, position);
        PikaConstant segment;
        uint32_t observed;
        char* merged;
        if (token->kind != first->kind) {
            free((void*)constant->as.bytes);
            return fail_at(
                parser, PIKA_STATUS_SYNTAX_ERROR, token);
        }
        status = make_string_constant(
            parser, token, &segment);
        if (status != PIKA_STATUS_OK) {
            free((void*)constant->as.bytes);
            return status;
        }
        observed =
            (uint32_t)constant->length + segment.length;
        if (observed > PIKA_FRONTEND_LITERAL_BYTE_LIMIT) {
            free((void*)segment.as.bytes);
            free((void*)constant->as.bytes);
            return fail_limit_at(
                parser, PIKA_FRONTEND_BOUND_LITERAL_BYTES,
                PIKA_FRONTEND_LITERAL_BYTE_LIMIT,
                observed, token);
        }
        merged = (char*)realloc(
            (void*)constant->as.bytes,
            (size_t)observed + 1u);
        if (merged == NULL) {
            free((void*)segment.as.bytes);
            free((void*)constant->as.bytes);
            return fail_at(
                parser, PIKA_STATUS_FRONTEND_LIMIT, token);
        }
        memcpy(
            &merged[constant->length],
            segment.as.bytes, segment.length);
        merged[observed] = '\0';
        free((void*)segment.as.bytes);
        constant->as.bytes = merged;
        constant->length = (uint16_t)observed;
    }
    return PIKA_STATUS_OK;
}

static PikaStatus parse_string_constant(PikaParser* parser,
                                        uint32_t start,
                                        PikaExpression* expression) {
    PikaConstant constant;
    uint32_t end = parser->position;
    PikaStatus status;
    while (is_string_constant_token(
               token_at(parser, end)->kind)) {
        ++end;
    }
    status = make_string_constant_sequence(
        parser, start, end, &constant);
    if (status != PIKA_STATUS_OK) return status;
    parser->position = end;
    status = account_object_result(
        parser, constant.length);
    if (status != PIKA_STATUS_OK) {
        free((void*)constant.as.bytes);
        return status;
    }
    return emit_constant(parser, &constant, expression);
}

static PikaStatus reject_comprehension(PikaParser* parser,
                                       PikaTokenKind close_kind) {
    uint32_t position = parser->position;
    uint32_t depth = 0u;
    while (position < parser->token_count) {
        const PikaToken* token = token_at(parser, position);
        if (token->kind == close_kind && depth == 0u) {
            return PIKA_STATUS_OK;
        }
        if (token->kind == PIKA_TOKEN_LEFT_PAREN ||
            token->kind == PIKA_TOKEN_LEFT_BRACKET ||
            token->kind == PIKA_TOKEN_LEFT_BRACE) {
            ++depth;
        } else if (token->kind == PIKA_TOKEN_RIGHT_PAREN ||
                   token->kind == PIKA_TOKEN_RIGHT_BRACKET ||
                   token->kind == PIKA_TOKEN_RIGHT_BRACE) {
            if (depth == 0u) {
                return PIKA_STATUS_OK;
            }
            --depth;
        } else if (token->kind == PIKA_TOKEN_FOR && depth == 0u) {
            return fail_at(
                parser, PIKA_STATUS_UNSUPPORTED_SYNTAX, token);
        }
        ++position;
    }
    return PIKA_STATUS_OK;
}

static PikaStatus parse_argument_list(PikaParser* parser,
                                      PikaExpression* arguments,
                                      uint32_t* count) {
    PikaStatus status = require(parser, PIKA_TOKEN_LEFT_PAREN);
    *count = 0u;
    if (status != PIKA_STATUS_OK) return status;
    status = reject_comprehension(parser, PIKA_TOKEN_RIGHT_PAREN);
    if (status != PIKA_STATUS_OK) return status;
    if (match(parser, PIKA_TOKEN_RIGHT_PAREN)) return PIKA_STATUS_OK;
    for (;;) {
        if (*count >= PIKA_FRONTEND_ARGUMENT_LIMIT) {
            return fail_limit_current(
                parser, PIKA_FRONTEND_BOUND_ARGUMENTS,
                PIKA_FRONTEND_ARGUMENT_LIMIT, *count + 1u);
        }
        status = parse_expression(parser, &arguments[*count]);
        if (status != PIKA_STATUS_OK) return status;
        ++*count;
        if (match(parser, PIKA_TOKEN_RIGHT_PAREN)) return PIKA_STATUS_OK;
        status = require(parser, PIKA_TOKEN_COMMA);
        if (status != PIKA_STATUS_OK) return status;
        if (match(parser, PIKA_TOKEN_RIGHT_PAREN)) return PIKA_STATUS_OK;
    }
}

static const PikaSemanticParameter* retained_parameter(
    const PikaParser* parser,
    const PikaFunctionDeclaration* declaration,
    uint32_t parameter_index) {
    const PikaSemanticFunction* function;
    const PikaSemanticParameter* parameter;
    if (declaration->parameters != NULL ||
        parameter_index >= declaration->parameter_count ||
        declaration->function_index >=
            parser->module->semantic_function_count) {
        return NULL;
    }
    function = &parser->module
                    ->semantic_functions[declaration->function_index];
    if (function->parameter_count != declaration->parameter_count ||
        (uint32_t)function->first_parameter +
                function->parameter_count >
            parser->module->semantic_parameter_count) {
        return NULL;
    }
    parameter = &parser->module->semantic_parameters[
        function->first_parameter + parameter_index];
    if (!semantic_name_valid(
            parser->module, parameter->name_offset,
            parameter->name_length)) {
        return NULL;
    }
    return parameter;
}

static int declaration_parameter_metadata(
    const PikaParser* parser,
    const PikaFunctionDeclaration* declaration,
    uint32_t parameter_index,
    uint32_t* name_position,
    uint32_t* default_start,
    uint32_t* default_end) {
    const PikaParameterDeclaration* parameter;
    (void)parser;
    if (declaration->parameters == NULL ||
        parameter_index >= declaration->parameter_count) {
        return 0;
    }
    parameter = &declaration->parameters[parameter_index];
    if (parameter->stable_name != NULL) {
        return 0;
    }
    *name_position = parameter->name_position;
    *default_start = parameter->default_start;
    *default_end = parameter->default_end;
    return 1;
}

static int find_declared_parameter(
    const PikaParser* parser,
    const PikaFunctionDeclaration* declaration,
    uint32_t first_parameter,
    const PikaToken* name,
    uint32_t* parameter_index) {
    uint32_t index;
    for (index = first_parameter;
         index < declaration->parameter_count;
         ++index) {
        const PikaParameterDeclaration* parameter;
        const PikaSemanticParameter* retained;
        uint32_t name_position;
        uint32_t default_start;
        uint32_t default_end;
        if (declaration->parameters == NULL) {
            retained = retained_parameter(
                parser, declaration, index);
            if (retained == NULL) return 0;
            if (name->length == retained->name_length &&
                memcmp(
                    &parser->source[name->offset],
                    &parser->module->semantic_names[
                        retained->name_offset],
                    name->length) == 0) {
                *parameter_index = index;
                return 1;
            }
            continue;
        }
        parameter = &declaration->parameters[index];
        if (parameter->stable_name != NULL) {
            if (name->length == parameter->stable_name_length &&
                memcmp(
                    &parser->source[name->offset],
                    parameter->stable_name,
                    name->length) == 0) {
                *parameter_index = index;
                return 1;
            }
            continue;
        }
        if (!declaration_parameter_metadata(
                parser, declaration, index, &name_position,
                &default_start, &default_end)) {
            return 0;
        }
        if (tokens_text_equal(
                parser, name,
                token_at(parser, name_position))) {
            *parameter_index = index;
            return 1;
        }
    }
    return 0;
}

static PikaStatus emit_declared_default(
    PikaParser* parser,
    const PikaFunctionDeclaration* declaration,
    uint32_t parameter_index,
    PikaExpression* expression) {
    uint32_t name_position;
    uint32_t default_start;
    uint32_t default_end;
    uint32_t call_position = parser->position;
    PikaStatus status;
    const PikaParameterDeclaration* parameter;
    const PikaSemanticParameter* retained;
    if (declaration->parameters == NULL) {
        retained = retained_parameter(
            parser, declaration, parameter_index);
        if (retained == NULL) {
            return fail_current(
                parser, PIKA_STATUS_INTERNAL_ERROR);
        }
        if (retained->default_constant == UINT16_MAX) {
            return fail_current(
                parser, PIKA_STATUS_ARGUMENT_COUNT);
        }
        return emit_semantic_constant(
            parser, retained->default_constant, expression);
    }
    parameter = &declaration->parameters[parameter_index];
    if (parameter->stable_name != NULL) {
        if (parameter->default_constant == UINT16_MAX) {
            return fail_current(
                parser, PIKA_STATUS_ARGUMENT_COUNT);
        }
        return emit_semantic_constant(
            parser, parameter->default_constant, expression);
    }
    if (!declaration_parameter_metadata(
            parser, declaration, parameter_index,
            &name_position, &default_start, &default_end) ||
        default_start == UINT32_MAX) {
        return fail_current(parser, PIKA_STATUS_ARGUMENT_COUNT);
    }
    parser->position = default_start;
    status = parse_expression(parser, expression);
    if (status == PIKA_STATUS_OK &&
        parser->position != default_end) {
        status = fail_current(
            parser, PIKA_STATUS_INTERNAL_ERROR);
    }
    parser->position = call_position;
    return status;
}

#if PIKA_CAPABILITY_CALL_VARIADIC_ENABLE
static PikaStatus emit_variadic_tuple(
    PikaParser* parser,
    const PikaExpression* elements,
    uint32_t count,
    PikaExpression* expression) {
    uint8_t base = 0u;
    uint32_t index;
    PikaStatus status = allocate_slot(
        parser, &expression->slot);
    for (index = 0u;
         status == PIKA_STATUS_OK && index < count;
         ++index) {
        uint8_t slot = 0u;
        status = allocate_slot(parser, &slot);
        if (status == PIKA_STATUS_OK && index == 0u) {
            base = slot;
        }
        if (status == PIKA_STATUS_OK) {
            status = emit_instruction(
                parser, PIKA_OP_MOVE, slot,
                elements[index].slot, 0u, 0, NULL);
        }
    }
    if (status != PIKA_STATUS_OK) return status;
    set_expression_kind(expression, PIKA_VALUE_TUPLE);
    status = append_expression_element_shapes(
        parser, elements, count,
        &expression->first_element_shape);
    if (status != PIKA_STATUS_OK) return status;
    expression->element_shape_count = (uint16_t)count;
    mark_typed(parser);
    parser->module->object_slot_estimate += 1u;
    parser->module->object_byte_estimate +=
        count * (uint32_t)sizeof(PikaRuntimeValue) + 32u;
    return emit_instruction(
        parser, PIKA_OP_BUILD_TUPLE,
        expression->slot, base, (uint8_t)count, 0, NULL);
}
#endif

static PikaStatus parse_declared_arguments(
    PikaParser* parser,
    const PikaFunctionDeclaration* declaration,
    uint32_t first_parameter,
    PikaExpression* arguments,
    uint32_t* count) {
    uint8_t provided[PIKA_FRONTEND_PARAMETER_LIMIT];
    uint32_t positional_index = first_parameter;
    uint32_t supplied_count = 0u;
    uint32_t index;
    int saw_keyword = 0;
#if PIKA_CAPABILITY_CALL_VARIADIC_ENABLE
    PikaExpression variadic_elements[
        PIKA_FRONTEND_COLLECTION_ELEMENT_LIMIT];
    uint32_t variadic_count = 0u;
#endif
    PikaStatus status = require(parser, PIKA_TOKEN_LEFT_PAREN);
    memset(provided, 0, sizeof(provided));
    *count = 0u;
    if (status != PIKA_STATUS_OK) return status;
    status = reject_comprehension(
        parser, PIKA_TOKEN_RIGHT_PAREN);
    if (status != PIKA_STATUS_OK) return status;
    while (!match(parser, PIKA_TOKEN_RIGHT_PAREN)) {
        uint32_t parameter_index;
        if (supplied_count >= PIKA_FRONTEND_ARGUMENT_LIMIT) {
            return fail_limit_current(
                parser, PIKA_FRONTEND_BOUND_ARGUMENTS,
                PIKA_FRONTEND_ARGUMENT_LIMIT,
                supplied_count + 1u);
        }
        if (current_token(parser)->kind == PIKA_TOKEN_NAME &&
            next_token(parser)->kind == PIKA_TOKEN_ASSIGN) {
            const PikaToken* keyword = current_token(parser);
            status = require_capability_at(
                parser, PIKA_CAPABILITY_KEYWORD_CALL, keyword);
            if (status != PIKA_STATUS_OK) return status;
            saw_keyword = 1;
            parser->position += 2u;
            if (!find_declared_parameter(
                    parser, declaration, first_parameter,
                    keyword, &parameter_index)) {
                return fail_at(
                    parser, PIKA_STATUS_UNKNOWN_KEYWORD, keyword);
            }
#if PIKA_CAPABILITY_CALL_VARIADIC_ENABLE
            if (parameter_index ==
                declaration->variadic_parameter) {
                return fail_at(
                    parser, PIKA_STATUS_UNKNOWN_KEYWORD, keyword);
            }
#endif
        } else {
            if (saw_keyword) {
                return fail_current(
                    parser,
                    PIKA_STATUS_POSITIONAL_AFTER_KEYWORD);
            }
            parameter_index = positional_index;
#if PIKA_CAPABILITY_CALL_VARIADIC_ENABLE
            if (parameter_index ==
                declaration->variadic_parameter) {
                if (variadic_count >=
                    PIKA_FRONTEND_COLLECTION_ELEMENT_LIMIT) {
                    return fail_limit_current(
                        parser,
                        PIKA_FRONTEND_BOUND_COLLECTION_ELEMENTS,
                        PIKA_FRONTEND_COLLECTION_ELEMENT_LIMIT,
                        variadic_count + 1u);
                }
                status = parse_expression(
                    parser,
                    &variadic_elements[variadic_count]);
                if (status != PIKA_STATUS_OK) return status;
                ++variadic_count;
                ++supplied_count;
                if (match(parser, PIKA_TOKEN_RIGHT_PAREN)) {
                    break;
                }
                status = require(parser, PIKA_TOKEN_COMMA);
                if (status != PIKA_STATUS_OK) return status;
                if (match(parser, PIKA_TOKEN_RIGHT_PAREN)) {
                    break;
                }
                continue;
            }
#endif
            ++positional_index;
            if (parameter_index >=
                declaration->parameter_count) {
                return fail_current(
                    parser, PIKA_STATUS_ARGUMENT_COUNT);
            }
        }
        if (provided[parameter_index] != 0u) {
            return fail_current(
                parser, PIKA_STATUS_DUPLICATE_ARGUMENT);
        }
        status = parse_expression(
            parser,
            &arguments[parameter_index - first_parameter]);
        if (status != PIKA_STATUS_OK) return status;
        provided[parameter_index] = 1u;
        ++supplied_count;
        if (match(parser, PIKA_TOKEN_RIGHT_PAREN)) {
            break;
        }
        status = require(parser, PIKA_TOKEN_COMMA);
        if (status != PIKA_STATUS_OK) return status;
        if (match(parser, PIKA_TOKEN_RIGHT_PAREN)) {
            break;
        }
    }
#if PIKA_CAPABILITY_CALL_VARIADIC_ENABLE
    if (declaration->variadic_parameter != UINT8_MAX &&
        declaration->variadic_parameter >= first_parameter) {
        uint32_t argument_index =
            declaration->variadic_parameter - first_parameter;
        status = emit_variadic_tuple(
            parser, variadic_elements, variadic_count,
            &arguments[argument_index]);
        if (status != PIKA_STATUS_OK) return status;
        provided[declaration->variadic_parameter] = 1u;
    }
#endif
    for (index = first_parameter;
         index < declaration->parameter_count;
         ++index) {
        if (provided[index] != 0u) continue;
        if (index < declaration->required_parameter_count) {
            return fail_current(
                parser, PIKA_STATUS_ARGUMENT_COUNT);
        }
        status = emit_declared_default(
            parser, declaration, index,
            &arguments[index - first_parameter]);
        if (status != PIKA_STATUS_OK) return status;
    }
    *count = declaration->parameter_count - first_parameter;
    return PIKA_STATUS_OK;
}

#if PIKA_BUILTIN_RUNTIME_ENABLE
static PikaValueKind builtin_result_kind(PikaBuiltinId id) {
    switch (id) {
        case PIKA_BUILTIN_BOOL:
        case PIKA_BUILTIN_ALL:
        case PIKA_BUILTIN_ANY:
        case PIKA_BUILTIN_ISINSTANCE:
            return PIKA_VALUE_BOOLEAN;
        case PIKA_BUILTIN_FLOAT:
            return PIKA_VALUE_FLOAT;
        case PIKA_BUILTIN_STR:
        case PIKA_BUILTIN_REPR:
        case PIKA_BUILTIN_ASCII:
        case PIKA_BUILTIN_BIN:
        case PIKA_BUILTIN_HEX:
        case PIKA_BUILTIN_OCT:
        case PIKA_BUILTIN_CHR:
        case PIKA_BUILTIN_FORMAT:
            return PIKA_VALUE_STRING;
        case PIKA_BUILTIN_BYTES:
            return PIKA_VALUE_BYTES;
        case PIKA_BUILTIN_BYTEARRAY:
            return PIKA_VALUE_BYTEARRAY;
        case PIKA_BUILTIN_LIST:
        case PIKA_BUILTIN_SORTED:
        case PIKA_BUILTIN_REVERSED:
        case PIKA_BUILTIN_ENUMERATE:
        case PIKA_BUILTIN_ZIP:
            return PIKA_VALUE_LIST;
        case PIKA_BUILTIN_TUPLE:
            return PIKA_VALUE_TUPLE;
        case PIKA_BUILTIN_DICT:
            return PIKA_VALUE_DICT;
        case PIKA_BUILTIN_SET:
            return PIKA_VALUE_SET;
        case PIKA_BUILTIN_RANGE:
            return PIKA_VALUE_RANGE;
        case PIKA_BUILTIN_SLICE:
            return PIKA_VALUE_UNKNOWN;
        case PIKA_BUILTIN_TYPE:
            return PIKA_VALUE_TYPE;
        default:
            return PIKA_VALUE_UNKNOWN;
    }
}
#endif

static PikaStatus emit_argument_moves(PikaParser* parser,
                                      const PikaExpression* arguments,
                                      uint32_t count,
                                      uint8_t* base) {
    PikaStatus status = PIKA_STATUS_OK;
    uint32_t index;
    *base = 0u;
    for (index = 0u; index < count; ++index) {
        uint8_t slot;
        status = allocate_slot(parser, &slot);
        if (status != PIKA_STATUS_OK) return status;
        if (index == 0u) *base = slot;
        status = emit_instruction(
            parser, PIKA_OP_MOVE, slot, arguments[index].slot,
            0u, 0, NULL);
        if (status != PIKA_STATUS_OK) return status;
    }
    return PIKA_STATUS_OK;
}

static const PikaFunctionDeclaration* find_expression_special_method(
    const PikaParser* parser,
    const PikaExpression* receiver,
    const char* name,
    uint32_t length) {
    if (receiver->kind != PIKA_VALUE_INSTANCE ||
        receiver->class_index == UINT16_MAX) {
        return NULL;
    }
    return find_named_method(
        parser, receiver->class_index, name, length);
}

static PikaStatus emit_special_method_call(
    PikaParser* parser,
    const PikaExpression* receiver,
    const PikaFunctionDeclaration* method,
    const PikaExpression* supplied,
    uint32_t supplied_count,
    PikaExpression* expression) {
    PikaExpression arguments[PIKA_FRONTEND_ARGUMENT_LIMIT + 1u];
    uint32_t provided_count = supplied_count + 1u;
    uint32_t index;
    uint8_t base = 0u;
    PikaStatus status;
    if (provided_count < method->required_parameter_count ||
        provided_count > method->parameter_count ||
        method->parameter_count >
            PIKA_FRONTEND_ARGUMENT_LIMIT + 1u) {
        return fail_current(
            parser, PIKA_STATUS_ARGUMENT_COUNT);
    }
    arguments[0] = *receiver;
    for (index = 0u; index < supplied_count; ++index) {
        arguments[index + 1u] = supplied[index];
    }
    for (index = provided_count;
         index < method->parameter_count;
         ++index) {
        status = emit_declared_default(
            parser, method, index, &arguments[index]);
        if (status != PIKA_STATUS_OK) return status;
    }
    status = allocate_slot(parser, &expression->slot);
    if (status == PIKA_STATUS_OK) {
        status = emit_argument_moves(
            parser, arguments, method->parameter_count, &base);
    }
    if (status != PIKA_STATUS_OK) return status;
    set_expression_from_return_shape(expression, method);
    mark_typed(parser);
    return emit_instruction(
        parser, PIKA_OP_CALL, expression->slot, base,
        method->parameter_count, method->function_index, NULL);
}

#if PIKA_BUILTIN_RUNTIME_ENABLE
static PikaStatus parse_builtin_call(PikaParser* parser,
                                     const PikaToken* name,
                                     PikaExpression* expression) {
    PikaBuiltinId id;
    const PikaBuiltinDescriptor* descriptor;
    PikaExpression arguments[PIKA_FRONTEND_ARGUMENT_LIMIT];
    uint32_t count;
    uint8_t base = 0u;
    uint16_t call_index = 0u;
    PikaStatus status;
    char name_text[PIKA_FRONTEND_IDENTIFIER_BYTE_LIMIT + 1u];
    status = copy_token_text(parser, name, name_text, sizeof(name_text));
    if (status != PIKA_STATUS_OK) return status;
    status = pika_builtin_find(name_text, &id);
    if (status != PIKA_STATUS_OK) {
        return fail_at(parser, status, name);
    }
    descriptor = pika_builtin_descriptor(id);
    if (descriptor == NULL) return fail_at(parser, PIKA_STATUS_UNDEFINED_NAME, name);
    if (id == PIKA_BUILTIN_TYPE) {
        status = require_capability_at(
            parser,
            PIKA_CAPABILITY_EMBEDDED_BUILTIN |
                PIKA_CAPABILITY_TYPE_BUILTIN,
            name);
        if (status != PIKA_STATUS_OK) return status;
    }
    ++parser->position;
    status = parse_argument_list(parser, arguments, &count);
    if (status != PIKA_STATUS_OK) return status;
    if (count < descriptor->minimum_arguments ||
        count > descriptor->maximum_arguments) {
        return fail_at(parser, PIKA_STATUS_ARGUMENT_COUNT, name);
    }
    if (id == PIKA_BUILTIN_STR && count == 1u) {
        const PikaFunctionDeclaration* method =
            find_expression_special_method(
                parser, &arguments[0], "__str__", 7u);
        if (method != NULL) {
            status = emit_special_method_call(
                parser, &arguments[0], method, NULL, 0u,
                expression);
            if (status != PIKA_STATUS_OK) return status;
            if (expression->kind != PIKA_VALUE_STRING &&
                expression->kind != PIKA_VALUE_UNKNOWN) {
                return fail_at(
                    parser, PIKA_STATUS_TYPE_MISMATCH, name);
            }
            set_expression_kind(
                expression, PIKA_VALUE_STRING);
            return PIKA_STATUS_OK;
        }
    }
    status = allocate_slot(parser, &expression->slot);
    if (status == PIKA_STATUS_OK) {
        status = emit_argument_moves(parser, arguments, count, &base);
    }
    if (status == PIKA_STATUS_OK) {
        status = append_builtin_call(
            parser, id, (uint8_t)count, builtin_result_kind(id), &call_index);
    }
    if (status != PIKA_STATUS_OK) return status;
    set_expression_kind(expression, builtin_result_kind(id));
    mark_typed(parser);
    parser->module->object_slot_estimate += 2u;
    parser->module->object_byte_estimate += 128u;
    return emit_instruction(
        parser, PIKA_OP_BUILTIN_CALL, expression->slot, base,
        (uint8_t)count, call_index, NULL);
}
#endif

static PikaStatus parse_declared_class_call(
    PikaParser* parser,
    const PikaToken* name,
    const PikaClassDeclaration* declaration,
    PikaExpression* expression) {
    const PikaFunctionDeclaration* constructor = find_named_method(
        parser, declaration->class_index, "__init__", 8u);
    PikaExpression arguments[PIKA_FRONTEND_ARGUMENT_LIMIT];
    PikaExpression call_arguments[PIKA_FRONTEND_ARGUMENT_LIMIT + 1u];
    uint32_t count;
    uint8_t base = 0u;
    PikaStatus status = require_capability_at(
        parser, PIKA_CAPABILITY_OBJECT_CLASS, name);
    if (status != PIKA_STATUS_OK) return status;
    if (constructor == NULL) {
        status = parse_argument_list(
            parser, arguments, &count);
        if (status == PIKA_STATUS_OK && count != 0u) {
            status = fail_at(
                parser, PIKA_STATUS_ARGUMENT_COUNT, name);
        }
    } else {
        status = parse_declared_arguments(
            parser, constructor, 1u, arguments, &count);
    }
    if (status != PIKA_STATUS_OK) return status;
    status = allocate_slot(parser, &expression->slot);
    if (status != PIKA_STATUS_OK) return status;
    set_expression_kind(expression, PIKA_VALUE_INSTANCE);
    expression->class_index = declaration->class_index;
    mark_typed(parser);
    status = account_object_result(
        parser,
        declaration->field_count *
            (uint32_t)sizeof(PikaRuntimeValue));
    if (status != PIKA_STATUS_OK) return status;
    status = emit_instruction(
        parser, PIKA_OP_NEW_INSTANCE, expression->slot,
        0u, 0u, declaration->class_index, NULL);
    if (status != PIKA_STATUS_OK || constructor == NULL) return status;

    call_arguments[0] = *expression;
    for (uint32_t index = 0u; index < count; ++index) {
        call_arguments[index + 1u] = arguments[index];
    }
    status = emit_argument_moves(
        parser, call_arguments, count + 1u, &base);
    if (status == PIKA_STATUS_OK) {
        uint8_t ignored = 0u;
        status = allocate_slot(parser, &ignored);
        if (status == PIKA_STATUS_OK) {
            status = emit_instruction(
                parser, PIKA_OP_CALL, ignored, base,
                constructor->parameter_count,
                constructor->function_index, NULL);
        }
    }
    return status;
}

static PikaStatus parse_class_call(
    PikaParser* parser,
    const PikaToken* name,
    const PikaClassDeclaration* declaration,
    PikaExpression* expression) {
    ++parser->position;
    return parse_declared_class_call(
        parser, name, declaration, expression);
}

static PikaStatus parse_declared_function_call(
    PikaParser* parser,
    const PikaFunctionDeclaration* declaration,
    PikaExpression* expression) {
    PikaExpression arguments[PIKA_FRONTEND_ARGUMENT_LIMIT];
    uint32_t count;
    uint8_t base = 0u;
    PikaStatus status = parse_declared_arguments(
        parser, declaration, 0u, arguments, &count);
    if (status != PIKA_STATUS_OK) return status;
    status = allocate_slot(parser, &expression->slot);
    if (status == PIKA_STATUS_OK) {
        status = emit_argument_moves(
            parser, arguments, count, &base);
    }
    if (status != PIKA_STATUS_OK) return status;
    set_expression_from_return_shape(
        expression, declaration);
    if (!is_numeric_kind(expression->kind)) mark_typed(parser);
    return emit_instruction(
        parser, PIKA_OP_CALL, expression->slot, base,
        (uint8_t)count, declaration->function_index, NULL);
}

#if PIKA_CAPABILITY_OBJECT_ATTRIBUTE_ENABLE
static PikaStatus parse_python_module_function_call(
    PikaParser* parser,
    const PikaToken* member,
    const PikaFunctionDeclaration* declaration,
    PikaExpression* expression) {
    PikaExpression arguments[PIKA_FRONTEND_ARGUMENT_LIMIT];
    uint32_t hidden_count = 0u;
    uint32_t user_count = 0u;
    uint8_t base = 0u;
    PikaStatus status;
    while (hidden_count < declaration->parameter_count) {
        const PikaSemanticParameter* parameter =
            retained_parameter(parser, declaration, hidden_count);
        PikaSymbol* global;
        if (parameter == NULL) break;
        global = find_stable_module_global(
            parser,
            &parser->module->semantic_names[parameter->name_offset],
            parameter->name_length);
        if (global == NULL) break;
        status = emit_module_global_value(
            parser, member, global, &arguments[hidden_count]);
        if (status != PIKA_STATUS_OK) return status;
        ++hidden_count;
    }
    status = parse_declared_arguments(
        parser, declaration, hidden_count,
        &arguments[hidden_count], &user_count);
    if (status != PIKA_STATUS_OK) return status;
    status = allocate_slot(parser, &expression->slot);
    if (status == PIKA_STATUS_OK) {
        status = emit_argument_moves(
            parser, arguments, hidden_count + user_count, &base);
    }
    if (status != PIKA_STATUS_OK) return status;
    set_expression_from_return_shape(expression, declaration);
    if (!is_numeric_kind(expression->kind)) mark_typed(parser);
    return emit_instruction(
        parser, PIKA_OP_CALL, expression->slot, base,
        declaration->parameter_count,
        declaration->function_index, NULL);
}
#endif

static PikaStatus parse_user_call(PikaParser* parser,
                                  const PikaToken* name,
                                  PikaExpression* expression) {
    const PikaClassDeclaration* class_declaration = find_class(parser, name);
    const PikaFunctionDeclaration* declaration = find_function(parser, name);
    if (class_declaration != NULL) {
        return parse_class_call(
            parser, name, class_declaration, expression);
    }
    if (declaration == NULL) return fail_at(parser, PIKA_STATUS_UNDEFINED_NAME, name);
    ++parser->position;
    return parse_declared_function_call(
        parser, declaration, expression);
}

static PikaStatus build_sequence(PikaParser* parser,
                                 PikaTokenKind close_kind,
                                 PikaValueKind kind,
                                 PikaExpression* expression) {
    PikaExpression elements[PIKA_FRONTEND_COLLECTION_ELEMENT_LIMIT];
    uint32_t count = 0u;
    uint8_t base;
    PikaStatus status = require_capability_at(
        parser,
        kind == PIKA_VALUE_LIST ? PIKA_CAPABILITY_CONTAINER_LIST
                                : PIKA_CAPABILITY_CONTAINER_TUPLE,
        token_at(parser, parser->position - 1u));
    if (status == PIKA_STATUS_OK) {
        status = reject_comprehension(parser, close_kind);
    }
    if (status != PIKA_STATUS_OK) return status;
    if (match(parser, close_kind)) {
        status = allocate_slot(parser, &expression->slot);
        if (status != PIKA_STATUS_OK) return status;
        set_expression_kind(expression, kind);
        mark_typed(parser);
        return emit_instruction(
            parser, kind == PIKA_VALUE_LIST ? PIKA_OP_BUILD_LIST
                                             : PIKA_OP_BUILD_TUPLE,
            expression->slot, 0u, 0u, 0, NULL);
    }
    for (;;) {
        if (count >= PIKA_FRONTEND_COLLECTION_ELEMENT_LIMIT) {
            return fail_limit_current(
                parser, PIKA_FRONTEND_BOUND_COLLECTION_ELEMENTS,
                PIKA_FRONTEND_COLLECTION_ELEMENT_LIMIT, count + 1u);
        }
        status = parse_expression(parser, &elements[count]);
        if (status != PIKA_STATUS_OK) return status;
        ++count;
        if (match(parser, close_kind)) break;
        if (current_token(parser)->kind == PIKA_TOKEN_FOR) {
            return fail_current(
                parser, PIKA_STATUS_UNSUPPORTED_SYNTAX);
        }
        status = require(parser, PIKA_TOKEN_COMMA);
        if (status != PIKA_STATUS_OK) return status;
        if (match(parser, close_kind)) break;
    }
    status = allocate_slot(parser, &expression->slot);
    if (status == PIKA_STATUS_OK) {
        status = emit_argument_moves(parser, elements, count, &base);
    }
    if (status != PIKA_STATUS_OK) return status;
    set_expression_kind(expression, kind);
    status = append_expression_element_shapes(
        parser, elements, count,
        &expression->first_element_shape);
    if (status != PIKA_STATUS_OK) return status;
    expression->element_shape_count = (uint16_t)count;
    mark_typed(parser);
    parser->module->object_slot_estimate += 1u;
    parser->module->object_byte_estimate +=
        count * (uint32_t)sizeof(PikaRuntimeValue) + 32u;
    return emit_instruction(
        parser, kind == PIKA_VALUE_LIST ? PIKA_OP_BUILD_LIST
                                         : PIKA_OP_BUILD_TUPLE,
        expression->slot, base, (uint8_t)count, 0, NULL);
}

#if PIKA_CAPABILITY_CONTAINER_DICT_ENABLE
static PikaStatus build_dictionary(PikaParser* parser,
                                   PikaExpression* expression) {
    PikaExpression elements[
        PIKA_FRONTEND_COLLECTION_ELEMENT_LIMIT * 2u];
    uint32_t count = 0u;
    uint32_t capacity = 0u;
    uint8_t base = 0u;
    PikaStatus status = require_capability_at(
        parser, PIKA_CAPABILITY_CONTAINER_DICT,
        token_at(parser, parser->position - 1u));
    if (status == PIKA_STATUS_OK) {
        status = reject_comprehension(
            parser, PIKA_TOKEN_RIGHT_BRACE);
    }
    if (status != PIKA_STATUS_OK) return status;
    while (!match(parser, PIKA_TOKEN_RIGHT_BRACE)) {
        uint32_t element = count * 2u;
        if (count >= PIKA_FRONTEND_COLLECTION_ELEMENT_LIMIT) {
            return fail_limit_current(
                parser, PIKA_FRONTEND_BOUND_COLLECTION_ELEMENTS,
                PIKA_FRONTEND_COLLECTION_ELEMENT_LIMIT,
                count + 1u);
        }
        status = parse_expression(parser, &elements[element]);
        if (status == PIKA_STATUS_OK) {
            status = require(parser, PIKA_TOKEN_COLON);
        }
        if (status == PIKA_STATUS_OK) {
            status = parse_expression(
                parser, &elements[element + 1u]);
        }
        if (status != PIKA_STATUS_OK) return status;
        ++count;
        if (match(parser, PIKA_TOKEN_RIGHT_BRACE)) break;
        status = require(parser, PIKA_TOKEN_COMMA);
        if (status != PIKA_STATUS_OK) return status;
        if (match(parser, PIKA_TOKEN_RIGHT_BRACE)) break;
    }
    status = allocate_slot(parser, &expression->slot);
    if (status == PIKA_STATUS_OK && count != 0u) {
        status = emit_argument_moves(
            parser, elements, count * 2u, &base);
    }
    if (status != PIKA_STATUS_OK) return status;
    set_expression_kind(expression, PIKA_VALUE_DICT);
    mark_typed(parser);
    parser->module->object_slot_estimate += 1u;
    if (count != 0u) {
        capacity = 1u;
        while (capacity < count) capacity *= 2u;
        parser->module->object_byte_estimate +=
            capacity * 4u *
                (uint32_t)sizeof(PikaRuntimeValue) +
            32u;
    }
    return emit_instruction(
        parser, PIKA_OP_BUILD_DICT, expression->slot,
        base, (uint8_t)count, 0, NULL);
}
#endif

static PikaStatus emit_module_global_value(
    PikaParser* parser,
    const PikaToken* name,
    const PikaSymbol* global,
    PikaExpression* expression) {
    PikaStatus status = require_capability_at(
        parser, PIKA_CAPABILITY_NAME_GLOBAL, name);
    if (status != PIKA_STATUS_OK) return status;
    status = allocate_slot(parser, &expression->slot);
    if (status != PIKA_STATUS_OK) return status;
    expression->kind = global->kind;
    expression->binding_kind = global->binding_kind;
    expression->binding_class = global->binding_class;
    expression->class_index = global->class_index;
    expression->first_element_shape =
        global->first_element_shape;
    expression->element_shape_count =
        global->element_shape_count;
    mark_typed(parser);
    return emit_instruction(
        parser, PIKA_OP_LOAD_GLOBAL,
        expression->slot, 0u, 0u,
        global->global_slot, NULL);
}

static PikaStatus parse_primary_new(PikaParser* parser,
                                    PikaExpression* expression) {
    const PikaToken* token = current_token(parser);
#if PIKA_CAPABILITY_MODULE_IMPORT_ENABLE
    const PikaPythonModule* python_imported =
        token->kind == PIKA_TOKEN_NAME
            ? find_python_import(parser, token)
            : NULL;
#endif
#if PIKA_BINDING_RUNTIME_ENABLE
    const PikaImportedModule* imported =
        token->kind == PIKA_TOKEN_NAME
            ? find_import(parser, token)
            : NULL;
#endif
    PikaStatus status;
    PikaConstant constant;
    if (token->kind == PIKA_TOKEN_LAMBDA) {
        const PikaFunctionDeclaration* lambda = NULL;
        uint32_t declaration_index;
        for (declaration_index = 0u;
             declaration_index < parser->declaration_count;
             ++declaration_index) {
            if (parser->declarations[declaration_index].is_lambda != 0u &&
                parser->declarations[declaration_index].header_position ==
                    parser->position) {
                lambda = &parser->declarations[declaration_index];
                break;
            }
        }
        if (lambda == NULL) {
            return fail_current(parser, PIKA_STATUS_UNSUPPORTED_SYNTAX);
        }
        status = require_capability_at(
            parser, PIKA_CAPABILITY_CALL_REFERENCE, token);
        if (status != PIKA_STATUS_OK) return status;
        parser->position = lambda->after_body;
        status = allocate_slot(parser, &expression->slot);
        if (status != PIKA_STATUS_OK) return status;
        set_expression_kind(expression, PIKA_VALUE_CALLABLE);
        expression->binding_kind = PIKA_BINDING_VALUE_CALLABLE;
        mark_typed(parser);
        return emit_instruction(parser, PIKA_OP_LOAD_CALLABLE,
                                expression->slot, 0u, 0u,
                                lambda->function_index, NULL);
    }
    if (match(parser, PIKA_TOKEN_INTEGER)) {
        if (integer_literal_is_embedded(parser, token)) {
            status = require_capability_at(
                parser, PIKA_CAPABILITY_EMBEDDED_INTEGER, token);
            if (status != PIKA_STATUS_OK) return status;
        }
        memset(&constant, 0, sizeof(constant));
        constant.kind = PIKA_CONSTANT_INTEGER;
        constant.as.integer = pika_token_integer_value(
            parser->source, token);
        return emit_constant(parser, &constant, expression);
    }
    if (match(parser, PIKA_TOKEN_FLOAT)) {
        status = require_capability_at(
            parser, PIKA_CAPABILITY_VALUE_FLOAT, token);
        if (status != PIKA_STATUS_OK) return status;
        memset(&constant, 0, sizeof(constant));
        constant.kind = PIKA_CONSTANT_FLOAT;
        status = pika_token_float_value(
            parser->source, token, &constant.as.floating);
        if (status != PIKA_STATUS_OK) {
            return fail_at(parser, status, token);
        }
        return emit_constant(parser, &constant, expression);
    }
    if (match(parser, PIKA_TOKEN_STRING) ||
        match(parser, PIKA_TOKEN_BYTES)) {
        return parse_string_constant(
            parser, parser->position - 1u, expression);
    }
    if (match(parser, PIKA_TOKEN_TRUE) || match(parser, PIKA_TOKEN_FALSE)) {
        status = require_capability_at(
            parser, PIKA_CAPABILITY_VALUE_BOOLEAN, token);
        if (status != PIKA_STATUS_OK) return status;
        memset(&constant, 0, sizeof(constant));
        constant.kind = PIKA_CONSTANT_BOOLEAN;
        constant.as.integer = token->kind == PIKA_TOKEN_TRUE ? 1 : 0;
        return emit_constant(parser, &constant, expression);
    }
    if (match(parser, PIKA_TOKEN_NONE)) {
        status = require_capability_at(
            parser, PIKA_CAPABILITY_VALUE_NONE, token);
        if (status != PIKA_STATUS_OK) return status;
        memset(&constant, 0, sizeof(constant));
        constant.kind = PIKA_CONSTANT_NONE;
        return emit_constant(parser, &constant, expression);
    }
#if PIKA_CAPABILITY_MODULE_IMPORT_ENABLE
    if (token->kind == PIKA_TOKEN_NAME &&
        next_token(parser)->kind != PIKA_TOKEN_DOT &&
        next_token(parser)->kind != PIKA_TOKEN_LEFT_PAREN &&
        python_imported != NULL) {
        ++parser->position;
        return emit_module_constant(
            parser, parser->module->semantic_names,
            python_imported->name_offset,
            python_imported->name_length, expression);
    }
#endif
#if PIKA_BINDING_RUNTIME_ENABLE
    if (token->kind == PIKA_TOKEN_NAME &&
        next_token(parser)->kind != PIKA_TOKEN_DOT &&
        next_token(parser)->kind != PIKA_TOKEN_LEFT_PAREN &&
        imported != NULL && imported->selected == 0u) {
        const char* source = imported->name_source != NULL
                                 ? imported->name_source
                                 : parser->source;
        ++parser->position;
        return emit_module_constant(
            parser, source, imported->offset,
            (uint16_t)imported->length, expression);
    }
#endif
#if PIKA_CAPABILITY_OBJECT_ATTRIBUTE_ENABLE || PIKA_CONTAINER_RUNTIME_ENABLE || PIKA_BINDING_RUNTIME_ENABLE
    if (token->kind == PIKA_TOKEN_NAME &&
        next_token(parser)->kind == PIKA_TOKEN_DOT) {
        return parse_binding_access(parser, expression);
    }
#endif
#if PIKA_BINDING_RUNTIME_ENABLE
    if (token->kind == PIKA_TOKEN_NAME &&
        imported != NULL && imported->selected != 0u &&
        next_token(parser)->kind == PIKA_TOKEN_LEFT_PAREN) {
        return parse_selected_binding_call(
            parser, token, imported, expression);
    }
#endif
    if ((token->kind == PIKA_TOKEN_NAME ||
         token->kind == PIKA_TOKEN_PRINT ||
         token->kind == PIKA_TOKEN_RANGE) &&
        next_token(parser)->kind == PIKA_TOKEN_LEFT_PAREN) {
        char name_text[PIKA_FRONTEND_IDENTIFIER_BYTE_LIMIT + 1u];
        status = copy_token_text(parser, token, name_text, sizeof(name_text));
        if (status != PIKA_STATUS_OK) return status;
#if PIKA_BUILTIN_RUNTIME_ENABLE
        {
            PikaBuiltinId id;
            PikaStatus lookup = pika_builtin_find(name_text, &id);
            if (lookup == PIKA_STATUS_OK || lookup == PIKA_STATUS_UNSUPPORTED_SYNTAX) {
                return parse_builtin_call(parser, token, expression);
            }
        }
#endif
        return parse_user_call(parser, token, expression);
    }
#if PIKA_BINDING_RUNTIME_ENABLE
    if (token->kind == PIKA_TOKEN_NAME &&
        imported != NULL && imported->selected != 0u) {
        return emit_selected_binding_constant(
            parser, token, imported, expression);
    }
#endif
    if ((token->kind == PIKA_TOKEN_NAME ||
         token->kind == PIKA_TOKEN_RANGE) &&
        next_token(parser)->kind != PIKA_TOKEN_LEFT_PAREN &&
        find_symbol(parser, token) == NULL &&
        find_module_global(parser, token) == NULL &&
        find_function(parser, token) == NULL &&
        find_class(parser, token) == NULL) {
        static const char* const type_names[] = {
            "bool", "bytearray", "bytes", "dict", "float",
            "int", "list", "range", "set", "str", "tuple",
            "type",
        };
        uint32_t type_index;
        for (type_index = 0u;
             type_index < sizeof(type_names) / sizeof(type_names[0]);
             ++type_index) {
            const char* name = type_names[type_index];
            if (token_matches_text(
                    parser, token, name, (uint32_t)strlen(name))) {
                status = require_capability_at(
                    parser, PIKA_CAPABILITY_TYPE_BUILTIN, token);
                if (status != PIKA_STATUS_OK) return status;
                status = account_object_result(
                    parser, (uint32_t)strlen(name));
                if (status != PIKA_STATUS_OK) return status;
                memset(&constant, 0, sizeof(constant));
                constant.kind = PIKA_CONSTANT_TYPE;
                constant.length = (uint16_t)strlen(name);
                constant.as.bytes = name;
                ++parser->position;
                return emit_constant(
                    parser, &constant, expression);
            }
        }
    }
    if (match(parser, PIKA_TOKEN_NAME)) {
        PikaSymbol* symbol = find_symbol(parser, token);
        PikaSymbol* global =
            symbol == NULL
                ? find_module_global(parser, token)
                : NULL;
        const PikaFunctionDeclaration* function =
            find_function(parser, token);
        status = require_capability_at(
            parser,
            global == NULL
                ? PIKA_CAPABILITY_NAME_LOCAL
                : PIKA_CAPABILITY_NAME_GLOBAL,
            token);
        if (status != PIKA_STATUS_OK) return status;
        if (global != NULL) {
            return emit_module_global_value(
                parser, token, global, expression);
        }
        if (symbol == NULL) {
            if (function != NULL) {
                status = require_capability_at(
                    parser,
                    PIKA_CAPABILITY_CALL_REFERENCE,
                    token);
                if (status != PIKA_STATUS_OK) {
                    return status;
                }
                status = allocate_slot(
                    parser, &expression->slot);
                if (status != PIKA_STATUS_OK) {
                    return status;
                }
                set_expression_kind(
                    expression, PIKA_VALUE_CALLABLE);
                expression->binding_kind =
                    PIKA_BINDING_VALUE_CALLABLE;
                mark_typed(parser);
                return emit_instruction(
                    parser, PIKA_OP_LOAD_CALLABLE,
                    expression->slot, 0u, 0u,
                    function->function_index, NULL);
            }
            if (find_class(parser, token) != NULL) {
                return fail_at(
                    parser, PIKA_STATUS_UNSUPPORTED_SYNTAX, token);
            }
            return fail_at(
                parser, PIKA_STATUS_UNDEFINED_NAME, token);
        }
        if (symbol->is_global != 0u) {
            status = allocate_slot(
                parser, &expression->slot);
            if (status != PIKA_STATUS_OK) {
                return status;
            }
            expression->kind = symbol->kind;
            expression->binding_kind =
                symbol->binding_kind;
            expression->binding_class =
                symbol->binding_class;
            expression->class_index =
                symbol->class_index;
            expression->first_element_shape =
                symbol->first_element_shape;
            expression->element_shape_count =
                symbol->element_shape_count;
            mark_typed(parser);
            return emit_instruction(
                parser, PIKA_OP_LOAD_GLOBAL,
                expression->slot, 0u, 0u,
                symbol->global_slot, NULL);
        }
        expression->slot = symbol->slot;
        expression->kind = symbol->kind;
        expression->binding_kind = symbol->binding_kind;
        expression->binding_class = symbol->binding_class;
        expression->class_index = symbol->class_index;
        expression->first_element_shape =
            symbol->first_element_shape;
        expression->element_shape_count =
            symbol->element_shape_count;
        return PIKA_STATUS_OK;
    }
    if (match(parser, PIKA_TOKEN_LEFT_PAREN)) {
        status = reject_comprehension(
            parser, PIKA_TOKEN_RIGHT_PAREN);
        if (status != PIKA_STATUS_OK) return status;
        if (match(parser, PIKA_TOKEN_RIGHT_PAREN)) {
            status = require_capability_at(
                parser, PIKA_CAPABILITY_CONTAINER_TUPLE, token);
            if (status != PIKA_STATUS_OK) return status;
            status = allocate_slot(parser, &expression->slot);
            if (status != PIKA_STATUS_OK) return status;
            set_expression_kind(expression, PIKA_VALUE_TUPLE);
            mark_typed(parser);
            return emit_instruction(
                parser, PIKA_OP_BUILD_TUPLE, expression->slot,
                0u, 0u, 0, NULL);
        }
        PikaExpression
            elements[PIKA_FRONTEND_COLLECTION_ELEMENT_LIMIT];
        uint32_t count = 0u;
        status = parse_expression(parser, &elements[count++]);
        if (status != PIKA_STATUS_OK) return status;
        if (current_token(parser)->kind == PIKA_TOKEN_FOR) {
            return fail_current(
                parser, PIKA_STATUS_UNSUPPORTED_SYNTAX);
        }
        if (!match(parser, PIKA_TOKEN_COMMA)) {
            *expression = elements[0];
            return require(parser, PIKA_TOKEN_RIGHT_PAREN);
        }
        status = require_capability_at(
            parser, PIKA_CAPABILITY_CONTAINER_TUPLE, token);
        if (status != PIKA_STATUS_OK) return status;
        while (!match(parser, PIKA_TOKEN_RIGHT_PAREN)) {
            if (count >= PIKA_FRONTEND_COLLECTION_ELEMENT_LIMIT) {
                return fail_limit_current(
                    parser, PIKA_FRONTEND_BOUND_COLLECTION_ELEMENTS,
                    PIKA_FRONTEND_COLLECTION_ELEMENT_LIMIT,
                    count + 1u);
            }
            status = parse_expression(parser, &elements[count++]);
            if (status != PIKA_STATUS_OK) return status;
            if (!match(parser, PIKA_TOKEN_COMMA) &&
                current_token(parser)->kind != PIKA_TOKEN_RIGHT_PAREN) {
                return fail_current(parser, PIKA_STATUS_SYNTAX_ERROR);
            }
        }
        status = allocate_slot(parser, &expression->slot);
        if (status == PIKA_STATUS_OK) {
            uint8_t base;
            status = emit_argument_moves(parser, elements, count, &base);
            if (status == PIKA_STATUS_OK) {
                set_expression_kind(expression, PIKA_VALUE_TUPLE);
                status = append_expression_element_shapes(
                    parser, elements, count,
                    &expression->first_element_shape);
                if (status != PIKA_STATUS_OK) return status;
                expression->element_shape_count =
                    (uint16_t)count;
                mark_typed(parser);
                parser->module->object_slot_estimate += 1u;
                parser->module->object_byte_estimate +=
                    count * (uint32_t)sizeof(PikaRuntimeValue) + 32u;
                status = emit_instruction(
                    parser, PIKA_OP_BUILD_TUPLE, expression->slot,
                    base, (uint8_t)count, 0, NULL);
            }
        }
        return status;
    }
    if (match(parser, PIKA_TOKEN_LEFT_BRACKET)) {
        return build_sequence(parser, PIKA_TOKEN_RIGHT_BRACKET,
                              PIKA_VALUE_LIST, expression);
    }
#if PIKA_CAPABILITY_CONTAINER_DICT_ENABLE
    if (match(parser, PIKA_TOKEN_LEFT_BRACE)) {
        return build_dictionary(parser, expression);
    }
#endif
    return fail_current(parser, PIKA_STATUS_SYNTAX_ERROR);
}

static PikaStatus parse_postfix_new(PikaParser* parser,
                                    PikaExpression* expression) {
    PikaStatus status = parse_primary_new(parser, expression);
    if (status != PIKA_STATUS_OK) return status;
    for (;;) {
        if (current_token(parser)->kind == PIKA_TOKEN_LEFT_PAREN) {
            PikaExpression callable = *expression;
            PikaExpression arguments[PIKA_FRONTEND_ARGUMENT_LIMIT];
            uint32_t count = 0u;
            uint8_t base = 0u;
            status = require_capability_at(
                parser,
                PIKA_CAPABILITY_POSITIONAL_CALL |
                    PIKA_CAPABILITY_CALL_REFERENCE,
                current_token(parser));
            if (status == PIKA_STATUS_OK) {
                status = parse_argument_list(
                    parser, arguments, &count);
            }
            if (status == PIKA_STATUS_OK) {
                status = allocate_slot(parser, &expression->slot);
            }
            if (status == PIKA_STATUS_OK) {
                status = emit_argument_moves(
                    parser, arguments, count, &base);
            }
            if (status != PIKA_STATUS_OK) return status;
            set_expression_kind(expression, PIKA_VALUE_UNKNOWN);
            mark_typed(parser);
            status = emit_instruction(
                parser, PIKA_OP_CALL_CALLABLE,
                expression->slot, callable.slot, base,
                (int32_t)count, NULL);
            if (status != PIKA_STATUS_OK) return status;
            continue;
        }
#if PIKA_OBJECT_BYTECODE_RUNTIME_ENABLE || PIKA_BINDING_RUNTIME_ENABLE
        if (current_token(parser)->kind == PIKA_TOKEN_DOT) {
            uint32_t member_position = parser->position;
            status = parse_field_member_chain(
                parser, expression);
            if (status != PIKA_STATUS_OK) return status;
            if (parser->position != member_position) {
                continue;
            }
        }
#endif
#if PIKA_CAPABILITY_PROTOCOL_TEXT_NETWORK_ENABLE
        if (current_token(parser)->kind == PIKA_TOKEN_DOT) {
            const PikaToken* member = next_token(parser);
            char member_name[
                PIKA_FRONTEND_IDENTIFIER_BYTE_LIMIT + 1u];
            uint16_t method_id;
            PikaExpression result;
            int text_receiver =
                expression->kind == PIKA_VALUE_STRING ||
                expression->kind == PIKA_VALUE_BYTES;
            if (member->kind != PIKA_TOKEN_NAME) break;
            status = copy_token_text(
                parser, member, member_name,
                (uint32_t)sizeof(member_name));
            if (status != PIKA_STATUS_OK) return status;
            status = pika_text_method_find(
                member_name, &method_id);
            if (status != PIKA_STATUS_OK) {
                if (!text_receiver) break;
                return fail_at(
                    parser, PIKA_STATUS_MEMBER_NOT_FOUND, member);
            }
            if (token_at(parser, parser->position + 2u)->kind !=
                    PIKA_TOKEN_LEFT_PAREN ||
                (!text_receiver &&
                 expression->kind != PIKA_VALUE_UNKNOWN)) {
                if (!text_receiver) break;
                return fail_at(
                    parser, PIKA_STATUS_MEMBER_NOT_FOUND, member);
            }
            parser->position += 2u;
            status = parse_text_method_call(
                parser, expression, member, method_id, &result);
            if (status != PIKA_STATUS_OK) return status;
            *expression = result;
            continue;
        }
#endif
        if (!match(parser, PIKA_TOKEN_LEFT_BRACKET)) break;
        PikaValueKind container_kind = expression->kind;
        PikaIndexSite site;
        uint16_t site_index = 0u;
        PikaExpression index_expression;
        PikaExpression result;
        memset(&site, 0, sizeof(site));
        site.container_slot = expression->slot;
        site.index_slot = UINT8_MAX;
        site.start_slot = UINT8_MAX;
        site.stop_slot = UINT8_MAX;
        site.step_slot = UINT8_MAX;
        if (current_token(parser)->kind == PIKA_TOKEN_COLON) {
            ++parser->position;
        } else {
            status = parse_expression(parser, &index_expression);
            if (status != PIKA_STATUS_OK) return status;
            if (match(parser, PIKA_TOKEN_COLON)) {
                site.flags |= 1u;
                site.start_slot = index_expression.slot;
            } else {
                const PikaFunctionDeclaration* method;
                site.index_slot = index_expression.slot;
                status = require(parser, PIKA_TOKEN_RIGHT_BRACKET);
                if (status != PIKA_STATUS_OK) return status;
                method = find_expression_special_method(
                    parser, expression, "__getitem__", 11u);
                if (method != NULL) {
                    status = require_capability_at(
                        parser,
                        PIKA_CAPABILITY_SUBSCRIPT_OPERATOR,
                        token_at(parser, parser->position - 1u));
                    if (status != PIKA_STATUS_OK) return status;
                    status = emit_special_method_call(
                        parser, expression, method,
                        &index_expression, 1u, &result);
                    if (status != PIKA_STATUS_OK) return status;
                    *expression = result;
                    continue;
                }
                status = allocate_slot(parser, &result.slot);
                if (status != PIKA_STATUS_OK) return status;
                set_expression_kind(
                    &result,
                    container_kind == PIKA_VALUE_STRING
                        ? PIKA_VALUE_STRING
                        : PIKA_VALUE_UNKNOWN);
                mark_typed(parser);
                if (container_kind == PIKA_VALUE_STRING) {
                    status = account_object_result(
                        parser, PIKA_FRONTEND_DYNAMIC_TEXT_BYTES);
                    if (status != PIKA_STATUS_OK) return status;
                }
                status = append_index_site(parser, &site, &site_index);
                if (status != PIKA_STATUS_OK) return status;
                status = require_capability_at(
                    parser, PIKA_CAPABILITY_SUBSCRIPT_OPERATOR,
                    token_at(parser, parser->position - 1u));
                if (status != PIKA_STATUS_OK) return status;
                status = emit_instruction(
                    parser, PIKA_OP_SUBSCRIPT, result.slot, 0u, 0u,
                    site_index, NULL);
                if (status != PIKA_STATUS_OK) return status;
                *expression = result;
                continue;
            }
        }
        if (current_token(parser)->kind != PIKA_TOKEN_COLON &&
            current_token(parser)->kind != PIKA_TOKEN_RIGHT_BRACKET) {
            status = parse_expression(parser, &index_expression);
            if (status != PIKA_STATUS_OK) return status;
            site.flags |= 2u;
            site.stop_slot = index_expression.slot;
        }
        if (match(parser, PIKA_TOKEN_COLON)) {
            if (current_token(parser)->kind != PIKA_TOKEN_RIGHT_BRACKET) {
                status = parse_expression(parser, &index_expression);
                if (status != PIKA_STATUS_OK) return status;
                site.flags |= 4u;
                site.step_slot = index_expression.slot;
            }
        }
        status = require(parser, PIKA_TOKEN_RIGHT_BRACKET);
        if (status != PIKA_STATUS_OK) return status;
        status = allocate_slot(parser, &result.slot);
        if (status != PIKA_STATUS_OK) return status;
        set_expression_kind(&result, expression->kind);
        mark_typed(parser);
        if (is_text_value_kind(container_kind)) {
            status = account_object_result(
                parser, PIKA_FRONTEND_DYNAMIC_TEXT_BYTES);
            if (status != PIKA_STATUS_OK) return status;
        }
        status = append_index_site(parser, &site, &site_index);
        if (status != PIKA_STATUS_OK) return status;
        status = require_capability_at(
            parser, PIKA_CAPABILITY_SLICE_OPERATOR,
            token_at(parser, parser->position - 1u));
        if (status != PIKA_STATUS_OK) return status;
        status = emit_instruction(
            parser, PIKA_OP_SLICE, result.slot, 0u, 0u,
            site_index, NULL);
        if (status != PIKA_STATUS_OK) return status;
        *expression = result;
    }
    return PIKA_STATUS_OK;
}

static PikaStatus emit_unary(PikaParser* parser,
                             PikaOpcode opcode,
                             PikaExpression operand,
                             PikaExpression* result) {
    PikaStatus status = allocate_slot(parser, &result->slot);
    if (status != PIKA_STATUS_OK) return status;
    set_expression_kind(result, operand.kind);
    if (opcode == PIKA_OP_LOGICAL_NOT) set_expression_kind(result, PIKA_VALUE_BOOLEAN);
    mark_typed(parser);
    return emit_instruction(
        parser, opcode, result->slot, operand.slot, 0u, 0, NULL);
}

static PikaStatus require_numeric_operator(PikaParser* parser,
                                            const PikaToken* token) {
    return require_capability_at(
        parser,
        PIKA_CAPABILITY_INTEGER_OPERATOR |
            PIKA_CAPABILITY_NUMERIC_OPERATOR,
        token);
}

static PikaStatus require_comparison_operator(
    PikaParser* parser,
    const PikaToken* token) {
    return require_capability_at(
        parser,
        PIKA_CAPABILITY_INTEGER_OPERATOR |
            PIKA_CAPABILITY_COMPARISON_OPERATOR,
        token);
}

static PikaStatus require_bitwise_operator(PikaParser* parser,
                                            const PikaToken* token) {
    return require_capability_at(
        parser, PIKA_CAPABILITY_BITWISE_OPERATOR, token);
}

static PikaStatus parse_power_new(PikaParser* parser,
                                  PikaExpression* expression);

static PikaStatus parse_unary_new(PikaParser* parser,
                                  PikaExpression* expression) {
    PikaTokenKind kind = current_token(parser)->kind;
    PikaExpression operand;
    PikaStatus status;
    if (kind == PIKA_TOKEN_PLUS || kind == PIKA_TOKEN_MINUS ||
        kind == PIKA_TOKEN_TILDE || kind == PIKA_TOKEN_NOT) {
        if (kind == PIKA_TOKEN_NOT) {
            status = require_capability_at(
                parser, PIKA_CAPABILITY_SHORT_CIRCUIT_LOGIC,
                current_token(parser));
        } else if (kind == PIKA_TOKEN_TILDE) {
            status = require_bitwise_operator(
                parser, current_token(parser));
        } else {
            status = require_numeric_operator(
                parser, current_token(parser));
        }
        if (status != PIKA_STATUS_OK) return status;
        ++parser->position;
        status = enter_expression_depth(parser);
        if (status != PIKA_STATUS_OK) return status;
        status = parse_unary_new(parser, &operand);
        leave_expression_depth(parser);
        if (status != PIKA_STATUS_OK) return status;
        return emit_unary(
            parser, kind == PIKA_TOKEN_PLUS
                       ? PIKA_OP_UNARY_PLUS
                       : kind == PIKA_TOKEN_MINUS
                             ? PIKA_OP_UNARY_MINUS
                             : kind == PIKA_TOKEN_TILDE
                                   ? PIKA_OP_BITWISE_NOT
                                   : PIKA_OP_LOGICAL_NOT,
            operand, expression);
    }
    return parse_power_new(parser, expression);
}

static PikaStatus parse_power_new(PikaParser* parser,
                                  PikaExpression* expression) {
    PikaStatus status = parse_postfix_new(parser, expression);
    if (status != PIKA_STATUS_OK) return status;
    if (match(parser, PIKA_TOKEN_DOUBLE_STAR)) {
        PikaExpression right;
        PikaExpression result;
        status = require_numeric_operator(
            parser, token_at(parser, parser->position - 1u));
        if (status != PIKA_STATUS_OK) return status;
        status = enter_expression_depth(parser);
        if (status != PIKA_STATUS_OK) return status;
        status = parse_unary_new(parser, &right);
        leave_expression_depth(parser);
        if (status != PIKA_STATUS_OK) return status;
        status = allocate_slot(parser, &result.slot);
        if (status != PIKA_STATUS_OK) return status;
        set_expression_kind(&result, PIKA_VALUE_UNKNOWN);
        mark_typed(parser);
        status = emit_instruction(
            parser, PIKA_OP_POWER, result.slot, expression->slot,
            right.slot, 0, NULL);
        if (status == PIKA_STATUS_OK) *expression = result;
    }
    return status;
}
static PikaStatus parse_multiplicative_new(PikaParser* parser,
                                           PikaExpression* expression) {
    PikaStatus status = parse_unary_new(parser, expression);
    while (status == PIKA_STATUS_OK &&
           (current_token(parser)->kind == PIKA_TOKEN_STAR ||
            current_token(parser)->kind == PIKA_TOKEN_SLASH ||
            current_token(parser)->kind == PIKA_TOKEN_DOUBLE_SLASH ||
            current_token(parser)->kind == PIKA_TOKEN_PERCENT ||
            current_token(parser)->kind == PIKA_TOKEN_AT)) {
        PikaTokenKind kind = current_token(parser)->kind;
        PikaExpression right;
        PikaExpression result;
        status = kind == PIKA_TOKEN_AT
                     ? require_capability_at(
                           parser, PIKA_CAPABILITY_MATRIX_OPERATOR,
                           current_token(parser))
                     : require_numeric_operator(
                           parser, current_token(parser));
        if (status != PIKA_STATUS_OK) break;
        ++parser->position;
        status = parse_unary_new(parser, &right);
        if (status != PIKA_STATUS_OK) break;
        status = allocate_slot(parser, &result.slot);
        if (status != PIKA_STATUS_OK) break;
        if (kind == PIKA_TOKEN_SLASH) {
            set_expression_kind(&result, PIKA_VALUE_FLOAT);
        } else if (kind == PIKA_TOKEN_STAR &&
                   is_text_value_kind(expression->kind) &&
                   right.kind == PIKA_VALUE_INTEGER) {
            set_expression_kind(&result, expression->kind);
        } else if (kind == PIKA_TOKEN_STAR &&
                   expression->kind == PIKA_VALUE_INTEGER &&
                   is_text_value_kind(right.kind)) {
            set_expression_kind(&result, right.kind);
        } else {
            set_expression_kind(&result, PIKA_VALUE_UNKNOWN);
        }
        mark_typed(parser);
        if (is_text_value_kind(result.kind)) {
            status = account_object_result(
                parser, PIKA_FRONTEND_DYNAMIC_TEXT_BYTES);
            if (status != PIKA_STATUS_OK) break;
        }
        status = emit_instruction(
            parser, kind == PIKA_TOKEN_STAR ? PIKA_OP_MULTIPLY
                     : kind == PIKA_TOKEN_SLASH ? PIKA_OP_DIVIDE
                     : kind == PIKA_TOKEN_DOUBLE_SLASH ? PIKA_OP_FLOOR_DIVIDE
                     : kind == PIKA_TOKEN_PERCENT ? PIKA_OP_MODULO
                                                  : PIKA_OP_MATRIX_MULTIPLY,
            result.slot, expression->slot, right.slot, 0, NULL);
        if (status == PIKA_STATUS_OK) *expression = result;
    }
    return status;
}
static PikaStatus parse_additive_new(PikaParser* parser,
                                     PikaExpression* expression) {
    PikaStatus status = parse_multiplicative_new(parser, expression);
    while (status == PIKA_STATUS_OK &&
           (current_token(parser)->kind == PIKA_TOKEN_PLUS ||
            current_token(parser)->kind == PIKA_TOKEN_MINUS)) {
        PikaTokenKind kind = current_token(parser)->kind;
        PikaExpression right;
        PikaExpression result;
        int sequence_concat;
        status = require_numeric_operator(
            parser, current_token(parser));
        if (status != PIKA_STATUS_OK) break;
        ++parser->position;
        status = parse_multiplicative_new(parser, &right);
        if (status != PIKA_STATUS_OK) break;
        status = allocate_slot(parser, &result.slot);
        if (status != PIKA_STATUS_OK) break;
        sequence_concat =
            kind == PIKA_TOKEN_PLUS &&
            ((expression->kind == right.kind &&
              (expression->kind == PIKA_VALUE_LIST ||
               expression->kind == PIKA_VALUE_TUPLE)) ||
             (expression->kind == PIKA_VALUE_UNKNOWN &&
              (right.kind == PIKA_VALUE_UNKNOWN ||
               right.kind == PIKA_VALUE_LIST ||
               right.kind == PIKA_VALUE_TUPLE)) ||
             (right.kind == PIKA_VALUE_UNKNOWN &&
              (expression->kind == PIKA_VALUE_LIST ||
               expression->kind == PIKA_VALUE_TUPLE)));
        if (kind == PIKA_TOKEN_PLUS && expression->kind == right.kind &&
            is_text_value_kind(expression->kind)) {
            set_expression_kind(&result, expression->kind);
        } else if (sequence_concat &&
                   expression->kind != PIKA_VALUE_UNKNOWN) {
            set_expression_kind(&result, expression->kind);
        } else if (sequence_concat &&
                   right.kind != PIKA_VALUE_UNKNOWN) {
            set_expression_kind(&result, right.kind);
        } else {
            set_expression_kind(
                &result,
                numeric_result_kind(expression, &right));
        }
        if (!(expression->kind == PIKA_VALUE_INTEGER &&
              right.kind == PIKA_VALUE_INTEGER)) mark_typed(parser);
        if (is_text_value_kind(result.kind)) {
            status = account_object_result(
                parser, PIKA_FRONTEND_DYNAMIC_TEXT_BYTES);
            if (status != PIKA_STATUS_OK) break;
        } else if (sequence_concat) {
            status = account_object_result(
                parser,
                PIKA_FRONTEND_DYNAMIC_SEQUENCE_ELEMENT_RESERVE *
                    (uint32_t)sizeof(PikaRuntimeValue));
            if (status != PIKA_STATUS_OK) break;
        }
        status = emit_instruction(
            parser, kind == PIKA_TOKEN_PLUS ? PIKA_OP_ADD : PIKA_OP_SUBTRACT,
            result.slot, expression->slot, right.slot, 0, NULL);
        if (status == PIKA_STATUS_OK) *expression = result;
    }
    return status;
}
static PikaStatus parse_shift_new(PikaParser* parser,
                                  PikaExpression* expression) {
    PikaStatus status = parse_additive_new(parser, expression);
    while (status == PIKA_STATUS_OK &&
           (current_token(parser)->kind == PIKA_TOKEN_LEFT_SHIFT ||
            current_token(parser)->kind == PIKA_TOKEN_RIGHT_SHIFT)) {
        PikaTokenKind kind = current_token(parser)->kind;
        PikaExpression right;
        PikaExpression result;
        status = require_bitwise_operator(
            parser, current_token(parser));
        if (status != PIKA_STATUS_OK) break;
        ++parser->position;
        status = parse_additive_new(parser, &right);
        if (status != PIKA_STATUS_OK) break;
        status = allocate_slot(parser, &result.slot);
        if (status != PIKA_STATUS_OK) break;
        set_expression_kind(&result, PIKA_VALUE_INTEGER);
        mark_typed(parser);
        status = emit_instruction(
            parser, kind == PIKA_TOKEN_LEFT_SHIFT ? PIKA_OP_SHIFT_LEFT
                                                   : PIKA_OP_SHIFT_RIGHT,
            result.slot, expression->slot, right.slot, 0, NULL);
        if (status == PIKA_STATUS_OK) *expression = result;
    }
    return status;
}
static PikaStatus parse_bitwise_and_new(PikaParser* parser,
                                        PikaExpression* expression) {
    PikaStatus status = parse_shift_new(parser, expression);
    while (status == PIKA_STATUS_OK && match(parser, PIKA_TOKEN_AMPERSAND)) {
        PikaExpression right;
        PikaExpression result;
        status = require_bitwise_operator(
            parser, token_at(parser, parser->position - 1u));
        if (status != PIKA_STATUS_OK) break;
        status = parse_shift_new(parser, &right);
        if (status != PIKA_STATUS_OK) break;
        status = allocate_slot(parser, &result.slot);
        if (status != PIKA_STATUS_OK) break;
        set_expression_kind(&result, PIKA_VALUE_INTEGER);
        mark_typed(parser);
        status = emit_instruction(
            parser, PIKA_OP_BITWISE_AND, result.slot, expression->slot,
            right.slot, 0, NULL);
        if (status == PIKA_STATUS_OK) *expression = result;
    }
    return status;
}
static PikaStatus parse_bitwise_xor_new(PikaParser* parser,
                                        PikaExpression* expression) {
    PikaStatus status = parse_bitwise_and_new(parser, expression);
    while (status == PIKA_STATUS_OK && match(parser, PIKA_TOKEN_CARET)) {
        PikaExpression right;
        PikaExpression result;
        status = require_bitwise_operator(
            parser, token_at(parser, parser->position - 1u));
        if (status != PIKA_STATUS_OK) break;
        status = parse_bitwise_and_new(parser, &right);
        if (status != PIKA_STATUS_OK) break;
        status = allocate_slot(parser, &result.slot);
        if (status != PIKA_STATUS_OK) break;
        set_expression_kind(&result, PIKA_VALUE_INTEGER);
        mark_typed(parser);
        status = emit_instruction(
            parser, PIKA_OP_BITWISE_XOR, result.slot, expression->slot,
            right.slot, 0, NULL);
        if (status == PIKA_STATUS_OK) *expression = result;
    }
    return status;
}
static PikaStatus parse_bitwise_or_new(PikaParser* parser,
                                       PikaExpression* expression) {
    PikaStatus status = parse_bitwise_xor_new(parser, expression);
    while (status == PIKA_STATUS_OK && match(parser, PIKA_TOKEN_PIPE)) {
        PikaExpression right;
        PikaExpression result;
        status = require_bitwise_operator(
            parser, token_at(parser, parser->position - 1u));
        if (status != PIKA_STATUS_OK) break;
        status = parse_bitwise_xor_new(parser, &right);
        if (status != PIKA_STATUS_OK) break;
        status = allocate_slot(parser, &result.slot);
        if (status != PIKA_STATUS_OK) break;
        set_expression_kind(&result, PIKA_VALUE_INTEGER);
        mark_typed(parser);
        status = emit_instruction(
            parser, PIKA_OP_BITWISE_OR, result.slot, expression->slot,
            right.slot, 0, NULL);
        if (status == PIKA_STATUS_OK) *expression = result;
    }
    return status;
}
static int comparison_operator_pending(const PikaParser* parser) {
    PikaTokenKind kind = current_token(parser)->kind;
    return kind == PIKA_TOKEN_IN || kind == PIKA_TOKEN_IS ||
           kind == PIKA_TOKEN_LESS || kind == PIKA_TOKEN_LESS_EQUAL ||
           kind == PIKA_TOKEN_GREATER ||
           kind == PIKA_TOKEN_GREATER_EQUAL ||
           kind == PIKA_TOKEN_EQUAL_EQUAL ||
           kind == PIKA_TOKEN_NOT_EQUAL ||
           (kind == PIKA_TOKEN_NOT &&
            next_token(parser)->kind == PIKA_TOKEN_IN);
}

static PikaStatus parse_comparison_new(PikaParser* parser,
                                       PikaExpression* expression) {
    PikaStatus status = parse_bitwise_or_new(parser, expression);
    PikaExpression left = *expression;
    PikaExpression result;
    int has_result = 0;
    int32_t false_branch_head = -1;
    while (status == PIKA_STATUS_OK &&
           comparison_operator_pending(parser)) {
        const PikaToken* operator_token = current_token(parser);
        PikaTokenKind kind = operator_token->kind;
        int negate = 0;
        PikaExpression right;
        PikaOpcode opcode;
        if (kind == PIKA_TOKEN_NOT) {
            negate = 1;
            parser->position += 2u;
            kind = PIKA_TOKEN_IN;
        } else if (kind == PIKA_TOKEN_IS) {
            ++parser->position;
            if (match(parser, PIKA_TOKEN_NOT)) negate = 1;
        } else {
            ++parser->position;
        }
        status = kind == PIKA_TOKEN_IN
                     ? require_capability_at(
                           parser,
                           PIKA_CAPABILITY_MEMBERSHIP_OPERATOR,
                           operator_token)
                     : require_comparison_operator(
                           parser, operator_token);
        if (status != PIKA_STATUS_OK) break;
        status = parse_bitwise_or_new(parser, &right);
        if (status != PIKA_STATUS_OK) break;
        if (!has_result) {
            status = allocate_slot(parser, &result.slot);
            if (status != PIKA_STATUS_OK) break;
            set_expression_kind(&result, PIKA_VALUE_BOOLEAN);
            if ((parser->enabled_capabilities &
                 PIKA_CAPABILITY_COMPARISON_OPERATOR) != 0u ||
                kind == PIKA_TOKEN_IN) {
                mark_typed(parser);
            }
            has_result = 1;
        }
        if (kind == PIKA_TOKEN_IN) {
            opcode = PIKA_OP_CONTAINS;
        } else if (kind == PIKA_TOKEN_IS) {
            opcode = PIKA_OP_IDENTITY;
        } else if (kind == PIKA_TOKEN_LESS) {
            opcode = PIKA_OP_LESS_THAN;
        } else if (kind == PIKA_TOKEN_LESS_EQUAL) {
            opcode = PIKA_OP_LESS_EQUAL;
        } else if (kind == PIKA_TOKEN_GREATER) {
            opcode = PIKA_OP_GREATER_THAN;
        } else if (kind == PIKA_TOKEN_GREATER_EQUAL) {
            opcode = PIKA_OP_GREATER_EQUAL;
        } else if (kind == PIKA_TOKEN_NOT_EQUAL) {
            opcode = PIKA_OP_NOT_EQUAL;
        } else {
            opcode = PIKA_OP_EQUAL;
        }
        status = emit_instruction(
            parser, opcode, result.slot, left.slot, right.slot,
            0, NULL);
        if (status == PIKA_STATUS_OK && negate) {
            status = emit_instruction(
                parser, PIKA_OP_LOGICAL_NOT, result.slot,
                result.slot, 0u, 0, NULL);
        }
        if (status != PIKA_STATUS_OK) break;
        left = right;
        if (comparison_operator_pending(parser)) {
            uint32_t branch_index;
            status = emit_instruction(
                parser, PIKA_OP_BRANCH_FALSE, result.slot,
                0u, 0u, false_branch_head, &branch_index);
            if (status == PIKA_STATUS_OK) {
                false_branch_head = (int32_t)branch_index;
            }
        }
    }
    if (status == PIKA_STATUS_OK && has_result) {
        uint32_t target =
            parser->module->program.instruction_count;
        while (status == PIKA_STATUS_OK && false_branch_head >= 0) {
            uint32_t branch_index = (uint32_t)false_branch_head;
            PikaInstruction* branch =
                mutable_instruction_at(parser, branch_index);
            if (branch == NULL) {
                status = fail_current(
                    parser, PIKA_STATUS_INTERNAL_ERROR);
            } else {
                false_branch_head = branch->immediate;
                branch->immediate = (int32_t)target;
            }
        }
        if (status == PIKA_STATUS_OK) *expression = result;
    }
    return status;
}

static PikaStatus emit_short_circuit(PikaParser* parser,
                                     PikaExpression left,
                                     PikaExpression right,
                                     PikaTokenKind kind,
                                     PikaExpression* result,
                                     uint32_t branch_index) {
    PikaStatus status = allocate_slot(parser, &result->slot);
    if (status != PIKA_STATUS_OK) return status;
    set_expression_kind(result, PIKA_VALUE_UNKNOWN);
    status = emit_instruction(
        parser, PIKA_OP_MOVE, result->slot, left.slot, 0u, 0, NULL);
    if (status == PIKA_STATUS_OK) {
        status = emit_instruction(
            parser, kind == PIKA_TOKEN_AND ? PIKA_OP_BRANCH_FALSE
                                           : PIKA_OP_BRANCH_TRUE,
            left.slot, 0u, 0u, 0, &branch_index);
    }
    (void)right;
    return status;
}

static PikaStatus parse_and_new(PikaParser* parser,
                                PikaExpression* expression) {
    PikaStatus status = parse_comparison_new(parser, expression);
    while (status == PIKA_STATUS_OK && match(parser, PIKA_TOKEN_AND)) {
        PikaExpression left = *expression;
        PikaExpression right;
        PikaExpression result;
        uint32_t branch_index;
        status = require_capability_at(
            parser, PIKA_CAPABILITY_SHORT_CIRCUIT_LOGIC,
            token_at(parser, parser->position - 1u));
        if (status != PIKA_STATUS_OK) break;
        status = emit_short_circuit(
            parser, left, left, PIKA_TOKEN_AND, &result, 0u);
        if (status != PIKA_STATUS_OK) break;
        branch_index = parser->module->program.instruction_count - 1u;
        status = parse_comparison_new(parser, &right);
        if (status != PIKA_STATUS_OK) break;
        status = emit_instruction(
            parser, PIKA_OP_MOVE, result.slot, right.slot, 0u, 0, NULL);
        if (status == PIKA_STATUS_OK) {
            status = set_instruction_immediate(
                parser, branch_index,
                (int32_t)parser->module->program.instruction_count);
        }
        if (status == PIKA_STATUS_OK) {
            *expression = result;
        }
    }
    return status;
}

static PikaStatus parse_or_new(PikaParser* parser,
                               PikaExpression* expression) {
    PikaStatus status = parse_and_new(parser, expression);
    while (status == PIKA_STATUS_OK && match(parser, PIKA_TOKEN_OR)) {
        PikaExpression left = *expression;
        PikaExpression right;
        PikaExpression result;
        uint32_t branch_index;
        status = require_capability_at(
            parser, PIKA_CAPABILITY_SHORT_CIRCUIT_LOGIC,
            token_at(parser, parser->position - 1u));
        if (status != PIKA_STATUS_OK) break;
        status = emit_short_circuit(
            parser, left, left, PIKA_TOKEN_OR, &result, 0u);
        if (status != PIKA_STATUS_OK) break;
        branch_index = parser->module->program.instruction_count - 1u;
        status = parse_and_new(parser, &right);
        if (status != PIKA_STATUS_OK) break;
        status = emit_instruction(
            parser, PIKA_OP_MOVE, result.slot, right.slot, 0u, 0, NULL);
        if (status == PIKA_STATUS_OK) {
            status = set_instruction_immediate(
                parser, branch_index,
                (int32_t)parser->module->program.instruction_count);
        }
        if (status == PIKA_STATUS_OK) {
            *expression = result;
        }
    }
    return status;
}

static PikaStatus parse_expression_body(PikaParser* parser,
                                        PikaExpression* expression) {
    PikaStatus status = parse_or_new(parser, expression);
    if (status != PIKA_STATUS_OK ||
        current_token(parser)->kind != PIKA_TOKEN_IF) {
        return status;
    }
    {
        PikaExpression condition;
        PikaExpression alternative;
        PikaExpression result;
        ++parser->position;
        status = parse_or_new(parser, &condition);
        if (status == PIKA_STATUS_OK) {
            status = require(parser, PIKA_TOKEN_ELSE);
        }
        if (status != PIKA_STATUS_OK) return status;
        status = allocate_slot(parser, &result.slot);
        if (status != PIKA_STATUS_OK) return status;
        set_expression_kind(&result, PIKA_VALUE_UNKNOWN);
        mark_typed(parser);

        /* Keep the unselected arm out of the execution path. */
        {
            uint32_t false_branch;
            uint32_t skip_alternative;
            uint32_t alternative_start;
            uint32_t end;
            status = emit_instruction(
                parser, PIKA_OP_BRANCH_FALSE, condition.slot,
                0u, 0u, 0, &false_branch);
            if (status == PIKA_STATUS_OK) {
                status = emit_instruction(
                    parser, PIKA_OP_MOVE, result.slot,
                    expression->slot, 0u, 0, NULL);
            }
            if (status == PIKA_STATUS_OK) {
                status = emit_instruction(
                    parser, PIKA_OP_JUMP, 0u, 0u, 0u,
                    0, &skip_alternative);
            }
            alternative_start = parser->module->program.instruction_count;
            if (status == PIKA_STATUS_OK) {
                status = set_instruction_immediate(
                    parser, false_branch,
                    (int32_t)alternative_start);
            }
            if (status == PIKA_STATUS_OK) {
                status = parse_expression(parser, &alternative);
            }
            if (status == PIKA_STATUS_OK) {
                status = emit_instruction(
                    parser, PIKA_OP_MOVE, result.slot,
                    alternative.slot, 0u, 0, NULL);
            }
            end = parser->module->program.instruction_count;
            if (status == PIKA_STATUS_OK) {
                status = set_instruction_immediate(
                    parser, skip_alternative, (int32_t)end);
            }
        }
        if (status == PIKA_STATUS_OK) *expression = result;
    }
    return status;
}

static PikaStatus parse_expression(PikaParser* parser,
                                   PikaExpression* expression) {
    PikaStatus status = enter_expression_depth(parser);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    status = parse_expression_body(parser, expression);
    leave_expression_depth(parser);
    return status;
}
