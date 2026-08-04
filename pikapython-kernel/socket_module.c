/* SPEC: PJ2026-050109 network-ready v0.1; socket binding callbacks. */
#include "_socket_binding.h"
#include "platform_socket.h"

#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#if PIKA_SOCKET_RECEIVE_BYTE_LIMIT > PIKA_BINDING_RESULT_BYTE_LIMIT
#error "PIKA_SOCKET_RECEIVE_BYTE_LIMIT exceeds PIKA_BINDING_RESULT_BYTE_LIMIT"
#endif

typedef struct {
    PikaPlatformSocket endpoint;
    char peer_host[PIKA_SOCKET_ADDRESS_BYTE_LIMIT];
    uint16_t peer_port;
    uint8_t active;
} PikaSocketObject;

static PikaSocketObject socket_objects[PIKA_SOCKET_OBJECT_LIMIT];
static uint8_t socket_receive_buffer[PIKA_SOCKET_RECEIVE_BYTE_LIMIT];
static char socket_address_buffer[PIKA_SOCKET_ADDRESS_BYTE_LIMIT];

static PikaStatus validate_call(const PikaBindingCall* call,
                                PikaBindingValue* result,
                                uint8_t argument_count) {
    if (call == NULL || result == NULL ||
        call->argument_count != argument_count ||
        (argument_count > 0u && call->arguments == NULL)) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    return PIKA_STATUS_OK;
}

static PikaStatus socket_status(PikaSocketStatus status) {
    switch (status) {
        case PIKA_SOCKET_OK:
            return PIKA_STATUS_OK;
        case PIKA_SOCKET_INVALID_ARGUMENT:
            return PIKA_STATUS_VALUE_ERROR;
        case PIKA_SOCKET_CLOSED:
            return PIKA_STATUS_SOCKET_CLOSED;
        case PIKA_SOCKET_WOULD_BLOCK:
            return PIKA_STATUS_SOCKET_WOULD_BLOCK;
        case PIKA_SOCKET_TIMEOUT:
            return PIKA_STATUS_SOCKET_TIMEOUT;
        case PIKA_SOCKET_DNS_ERROR:
            return PIKA_STATUS_SOCKET_DNS_ERROR;
        case PIKA_SOCKET_CONNECTION_REFUSED:
            return PIKA_STATUS_SOCKET_CONNECTION_REFUSED;
        case PIKA_SOCKET_CONNECTION_RESET:
            return PIKA_STATUS_SOCKET_CONNECTION_RESET;
        case PIKA_SOCKET_IO_ERROR:
            return PIKA_STATUS_OS_ERROR;
        default:
            return PIKA_STATUS_INTERNAL_ERROR;
    }
}

static void set_none(PikaBindingValue* result) {
    result->kind = PIKA_BINDING_VALUE_NONE;
}

static PikaSocketObject* allocate_socket_object(void) {
    uint32_t index;
    for (index = 0u; index < PIKA_SOCKET_OBJECT_LIMIT; ++index) {
        PikaSocketObject* object = &socket_objects[index];
        if (object->active == 0u) {
            memset(object, 0, sizeof(*object));
            pika_platform_socket_initialize(&object->endpoint);
            object->active = 1u;
            return object;
        }
    }
    return NULL;
}

static PikaSocketObject* find_socket_object(void* instance) {
    uint32_t index;
    for (index = 0u; index < PIKA_SOCKET_OBJECT_LIMIT; ++index) {
        if (socket_objects[index].active != 0u &&
            instance == &socket_objects[index]) {
            return &socket_objects[index];
        }
    }
    return NULL;
}

static void close_socket_object(PikaSocketObject* object) {
    if (object == NULL || object->active == 0u) {
        return;
    }
    (void)pika_platform_socket_close(&object->endpoint);
}

static void release_socket_object(PikaSocketObject* object) {
    if (object == NULL || object->active == 0u) {
        return;
    }
    close_socket_object(object);
    memset(object, 0, sizeof(*object));
}

static void reset_socket_objects(void) {
    uint32_t index;
    for (index = 0u; index < PIKA_SOCKET_OBJECT_LIMIT; ++index) {
        release_socket_object(&socket_objects[index]);
    }
    memset(socket_receive_buffer, 0, sizeof(socket_receive_buffer));
    memset(socket_address_buffer, 0, sizeof(socket_address_buffer));
}

