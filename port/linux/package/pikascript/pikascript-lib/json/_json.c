#include "_json.h"
#include "jsmn.h"

#if !PIKASCRIPT_VERSION_REQUIRE_MINIMUN(1, 13, 0)
#error "pikapython version must be greater than 1.13.0"
#endif

int parse_json(const char* json_string,
               jsmn_parser* parser,
               jsmntok_t** tokens) {
    jsmn_init(parser);

    int token_count =
        jsmn_parse(parser, json_string, strlen(json_string), NULL, 0);

    if (token_count < 0) {
        // JSMN_ERROR_INVAL or JSMN_ERROR_PART
        pika_platform_printf("Failed to parse JSON: %d\n", token_count);
        return -1;
    }

    *tokens = pika_platform_malloc(sizeof(jsmntok_t) * token_count);
    if (*tokens == NULL) {
        pika_platform_printf("Failed to allocate memory: %d\n", token_count);
        return -1;
    }

    jsmn_init(parser);
    int result = jsmn_parse(parser, json_string, strlen(json_string), *tokens,
                            token_count);
    if (result < 0) {
        pika_platform_printf("Failed to parse JSON: %d\n", result);
        pika_platform_free(*tokens);
        *tokens = NULL;
        return -1;
    }

    return result;
}

void PikaStdData_List_append(PikaObj* self, Arg* arg);
void PikaStdData_Dict_set(PikaObj* self, char* key, Arg* arg);

Arg* json_to_arg_recursive(jsmntok_t* t, int* index, char* json_str) {
    Arg* val;
    jsmntype_t type = t[*index].type;
    switch (type) {
        case JSMN_PRIMITIVE: {
            char buff[PIKA_NAME_BUFF_SIZE] = {0};
            pika_platform_memcpy(buff, json_str + t[*index].start,
                                 t[*index].end - t[*index].start);
            if (strEqu(buff, "true")) {
                val = arg_newBool(1);
            } else if (strEqu(buff, "false")) {
                val = arg_newBool(0);
            } else if (strEqu(buff, "null")) {
                val = arg_newNull();
            } else {
                /* float */
                if (strIsContain(buff, '.') ||
                    (strIsContain(buff, 'e') || strIsContain(buff, 'E'))) {
                    val = arg_newFloat(strtod(buff, NULL));
                } else {
                    /* int */
                    val = arg_newInt(fast_atoi(buff));
                }
            }
            break;
        }
        case JSMN_STRING:
            val = arg_newStrN(json_str + t[*index].start,
                              t[*index].end - t[*index].start);
            break;
        case JSMN_OBJECT: {
            PikaObj* ret = obj_newDict(NULL);
            int num_keys = t[*index].size;
            (*index)++;
            for (int i = 0; i < num_keys; i++) {
                char key[PIKA_NAME_BUFF_SIZE] = {0};
                pika_platform_memcpy(key, json_str + t[*index].start,
                                     t[*index].end - t[*index].start);
                (*index)++;
                Arg* val_nested = json_to_arg_recursive(t, index, json_str);
                PikaStdData_Dict_set(ret, key, val_nested);
                arg_deinit(val_nested);
                if (i < num_keys - 1) {
                    (*index)++;
                }
            }
            val = arg_newObj(ret);
            break;
        }
        case JSMN_ARRAY: {
            PikaObj* ret = obj_newList(NULL);
            int num_elements = t[*index].size;
            (*index)++;
            for (int i = 0; i < num_elements; i++) {
                Arg* val_nested = json_to_arg_recursive(t, index, json_str);
                PikaStdData_List_append(ret, val_nested);
                arg_deinit(val_nested);
                if (i < num_elements - 1) {
                    (*index)++;
                }
            }
            val = arg_newObj(ret);
            break;
        }
        default: /* Should not reach here */
            val = NULL;
    }
    return val;
}

Arg* _json_loads(PikaObj* self, char* json_str) {
    jsmn_parser parser;
    jsmntok_t* tokens = NULL;
    Arg* ret = NULL;
    int token_count = parse_json(json_str, &parser, &tokens);
    if (token_count < 0) {
        ret = NULL;
        goto __exit;
    }
    jsmntok_t tokens_[50];
    pika_platform_memcpy(tokens_, tokens, sizeof(jsmntok_t) * token_count);
    ret = json_to_arg_recursive(tokens, &(int){0}, json_str);
__exit:
    if (NULL != tokens) {
        free(tokens);
    }
    return ret;
}
