/* SPEC: PJ2026-050106 REPL v0.3; Linux terminal adapter. */
#if defined(__linux__) && !defined(_POSIX_C_SOURCE)
#define _POSIX_C_SOURCE 200809L
#endif

#include "pika_repl.h"

#ifndef PIKA_REPL_SHELL_MAIN_ENABLE
#define PIKA_REPL_SHELL_MAIN_ENABLE 1
#endif

#if !PIKA_REPL_ENABLE || !defined(__linux__) || \
    !PIKA_REPL_SHELL_MAIN_ENABLE
typedef unsigned char PikaReplDisabledTranslationUnit;
#endif

#if PIKA_REPL_ENABLE && defined(__linux__) && \
    PIKA_REPL_SHELL_MAIN_ENABLE

#include <errno.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#ifndef PIKA_REPL_SHELL_FRAME_BYTES
#define PIKA_REPL_SHELL_FRAME_BYTES 32768u
#endif

#ifndef PIKA_REPL_SHELL_VALUE_COUNT
#define PIKA_REPL_SHELL_VALUE_COUNT 2048u
#endif

#ifndef PIKA_REPL_SHELL_OBJECT_SLOTS
#define PIKA_REPL_SHELL_OBJECT_SLOTS 256u
#endif

#ifndef PIKA_REPL_SHELL_OBJECT_BYTES
#define PIKA_REPL_SHELL_OBJECT_BYTES 65536u
#endif

#define PIKA_REPL_SHELL_READ_BYTES 128u

typedef union {
    uint64_t alignment;
    uint8_t bytes[PIKA_REPL_SHELL_FRAME_BYTES];
} PikaReplShellFrames;

typedef struct {
    int output_fd;
    int terminal_active;
    struct termios original_terminal;
} PikaReplShellIo;

typedef struct {
    PikaReplSession session;
    PikaStorage storage;
    PikaObjectStorage objects;
    PikaReplShellFrames frames;
    int64_t values[PIKA_REPL_SHELL_VALUE_COUNT];
    PikaRuntimeValue typed_values[PIKA_REPL_SHELL_VALUE_COUNT];
    PikaObjectSlot object_slots[PIKA_REPL_SHELL_OBJECT_SLOTS];
    uint8_t object_bytes[PIKA_REPL_SHELL_OBJECT_BYTES];
    uint8_t line[PIKA_REPL_MAX_LINE_BYTES];
    uint8_t block[PIKA_REPL_MAX_BLOCK_BYTES];
    uint8_t escape[PIKA_REPL_MAX_ESCAPE_BYTES];
    uint8_t history[PIKA_REPL_MAX_HISTORY_BYTES];
    uint32_t history_offsets[PIKA_REPL_MAX_HISTORY_ENTRIES];
    uint32_t history_lengths[PIKA_REPL_MAX_HISTORY_ENTRIES];
} PikaReplShellMemory;

static PikaStatus repl_shell_write(
    void* context,
    const char* bytes,
    uint32_t length) {
    PikaReplShellIo* io = (PikaReplShellIo*)context;
    uint32_t offset = 0u;
    while (offset < length) {
        ssize_t written = write(
            io->output_fd, &bytes[offset],
            (size_t)(length - offset));
        if (written > 0) {
            offset += (uint32_t)written;
            continue;
        }
        if (written < 0 && errno == EINTR) {
            continue;
        }
        return PIKA_STATUS_OUTPUT_ERROR;
    }
    return PIKA_STATUS_OK;
}

static int repl_shell_enter_raw(PikaReplShellIo* io) {
    struct termios terminal;
    if (!isatty(STDIN_FILENO)) {
        return 1;
    }
    if (tcgetattr(STDIN_FILENO, &io->original_terminal) != 0) {
        return 0;
    }
    terminal = io->original_terminal;
    terminal.c_iflag &=
        (tcflag_t)~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    terminal.c_oflag &= (tcflag_t)~OPOST;
    terminal.c_cflag |= (tcflag_t)CS8;
    terminal.c_lflag &=
        (tcflag_t)~(ECHO | ICANON | IEXTEN | ISIG);
    terminal.c_cc[VMIN] = 1;
    terminal.c_cc[VTIME] = 0;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &terminal) != 0) {
        return 0;
    }
    io->terminal_active = 1;
    return 1;
}

