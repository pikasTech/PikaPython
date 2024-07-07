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

// Function pointers for memory management
static void* (*port_mem_malloc)(size_t size) = malloc;
static void (*port_mem_free)(void* ptr) = free;
static int (*port_vprintf)(const char* format, va_list args) = vprintf;

static void jrpc_debug(const char* format, ...) {
    va_list args;
    va_start(args, format);
    if (port_vprintf) {
        port_vprintf(format, args);
    }
    va_end(args);
}

// API to set memory management functions
void set_jrpc_memory_functions(void* (*malloc_func)(size_t),
                               void (*free_func)(void*)) {
    port_mem_malloc = malloc_func;
    port_mem_free = free_func;
    cJSON_Hooks cJson_hooks = {.free_fn = free_func, .malloc_fn = malloc_func};
    cJSON_InitHooks(&cJson_hooks);
}

void set_jrpc_vprintf_function(int (*vprintf_func)(const char*, va_list)) {
    port_vprintf = vprintf_func;
}

char* jrpc_strdup(const char* str) {
    size_t len = strlen(str) + 1;
    char* copy = (char*)port_mem_malloc(len);
    if (copy) {
        memcpy(copy, str, len);
    }
    return copy;
}

void* jrpc_malloc(size_t size) {
    return port_mem_malloc(size);
}

void jrpc_free(void* ptr) {
    port_mem_free(ptr);
}

/* private function */
static int JRPC_send_message_with_retry(JRPC* self,
                                        const char* request_str,
                                        int retry_count,
                                        unsigned long ack_timeout,
                                        int id,
                                        int type,
                                        const char* label);

static void JRPC_send_acknowledgement(JRPC* self,
                                      int id,
                                      ack_status status,
                                      const char* label);

static const char* JRPC_type_2_string(int type) {
    switch (type) {
        case TYPE_REQUEST:
            return STR_TYPE_REQUEST;
        case TYPE_ACK:
            return STR_TYPE_ACK;
        case TYPE_RESULT:
            return STR_TYPE_RESULT;
        default:
            return "UNKNOWN";
    }
}

// Example function: add
static cJSON* add(cJSON* params[], int param_count) {
    int a = params[0]->valueint;
    int b = params[1]->valueint;
    int sum = a + b;
    cJSON* result = cJSON_CreateNumber(sum);
    return result;
}

// Example non-blocking function: add_nonblocking
static int add_nonblocking(int id,
                           cJSON* params[],
                           int param_count,
                           JRPC* self) {
    int a = params[0]->valueint;
    int b = params[1]->valueint;
    int sum = a + b;
    cJSON* result = cJSON_CreateNumber(sum);
    JRPC_send_response(self, id, result);
    return 0;
}

// Example function: subtract
static cJSON* subtract(cJSON* params[], int param_count) {
    int a = params[0]->valueint;
    int b = params[1]->valueint;
    int difference = a - b;
    cJSON* result = cJSON_CreateNumber(difference);
    return result;
}

static rpc_mapping default_rpc_map[] = {{"add", add, 2},
                                        {"subtract", subtract, 2},
                                        RPC_MAP_END};

static rpc_mapping_nonblocking default_nonblocking_rpc_map[] = {
    {"add_nonblocking", add_nonblocking, 2},
    RPC_MAP_END};

// Function to create an acknowledgement string
char* create_acknowledgement_string(int id, ack_status status) {
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
                            TYPE_ACK);  // Add type field

    char* response_str = cJSON_Print(response);
    cJSON_Delete(response);
    return response_str;
}

// Function to send an acknowledgement
static void JRPC_send_acknowledgement(JRPC* self,
                                      int id,
                                      ack_status status,
                                      const char* label) {
    char* response_str = create_acknowledgement_string(id, status);
    jrpc_debug("[%s] ACK: %s\n", label, response_str);

    self->send(response_str);

    jrpc_free(response_str);
}

