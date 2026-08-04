/* SPEC: PJ2026-050106 REPL v0.4; transport-independent session API. */
#ifndef PIKA_REPL_H
#define PIKA_REPL_H

#include "pika_capability_config.h"

#if PIKA_REPL_ENABLE != 0 && PIKA_REPL_ENABLE != 1
#error "PIKA_REPL_ENABLE must be 0 or 1"
#endif

#if PIKA_REPL_ENABLE

#include <stddef.h>
#include <stdint.h>

#include "pika_frontend.h"
#include "pika_kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PIKA_REPL_MAX_LINE_BYTES
#define PIKA_REPL_MAX_LINE_BYTES 256u
#endif

#ifndef PIKA_REPL_MAX_BLOCK_BYTES
#define PIKA_REPL_MAX_BLOCK_BYTES 4096u
#endif

#ifndef PIKA_REPL_MAX_HISTORY_ENTRIES
#define PIKA_REPL_MAX_HISTORY_ENTRIES 16u
#endif

#ifndef PIKA_REPL_MAX_HISTORY_BYTES
#define PIKA_REPL_MAX_HISTORY_BYTES 1024u
#endif

#ifndef PIKA_REPL_MAX_CODE_BYTES
#define PIKA_REPL_MAX_CODE_BYTES 65536u
#endif

#ifndef PIKA_REPL_MAX_ESCAPE_BYTES
#define PIKA_REPL_MAX_ESCAPE_BYTES 8u
#endif

#ifndef PIKA_REPL_MAX_OUTPUT_BYTES_PER_COMMAND
#define PIKA_REPL_MAX_OUTPUT_BYTES_PER_COMMAND 4096u
#endif

#ifndef PIKA_REPL_MAX_TRACEBACK_FRAMES
#define PIKA_REPL_MAX_TRACEBACK_FRAMES 8u
#endif

#ifndef PIKA_REPL_INTERRUPT_POLL_INTERVAL
#define PIKA_REPL_INTERRUPT_POLL_INTERVAL 256u
#endif

#ifndef PIKA_REPL_MAX_CONSECUTIVE_IO_FAILURES
#define PIKA_REPL_MAX_CONSECUTIVE_IO_FAILURES 3u
#endif

#if PIKA_REPL_MAX_LINE_BYTES == 0u ||                              \
    PIKA_REPL_MAX_BLOCK_BYTES == 0u ||                             \
    PIKA_REPL_MAX_HISTORY_ENTRIES == 0u ||                         \
    PIKA_REPL_MAX_HISTORY_BYTES == 0u ||                           \
    PIKA_REPL_MAX_CODE_BYTES == 0u ||                              \
    PIKA_REPL_MAX_ESCAPE_BYTES < 3u ||                             \
    PIKA_REPL_MAX_OUTPUT_BYTES_PER_COMMAND == 0u ||                \
    PIKA_REPL_MAX_TRACEBACK_FRAMES == 0u ||                        \
    PIKA_REPL_INTERRUPT_POLL_INTERVAL == 0u ||                     \
    PIKA_REPL_MAX_CONSECUTIVE_IO_FAILURES == 0u
#error "REPL limits must be greater than zero"
#endif

#if PIKA_REPL_AUTO_INDENT_ENABLE != 0 && \
    PIKA_REPL_AUTO_INDENT_ENABLE != 1
#error "PIKA_REPL_AUTO_INDENT_ENABLE must be 0 or 1"
#endif

typedef enum {
    PIKA_REPL_RESULT_CONSUMED = 0,
    PIKA_REPL_RESULT_NEED_MORE = 1,
    PIKA_REPL_RESULT_COMMAND_COMPLETE = 2,
    PIKA_REPL_RESULT_RECOVERABLE_ERROR = 3,
    PIKA_REPL_RESULT_ENDED = 4,
    PIKA_REPL_RESULT_FATAL_ERROR = 5,
} PikaReplResult;

typedef enum {
    PIKA_REPL_STATE_READY = 0,
    PIKA_REPL_STATE_EDITING = 1,
    PIKA_REPL_STATE_CONTINUATION = 2,
    PIKA_REPL_STATE_EXECUTING = 3,
    PIKA_REPL_STATE_IO_FAULT = 4,
    PIKA_REPL_STATE_ENDED = 5,
    PIKA_REPL_STATE_FATAL = 6,
} PikaReplState;