static PikaStatus copy_host(const PikaBindingValue* value,
                            char* destination,
                            uint32_t capacity) {
    uint32_t length;
    if (value == NULL || destination == NULL || capacity == 0u ||
        value->kind != PIKA_BINDING_VALUE_STRING) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    length = value->as.buffer.length;
    if (length > PIKA_SOCKET_HOST_BYTE_LIMIT || length >= capacity) {
        return PIKA_STATUS_SOCKET_HOST_BYTE_LIMIT;
    }
    if ((length > 0u && value->as.buffer.data == NULL) ||
        (length > 0u &&
         memchr(value->as.buffer.data, '\0', length) != NULL)) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    if (length > 0u) {
        memcpy(destination, value->as.buffer.data, length);
    }
    destination[length] = '\0';
    return PIKA_STATUS_OK;
}

static PikaStatus read_port(const PikaBindingValue* value,
                            uint16_t* port) {
    if (value == NULL || port == NULL ||
        value->kind != PIKA_BINDING_VALUE_INTEGER ||
        value->as.integer < 0 || value->as.integer > UINT16_MAX) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    *port = (uint16_t)value->as.integer;
    return PIKA_STATUS_OK;
}

static PikaStatus require_socket(const PikaBindingCall* call,
                                 PikaSocketObject** object) {
    if (call == NULL || object == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    *object = find_socket_object(call->instance);
    return *object != NULL ? PIKA_STATUS_OK
                           : PIKA_STATUS_SOCKET_CLOSED;
}

static PikaStatus timeout_milliseconds(const PikaBindingValue* value,
                                       int32_t* milliseconds) {
    double seconds;
    double scaled;
    if (value == NULL || milliseconds == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (value->kind == PIKA_BINDING_VALUE_NONE) {
        *milliseconds = -1;
        return PIKA_STATUS_OK;
    }
    if (value->kind == PIKA_BINDING_VALUE_INTEGER) {
        if (value->as.integer < 0 ||
            value->as.integer > INT32_MAX / 1000) {
            return PIKA_STATUS_VALUE_ERROR;
        }
        *milliseconds = (int32_t)value->as.integer * 1000;
        return PIKA_STATUS_OK;
    }
    if (value->kind != PIKA_BINDING_VALUE_FLOAT) {
        return PIKA_STATUS_TYPE_ERROR;
    }
    seconds = value->as.floating;
    if (!isfinite(seconds) || seconds < 0.0 ||
        seconds > (double)INT32_MAX / 1000.0) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    scaled = seconds * 1000.0;
    *milliseconds = scaled > 0.0 && scaled < 1.0
                        ? 1
                        : (int32_t)scaled;
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding__socket_gethostbyname(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    char host[PIKA_SOCKET_HOST_BYTE_LIMIT + 1u];
    PikaStatus status = validate_call(call, result, 1u);
    if (status == PIKA_STATUS_OK) {
        status = copy_host(&call->arguments[0], host, sizeof(host));
    }
    if (status == PIKA_STATUS_OK) {
        status = socket_status(pika_platform_socket_resolve_ipv4(
            host, socket_address_buffer,
            (uint32_t)sizeof(socket_address_buffer)));
    }
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    result->kind = PIKA_BINDING_VALUE_STRING;
    result->as.buffer.data =
        (const uint8_t*)socket_address_buffer;
    result->as.buffer.length =
        (uint32_t)strlen(socket_address_buffer);
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding__socket_Socket_init(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaSocketObject* object;
    PikaSocketStatus open_status;
    PikaStatus status = validate_call(call, result, 3u);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    if (call->arguments[0].as.integer < INT_MIN ||
        call->arguments[0].as.integer > INT_MAX ||
        call->arguments[1].as.integer < INT_MIN ||
        call->arguments[1].as.integer > INT_MAX ||
        call->arguments[2].as.integer < INT_MIN ||
        call->arguments[2].as.integer > INT_MAX) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    object = allocate_socket_object();
    if (object == NULL) {
        return PIKA_STATUS_SOCKET_OBJECT_LIMIT;
    }
    open_status = pika_platform_socket_open(
        &object->endpoint,
        (int)call->arguments[0].as.integer,
        (int)call->arguments[1].as.integer,
        (int)call->arguments[2].as.integer);
    if (open_status != PIKA_SOCKET_OK) {
        release_socket_object(object);
        return socket_status(open_status);
    }
    result->kind = PIKA_BINDING_VALUE_OPAQUE;
    result->as.opaque = object;
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding__socket_Socket_bind(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaSocketObject* object;
    char host[PIKA_SOCKET_HOST_BYTE_LIMIT + 1u];
    uint16_t port;
    PikaStatus status = validate_call(call, result, 2u);
    if (status == PIKA_STATUS_OK) status = require_socket(call, &object);
    if (status == PIKA_STATUS_OK) {
        status = copy_host(&call->arguments[0], host, sizeof(host));
    }
    if (status == PIKA_STATUS_OK) {
        status = read_port(&call->arguments[1], &port);
    }
    if (status == PIKA_STATUS_OK) {
        status = socket_status(
            pika_platform_socket_bind(&object->endpoint, host, port));
    }
    if (status == PIKA_STATUS_OK) set_none(result);
    return status;
}

PikaStatus pika_binding__socket_Socket_listen(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaSocketObject* object;
    PikaStatus status = validate_call(call, result, 1u);
    if (status == PIKA_STATUS_OK) status = require_socket(call, &object);
    if (status == PIKA_STATUS_OK &&
        (call->arguments[0].as.integer < 0 ||
         call->arguments[0].as.integer > INT_MAX)) {
        status = PIKA_STATUS_VALUE_ERROR;
    }
    if (status == PIKA_STATUS_OK) {
        status = socket_status(pika_platform_socket_listen(
            &object->endpoint,
            (int)call->arguments[0].as.integer));
    }
    if (status == PIKA_STATUS_OK) set_none(result);
    return status;
}

PikaStatus pika_binding__socket_Socket_accept(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaSocketObject* server;
    PikaSocketObject* accepted;
    PikaSocketStatus accept_status;
    PikaStatus status = validate_call(call, result, 0u);
    if (status == PIKA_STATUS_OK) status = require_socket(call, &server);
    if (status != PIKA_STATUS_OK) return status;
    accepted = allocate_socket_object();
    if (accepted == NULL) return PIKA_STATUS_SOCKET_OBJECT_LIMIT;
    accept_status = pika_platform_socket_accept(
        &server->endpoint, &accepted->endpoint,
        accepted->peer_host,
        (uint32_t)sizeof(accepted->peer_host),
        &accepted->peer_port);
    if (accept_status != PIKA_SOCKET_OK) {
        release_socket_object(accepted);
        return socket_status(accept_status);
    }
    result->kind = PIKA_BINDING_VALUE_OBJECT;
    result->as.opaque = accepted;
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding__socket_Socket_peer_host(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaSocketObject* object;
    PikaStatus status = validate_call(call, result, 0u);
    if (status == PIKA_STATUS_OK) status = require_socket(call, &object);
    if (status != PIKA_STATUS_OK) return status;
    result->kind = PIKA_BINDING_VALUE_STRING;
    result->as.buffer.data = (const uint8_t*)object->peer_host;
    result->as.buffer.length = (uint32_t)strlen(object->peer_host);
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding__socket_Socket_peer_port(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaSocketObject* object;
    PikaStatus status = validate_call(call, result, 0u);
    if (status == PIKA_STATUS_OK) status = require_socket(call, &object);
    if (status != PIKA_STATUS_OK) return status;
    result->kind = PIKA_BINDING_VALUE_INTEGER;
    result->as.integer = object->peer_port;
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding__socket_Socket_connect(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaSocketObject* object;
    char host[PIKA_SOCKET_HOST_BYTE_LIMIT + 1u];
    uint16_t port;
    PikaStatus status = validate_call(call, result, 2u);
    if (status == PIKA_STATUS_OK) status = require_socket(call, &object);
    if (status == PIKA_STATUS_OK) {
        status = copy_host(&call->arguments[0], host, sizeof(host));
    }
    if (status == PIKA_STATUS_OK) {
        status = read_port(&call->arguments[1], &port);
    }
    if (status == PIKA_STATUS_OK) {
        status = socket_status(pika_platform_socket_connect(
            &object->endpoint, host, port));
    }
    if (status == PIKA_STATUS_OK) set_none(result);
    return status;
}

PikaStatus pika_binding__socket_Socket_send(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaSocketObject* object;
    uint32_t sent = 0u;
    PikaStatus status = validate_call(call, result, 1u);
    if (status == PIKA_STATUS_OK) status = require_socket(call, &object);
    if (status == PIKA_STATUS_OK) {
        status = socket_status(pika_platform_socket_send(
            &object->endpoint, call->arguments[0].as.buffer.data,
            call->arguments[0].as.buffer.length, &sent));
    }
    if (status != PIKA_STATUS_OK) return status;
    result->kind = PIKA_BINDING_VALUE_INTEGER;
    result->as.integer = sent;
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding__socket_Socket_sendall(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaSocketObject* object;
    uint32_t offset = 0u;
    PikaStatus status = validate_call(call, result, 1u);
    if (status == PIKA_STATUS_OK) status = require_socket(call, &object);
    while (status == PIKA_STATUS_OK &&
           offset < call->arguments[0].as.buffer.length) {
        uint32_t sent = 0u;
        status = socket_status(pika_platform_socket_send(
            &object->endpoint,
            &call->arguments[0].as.buffer.data[offset],
            call->arguments[0].as.buffer.length - offset,
            &sent));
        if (status == PIKA_STATUS_OK && sent == 0u) {
            status = PIKA_STATUS_OS_ERROR;
        }
        offset += sent;
    }
    if (status == PIKA_STATUS_OK) set_none(result);
    return status;
}

PikaStatus pika_binding__socket_Socket_recv(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaSocketObject* object;
    uint32_t received = 0u;
    int64_t requested;
    PikaStatus status = validate_call(call, result, 1u);
    if (status == PIKA_STATUS_OK) status = require_socket(call, &object);
    requested = call->arguments[0].as.integer;
    if (status == PIKA_STATUS_OK && requested < 0) {
        status = PIKA_STATUS_VALUE_ERROR;
    }
    if (status == PIKA_STATUS_OK &&
        requested > PIKA_SOCKET_RECEIVE_BYTE_LIMIT) {
        status = PIKA_STATUS_SOCKET_RECEIVE_BYTE_LIMIT;
    }
    if (status == PIKA_STATUS_OK) {
        status = socket_status(pika_platform_socket_receive(
            &object->endpoint, socket_receive_buffer,
            (uint32_t)requested, &received));
    }
    if (status != PIKA_STATUS_OK) return status;
    result->kind = PIKA_BINDING_VALUE_BYTES;
    result->as.buffer.data = socket_receive_buffer;
    result->as.buffer.length = received;
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding__socket_Socket_setblocking(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaSocketObject* object;
    PikaStatus status = validate_call(call, result, 1u);
    if (status == PIKA_STATUS_OK) status = require_socket(call, &object);
    if (status == PIKA_STATUS_OK) {
        status = socket_status(pika_platform_socket_set_timeout(
            &object->endpoint,
            call->arguments[0].as.boolean != 0u ? -1 : 0));
    }
    if (status == PIKA_STATUS_OK) set_none(result);
    return status;
}

PikaStatus pika_binding__socket_Socket_settimeout(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaSocketObject* object;
    int32_t milliseconds = -1;
    PikaStatus status = validate_call(call, result, 1u);
    if (status == PIKA_STATUS_OK) status = require_socket(call, &object);
    if (status == PIKA_STATUS_OK) {
        status = timeout_milliseconds(
            &call->arguments[0], &milliseconds);
    }
    if (status == PIKA_STATUS_OK) {
        status = socket_status(pika_platform_socket_set_timeout(
            &object->endpoint, milliseconds));
    }
    if (status == PIKA_STATUS_OK) set_none(result);
    return status;
}

PikaStatus pika_binding__socket_Socket_close(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaSocketObject* object;
    PikaStatus status = validate_call(call, result, 0u);
    if (status != PIKA_STATUS_OK) return status;
    object = find_socket_object(call->instance);
    close_socket_object(object);
    set_none(result);
    return PIKA_STATUS_OK;
}

void pika_binding__socket_Socket_destroy(void* module_context,
                                         void* instance) {
    (void)module_context;
    release_socket_object(find_socket_object(instance));
}

PikaStatus pika_binding_register_network(
    PikaBindingRegistry* registry) {
    static const PikaBindingModule* modules[1];
    PikaStatus status;
    reset_socket_objects();
    status = pika_binding_registry_initialize(registry, modules, 1u);
    if (status != PIKA_STATUS_OK) return status;
    return pika_binding_registry_register(
        registry, pika_binding_module__socket());
}