void JRPC_send_response(JRPC* self, int id, cJSON* result) {
    cJSON* response = cJSON_CreateObject();
    cJSON_AddStringToObject(response, STR_JSON_RPC_FIELD, STR_JSON_RPC_VERSION);
    cJSON_AddItemToObject(response, STR_RESULT_FIELD, result);
    cJSON_AddNumberToObject(response, STR_ID_FIELD, id);
    cJSON_AddNumberToObject(response, STR_TYPE_FIELD,
                            TYPE_RESULT);  // Add type field

    char* response_str = cJSON_Print(response);
    jrpc_debug("[Server] Response: %s\n", response_str);

    self->send(response_str);

    jrpc_free(response_str);
    cJSON_Delete(response);
}

void JRPC_server_handle_string(JRPC* self, char* json_str) {
    cJSON* json = cJSON_Parse(json_str);
    if (json == NULL) {
        jrpc_debug("Error parsing JSON\n");
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
        jrpc_debug("[Server] Invalid JSON RPC request format: %s\n", json_str);
        cJSON_Delete(json);
        return;
    }

    if (strcmp(jsonrpc->valuestring, STR_JSON_RPC_VERSION) != 0) {
        jrpc_debug("Unsupported JSON RPC version: %s\n", jsonrpc->valuestring);
        cJSON_Delete(json);
        return;
    }

    if (type->valueint != TYPE_REQUEST) {
        jrpc_debug("Invalid JSON RPC message type\n");
        cJSON_Delete(json);
        return;
    }

    int expected_param_count;
    rpc_function_nonblocking func_nonblocking =
        JRPC_find_nonblocking_rpc_function(self, method->valuestring,
                                           &expected_param_count);
    rpc_function func = NULL;
    int is_nonblocking = (func_nonblocking != NULL);

    if (!is_nonblocking) {
        func = JRPC_find_rpc_function(self, method->valuestring,
                                      &expected_param_count);
        if (func == NULL) {
            JRPC_send_acknowledgement(self, id->valueint, ACK_METHOD_NOT_FOUND,
                                      "Server");
            cJSON_Delete(json);
            return;
        }
    }

    int param_count = cJSON_GetArraySize(params);
    if (expected_param_count != PARAM_COUNT_NO_CHECK &&
        param_count != expected_param_count) {
        JRPC_send_acknowledgement(self, id->valueint, ACK_INVALID_PARAMS,
                                  "Server");
        cJSON_Delete(json);
        return;
    }

    // Try to get ACK from client to start task
    char* ack_str = create_acknowledgement_string(id->valueint, ACK_SUCCESS);
#if JRPC_USING_DOUBLE_ACK
    JRPC_send_message_with_retry(self, ack_str, 1, ACK_TIMEOUT, id->valueint,
                                 TYPE_ACK, "Server");

#else
    self->send(ack_str);
#endif
    if (ack_str) {
        jrpc_free(ack_str);
    }

    cJSON** param_array = (cJSON**)jrpc_malloc(param_count * sizeof(cJSON*));
    if (param_array == NULL) {
        jrpc_debug("Memory allocation failed for param_array\n");
        JRPC_send_acknowledgement(self, id->valueint, ACK_MEMORY_ERROR,
                                  "Server");
        cJSON_Delete(json);
        return;
    }

    for (int i = 0; i < param_count; i++) {
        param_array[i] = cJSON_GetArrayItem(params, i);
    }

    if (is_nonblocking) {
        func_nonblocking(id->valueint, param_array, param_count, self);
    } else {
        cJSON* result = func(param_array, param_count);
        JRPC_send_response(self, id->valueint, result);
    }

    jrpc_free(param_array);
    cJSON_Delete(json);
}

void JRPC_server_handle(JRPC* self) {
    char* json_str = self->receive();
    if (NULL != json_str) {
        JRPC_server_handle_string(self, json_str);
    }
    if (self->receive_need_free) {
        jrpc_free(json_str);
    }
}

