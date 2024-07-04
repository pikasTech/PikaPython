/*
 * This file is part of the PikaPython project.
 * http://github.com/pikastech/pikapython
 *
 * MIT License
 *
 * Copyright (c) 2024 lyon liang6516@outlook.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "jrpc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 示例函数：add
cJSON* add(cJSON* params[], int param_count) {
    int a = params[0]->valueint;
    int b = params[1]->valueint;
    int sum = a + b;
    cJSON* result = cJSON_CreateNumber(sum);
    return result;
}

// 示例非阻塞函数：add_nonblocking
int add_nonblocking(int id, cJSON* params[], int param_count, JRPC* self) {
    int a = params[0]->valueint;
    int b = params[1]->valueint;
    int sum = a + b;
    cJSON* result = cJSON_CreateNumber(sum);
    JRPC_send_response(self, id, result);
    return 0;
}

// 示例函数：subtract
cJSON* subtract(cJSON* params[], int param_count) {
    int a = params[0]->valueint;
    int b = params[1]->valueint;
    int difference = a - b;
    cJSON* result = cJSON_CreateNumber(difference);
    return result;
}

rpc_mapping default_rpc_map[] = {{"add", add, 2},
                                 {"subtract", subtract, 2},
                                 RPC_MAP_END};

rpc_mapping_nonblocking default_nonblocking_rpc_map[] = {
    {"add_nonblocking", add_nonblocking, 2},
    RPC_MAP_END};

void JRPC_send_acknowledgement(JRPC* self, int id, ack_status status) {
    cJSON* response = cJSON_CreateObject();
    cJSON_AddStringToObject(response, STR_JSON_RPC_FIELD, STR_JSON_RPC_VERSION);
    cJSON_AddStringToObject(
        response, STR_STATUS_FIELD,
        (status == ACK_SUCCESS)            ? STR_RECEIVED_STATUS
        : (status == ACK_METHOD_NOT_FOUND) ? STR_METHOD_NOT_FOUND_STATUS
        : (status == ACK_INVALID_PARAMS)   ? STR_INVALID_PARAMS_STATUS
                                           : STR_UNKNOWN_STATUS);
    cJSON_AddNumberToObject(response, STR_ID_FIELD, id);
    cJSON_AddNumberToObject(response, STR_TYPE_FIELD,
                            TYPE_ACK);  // 添加类型字段

    char* response_str = cJSON_Print(response);
    printf("Acknowledgement: %s\n", response_str);

    self->send(response_str);

    free(response_str);
    cJSON_Delete(response);
}

void JRPC_send_response(JRPC* self, int id, cJSON* result) {
    cJSON* response = cJSON_CreateObject();
    cJSON_AddStringToObject(response, STR_JSON_RPC_FIELD, STR_JSON_RPC_VERSION);
    cJSON_AddItemToObject(response, STR_RESULT_FIELD, result);
    cJSON_AddNumberToObject(response, STR_ID_FIELD, id);
    cJSON_AddNumberToObject(response, STR_TYPE_FIELD,
                            TYPE_RESULT);  // 添加类型字段

    char* response_str = cJSON_Print(response);
    printf("Response: %s\n", response_str);

    self->send(response_str);

    free(response_str);
}

void JRPC_handle_request(JRPC* self, const char* json_str) {
    cJSON* json = cJSON_Parse(json_str);
    if (json == NULL) {
        printf("Error parsing JSON\n");
        return;
    }

    cJSON* jsonrpc = cJSON_GetObjectItem(json, STR_JSON_RPC_FIELD);
    cJSON* method = cJSON_GetObjectItem(json, STR_METHOD_FIELD);
    cJSON* params = cJSON_GetObjectItem(json, STR_PARAMS_FIELD);
    cJSON* id = cJSON_GetObjectItem(json, STR_ID_FIELD);
    cJSON* type = cJSON_GetObjectItem(json, STR_TYPE_FIELD);

    if (!cJSON_IsString(jsonrpc) || !cJSON_IsString(method) ||
        !cJSON_IsArray(params) || !cJSON_IsNumber(id) ||
        !cJSON_IsNumber(type)) {
        printf("Invalid JSON RPC request format\n");
        cJSON_Delete(json);
        return;
    }

    if (strcmp(jsonrpc->valuestring, STR_JSON_RPC_VERSION) != 0) {
        printf("Unsupported JSON RPC version: %s\n", jsonrpc->valuestring);
        cJSON_Delete(json);
        return;
    }

    if (type->valueint != TYPE_REQUEST) {
        printf("Invalid JSON RPC message type\n");
        cJSON_Delete(json);
        return;
    }

    int expected_param_count;
    rpc_function_nonblocking func_nonblocking =
        JRPC_find_nonblocking_rpc_function(self, method->valuestring,
                                           &expected_param_count);
    if (func_nonblocking != NULL) {
        int param_count = cJSON_GetArraySize(params);
        if (expected_param_count != PARAM_COUNT_NO_CHECK &&
            param_count != expected_param_count) {
            JRPC_send_acknowledgement(self, id->valueint, ACK_INVALID_PARAMS);
            cJSON_Delete(json);
            return;
        }

        // 发送成功ACK
        JRPC_send_acknowledgement(self, id->valueint, ACK_SUCCESS);

        // 调用非阻塞函数
        cJSON* param_array[param_count];
        for (int i = 0; i < param_count; i++) {
            param_array[i] = cJSON_GetArrayItem(params, i);
        }

        func_nonblocking(id->valueint, param_array, param_count, self);
    } else {
        rpc_function func = JRPC_find_rpc_function(self, method->valuestring,
                                                   &expected_param_count);
        if (func == NULL) {
            JRPC_send_acknowledgement(self, id->valueint, ACK_METHOD_NOT_FOUND);
            cJSON_Delete(json);
            return;
        }

        int param_count = cJSON_GetArraySize(params);
        if (expected_param_count != PARAM_COUNT_NO_CHECK &&
            param_count != expected_param_count) {
            JRPC_send_acknowledgement(self, id->valueint, ACK_INVALID_PARAMS);
            cJSON_Delete(json);
            return;
        }

        // 发送成功ACK
        JRPC_send_acknowledgement(self, id->valueint, ACK_SUCCESS);

        // 调用函数
        cJSON* param_array[param_count];
        for (int i = 0; i < param_count; i++) {
            param_array[i] = cJSON_GetArrayItem(params, i);
        }

        cJSON* result = func(param_array, param_count);
        JRPC_send_response(self, id->valueint, result);
    }

    cJSON_Delete(json);
}

rpc_function JRPC_find_rpc_function(JRPC* self,
                                    const char* name,
                                    int* param_count) {
    for (int i = 0; self->map[i].name != NULL; i++) {
        if (strcmp(self->map[i].name, name) == 0) {
            *param_count = self->map[i].param_count;
            return self->map[i].func;
        }
    }
    return NULL;  // 没有找到对应的函数
}

rpc_function_nonblocking JRPC_find_nonblocking_rpc_function(JRPC* self,
                                                            const char* name,
                                                            int* param_count) {
    for (int i = 0; self->nonblocking_map[i].name != NULL; i++) {
        if (strcmp(self->nonblocking_map[i].name, name) == 0) {
            *param_count = self->nonblocking_map[i].param_count;
            return self->nonblocking_map[i].func;
        }
    }
    return NULL;  // 没有找到对应的函数
}

// 缓存添加函数
void JRPC_cache_add(JRPC* self, cJSON* item) {
    if (self->cache_count < CACHE_SIZE) {
        self->cache[self->cache_count++] = item;
    } else {
        // 缓存已满，删除最旧的
        cJSON_Delete(self->cache[0]);
        for (int i = 0; i < CACHE_SIZE - 1; i++) {
            self->cache[i] = self->cache[i + 1];
        }
        self->cache[CACHE_SIZE - 1] = item;
    }
}

// 缓存获取函数
cJSON* JRPC_cache_get(JRPC* self, int id, int type) {
    for (int i = 0; i < self->cache_count; i++) {
        cJSON* cached_json = self->cache[i];
        cJSON* cached_id = cJSON_GetObjectItem(cached_json, STR_ID_FIELD);
        cJSON* cached_type = cJSON_GetObjectItem(cached_json, STR_TYPE_FIELD);
        if (cached_id && cJSON_IsNumber(cached_id) &&
            cached_id->valueint == id && cached_type &&
            cJSON_IsNumber(cached_type) && cached_type->valueint == type) {
            // 找到匹配项，返回并从缓存中删除
            cJSON* result = cached_json;
            for (int j = i; j < self->cache_count - 1; j++) {
                self->cache[j] = self->cache[j + 1];
            }
            self->cache[--self->cache_count] = NULL;
            return result;
        }
    }
    return NULL;
}

cJSON* JRPC_receive_with_id_and_type(JRPC* self, int id, int type) {
    // 先检查缓存
    cJSON* cached_json = JRPC_cache_get(self, id, type);
    if (cached_json != NULL) {
        return cached_json;
    }

    // 缓存中没有匹配项，从接收接口获取
    char* received_str = self->receive();
    if (received_str != NULL) {
        cJSON* received_json = cJSON_Parse(received_str);
        free(received_str);
        if (received_json != NULL) {
            cJSON* received_id =
                cJSON_GetObjectItem(received_json, STR_ID_FIELD);
            cJSON* received_type =
                cJSON_GetObjectItem(received_json, STR_TYPE_FIELD);
            if (received_id && cJSON_IsNumber(received_id) &&
                received_id->valueint == id && received_type &&
                cJSON_IsNumber(received_type) &&
                received_type->valueint == type) {
                return received_json;
            } else {
                // 缓存数据
                JRPC_cache_add(self, received_json);
            }
        }
    }
    return NULL;
}

void JRPC_send_request_no_blocking(JRPC* self,
                                   const char* method,
                                   cJSON* params[],
                                   int param_count,
                                   rpc_callback callback) {
    // 构建请求
    int id = ++self->current_id;
    cJSON* request = cJSON_CreateObject();
    cJSON_AddStringToObject(request, STR_JSON_RPC_FIELD, STR_JSON_RPC_VERSION);
    cJSON_AddStringToObject(request, STR_METHOD_FIELD, method);

    cJSON* params_array = cJSON_CreateArray();
    for (int i = 0; i < param_count; i++) {
        cJSON_AddItemToArray(params_array, cJSON_Duplicate(params[i], 1));
    }
    cJSON_AddItemToObject(request, STR_PARAMS_FIELD, params_array);
    cJSON_AddNumberToObject(request, STR_ID_FIELD, id);
    cJSON_AddNumberToObject(request, STR_TYPE_FIELD,
                            TYPE_REQUEST);  // 添加类型字段

    char* request_str = cJSON_Print(request);
    printf("Sending Request (no_blocking): %s\n", request_str);

    // 发送请求并重试
    int retry;
    for (retry = 0; retry < RETRY_COUNT; retry++) {
        self->send(request_str);

        unsigned long start_time = self->tick();
        cJSON* ack_json = NULL;
        while (1) {
            ack_json = JRPC_receive_with_id_and_type(self, id, TYPE_ACK);
            if (ack_json != NULL) {
                printf("Received ACK, id: %d\n", id);
                cJSON_Delete(ack_json);
                break;  // 收到正确的ACK
            }
            if (self->tick() - start_time >= ACK_TIMEOUT) {
                printf("ACK timeout, retrying...\n");
                break;
            }
            self->yield();  // 多线程切换
        }

        if (ack_json != NULL) {
            break;
        }
    }

    // 如果收到ACK
    if (retry < RETRY_COUNT) {
        // 调用回调函数处理结果
        callback(NULL);  // 模拟调用回调函数，不传递结果
    } else {
        printf("Failed to receive ACK after %d retries\n", RETRY_COUNT);
    }

    free(request_str);
    cJSON_Delete(request);
}

cJSON* JRPC_send_request_blocking(JRPC* self,
                                  const char* method,
                                  cJSON* params[],
                                  int param_count) {
    // 构建请求
    int id = ++self->current_id;
    cJSON* request = cJSON_CreateObject();
    cJSON_AddStringToObject(request, STR_JSON_RPC_FIELD, STR_JSON_RPC_VERSION);
    cJSON_AddStringToObject(request, STR_METHOD_FIELD, method);

    cJSON* params_array = cJSON_CreateArray();
    for (int i = 0; i < param_count; i++) {
        cJSON_AddItemToArray(params_array, cJSON_Duplicate(params[i], 1));
    }
    cJSON_AddItemToObject(request, STR_PARAMS_FIELD, params_array);
    cJSON_AddNumberToObject(request, STR_ID_FIELD, id);
    cJSON_AddNumberToObject(request, STR_TYPE_FIELD,
                            TYPE_REQUEST);  // 添加类型字段

    char* request_str = cJSON_Print(request);
    printf("Sending Request (blocking): %s\n", request_str);

    cJSON* ack_json = NULL;
    // 发送请求并重试
    for (int retry = 0; retry < RETRY_COUNT; retry++) {
        self->send(request_str);

        unsigned long start_time = self->tick();
        while (1) {
            ack_json = JRPC_receive_with_id_and_type(self, id, TYPE_ACK);
            if (ack_json != NULL) {
                cJSON_Delete(ack_json);
                break;  // 收到正确的ACK
            }
            if (self->tick() - start_time >= ACK_TIMEOUT) {
                printf("ACK timeout, retrying...\n");
                break;
            }
            self->yield();  // 多线程切换
        }

        if (ack_json != NULL) {
            break;
        }
    }

    if (ack_json == NULL) {
        printf("Failed to receive ACK after %d retries\n", RETRY_COUNT);
        free(request_str);
        cJSON_Delete(request);
        return NULL;
    }

    printf("Received ACK: %d\n", id);

    // 等待执行完毕响应
    unsigned long start_time = self->tick();
    while (1) {
        cJSON* response_json =
            JRPC_receive_with_id_and_type(self, id, TYPE_RESULT);
        if (response_json != NULL) {
            cJSON_Delete(request);
            free(request_str);
            return response_json;
        }
        if (self->tick() - start_time >= BLOCKING_TIMEOUT) {
            printf("Response timeout\n");
            free(request_str);
            cJSON_Delete(request);
            return NULL;
        }
        self->yield();  // 多线程切换
    }
}

// 模拟发送函数
static void mock_send(const char* message) {
    printf("mock send: %s\n", message);
}

// 模拟接收函数（非阻塞）
static char* mock_receive(void) {
    static int call_count = 0;
    call_count++;
    switch (call_count) {
        case 3:
            return strdup(
                "{\"jsonrpc\": \"1.0\", \"status\": \"received\", \"id\": 1, "
                "\"type\": 1}");
        case 6:
            return strdup(
                "{\"jsonrpc\": \"1.0\", \"status\": \"received\", \"id\": 2, "
                "\"type\": 1}");
        case 9:
            return strdup(
                "{\"jsonrpc\": \"1.0\", \"result\": 8, \"id\": 2, \"type\": "
                "2}");
        default:
            return NULL;
    }
}

// 模拟 yield 函数
static void mock_yield(void) {
    printf("$");
}

// 模拟 tick 函数
static unsigned long mock_tick_ms(void) {
    static unsigned long tick = 0;
    tick += 100;  // 模拟每次调用增加100ms
    return tick;
}

static void result_callback(cJSON* result) {
    printf("Callback executed. Result: %s\n",
           result ? cJSON_Print(result) : "No result");
}

int jrpc_base_test() {
    int ret = 0;
    JRPC jrpc = {default_rpc_map,
                 default_nonblocking_rpc_map,
                 mock_send,
                 mock_receive,
                 mock_yield,
                 mock_tick_ms,
                 0,
                 {NULL},
                 0};

    // 测试 no_blocking
    cJSON* params1[] = {cJSON_CreateNumber(5), cJSON_CreateNumber(3)};
    JRPC_send_request_no_blocking(&jrpc, "add_nonblocking", params1, 2,
                                  result_callback);

    // 测试 blocking
    cJSON* params2[] = {cJSON_CreateNumber(5), cJSON_CreateNumber(3)};
    cJSON* response = JRPC_send_request_blocking(&jrpc, "add", params2, 2);
    char* call_result = cJSON_Print(response);
    printf("Blocking call result: %s\n", call_result);
    free(call_result);
    // 计算结果应为 8
    if (response == NULL ||
        cJSON_GetObjectItem(response, "result")->valueint != 8) {
        ret = -1;
    }

    cJSON_Delete(response);
    for (int i = 0; i < 2; i++) {
        cJSON_Delete(params1[i]);
        cJSON_Delete(params2[i]);
    }

    return ret;
}
