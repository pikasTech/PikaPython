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

// jrpc.h
#ifndef JRPC_H
#define JRPC_H

#include "cJSON.h"

// 宏定义字符串常量
#define STR_JSON_RPC_VERSION "1.0"
#define STR_JSON_RPC_FIELD "jsonrpc"
#define STR_METHOD_FIELD "method"
#define STR_PARAMS_FIELD "params"
#define STR_ID_FIELD "id"
#define STR_STATUS_FIELD "status"
#define STR_RESULT_FIELD "result"
#define STR_RECEIVED_STATUS "received"
#define STR_METHOD_NOT_FOUND_STATUS "method not found"
#define STR_INVALID_PARAMS_STATUS "invalid params"
#define STR_UNKNOWN_STATUS "unknown"

#define RPC_MAP_END \
    { NULL, NULL, 0 }
#define PARAM_COUNT_NO_CHECK -1

// 超时宏定义
#define ACK_TIMEOUT 1000
#define BLOCKING_TIMEOUT 5000
#define RETRY_COUNT 5

// 数据包类型的宏定义
#define TYPE_REQUEST 0
#define TYPE_ACK 1
#define TYPE_RESULT 2

#define STR_TYPE_FIELD "type"

// 缓存数组大小
#define CACHE_SIZE 16

typedef enum {
    ACK_SUCCESS,
    ACK_METHOD_NOT_FOUND,
    ACK_INVALID_PARAMS
} ack_status;

typedef struct JRPC_ JRPC;

typedef cJSON* (*rpc_function)(cJSON* params[], int param_count);
typedef int (*rpc_function_nonblocking)(int id,
                                        cJSON* params[],
                                        int param_count,
                                        JRPC* self);

typedef struct {
    const char* name;
    rpc_function func;
    int param_count;  // 参数数量
} rpc_mapping;

typedef struct {
    const char* name;
    rpc_function_nonblocking func;
    int param_count;  // 参数数量
} rpc_mapping_nonblocking;

typedef void (*rpc_callback)(cJSON* result);
typedef void (*send_function)(const char* message);
typedef char* (*receive_function)(void);
typedef void (*yield_function)(void);
typedef unsigned long (*tick_function)(void);

struct JRPC_ {
    rpc_mapping* map;
    rpc_mapping_nonblocking* nonblocking_map;
    send_function send;
    receive_function receive;
    yield_function yield;
    tick_function tick;
    int current_id;
    cJSON* cache[CACHE_SIZE];  // 添加缓存
    int cache_count;
};

// 函数声明
void JRPC_send_acknowledgement(JRPC* self, int id, ack_status status);
void JRPC_handle_request(JRPC* self, const char* json_str);
rpc_function JRPC_find_rpc_function(JRPC* self,
                                    const char* name,
                                    int* param_count);
rpc_function_nonblocking JRPC_find_nonblocking_rpc_function(JRPC* self,
                                                            const char* name,
                                                            int* param_count);
void JRPC_send_response(JRPC* self, int id, cJSON* result);
void JRPC_send_request_no_blocking(JRPC* self,
                                   const char* method,
                                   cJSON* params[],
                                   int param_count,
                                   rpc_callback callback);
cJSON* JRPC_send_request_blocking(JRPC* self,
                                  const char* method,
                                  cJSON* params[],
                                  int param_count);

cJSON* JRPC_receive_with_id_and_type(JRPC* self, int id, int type);

int jrpc_base_test();
#endif  // jrpc.h