rpc_function JRPC_find_rpc_function(JRPC* self,
                                    const char* name,
                                    int* param_count) {
    if (NULL == self->map) {
        return NULL;
    }
    for (int i = 0; self->map[i].name != NULL; i++) {
        if (strcmp(self->map[i].name, name) == 0) {
            *param_count = self->map[i].param_count;
            return self->map[i].func;
        }
    }
    return NULL;  // Function not found
}

rpc_function_nonblocking JRPC_find_nonblocking_rpc_function(JRPC* self,
                                                            const char* name,
                                                            int* param_count) {
    if (NULL == self->nonblocking_map) {
        return NULL;
    }
    for (int i = 0; self->nonblocking_map[i].name != NULL; i++) {
        if (strcmp(self->nonblocking_map[i].name, name) == 0) {
            *param_count = self->nonblocking_map[i].param_count;
            return self->nonblocking_map[i].func;
        }
    }
    return NULL;  // Function not found
}

// Cache add function
void JRPC_cache_add(JRPC* self, cJSON* item) {
    if (self->cache_count < CACHE_SIZE) {
        self->cache[self->cache_count++] = item;
    } else {
        // Cache full, delete oldest
        cJSON_Delete(self->cache[0]);
        for (int i = 0; i < CACHE_SIZE - 1; i++) {
            self->cache[i] = self->cache[i + 1];
        }
        self->cache[CACHE_SIZE - 1] = item;
    }
}

// Cache get function
cJSON* JRPC_cache_get(JRPC* self, int id, int type) {
    for (int i = 0; i < self->cache_count; i++) {
        cJSON* cached_json = self->cache[i];
        cJSON* cached_id = cJSON_GetObjectItem(cached_json, STR_ID_FIELD);
        cJSON* cached_type = cJSON_GetObjectItem(cached_json, STR_TYPE_FIELD);
        if (cached_id && cJSON_IsNumber(cached_id) &&
            cached_id->valueint == id && cached_type &&
            cJSON_IsNumber(cached_type) && cached_type->valueint == type) {
            // Found match, return and remove from cache
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
    // Check cache first
    cJSON* cached_json = JRPC_cache_get(self, id, type);
    if (cached_json != NULL) {
        return cached_json;
    }

    // No match in cache, receive from interface
    char* received_str = self->receive();
    if (received_str != NULL) {
        cJSON* received_json = cJSON_Parse(received_str);
        if (self->receive_need_free) {
            jrpc_free(received_str);
        }
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
                // Cache data
                JRPC_cache_add(self, received_json);
            }
        }
    }
    return NULL;
}

static int JRPC_send_message_with_retry(JRPC* self,
                                        const char* request_str,
                                        int retry_count,
                                        unsigned long ack_timeout,
                                        int id,
                                        int type,
                                        const char* label) {
    for (int retry = 0; retry < retry_count; retry++) {
        jrpc_debug("[%s] Send and await %s with retry [%d]: %s\n", label,
                   JRPC_type_2_string(type), retry, request_str);
        self->send(request_str);

        unsigned long start_time = self->tick();
        cJSON* ack_json = NULL;
        while (1) {
            ack_json = JRPC_receive_with_id_and_type(self, id, type);
            if (ack_json != NULL) {
                jrpc_debug("[%s] Received ACK, id: %d\n", label, id);
                cJSON_Delete(ack_json);
                return 0;  // Received correct ACK
            }
            if (self->tick() - start_time >= ack_timeout) {
                jrpc_debug("[%s] ACK timeout, retrying...\n", label);
                break;
            }
            self->yield();  // Thread switch
        }
    }
    jrpc_debug("[%s] Failed to receive ACK after %d retries\n", label,
               retry_count);
    return -1;  // Failed to receive correct ACK after retries
}