static void repl_shell_restore(PikaReplShellIo* io) {
    if (io->terminal_active) {
        (void)tcsetattr(
            STDIN_FILENO, TCSAFLUSH,
            &io->original_terminal);
        io->terminal_active = 0;
    }
}

static void repl_shell_configure(
    PikaReplShellMemory* memory,
    PikaReplConfig* config) {
    memset(memory, 0, sizeof(*memory));
    memset(config, 0, sizeof(*config));
    memory->objects.slots = memory->object_slots;
    memory->objects.capacity = PIKA_REPL_SHELL_OBJECT_SLOTS;
    memory->objects.arena = memory->object_bytes;
    memory->objects.arena_capacity =
        PIKA_REPL_SHELL_OBJECT_BYTES;
    memory->storage.frames = memory->frames.bytes;
    memory->storage.frame_bytes = PIKA_REPL_SHELL_FRAME_BYTES;
    memory->storage.values = memory->values;
    memory->storage.value_capacity = PIKA_REPL_SHELL_VALUE_COUNT;
    memory->storage.typed_values = memory->typed_values;
    memory->storage.typed_value_capacity =
        PIKA_REPL_SHELL_VALUE_COUNT;
    memory->storage.objects = &memory->objects;
    config->frontend.enabled = PIKA_CAPABILITY_ALL;
    config->storage = &memory->storage;
    config->buffers.line = memory->line;
    config->buffers.line_capacity =
        (uint32_t)sizeof(memory->line);
    config->buffers.block = memory->block;
    config->buffers.block_capacity =
        (uint32_t)sizeof(memory->block);
    config->buffers.escape = memory->escape;
    config->buffers.escape_capacity =
        (uint32_t)sizeof(memory->escape);
    config->buffers.history = memory->history;
    config->buffers.history_capacity =
        (uint32_t)sizeof(memory->history);
    config->buffers.history_offsets =
        memory->history_offsets;
    config->buffers.history_lengths =
        memory->history_lengths;
    config->buffers.history_entry_capacity =
        PIKA_REPL_MAX_HISTORY_ENTRIES;
    config->output_byte_limit =
        PIKA_REPL_MAX_OUTPUT_BYTES_PER_COMMAND;
}

int main(void) {
    static PikaReplShellMemory memory;
    PikaReplShellIo shell_io;
    PikaReplConfig config;
    PikaReplIo repl_io;
    uint8_t input[PIKA_REPL_SHELL_READ_BYTES];
    int exit_code = 0;
    memset(&shell_io, 0, sizeof(shell_io));
    shell_io.output_fd = STDOUT_FILENO;
    repl_shell_configure(&memory, &config);
    repl_io.write = repl_shell_write;
    repl_io.context = &shell_io;
    if (!repl_shell_enter_raw(&shell_io)) {
        return 1;
    }
    if (pika_repl_session_init(
            &memory.session, &config, &repl_io) >=
        PIKA_REPL_RESULT_RECOVERABLE_ERROR) {
        repl_shell_restore(&shell_io);
        pika_repl_session_deinit(&memory.session);
        return 1;
    }
    for (;;) {
        ssize_t length = read(
            STDIN_FILENO, input, sizeof(input));
        PikaReplResult result;
        if (length < 0 && errno == EINTR) {
            continue;
        }
        if (length < 0) {
            exit_code = 1;
            break;
        }
        if (length == 0) {
            result = pika_repl_stream_accept(
                &memory.session, PIKA_REPL_STREAM_END,
                NULL, 0u);
        } else {
            result = pika_repl_stream_accept(
                &memory.session, PIKA_REPL_STREAM_BYTES,
                input, (size_t)length);
        }
        if (result == PIKA_REPL_RESULT_ENDED) {
            break;
        }
        if (result == PIKA_REPL_RESULT_FATAL_ERROR ||
            pika_repl_session_state(&memory.session) ==
                PIKA_REPL_STATE_IO_FAULT) {
            exit_code = 1;
            break;
        }
        if (length == 0) {
            result = pika_repl_stream_accept(
                &memory.session, PIKA_REPL_STREAM_END,
                NULL, 0u);
            if (result != PIKA_REPL_RESULT_ENDED) {
                exit_code = 1;
            }
            break;
        }
    }
    pika_repl_session_deinit(&memory.session);
    repl_shell_restore(&shell_io);
    return exit_code;
}

#endif
