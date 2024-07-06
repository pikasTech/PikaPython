#ifndef JRPC_H
#define JRPC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cJSON.h"
#include <stdint.h>
#include <stdarg.h>

// Define string constants
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

// Timeout definitions
#define ACK_TIMEOUT 1000
#define BLOCKING_TIMEOUT 20000
#define RETRY_COUNT 5

// Packet type definitions
#define TYPE_REQUEST 0
#define TYPE_ACK 1
#define TYPE_RESULT 2

#define STR_TYPE_REQUEST "REQ"
#define STR_TYPE_ACK "ACK"
#define STR_TYPE_RESULT "RES"

#define STR_TYPE_FIELD "type"

// Cache array size
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
    int param_count;  // Number of parameters
} rpc_mapping;

typedef struct {
    const char* name;
    rpc_function_nonblocking func;
    int param_count;  // Number of parameters
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
    uint8_t receive_need_free;
    yield_function yield;
    tick_function tick;
    int current_id;
    cJSON* cache[CACHE_SIZE];  // Add cache
    int cache_count;
};

// Function declarations
void JRPC_server_handle(JRPC* self, const char* json_str);
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
int jrpc_compare_json_strings(const char* json_str1, const char* json_str2);
int jrpc_validate_response(const char* expected_response);
void set_jrpc_memory_functions(void* (*malloc_func)(size_t),
                               void (*free_func)(void*));
void set_jrpc_vprintf_function(int (*vprintf_func)(const char*, va_list));
int jrpc_test_client();
int jrpc_test_server();
char* jrpc_cmd(JRPC* jrpc, const char* cmd);

#ifdef __cplusplus
}
#endif

#endif  // JRPC_H