void JRPC_send_request_no_blocking(JRPC* self,
                                   const char* method,
                                   cJSON* params[],
                                   int param_count,
                                   rpc_callback callback) {
    // Build request
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
                            TYPE_REQUEST);  // Add type field

    char* request_str = cJSON_Print(request);
    jrpc_debug("[Client] Sending Request (no_blocking): %s\n", request_str);

    if (JRPC_send_message_with_retry(self, request_str, RETRY_COUNT,
                                     ACK_TIMEOUT, id, TYPE_ACK,
                                     "Client") != 0) {
        // If ACK received
        callback(NULL);  // Simulate callback, no result
    }

    JRPC_send_acknowledgement(self, id, ACK_SUCCESS, "Client");

    jrpc_free(request_str);
    cJSON_Delete(request);
}

cJSON* JRPC_send_request_blocking(JRPC* self,
                                  const char* method,
                                  cJSON* params[],
                                  int param_count) {
    // Build request
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
                            TYPE_REQUEST);  // Add type field

    char* request_str = cJSON_Print(request);
    jrpc_debug("[Client] Sending Request (blocking): %s\n", request_str);

    if (JRPC_send_message_with_retry(self, request_str, RETRY_COUNT,
                                     ACK_TIMEOUT, id, TYPE_ACK,
                                     "Client") != 0) {
        jrpc_free(request_str);
        cJSON_Delete(request);
        return NULL;
    }
    JRPC_send_acknowledgement(self, id, ACK_SUCCESS, "Client");
    // Wait for response
    unsigned long start_time = self->tick();
    while (1) {
        cJSON* response_json =
            JRPC_receive_with_id_and_type(self, id, TYPE_RESULT);
        if (response_json != NULL) {
            cJSON_Delete(request);
            jrpc_free(request_str);
            char* response_str = cJSON_Print(response_json);
            jrpc_debug("[Client] Received Response: %s\n", response_str);
            jrpc_free(response_str);
            return response_json;
        }
        if (self->tick() - start_time >= BLOCKING_TIMEOUT) {
            jrpc_debug("[Client] Response timeout\n");
            jrpc_free(request_str);
            cJSON_Delete(request);
            return NULL;
        }
        self->yield();  // Thread switch
    }
}

// Mock send function with validation
static char* mock_sent_message = NULL;

static void mock_send(const char* message) {
    jrpc_debug("[Mock] send: %s\n", message);
    if (mock_sent_message) {
        jrpc_free(mock_sent_message);
    }
    mock_sent_message = jrpc_strdup(message);  // Capture sent message
}

// Mock receive function (non-blocking)
static char* mock_receive(void) {
    static int call_count = 0;
    call_count++;
    switch (call_count) {
        case 3:
            return jrpc_strdup(
                "{\"jsonrpc\": \"1.0\", \"status\": \"received\", \"id\": 1, "
                "\"type\": 1}");
        case 6:
            return jrpc_strdup(
                "{\"jsonrpc\": \"1.0\", \"status\": \"received\", \"id\": 2, "
                "\"type\": 1}");
        case 9:
            return jrpc_strdup(
                "{\"jsonrpc\": \"1.0\", \"result\": 8, \"id\": 2, \"type\": "
                "2}");
        default:
            return NULL;
    }
}

static char* mock_receive_server_test(void) {
    static int call_count = 0;
    call_count++;
    switch (call_count) {
        case 3:
            return jrpc_strdup(
                "{\"jsonrpc\": \"1.0\", \"status\": \"received\", \"id\": 1, "
                "\"type\": 1}");
        case 6:
            return jrpc_strdup(
                "{\"jsonrpc\": \"1.0\", \"status\": \"received\", \"id\": 2, "
                "\"type\": 1}");
        case 9:
            return jrpc_strdup(
                "{\"jsonrpc\": \"1.0\", \"status\": \"received\", \"id\": 3, "
                "\"type\": 1}");
        default:
            return NULL;
    }
}