typedef enum {
    PIKA_REPL_ERROR_NONE = 0,
    PIKA_REPL_ERROR_INPUT = 1,
    PIKA_REPL_ERROR_RESOURCE = 2,
    PIKA_REPL_ERROR_COMPILE = 3,
    PIKA_REPL_ERROR_RUNTIME = 4,
    PIKA_REPL_ERROR_OUTPUT = 5,
    PIKA_REPL_ERROR_STATE = 6,
} PikaReplError;

typedef enum {
    PIKA_REPL_STREAM_BYTES = 0,
    PIKA_REPL_STREAM_END = 1,
    PIKA_REPL_STREAM_INTERRUPT = 2,
} PikaReplStreamEvent;

typedef struct {
    PikaReplError error;
    PikaStatus status;
    const char* macro_name;
    uint32_t configured;
    uint32_t observed;
    uint32_t offset;
    uint8_t recoverable;
} PikaReplDiagnostic;

typedef struct {
    uint8_t* line;
    uint32_t line_capacity;
    uint8_t* block;
    uint32_t block_capacity;
    uint8_t* escape;
    uint32_t escape_capacity;
    uint8_t* history;
    uint32_t history_capacity;
    uint32_t* history_offsets;
    uint32_t* history_lengths;
    uint16_t history_entry_capacity;
} PikaReplBuffers;

typedef PikaStatus (*PikaReplEnsureStorage)(
    void* context,
    const PikaProgramRequirements* requirements);

typedef void (*PikaReplReleaseIdleStorage)(
    void* context,
    const PikaExecutionState* execution);

typedef struct {
    PikaFrontendConfig frontend;
    const PikaModuleProvider* modules;
    PikaStorage* storage;
    PikaBindingStorage* binding_storage;
    PikaReplEnsureStorage ensure_storage;
    void* ensure_storage_context;
    PikaReplReleaseIdleStorage release_idle_storage;
    void* release_idle_storage_context;
    PikaCompiledModule* initial_module;
    const PikaExecutionState* initial_execution;
    PikaReplBuffers buffers;
    uint32_t output_byte_limit;
} PikaReplConfig;

typedef struct {
    PikaOutputWrite write;
    void* context;
} PikaReplIo;

typedef struct {
    PikaReplConfig config;
    PikaReplIo io;
    PikaCompiledModule* module;
    PikaExecutionState execution;
    PikaResult result;
    PikaMetrics metrics;
    PikaReplDiagnostic diagnostic;
    uint32_t line_length;
    uint32_t cursor;
    uint32_t block_length;
    uint32_t escape_length;
    uint32_t history_used;
    uint32_t history_draft_length;
    uint32_t output_count;
    uint32_t input_offset;
    uint16_t auto_indent_replay_remaining;
    uint16_t auto_indent_width;
    uint16_t history_count;
    uint16_t history_position;
    uint16_t consecutive_io_failures;
    uint8_t escape_state;
    uint8_t last_input_was_cr;
    uint8_t last_output_was_cr;
    uint8_t initialized;
    uint8_t auto_indent_accepted;
    PikaReplState state;
} PikaReplSession;

PikaReplResult pika_repl_session_init(
    PikaReplSession* session,
    const PikaReplConfig* config,
    const PikaReplIo* io);
PikaReplResult pika_repl_feed(
    PikaReplSession* session,
    const uint8_t* bytes,
    size_t length);
PikaReplResult pika_repl_end_input(PikaReplSession* session);
PikaReplResult pika_repl_request_interrupt(
    PikaReplSession* session);
PikaReplResult pika_repl_stream_accept(
    PikaReplSession* session,
    PikaReplStreamEvent event,
    const uint8_t* bytes,
    size_t length);
PikaReplResult pika_repl_session_reset(
    PikaReplSession* session);
void pika_repl_session_deinit(PikaReplSession* session);
PikaReplState pika_repl_session_state(
    const PikaReplSession* session);
const PikaReplDiagnostic* pika_repl_last_diagnostic(
    const PikaReplSession* session);

#ifdef __cplusplus
}
#endif

#endif

#endif
