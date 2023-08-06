#include "_json.h"
#include "_pika_cJSON.h"
#include "jsmn.h"

#if !PIKASCRIPT_VERSION_REQUIRE_MINIMUN(1, 12, 4)
#error "pikapython version must be greater than 1.13.0"
#endif

cJSON* _pika_cJSON_decode(Arg* d);

#define USING_JSMN 0

int parse_json_jsmn(const char* json_string,
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

Arg* json_encode_jsmn(jsmntok_t* t,
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
        case JSMN_STRING: {
            val = arg_newStrN(json_str + t[*index].start,
                              t[*index].end - t[*index].start);
            char* raw = arg_getStr(val);
            if (strIsContain(arg_getStr(val), '\\')) {
                Args buffs = {0};
                size_t i = 0;
                char* transfered_str = strsTransfer(&buffs, raw, &i);
                Arg* val_transfered = arg_newStr(transfered_str);
                arg_deinit(val);
                val = val_transfered;
                strsDeinit(&buffs);
            }
            break;
        }
        case JSMN_OBJECT: {
            PikaObj* ret = New_pikaDict();
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
                    json_encode_jsmn(t, index, json_str, token_count);
                pikaDict_set(ret, key, val_nested);
                arg_deinit(val_nested);
            }
            val = arg_newObj(ret);
            break;
        }
        case JSMN_ARRAY: {
            PikaObj* ret = New_pikaListFromVarArgs(NULL);
            jsmntok_t* key_tok = &t[*index];
            int num_elements = key_tok->size;
            for (int i = 0; i < num_elements; i++) {
                (*index)++;
                Arg* val_nested =
                    json_encode_jsmn(t, index, json_str, token_count);
                pikaList_append(ret, val_nested);
            }
            val = arg_newObj(ret);
            break;
        }
        default: /* Should not reach here */
            val = NULL;
    }
    return val;
}

Arg* json_encode_cjson(cJSON* cjson) {
    if (cjson == NULL) {
        return NULL;
    }

    switch (cjson->type) {
        case pika_cJSON_Invalid: {
            return NULL;
        }
        case pika_cJSON_False: {
            return arg_newBool(pika_false);
        }
        case pika_cJSON_True: {
            return arg_newBool(pika_true);
        }
        case pika_cJSON_NULL: {
            return arg_newNull();
        }
        case pika_cJSON_Number: {
            pika_float num_f = cjson->valuedouble;
            int num_i = cjson->valueint;
            if (num_f == num_i) {
                return arg_newInt(num_i);
            } else {
                return arg_newFloat(num_f);
            }
        }
        case pika_cJSON_String: {
            return arg_newStr(cjson->valuestring);
        }
        case pika_cJSON_Array: {
            PikaObj* ret = New_pikaListFromVarArgs(NULL);
            for (int i = 0; i < pika_cJSON_GetArraySize(cjson); i++) {
                cJSON* item = pika_cJSON_GetArrayItem(cjson, i);
                Arg* nested_arg = json_encode_cjson(item);
                pikaList_append(ret, nested_arg);
            }
            return arg_newObj(ret);
        }
        case pika_cJSON_Object: {
            PikaObj* ret = New_pikaDictFromVarArgs(NULL);
            cJSON* child = cjson->child;
            for (int i = 0; i < pika_cJSON_GetArraySize(cjson); i++) {
                char* key = child->string;
                Arg* nested_arg = json_encode_cjson(child);
                pikaDict_set(ret, key, nested_arg);
                child = child->next;
            }
            return arg_newObj(ret);
        }
        case pika_cJSON_Raw: {
            return arg_newStr(cjson->valuestring);
        }
        default: {
            return NULL;
        }
    }
}

Arg* _json_loads(PikaObj* self, char* json_str) {
#if USING_JSMN
    jsmn_parser parser;
    Arg* ret = NULL;
    jsmntok_t* tokens = NULL;
    int token_count = parse_json_jsmn(json_str, &parser, &tokens);
    if (token_count < 0) {
        ret = NULL;
        goto __exit;
    }
    ret = json_encode_jsmn(tokens, &(int){0}, json_str, token_count);
__exit:
    if (NULL != tokens) {
        free(tokens);
    }
    return ret;
#else
    cJSON* cjson = pika_cJSON_Parse(json_str);
    if (cjson == NULL) {
        pika_platform_printf("JSONDecodeError: at \'%s\'\r\n",
                             pika_cJSON_GetErrorPtr());
        obj_setErrorCode(self, PIKA_RES_ERR_RUNTIME_ERROR);
        return NULL;
    }
    Arg* ret = json_encode_cjson(cjson);
    pika_cJSON_Delete(cjson);
    return ret;
#endif
}

typedef struct {
    cJSON* jsonArray;
} JsonListContext;

int32_t jsonListEachHandle(PikaObj* self,
                           int itemIndex,
                           Arg* itemEach,
                           void* context) {
    JsonListContext* ctx = (JsonListContext*)context;
    pika_cJSON_AddItemToArray(ctx->jsonArray, _pika_cJSON_decode(itemEach));
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
    pika_cJSON_AddItemToObject(ctx->jsonObject, arg_getStr(keyEach),
                               _pika_cJSON_decode(valEach));
    return 0;
}

cJSON* _pika_cJSON_decode(Arg* d) {
    ArgType type = arg_getType(d);
    switch (type) {
        case ARG_TYPE_NONE:
            return pika_cJSON_CreateNull();

        case ARG_TYPE_INT:
            return pika_cJSON_CreateNumber(arg_getInt(d));

        case ARG_TYPE_FLOAT:
            return pika_cJSON_CreateNumber(arg_getFloat(d));

        case ARG_TYPE_BOOL:
            return arg_getBool(d) ? pika_cJSON_CreateTrue()
                                  : pika_cJSON_CreateFalse();

        case ARG_TYPE_STRING:
            return pika_cJSON_CreateString(arg_getStr(d));

        default:
            if (arg_isList(d)) {
                JsonListContext context;
                context.jsonArray = pika_cJSON_CreateArray();
                pikaList_forEach(arg_getObj(d), jsonListEachHandle, &context);
                return context.jsonArray;
            }

            if (arg_isDict(d)) {
                JsonDictContext context;
                context.jsonObject = pika_cJSON_CreateObject();
                pikaDict_forEach(arg_getObj(d), jsonDictEachHandle, &context);
                return context.jsonObject;
            }
            return pika_cJSON_CreateNull();
    }
}

char* _json_dumps(PikaObj* self, Arg* d) {
    cJSON* json = _pika_cJSON_decode(d);
    char* ret = pika_cJSON_Print(json);
    char* cached = obj_cacheStr(self, ret);
    pika_cJSON_Delete(json);
    pika_platform_free(ret);
    return cached;
}