// Mock yield function
static void mock_yield(void) {
    jrpc_debug("[Y]");
}

// Mock tick function
static unsigned long mock_tick_ms(void) {
    static unsigned long tick = 0;
    tick += 100;  // Simulate 100ms per call
    return tick;
}

static void result_callback(cJSON* result) {
    jrpc_debug("Callback executed. Result: %s\n",
               result ? cJSON_Print(result) : "No result");
}

int jrpc_test_client() {
    int ret = 0;
    JRPC jrpc = {0};
    JRPC_init(&jrpc, default_rpc_map, default_nonblocking_rpc_map, mock_send,
              mock_receive, 1, mock_yield, mock_tick_ms);

    // Test no_blocking
    cJSON* params1[] = {cJSON_CreateNumber(5), cJSON_CreateNumber(3)};
    JRPC_send_request_no_blocking(&jrpc, "add_nonblocking", params1, 2,
                                  result_callback);

    // Test blocking
    cJSON* params2[] = {cJSON_CreateNumber(5), cJSON_CreateNumber(3)};
    cJSON* response = JRPC_send_request_blocking(&jrpc, "add", params2, 2);
    char* call_result = cJSON_Print(response);
    jrpc_debug("[Client] Blocking call result: %s\n", call_result);
    jrpc_free(call_result);
    // Result should be 8
    if (response == NULL ||
        cJSON_GetObjectItem(response, "result")->valueint != 8) {
        ret = -1;
    }

    cJSON_Delete(response);
    for (int i = 0; i < 2; i++) {
        cJSON_Delete(params1[i]);
        cJSON_Delete(params2[i]);
    }

    if (mock_sent_message) {
        jrpc_free(mock_sent_message);
        mock_sent_message = NULL;
    }

    return ret;
}

int jrpc_compare_json_strings(const char* json_str1, const char* json_str2) {
    cJSON* json1 = cJSON_Parse(json_str1);
    cJSON* json2 = cJSON_Parse(json_str2);

    if (json1 == NULL || json2 == NULL) {
        if (json1) {
            cJSON_Delete(json1);
        } else {
            jrpc_debug("json1 is NULL\n");
        }
        if (json2) {
            cJSON_Delete(json2);
        } else {
            jrpc_debug("json2 is NULL\n");
        }
        return -1;
    }

    int result = cJSON_Compare(json1, json2, 1) ? 0 : -1;
    if (0 != result) {
        jrpc_debug("Json compare failed\n");
        jrpc_debug("json1: %s\n", json_str1);
        jrpc_debug("json2: %s\n", json_str2);
    }

    cJSON_Delete(json1);
    cJSON_Delete(json2);

    return result;
}

int jrpc_validate_response(const char* expected_response) {
    if (mock_sent_message == NULL) {
        return -1;
    }

    return jrpc_compare_json_strings(mock_sent_message, expected_response);
}

int jrpc_test_server() {
    JRPC jrpc = {0};
    JRPC_init(&jrpc, default_rpc_map, default_nonblocking_rpc_map, mock_send,
              mock_receive_server_test, 1, mock_yield, mock_tick_ms);

    const char* requests[] = {
        "{\"jsonrpc\": \"1.0\", \"method\": \"add\", \"params\": [5, 3], "
        "\"id\": 1, \"type\": 0}",
        "{\"jsonrpc\": \"1.0\", \"method\": \"subtract\", \"params\": [10, 4], "
        "\"id\": 2, \"type\": 0}",
        "{\"jsonrpc\": \"1.0\", \"method\": \"add_nonblocking\", \"params\": "
        "[2, 2], \"id\": 3, \"type\": 0}",
    };

    const char* expected_responses[] = {
        "{\"jsonrpc\": \"1.0\", \"result\": 8, \"id\": 1, \"type\": 2}",
        "{\"jsonrpc\": \"1.0\", \"result\": 6, \"id\": 2, \"type\": 2}",
        "{\"jsonrpc\": \"1.0\", \"result\": 4, \"id\": 3, \"type\": 2}"};

    int ret = 0;

    for (int i = 0; i < sizeof(requests) / sizeof(requests[0]); i++) {
        JRPC_server_handle_string(&jrpc, (char*)requests[i]);
        if (jrpc_validate_response(expected_responses[i]) != 0) {
            ret = -1;
            break;
        }
    }

    JRPC_deinit(&jrpc);

    if (mock_sent_message) {
        jrpc_free(mock_sent_message);
        mock_sent_message = NULL;
    }

    return ret;
}

