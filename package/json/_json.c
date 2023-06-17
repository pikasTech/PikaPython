#include "_json.h"
#include "../pikascript-lib/pika_cjson/cJSON.h"
#include "jsmn.h"

#if !PIKASCRIPT_VERSION_REQUIRE_MINIMUN(1, 13, 0)
#error "pikapython version must be greater than 1.13.0"
#endif

cJSON* _cjson_decode(Arg* d);

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

Arg* json_to_arg_recursive(jsmntok_t* t,
                           int* index,
                           char* json_str,
                           int token_count) {
    Arg* val;
    pika_assert(*index >= 0);
    pika_assert(*index < token_count);
    jsmntype_t type = t[*index].type;
    switch (type) {
        case JSMN_PRIMITIVE: {
            char buff[PIKA_NAME_BUFF_SIZE] = {0};
            pika_assert(t[*index].end - t[*index].start < PIKA_NAME_BUFF_SIZE);
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
            for (int i = 0; i < num_keys; i++) {
                (*index)++;
                jsmntok_t* key_tok = &t[*index];
                char* value = json_str + key_tok->start;
                int size = key_tok->end - key_tok->start;
                pika_assert(key_tok->type == JSMN_STRING);
                char key[PIKA_NAME_BUFF_SIZE] = {0};
                pika_assert(size < PIKA_NAME_BUFF_SIZE);
                pika_platform_memcpy(key, value, size);
                (*index)++;
                Arg* val_nested =
                    json_to_arg_recursive(t, index, json_str, token_count);
                objDict_set(ret, key, val_nested);
                arg_deinit(val_nested);
            }
            val = arg_newObj(ret);
            break;
        }
        case JSMN_ARRAY: {
            PikaObj* ret = obj_newList(NULL);
            jsmntok_t* key_tok = &t[*index];
            int num_elements = key_tok->size;
            for (int i = 0; i < num_elements; i++) {
                (*index)++;
                Arg* val_nested =
                    json_to_arg_recursive(t, index, json_str, token_count);
                objList_append(ret, val_nested);
                arg_deinit(val_nested);
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
    ret = json_to_arg_recursive(tokens, &(int){0}, json_str, token_count);
__exit:
    if (NULL != tokens) {
        free(tokens);
    }
    return ret;
}

typedef struct {
    cJSON* jsonArray;
} JsonListContext;

int32_t jsonListEachHandle(PikaObj* self,
                           int itemIndex,
                           Arg* itemEach,
                           void* context) {
    JsonListContext* ctx = (JsonListContext*)context;
    cJSON_AddItemToArray(ctx->jsonArray, _cjson_decode(itemEach));
    return 0;
}

typedef struct {
    cJSON* jsonObject;
} JsonDictContext;

int32_t jsonDictEachHandle(PikaObj* self,
                           Arg* keyEach,
                           Arg* valEach,
                           void* context) {
    JsonDictContext* ctx = (JsonDictContext*)context;
    cJSON_AddItemToObject(ctx->jsonObject, arg_getStr(keyEach),
                          _cjson_decode(valEach));
    return 0;
}

cJSON* _cjson_decode(Arg* d) {
    ArgType type = arg_getType(d);
    if (type == ARG_TYPE_NONE) {
        return cJSON_CreateNull();
    } else if (type == ARG_TYPE_INT) {
        return cJSON_CreateNumber(arg_getInt(d));
    } else if (type == ARG_TYPE_FLOAT) {
        return cJSON_CreateNumber(arg_getFloat(d));
    } else if (type == ARG_TYPE_BOOL) {
        if (arg_getBool(d)) {
            return cJSON_CreateTrue();
        } else {
            return cJSON_CreateFalse();
        }
    } else if (type == ARG_TYPE_STRING) {
        return cJSON_CreateString(arg_getStr(d));
    } else if (arg_isList(d)) {
        JsonListContext context;
        context.jsonArray = cJSON_CreateArray();
        objList_forEach(arg_getObj(d), jsonListEachHandle, &context);
        return context.jsonArray;
    } else if (arg_isDict(d)) {
        JsonDictContext context;
        context.jsonObject = cJSON_CreateObject();
        objDict_forEach(arg_getObj(d), jsonDictEachHandle, &context);
        return context.jsonObject;
    } else {
        return cJSON_CreateNull();
    }
}

char* _json_dumps(PikaObj* self, Arg* d) {
    cJSON* json = _cjson_decode(d);
    char* ret = cJSON_Print(json);
    char* cached = obj_cacheStr(self, ret);
    cJSON_Delete(json);
    pika_platform_free(ret);
    return cached;
}