char* jrpc_strtok(char* str, const char* delimiters, char** context) {
    char* start = str ? str : *context;
    if (!start)
        return NULL;

    while (*start && strchr(delimiters, *start))
        ++start;
    if (!*start)
        return NULL;

    char* end = start;
    while (*end && !strchr(delimiters, *end))
        ++end;

    if (*end) {
        *end = '\0';
        *context = end + 1;
    } else {
        *context = NULL;
    }

    return start;
}

char* JRPC_cmd(JRPC* jrpc, const char* cmd) {
    char* cmd_copy = jrpc_strdup(cmd);
    char* context = NULL;

    char* token = jrpc_strtok(cmd_copy, " ", &context);
    if (token == NULL) {
        jrpc_debug("Invalid command\n");
        jrpc_free(cmd_copy);
        return NULL;
    }

    char* method = jrpc_strdup(token);

    cJSON* params_array[10];
    int param_count = 0;
    while ((token = jrpc_strtok(NULL, " ", &context)) != NULL) {
        int param_value = atoi(token);
        params_array[param_count] = cJSON_CreateNumber(param_value);
        param_count++;
    }

    if (param_count == 0) {
        jrpc_debug("No parameters provided\n");
        jrpc_free(method);
        jrpc_free(cmd_copy);
        return NULL;
    }

    cJSON* result =
        JRPC_send_request_blocking(jrpc, method, params_array, param_count);

    if (result == NULL) {
        jrpc_debug("No result\n", NULL);
        jrpc_free(method);
        jrpc_free(cmd_copy);
        return NULL;
    }

    cJSON* result_data = cJSON_GetObjectItem(result, "result");
    if (NULL == result_data) {
        jrpc_debug("No result Item\n", NULL);
        jrpc_free(method);
        jrpc_free(cmd_copy);
        cJSON_Delete(result);
        return NULL;
    }

    char* result_str = NULL;
    if (result_data) {
        result_str = cJSON_Print(result_data);
        // jrpc_debug("%s\n", result_str);
        cJSON_Delete(result);
    }
    for (int i = 0; i < param_count; i++) {
        cJSON_Delete(params_array[i]);
    }
    jrpc_free(method);
    jrpc_free(cmd_copy);
    return result_str;
}

void JRPC_init(JRPC* jrpc,
               rpc_mapping* rpc_map,
               rpc_mapping_nonblocking* nonblocking_rpc_map,
               void (*send_func)(const char* message),
               char* (*receive_func)(void),
               int receive_need_free,
               void (*yield_func)(void),
               unsigned long (*tick_func)(void)) {
    jrpc->map = rpc_map;
    jrpc->nonblocking_map = nonblocking_rpc_map;
    jrpc->send = send_func;
    jrpc->receive = receive_func;
    jrpc->receive_need_free = receive_need_free;
    jrpc->yield = yield_func;
    jrpc->tick = tick_func;
    jrpc->current_id = 0;
    memset(jrpc->cache, 0, sizeof(jrpc->cache));
    jrpc->cache_count = 0;
}

void JRPC_deinit(JRPC* jrpc) {
    for (int i = 0; i < jrpc->cache_count; i++) {
        cJSON_Delete(jrpc->cache[i]);
    }
}
