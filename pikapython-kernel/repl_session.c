/* SPEC: PJ2026-050106 REPL v0.9、PJ2026-0501 v0.47；有界字节流会话。 */
#include "pika_repl.h"

#if !PIKA_REPL_ENABLE
typedef unsigned char PikaReplDisabledTranslationUnit;
#endif

#if PIKA_REPL_ENABLE

#include "runtime_internal.h"

#include <stdio.h>
#include <string.h>

static const uint8_t repl_primary_prompt[] = {'>', '>', '>', ' '};
static const uint8_t repl_continuation_prompt[] = {'.', '.', '.', ' '};
static const uint8_t repl_crlf[] = {'\r', '\n'};
static const uint8_t repl_cursor_left[] = {0x1bu, '[', 'D'};

static PikaReplResult repl_insert_byte(
    PikaReplSession* session,
    uint8_t byte);

static uint32_t repl_minimum(uint32_t left, uint32_t right) {
    return left < right ? left : right;
}

static uint32_t repl_line_limit(const PikaReplSession* session) {
    return repl_minimum(
        session->config.buffers.line_capacity,
        PIKA_REPL_MAX_LINE_BYTES);
}

static uint32_t repl_block_limit(const PikaReplSession* session) {
    return repl_minimum(
        session->config.buffers.block_capacity,
        PIKA_REPL_MAX_BLOCK_BYTES);
}

static uint32_t repl_escape_limit(const PikaReplSession* session) {
    return repl_minimum(
        session->config.buffers.escape_capacity,
        PIKA_REPL_MAX_ESCAPE_BYTES);
}

static uint32_t repl_output_limit(const PikaReplSession* session) {
    return session->config.output_byte_limit == 0u
               ? PIKA_REPL_MAX_OUTPUT_BYTES_PER_COMMAND
               : session->config.output_byte_limit;
}

static void repl_clear_diagnostic(PikaReplSession* session) {
    memset(&session->diagnostic, 0, sizeof(session->diagnostic));
    session->diagnostic.status = PIKA_STATUS_OK;
}

static void repl_set_diagnostic(
    PikaReplSession* session,
    PikaReplError error,
    PikaStatus status,
    const char* macro_name,
    uint32_t configured,
    uint32_t observed,
    uint32_t offset,
    int recoverable) {
    session->diagnostic.error = error;
    session->diagnostic.status = status;
    session->diagnostic.macro_name = macro_name;
    session->diagnostic.configured = configured;
    session->diagnostic.observed = observed;
    session->diagnostic.offset = offset;
    session->diagnostic.recoverable =
        recoverable ? 1u : 0u;
}

static PikaReplResult repl_output_failure(
    PikaReplSession* session,
    PikaStatus status) {
    ++session->consecutive_io_failures;
    repl_set_diagnostic(
        session, PIKA_REPL_ERROR_OUTPUT, status,
        "PIKA_REPL_MAX_CONSECUTIVE_IO_FAILURES",
        PIKA_REPL_MAX_CONSECUTIVE_IO_FAILURES,
        session->consecutive_io_failures,
        session->output_count, 1);
    if (session->consecutive_io_failures >=
        PIKA_REPL_MAX_CONSECUTIVE_IO_FAILURES) {
        session->state = PIKA_REPL_STATE_FATAL;
        session->diagnostic.recoverable = 0u;
        return PIKA_REPL_RESULT_FATAL_ERROR;
    }
    session->state = PIKA_REPL_STATE_IO_FAULT;
    return PIKA_REPL_RESULT_RECOVERABLE_ERROR;
}

static PikaReplResult repl_emit(
    PikaReplSession* session,
    const uint8_t* bytes,
    uint32_t length) {
    uint32_t limit;
    PikaStatus status;
    if (length == 0u) {
        return PIKA_REPL_RESULT_CONSUMED;
    }
    if (session == NULL || bytes == NULL ||
        session->io.write == NULL) {
        return PIKA_REPL_RESULT_FATAL_ERROR;
    }
    limit = repl_output_limit(session);
    if (session->output_count > limit ||
        length > limit - session->output_count) {
        repl_set_diagnostic(
            session, PIKA_REPL_ERROR_RESOURCE,
            PIKA_STATUS_OUTPUT_ERROR,
            "PIKA_REPL_MAX_OUTPUT_BYTES_PER_COMMAND",
            limit, session->output_count + length,
            session->output_count, 1);
        session->state = PIKA_REPL_STATE_IO_FAULT;
        return PIKA_REPL_RESULT_RECOVERABLE_ERROR;
    }
    status = session->io.write(
        session->io.context, (const char*)bytes, length);
    if (status != PIKA_STATUS_OK) {
        return repl_output_failure(session, status);
    }
    session->output_count += length;
    session->last_output_was_cr =
        bytes[length - 1u] == '\r' ? 1u : 0u;
    session->consecutive_io_failures = 0u;
    return PIKA_REPL_RESULT_CONSUMED;
}

static PikaReplResult repl_emit_text(
    PikaReplSession* session,
    const char* text) {
    return repl_emit(
        session, (const uint8_t*)text,
        (uint32_t)strlen(text));
}

static PikaReplResult repl_emit_primary_prompt(
    PikaReplSession* session) {
    PikaReplResult result;
    session->state = PIKA_REPL_STATE_READY;
    result = repl_emit(
        session, repl_primary_prompt,
        (uint32_t)sizeof(repl_primary_prompt));
    if (result < PIKA_REPL_RESULT_RECOVERABLE_ERROR) {
        session->output_count = 0u;
        session->last_output_was_cr = 0u;
    }
    return result;
}

static PikaReplResult repl_emit_continuation_prompt(
    PikaReplSession* session) {
    session->state = PIKA_REPL_STATE_CONTINUATION;
    return repl_emit(
        session, repl_continuation_prompt,
        (uint32_t)sizeof(repl_continuation_prompt));
}

#if PIKA_REPL_AUTO_INDENT_ENABLE
static uint32_t repl_continuation_indent(
    const PikaReplSession* session) {
    const uint8_t* block = session->config.buffers.block;
    uint32_t end = session->block_length;
    uint32_t start;
    uint32_t indent = 0u;
    uint32_t index;
    uint8_t last = 0u;
    int leading = 1;
    while (end > 0u &&
           (block[end - 1u] == '\n' ||
            block[end - 1u] == '\r')) {
        --end;
    }
    start = end;
    while (start > 0u && block[start - 1u] != '\n') {
        --start;
    }
    index = start;
    while (index < end) {
        uint8_t byte = block[index++];
        if (leading && byte == ' ') {
            ++indent;
        } else if (leading && byte == '\t') {
            indent += 4u;
        } else if (byte == '\r' || byte == '\n') {
            continue;
        } else {
            leading = 0;
            last = byte;
        }
    }
    if (last == ':') {
        indent += 4u;
    } else if (!session->auto_indent_accepted) {
        indent = 0u;
    }
    return indent;
}

static PikaReplResult repl_insert_auto_indent(
    PikaReplSession* session) {
    uint32_t count = repl_continuation_indent(session);
    uint32_t index;
    PikaReplResult result = PIKA_REPL_RESULT_CONSUMED;
    for (index = 0u; index < count; ++index) {
        result = repl_insert_byte(session, ' ');
        if (result >= PIKA_REPL_RESULT_RECOVERABLE_ERROR) {
            return result;
        }
    }
    session->auto_indent_replay_remaining =
        count > UINT16_MAX ? UINT16_MAX : (uint16_t)count;
    session->auto_indent_width =
        session->auto_indent_replay_remaining;
    session->auto_indent_accepted = 0u;
    return result;
}
#endif

static void repl_clear_command(PikaReplSession* session) {
    session->line_length = 0u;
    session->cursor = 0u;
    session->block_length = 0u;
    session->escape_length = 0u;
    session->escape_state = 0u;
    session->auto_indent_replay_remaining = 0u;
    session->auto_indent_width = 0u;
    session->auto_indent_accepted = 0u;
    session->history_draft_length = 0u;
    session->history_position = session->history_count;
    session->input_offset = 0u;
}

static int repl_line_is_blank(
    const PikaReplSession* session) {
    uint32_t index;
    for (index = 0u; index < session->line_length; ++index) {
        uint8_t byte = session->config.buffers.line[index];
        if (byte != ' ' && byte != '\t') {
            return 0;
        }
    }
    return 1;
}

static PikaReplResult repl_finish_error(
    PikaReplSession* session,
    const char* text,
    int begin_new_line) {
    PikaReplResult result;
    if (begin_new_line) {
        result = repl_emit(
            session, repl_crlf,
            (uint32_t)sizeof(repl_crlf));
        if (result >= PIKA_REPL_RESULT_RECOVERABLE_ERROR) {
            return result;
        }
    }
    result = repl_emit_text(session, text);
    if (result >= PIKA_REPL_RESULT_RECOVERABLE_ERROR) {
        return result;
    }
    repl_clear_command(session);
    result = repl_emit_primary_prompt(session);
    return result >= PIKA_REPL_RESULT_RECOVERABLE_ERROR
               ? result
               : PIKA_REPL_RESULT_RECOVERABLE_ERROR;
}

static PikaReplResult repl_reject_nul(
    PikaReplSession* session) {
    char message[96];
    uint32_t offset =
        session->block_length + session->line_length;
    int written = snprintf(
        message, sizeof(message),
        "InputError: NUL byte at offset %u\r\n",
        (unsigned int)offset);
    repl_set_diagnostic(
        session, PIKA_REPL_ERROR_INPUT,
        PIKA_STATUS_LEX_ERROR, NULL, 0u, 0u,
        offset, 1);
    if (written < 0 || (size_t)written >= sizeof(message)) {
        return repl_finish_error(
            session, "InputError: NUL byte\r\n",
            session->line_length > 0u);
    }
    return repl_finish_error(
        session, message, session->line_length > 0u);
}

static PikaReplResult repl_reject_utf8(
    PikaReplSession* session,
    uint32_t line_offset) {
    char message[96];
    uint32_t offset =
        session->block_length + line_offset;
    int written = snprintf(
        message, sizeof(message),
        "InputError: invalid UTF-8 at offset %u\r\n",
        (unsigned int)offset);
    repl_set_diagnostic(
        session, PIKA_REPL_ERROR_INPUT,
        PIKA_STATUS_LEX_ERROR, NULL, 0u, 0u,
        offset, 1);
    return repl_finish_error(
        session,
        written >= 0 && (size_t)written < sizeof(message)
            ? message
            : "InputError: invalid UTF-8\r\n",
        session->line_length > 0u);
}

static int repl_utf8_prefix_status(
    const uint8_t* bytes,
    uint32_t length,
    uint32_t* failure_offset) {
    uint32_t index = 0u;
    while (index < length) {
        uint8_t first = bytes[index];
        uint32_t count;
        uint8_t second_min = 0x80u;
        uint8_t second_max = 0xbfu;
        uint32_t continuation;
        if (first <= 0x7fu) {
            ++index;
            continue;
        }
        if (first >= 0xc2u && first <= 0xdfu) {
            count = 1u;
        } else if (first >= 0xe0u && first <= 0xefu) {
            count = 2u;
            if (first == 0xe0u) {
                second_min = 0xa0u;
            } else if (first == 0xedu) {
                second_max = 0x9fu;
            }
        } else if (first >= 0xf0u && first <= 0xf4u) {
            count = 3u;
            if (first == 0xf0u) {
                second_min = 0x90u;
            } else if (first == 0xf4u) {
                second_max = 0x8fu;
            }
        } else {
            *failure_offset = index;
            return -1;
        }
        for (continuation = 1u;
             continuation <= count;
             ++continuation) {
            uint8_t current;
            if (index + continuation >= length) {
                *failure_offset = index;
                return 0;
            }
            current = bytes[index + continuation];
            if ((current & 0xc0u) != 0x80u ||
                (continuation == 1u &&
                 (current < second_min ||
                  current > second_max))) {
                *failure_offset = index + continuation;
                return -1;
            }
        }
        index += count + 1u;
    }
    return 1;
}

static PikaReplResult repl_reject_escape(
    PikaReplSession* session,
    int resource_error) {
    if (resource_error) {
        repl_set_diagnostic(
            session, PIKA_REPL_ERROR_RESOURCE,
            PIKA_STATUS_FRONTEND_LIMIT,
            "PIKA_REPL_MAX_ESCAPE_BYTES",
            repl_escape_limit(session),
            session->escape_length + 1u,
            session->input_offset, 1);
        return repl_finish_error(
            session,
            "InputError: PIKA_REPL_MAX_ESCAPE_BYTES exceeded\r\n",
            session->line_length > 0u);
    }
    repl_set_diagnostic(
        session, PIKA_REPL_ERROR_INPUT,
        PIKA_STATUS_UNSUPPORTED_SYNTAX,
        NULL, 0u, 0u, session->input_offset, 1);
    return repl_finish_error(
        session,
        "InputError: unsupported escape sequence\r\n",
        session->line_length > 0u);
}

static PikaReplResult repl_reject_control(
    PikaReplSession* session,
    uint8_t byte) {
    char message[80];
    int written = snprintf(
        message, sizeof(message),
        "InputError: unsupported control byte 0x%02x\r\n",
        (unsigned int)byte);
    repl_set_diagnostic(
        session, PIKA_REPL_ERROR_INPUT,
        PIKA_STATUS_UNSUPPORTED_SYNTAX,
        NULL, 0u, byte, session->input_offset, 1);
    if (written < 0 || (size_t)written >= sizeof(message)) {
        return repl_finish_error(
            session, "InputError: unsupported control byte\r\n",
            session->line_length > 0u);
    }
    return repl_finish_error(
        session, message, session->line_length > 0u);
}

static uint32_t repl_previous_unit(
    const uint8_t* bytes,
    uint32_t position) {
    uint32_t start;
    if (position == 0u) {
        return 0u;
    }
    start = position - 1u;
    while (start > 0u &&
           (bytes[start] & 0xc0u) == 0x80u) {
        --start;
    }
    return start;
}

static uint32_t repl_next_unit(
    const uint8_t* bytes,
    uint32_t length,
    uint32_t position) {
    uint32_t next;
    if (position >= length) {
        return length;
    }
    next = position + 1u;
    while (next < length &&
           (bytes[next] & 0xc0u) == 0x80u) {
        ++next;
    }
    return next;
}

static uint32_t repl_unit_count(
    const uint8_t* bytes,
    uint32_t length) {
    uint32_t count = 0u;
    uint32_t position = 0u;
    while (position < length) {
        position = repl_next_unit(bytes, length, position);
        ++count;
    }
    return count;
}

static PikaReplResult repl_emit_left(
    PikaReplSession* session,
    uint32_t count) {
    uint32_t index;
    for (index = 0u; index < count; ++index) {
        PikaReplResult result = repl_emit(
            session, repl_cursor_left,
            (uint32_t)sizeof(repl_cursor_left));
        if (result >= PIKA_REPL_RESULT_RECOVERABLE_ERROR) {
            return result;
        }
    }
    return PIKA_REPL_RESULT_CONSUMED;
}

static PikaReplResult repl_insert_byte(
    PikaReplSession* session,
    uint8_t byte) {
    uint8_t* line = session->config.buffers.line;
    uint32_t tail;
    uint32_t failure_offset = 0u;
    int utf8_status;
    PikaReplResult result;
    if (session->line_length >= repl_line_limit(session)) {
        char message[128];
        int written;
        repl_set_diagnostic(
            session, PIKA_REPL_ERROR_RESOURCE,
            PIKA_STATUS_FRONTEND_LIMIT,
            "PIKA_REPL_MAX_LINE_BYTES",
            repl_line_limit(session),
            session->line_length + 1u,
            session->line_length, 1);
        written = snprintf(
            message, sizeof(message),
            "InputError: PIKA_REPL_MAX_LINE_BYTES limit %u, observed %u\r\n",
            (unsigned int)repl_line_limit(session),
            (unsigned int)(session->line_length + 1u));
        return repl_finish_error(
            session,
            written >= 0 && (size_t)written < sizeof(message)
                ? message
                : "InputError: line limit exceeded\r\n",
            session->line_length > 0u);
    }
    tail = session->line_length - session->cursor;
    memmove(
        &line[session->cursor + 1u],
        &line[session->cursor], tail);
    line[session->cursor] = byte;
    utf8_status = repl_utf8_prefix_status(
        line, session->line_length + 1u,
        &failure_offset);
    if (utf8_status < 0) {
        memmove(
            &line[session->cursor],
            &line[session->cursor + 1u], tail);
        return repl_reject_utf8(
            session, failure_offset);
    }
    ++session->cursor;
    ++session->line_length;
    if (session->state != PIKA_REPL_STATE_CONTINUATION) {
        session->state = PIKA_REPL_STATE_EDITING;
    }
    result = repl_emit(session, &byte, 1u);
    if (result >= PIKA_REPL_RESULT_RECOVERABLE_ERROR ||
        tail == 0u) {
        return result;
    }
    result = repl_emit(
        session, &line[session->cursor], tail);
    if (result >= PIKA_REPL_RESULT_RECOVERABLE_ERROR) {
        return result;
    }
    return repl_emit_left(
        session,
        repl_unit_count(&line[session->cursor], tail));
}

static PikaReplResult repl_backspace(
    PikaReplSession* session) {
    uint8_t* line = session->config.buffers.line;
    uint32_t start;
    uint32_t removed;
    uint32_t tail;
    PikaReplResult result;
    static const uint8_t erase_at_end[] = {'\b', ' ', '\b'};
    if (session->cursor == 0u) {
        return PIKA_REPL_RESULT_NEED_MORE;
    }
    start = repl_previous_unit(line, session->cursor);
    removed = session->cursor - start;
    tail = session->line_length - session->cursor;
    memmove(
        &line[start], &line[session->cursor], tail);
    session->line_length -= removed;
    session->cursor = start;
    if (tail == 0u) {
        return repl_emit(
            session, erase_at_end,
            (uint32_t)sizeof(erase_at_end));
    }
    result = repl_emit_text(session, "\b");
    if (result < PIKA_REPL_RESULT_RECOVERABLE_ERROR) {
        result = repl_emit(session, &line[start], tail);
    }
    if (result < PIKA_REPL_RESULT_RECOVERABLE_ERROR) {
        result = repl_emit_text(session, " ");
    }
    if (result >= PIKA_REPL_RESULT_RECOVERABLE_ERROR) {
        return result;
    }
    return repl_emit_left(
        session,
        repl_unit_count(&line[start], tail) + 1u);
}

static PikaReplResult repl_delete_at_cursor(
    PikaReplSession* session) {
    uint8_t* line = session->config.buffers.line;
    uint32_t next;
    uint32_t removed;
    uint32_t tail;
    PikaReplResult result;
    if (session->cursor >= session->line_length) {
        return PIKA_REPL_RESULT_NEED_MORE;
    }
    next = repl_next_unit(
        line, session->line_length, session->cursor);
    removed = next - session->cursor;
    tail = session->line_length - next;
    memmove(&line[session->cursor], &line[next], tail);
    session->line_length -= removed;
    result = repl_emit(
        session, &line[session->cursor], tail);
    if (result < PIKA_REPL_RESULT_RECOVERABLE_ERROR) {
        result = repl_emit_text(session, " ");
    }
    if (result >= PIKA_REPL_RESULT_RECOVERABLE_ERROR) {
        return result;
    }
    return repl_emit_left(
        session,
        repl_unit_count(&line[session->cursor], tail) + 1u);
}

static void repl_history_remove_first(
    PikaReplSession* session) {
    PikaReplBuffers* buffers = &session->config.buffers;
    uint32_t removed;
    uint16_t index;
    if (session->history_count == 0u) {
        return;
    }
    removed = buffers->history_lengths[0];
    memmove(
        buffers->history,
        &buffers->history[removed],
        session->history_used - removed);
    for (index = 1u; index < session->history_count; ++index) {
        buffers->history_offsets[index - 1u] =
            buffers->history_offsets[index] - removed;
        buffers->history_lengths[index - 1u] =
            buffers->history_lengths[index];
    }
    --session->history_count;
    session->history_used -= removed;
}

static uint16_t repl_history_entry_limit(
    const PikaReplSession* session) {
    uint32_t limit = repl_minimum(
        session->config.buffers.history_entry_capacity,
        PIKA_REPL_MAX_HISTORY_ENTRIES);
    return (uint16_t)limit;
}

static uint32_t repl_history_byte_limit(
    const PikaReplSession* session) {
    return repl_minimum(
        session->config.buffers.history_capacity,
        PIKA_REPL_MAX_HISTORY_BYTES);
}

static void repl_history_add(
    PikaReplSession* session,
    const uint8_t* bytes,
    uint32_t length) {
    PikaReplBuffers* buffers = &session->config.buffers;
    uint16_t entry_limit = repl_history_entry_limit(session);
    uint32_t byte_limit = repl_history_byte_limit(session);
    if (length == 0u || entry_limit == 0u ||
        buffers->history == NULL ||
        buffers->history_offsets == NULL ||
        buffers->history_lengths == NULL ||
        length > byte_limit) {
        return;
    }
    if (session->history_count > 0u) {
        uint16_t last = (uint16_t)(session->history_count - 1u);
        if (buffers->history_lengths[last] == length &&
            memcmp(
                &buffers->history[
                    buffers->history_offsets[last]],
                bytes, length) == 0) {
            session->history_position = session->history_count;
            return;
        }
    }
    while (session->history_count >= entry_limit ||
           session->history_used > byte_limit - length) {
        repl_history_remove_first(session);
    }
    buffers->history_offsets[session->history_count] =
        session->history_used;
    buffers->history_lengths[session->history_count] = length;
    memcpy(
        &buffers->history[session->history_used],
        bytes, length);
    session->history_used += length;
    ++session->history_count;
    session->history_position = session->history_count;
}

static PikaReplResult repl_redraw_line(
    PikaReplSession* session,
    const uint8_t* replacement,
    uint32_t replacement_length) {
    uint8_t* line = session->config.buffers.line;
    uint32_t old_length = session->line_length;
    uint32_t erase_count;
    uint32_t index;
    PikaReplResult result;
    if (replacement_length > repl_line_limit(session)) {
        return PIKA_REPL_RESULT_RECOVERABLE_ERROR;
    }
    memcpy(line, replacement, replacement_length);
    session->line_length = replacement_length;
    session->cursor = replacement_length;
    result = repl_emit_text(session, "\r>>> ");
    if (result < PIKA_REPL_RESULT_RECOVERABLE_ERROR) {
        result = repl_emit(session, line, replacement_length);
    }
    erase_count =
        old_length > replacement_length
            ? old_length - replacement_length
            : 0u;
    for (index = 0u;
         result < PIKA_REPL_RESULT_RECOVERABLE_ERROR &&
         index < erase_count;
         ++index) {
        result = repl_emit_text(session, " ");
    }
    if (result >= PIKA_REPL_RESULT_RECOVERABLE_ERROR) {
        return result;
    }
    return repl_emit_left(session, erase_count);
}

static PikaReplResult repl_history_move(
    PikaReplSession* session,
    int direction) {
    PikaReplBuffers* buffers = &session->config.buffers;
    const uint8_t* replacement;
    uint32_t length;
    if (session->history_count == 0u ||
        session->state == PIKA_REPL_STATE_CONTINUATION) {
        return PIKA_REPL_RESULT_NEED_MORE;
    }
    if (session->history_position == session->history_count) {
        if (session->line_length >
            repl_block_limit(session)) {
            return PIKA_REPL_RESULT_RECOVERABLE_ERROR;
        }
        memcpy(
            buffers->block, buffers->line,
            session->line_length);
        session->history_draft_length =
            session->line_length;
    }
    if (direction < 0) {
        if (session->history_position == 0u) {
            return PIKA_REPL_RESULT_NEED_MORE;
        }
        --session->history_position;
    } else {
        if (session->history_position >=
            session->history_count) {
            return PIKA_REPL_RESULT_NEED_MORE;
        }
        ++session->history_position;
    }
    if (session->history_position ==
        session->history_count) {
        replacement = buffers->block;
        length = session->history_draft_length;
    } else {
        replacement = &buffers->history[
            buffers->history_offsets[
                session->history_position]];
        length = buffers->history_lengths[
            session->history_position];
    }
    return repl_redraw_line(session, replacement, length);
}

static PikaReplResult repl_handle_escape(
    PikaReplSession* session,
    uint8_t byte) {
    uint8_t* escape = session->config.buffers.escape;
    PikaReplResult result;
    if (session->escape_length >=
        repl_escape_limit(session)) {
        return repl_reject_escape(session, 1);
    }
    escape[session->escape_length++] = byte;
    if (session->escape_state == 1u) {
        if (byte != '[') {
            return repl_reject_escape(session, 0);
        }
        session->escape_state = 2u;
        return PIKA_REPL_RESULT_NEED_MORE;
    }
    if (session->escape_state != 2u) {
        return repl_reject_escape(session, 0);
    }
    if ((byte >= 0x30u && byte <= 0x3fu) ||
        (byte >= 0x20u && byte <= 0x2fu)) {
        return PIKA_REPL_RESULT_NEED_MORE;
    }
    if (byte < 0x40u || byte > 0x7eu) {
        return repl_reject_escape(session, 0);
    }
    if (session->escape_length == 3u &&
        byte == 'D' && session->cursor > 0u) {
        session->cursor = repl_previous_unit(
            session->config.buffers.line,
            session->cursor);
        result = repl_emit(
            session, escape, session->escape_length);
    } else if (session->escape_length == 3u &&
               byte == 'C' &&
               session->cursor < session->line_length) {
        session->cursor = repl_next_unit(
            session->config.buffers.line,
            session->line_length, session->cursor);
        result = repl_emit(
            session, escape, session->escape_length);
    } else if (session->escape_length == 3u &&
               byte == 'A') {
        result = repl_history_move(session, -1);
    } else if (session->escape_length == 3u &&
               byte == 'B') {
        result = repl_history_move(session, 1);
    } else if (session->escape_length == 3u &&
               (byte == 'C' || byte == 'D')) {
        result = PIKA_REPL_RESULT_NEED_MORE;
    } else {
        return repl_reject_escape(session, 0);
    }
    session->escape_length = 0u;
    session->escape_state = 0u;
    return result;
}

static PikaStatus repl_runtime_output(
    void* context,
    const char* bytes,
    uint32_t length) {
    PikaReplSession* session = (PikaReplSession*)context;
    uint32_t start = 0u;
    uint32_t index;
    for (index = 0u; index < length; ++index) {
        PikaReplResult result;
        if (bytes[index] != '\n') {
            continue;
        }
        if (index > start) {
            result = repl_emit(
                session, (const uint8_t*)&bytes[start],
                index - start);
            if (result >= PIKA_REPL_RESULT_RECOVERABLE_ERROR) {
                return PIKA_STATUS_OUTPUT_ERROR;
            }
        }
        if (session->last_output_was_cr) {
            result = repl_emit(
                session, (const uint8_t*)"\n", 1u);
        } else {
            result = repl_emit(
                session, repl_crlf,
                (uint32_t)sizeof(repl_crlf));
        }
        if (result >= PIKA_REPL_RESULT_RECOVERABLE_ERROR) {
            return PIKA_STATUS_OUTPUT_ERROR;
        }
        start = index + 1u;
    }
    if (start < length &&
        repl_emit(
            session, (const uint8_t*)&bytes[start],
            length - start) >=
            PIKA_REPL_RESULT_RECOVERABLE_ERROR) {
        return PIKA_STATUS_OUTPUT_ERROR;
    }
    return PIKA_STATUS_OK;
}

static PikaStatus repl_write_result(
    PikaReplSession* session,
    const PikaOutput* output) {
    PikaRuntimeContext context;
    PikaStatus status;
    if (session->result.typed_value.kind ==
        PIKA_RUNTIME_VALUE_NONE) {
        return PIKA_STATUS_OK;
    }
    memset(&context, 0, sizeof(context));
    context.objects =
        session->config.storage != NULL
            ? session->config.storage->objects
            : NULL;
    context.output = output;
    status = pika_runtime_write_value_text(
        &context, session->result.typed_value, 1, 0);
    if (status == PIKA_STATUS_OK) {
        status = output->write(
            output->context, "\n", 1u);
    }
    return status;
}

static uint32_t repl_program_code_bytes(
    const PikaProgram* program) {
    uint64_t total;
    uint16_t index;
    if (program == NULL) {
        return 0u;
    }
    total =
        (uint64_t)program->instruction_count *
            sizeof(PikaInstruction) +
        (uint64_t)program->function_count *
            sizeof(PikaFunction) +
        (uint64_t)program->binding_call_count *
            sizeof(PikaBindingCallSite) +
        (uint64_t)program->builtin_call_count *
            sizeof(PikaBuiltinCallSite) +
        (uint64_t)program->constant_count *
            sizeof(PikaConstant) +
        (uint64_t)program->index_site_count *
            sizeof(PikaIndexSite) +
        (uint64_t)program->class_count *
            sizeof(PikaClassLayout) +
        (uint64_t)program->exception_handler_count *
            sizeof(PikaExceptionHandler);
    for (index = 0u; index < program->constant_count; ++index) {
        if (program->constants[index].kind ==
                PIKA_CONSTANT_STRING ||
            program->constants[index].kind ==
                PIKA_CONSTANT_BYTES ||
            program->constants[index].kind ==
                PIKA_CONSTANT_TYPE ||
            program->constants[index].kind ==
                PIKA_CONSTANT_MODULE) {
            total += program->constants[index].length;
        }
    }
    return total > UINT32_MAX
               ? UINT32_MAX
               : (uint32_t)total;
}

static int repl_storage_matches(
    const PikaReplSession* session,
    const PikaProgramRequirements* requirements) {
    const PikaStorage* storage = session->config.storage;
    const PikaBindingStorage* binding =
        session->config.binding_storage;
    if (storage == NULL || requirements == NULL ||
        storage->frames == NULL ||
        storage->frame_bytes < requirements->frame_bytes ||
        (requirements->value_count > 0u &&
         (storage->values == NULL ||
          storage->value_capacity < requirements->value_count)) ||
        (requirements->typed_value_count > 0u &&
         (storage->typed_values == NULL ||
          storage->typed_value_capacity <
              requirements->typed_value_count)) ||
        (requirements->object_slot_count > 0u &&
         !pika_runtime_object_storage_accepts(
             storage->objects,
             requirements->object_slot_count,
             requirements->object_bytes)) ||
        (requirements->binding_value_count > 0u &&
         (binding == NULL || binding->values == NULL ||
          binding->value_capacity <
              requirements->binding_value_count)) ||
        (requirements->binding_object_count > 0u &&
         (binding == NULL || binding->objects == NULL ||
          binding->object_capacity <
              requirements->binding_object_count))) {
        return 0;
    }
    return 1;
}

static int repl_storage_accepts(
    PikaReplSession* session,
    const PikaProgram* program) {
    PikaProgramRequirements requirements;
    PikaStatus status;
    status = pika_program_storage_requirements(
        program, &requirements);
    if (status != PIKA_STATUS_OK) return 0;
    if (repl_storage_matches(session, &requirements)) return 1;
    if (session->config.ensure_storage == NULL) return 0;
    status = session->config.ensure_storage(
        session->config.ensure_storage_context,
        &requirements);
    return status == PIKA_STATUS_OK &&
           repl_storage_matches(session, &requirements);
}

typedef enum {
    PIKA_REPL_COMMIT_ACCEPTED = 0,
    PIKA_REPL_COMMIT_CODE_LIMIT = 1,
    PIKA_REPL_COMMIT_STORAGE_LIMIT = 2,
} PikaReplCommitResult;

typedef struct {
    PikaReplSession* session;
    uint32_t code_bytes;
    PikaReplCommitResult result;
} PikaReplCommitCheck;

static PikaStatus repl_check_compiled_module(
    void* context,
    const PikaCompiledModule* module) {
    PikaReplCommitCheck* check =
        (PikaReplCommitCheck*)context;
    const PikaProgram* program =
        pika_compiled_module_program(module);
    check->code_bytes = repl_program_code_bytes(program);
    if (check->code_bytes > PIKA_REPL_MAX_CODE_BYTES) {
        check->result = PIKA_REPL_COMMIT_CODE_LIMIT;
        return PIKA_STATUS_FRONTEND_LIMIT;
    }
    if (!repl_storage_accepts(check->session, program)) {
        check->result = PIKA_REPL_COMMIT_STORAGE_LIMIT;
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    check->result = PIKA_REPL_COMMIT_ACCEPTED;
    return PIKA_STATUS_OK;
}

static PikaReplResult repl_emit_compile_error(
    PikaReplSession* session,
    PikaStatus status,
    const PikaDiagnostic* diagnostic) {
    char message[192];
    int written;
    if (status == PIKA_STATUS_UNDEFINED_NAME) {
        repl_set_diagnostic(
            session, PIKA_REPL_ERROR_RUNTIME, status,
            NULL, 0u, 0u, diagnostic->offset, 1);
        written = snprintf(
            message, sizeof(message),
            "NameError: undefined name\r\n");
    } else if (status == PIKA_STATUS_FRONTEND_LIMIT &&
        diagnostic->bound != PIKA_FRONTEND_BOUND_NONE) {
        const char* macro_name =
            pika_frontend_bound_macro(diagnostic->bound);
        repl_set_diagnostic(
            session, PIKA_REPL_ERROR_RESOURCE, status,
            macro_name, diagnostic->configured,
            diagnostic->observed, diagnostic->offset, 1);
        written = snprintf(
            message, sizeof(message),
            "ResourceError: %s limit %u, observed %u at line %u column %u\r\n",
            macro_name,
            (unsigned int)diagnostic->configured,
            (unsigned int)diagnostic->observed,
            (unsigned int)diagnostic->line,
            (unsigned int)diagnostic->column);
    } else {
        repl_set_diagnostic(
            session, PIKA_REPL_ERROR_COMPILE, status,
            NULL, 0u, 0u, diagnostic->offset, 1);
        written = snprintf(
            message, sizeof(message),
            "SyntaxError: %s at line %u column %u\r\n",
            pika_status_name(status),
            (unsigned int)diagnostic->line,
            (unsigned int)diagnostic->column);
    }
    return repl_finish_error(
        session,
        written >= 0 && (size_t)written < sizeof(message)
            ? message
            : "SyntaxError\r\n",
        0);
}

static PikaReplResult repl_emit_runtime_error(
    PikaReplSession* session,
    PikaStatus status) {
    char message[128];
    const char* exception_name = "RuntimeError";
    const char* detail = pika_status_name(status);
    int written;
    if (status == PIKA_STATUS_DIVISION_BY_ZERO) {
        exception_name = "ZeroDivisionError";
        detail = "division by zero";
    } else if (status == PIKA_STATUS_UNDEFINED_NAME) {
        exception_name = "NameError";
        detail = "undefined name";
    } else if (status == PIKA_STATUS_TYPE_ERROR ||
               status == PIKA_STATUS_TYPE_MISMATCH) {
        exception_name = "TypeError";
    } else if (status == PIKA_STATUS_VALUE_ERROR ||
               status == PIKA_STATUS_INVALID_OPERAND) {
        exception_name = "ValueError";
    } else if (status == PIKA_STATUS_OVERFLOW_ERROR) {
        exception_name = "OverflowError";
    } else if (status == PIKA_STATUS_ASSERTION_ERROR) {
        exception_name = "AssertionError";
    } else if (status == PIKA_STATUS_OS_ERROR ||
               status == PIKA_STATUS_IO_ERROR) {
        exception_name = "OSError";
    }
    repl_set_diagnostic(
        session, PIKA_REPL_ERROR_RUNTIME, status,
        NULL, 0u, 0u,
        session->result.instruction_index, 1);
    written = snprintf(
        message, sizeof(message), "%s: %s\r\n",
        exception_name, detail);
    if (written < 0 || (size_t)written >= sizeof(message)) {
        return repl_finish_error(
            session, "RuntimeError\r\n", 0);
    }
    return repl_finish_error(session, message, 0);
}

static PikaReplResult repl_emit_code_limit_error(
    PikaReplSession* session,
    uint32_t code_bytes) {
    char message[128];
    int written;
    repl_set_diagnostic(
        session, PIKA_REPL_ERROR_RESOURCE,
        PIKA_STATUS_FRONTEND_LIMIT,
        "PIKA_REPL_MAX_CODE_BYTES",
        PIKA_REPL_MAX_CODE_BYTES, code_bytes,
        session->block_length, 1);
    written = snprintf(
        message, sizeof(message),
        "ResourceError: PIKA_REPL_MAX_CODE_BYTES limit %u, observed %u\r\n",
        (unsigned int)PIKA_REPL_MAX_CODE_BYTES,
        (unsigned int)code_bytes);
    return repl_finish_error(
        session,
        written >= 0 && (size_t)written < sizeof(message)
            ? message
            : "ResourceError: code limit exceeded\r\n",
        0);
}

static PikaReplResult repl_emit_storage_limit_error(
    PikaReplSession* session) {
    repl_set_diagnostic(
        session, PIKA_REPL_ERROR_RESOURCE,
        PIKA_STATUS_STORAGE_TOO_SMALL,
        NULL, 0u, 0u, session->block_length, 1);
    return repl_finish_error(
        session,
        "ResourceError: runtime storage too small\r\n",
        0);
}

static PikaReplResult repl_execute_block(
    PikaReplSession* session) {
    PikaCompiledModule* candidate = NULL;
    PikaCompiledModule* previous;
    PikaInteractiveCompileState compile_state =
        PIKA_INTERACTIVE_COMPILE_INVALID;
    PikaDiagnostic diagnostic;
    PikaArguments arguments;
    PikaOutput output;
    const PikaProgram* program;
    PikaStatus status;
    PikaReplCommitCheck commit_check;
    memset(&diagnostic, 0, sizeof(diagnostic));
    memset(&commit_check, 0, sizeof(commit_check));
    commit_check.session = session;
    if (session->config.modules != NULL) {
        status =
            pika_module_graph_compile_incremental_owned_interactive(
                session->module,
                (const char*)session->config.buffers.block,
                session->block_length,
                session->config.modules,
                &session->config.frontend,
                repl_check_compiled_module, &commit_check, &candidate,
                &compile_state, &diagnostic);
    } else {
        status = pika_module_compile_incremental_owned_interactive(
            session->module,
            (const char*)session->config.buffers.block,
            session->block_length,
            &session->config.frontend,
            repl_check_compiled_module, &commit_check, &candidate,
            &compile_state, &diagnostic);
    }
    if (status != PIKA_STATUS_OK ||
        compile_state == PIKA_INTERACTIVE_COMPILE_INVALID) {
        if (commit_check.result ==
            PIKA_REPL_COMMIT_CODE_LIMIT) {
            return repl_emit_code_limit_error(
                session, commit_check.code_bytes);
        }
        if (commit_check.result ==
            PIKA_REPL_COMMIT_STORAGE_LIMIT) {
            return repl_emit_storage_limit_error(session);
        }
        return repl_emit_compile_error(
            session,
            status == PIKA_STATUS_OK
                ? diagnostic.status
                : status,
            &diagnostic);
    }
    if (compile_state == PIKA_INTERACTIVE_COMPILE_INCOMPLETE) {
        PikaReplResult prompt_result;
        session->line_length = 0u;
        session->cursor = 0u;
        prompt_result = repl_emit_continuation_prompt(session);
        if (prompt_result >= PIKA_REPL_RESULT_RECOVERABLE_ERROR) {
            return prompt_result;
        }
#if PIKA_REPL_AUTO_INDENT_ENABLE
        return repl_insert_auto_indent(session);
#else
        return prompt_result;
#endif
    }
    program = pika_compiled_module_program(candidate);
    previous = session->module;
    session->module = candidate;
    pika_compiled_module_destroy(previous);
    session->state = PIKA_REPL_STATE_EXECUTING;
    arguments.values = NULL;
    arguments.count = 0u;
    output.write = repl_runtime_output;
    output.context = session;
    status = pika_program_execute_stateful(
        program, &arguments, session->config.storage,
        session->config.binding_storage, &output,
        pika_compiled_module_global_count(candidate),
        &session->execution, &session->result,
        &session->metrics);
    if (status == PIKA_STATUS_OK &&
        pika_compiled_module_has_interactive_result(candidate)) {
        status = repl_write_result(session, &output);
    }
    {
        PikaStatus release_status =
            pika_compiled_module_release_interactive_entry(
                candidate);
        if (status == PIKA_STATUS_OK &&
            release_status != PIKA_STATUS_OK) {
            status = release_status;
        }
    }
    if (session->config.release_idle_storage != NULL) {
        session->config.release_idle_storage(
            session->config.release_idle_storage_context,
            &session->execution);
    }
    session->block_length = 0u;
    session->line_length = 0u;
    session->cursor = 0u;
    session->history_draft_length = 0u;
    session->history_position = session->history_count;
    session->input_offset = 0u;
    if (status != PIKA_STATUS_OK) {
        if (session->state == PIKA_REPL_STATE_IO_FAULT ||
            session->state == PIKA_REPL_STATE_FATAL) {
            return session->state == PIKA_REPL_STATE_FATAL
                       ? PIKA_REPL_RESULT_FATAL_ERROR
                       : PIKA_REPL_RESULT_RECOVERABLE_ERROR;
        }
        return repl_emit_runtime_error(session, status);
    }
    if (repl_emit_primary_prompt(session) >=
        PIKA_REPL_RESULT_RECOVERABLE_ERROR) {
        return session->state == PIKA_REPL_STATE_FATAL
                   ? PIKA_REPL_RESULT_FATAL_ERROR
                   : PIKA_REPL_RESULT_RECOVERABLE_ERROR;
    }
    repl_clear_diagnostic(session);
    return PIKA_REPL_RESULT_COMMAND_COMPLETE;
}

static PikaReplResult repl_append_line(
    PikaReplSession* session) {
    uint32_t required =
        session->line_length + 1u;
    uint32_t limit = repl_block_limit(session);
    if (session->block_length > limit ||
        required > limit - session->block_length) {
        char message[128];
        int written;
        repl_set_diagnostic(
            session, PIKA_REPL_ERROR_RESOURCE,
            PIKA_STATUS_FRONTEND_LIMIT,
            "PIKA_REPL_MAX_BLOCK_BYTES",
            limit, session->block_length + required,
            session->block_length, 1);
        written = snprintf(
            message, sizeof(message),
            "InputError: PIKA_REPL_MAX_BLOCK_BYTES limit %u, observed %u\r\n",
            (unsigned int)limit,
            (unsigned int)(session->block_length + required));
        return repl_finish_error(
            session,
            written >= 0 && (size_t)written < sizeof(message)
                ? message
                : "InputError: block limit exceeded\r\n",
            0);
    }
    memcpy(
        &session->config.buffers.block[
            session->block_length],
        session->config.buffers.line,
        session->line_length);
    session->block_length += session->line_length;
    session->config.buffers.block[
        session->block_length++] = '\n';
    return PIKA_REPL_RESULT_CONSUMED;
}

static PikaReplResult repl_submit_line(
    PikaReplSession* session) {
    PikaReplResult result;
    int continuing =
        session->state == PIKA_REPL_STATE_CONTINUATION;
    result = repl_emit(
        session, repl_crlf,
        (uint32_t)sizeof(repl_crlf));
    if (result >= PIKA_REPL_RESULT_RECOVERABLE_ERROR) {
        return result;
    }
    if (!continuing && session->line_length == 0u) {
        repl_clear_command(session);
        result = repl_emit_primary_prompt(session);
        return result >= PIKA_REPL_RESULT_RECOVERABLE_ERROR
                   ? result
                   : PIKA_REPL_RESULT_COMMAND_COMPLETE;
    }
    if (continuing && repl_line_is_blank(session)) {
        session->line_length = 0u;
        session->cursor = 0u;
        session->auto_indent_replay_remaining = 0u;
        return repl_execute_block(session);
    }
#if PIKA_CAPABILITY_BUILTIN_HELP_ENABLE
    if (!continuing && session->line_length == 4u &&
        memcmp(session->config.buffers.line, "help", 4u) == 0) {
        repl_history_add(
            session, session->config.buffers.line,
            session->line_length);
        repl_clear_command(session);
        result = repl_emit_text(
            session,
            "Type help() for interactive help, or help(object) for help about object.\r\n");
        if (result >= PIKA_REPL_RESULT_RECOVERABLE_ERROR) {
            return result;
        }
        result = repl_emit_primary_prompt(session);
        return result >= PIKA_REPL_RESULT_RECOVERABLE_ERROR
                   ? result
                   : PIKA_REPL_RESULT_COMMAND_COMPLETE;
    }
#endif
    repl_history_add(
        session, session->config.buffers.line,
        session->line_length);
    result = repl_append_line(session);
    if (result >= PIKA_REPL_RESULT_RECOVERABLE_ERROR) {
        return result;
    }
    session->line_length = 0u;
    session->cursor = 0u;
    session->auto_indent_replay_remaining = 0u;
    session->history_draft_length = 0u;
    session->history_position = session->history_count;
    if (continuing) {
        result = repl_emit_continuation_prompt(session);
        if (result >= PIKA_REPL_RESULT_RECOVERABLE_ERROR) {
            return result;
        }
#if PIKA_REPL_AUTO_INDENT_ENABLE
        return repl_insert_auto_indent(session);
#else
        return result;
#endif
    }
    return repl_execute_block(session);
}

static PikaReplResult repl_interrupt_edit(
    PikaReplSession* session) {
    PikaReplResult result;
    result = repl_emit_text(session, "^C\r\n");
    if (result < PIKA_REPL_RESULT_RECOVERABLE_ERROR) {
        result = repl_emit_text(
            session, "KeyboardInterrupt\r\n");
    }
    if (result >= PIKA_REPL_RESULT_RECOVERABLE_ERROR) {
        return result;
    }
    repl_set_diagnostic(
        session, PIKA_REPL_ERROR_RUNTIME,
        PIKA_STATUS_RUNTIME_ERROR,
        NULL, 0u, 0u, session->input_offset, 1);
    repl_clear_command(session);
    result = repl_emit_primary_prompt(session);
    return result >= PIKA_REPL_RESULT_RECOVERABLE_ERROR
               ? result
               : PIKA_REPL_RESULT_RECOVERABLE_ERROR;
}

static PikaReplResult repl_process_byte(
    PikaReplSession* session,
    uint8_t byte) {
    PikaReplResult result;
    uint32_t index;
    uint32_t failure_offset = 0u;
    int utf8_status = repl_utf8_prefix_status(
        session->config.buffers.line,
        session->line_length, &failure_offset);
    if (utf8_status == 0 && byte < 0x80u &&
        byte != 0x03u && byte != 0x08u &&
        byte != 0x7fu) {
        return repl_reject_utf8(
            session, failure_offset);
    }
    if (session->last_input_was_cr) {
        session->last_input_was_cr = 0u;
        if (byte == '\n') {
            return PIKA_REPL_RESULT_CONSUMED;
        }
    }
    if (session->auto_indent_replay_remaining > 0u) {
        if (byte == ' ') {
            --session->auto_indent_replay_remaining;
            return PIKA_REPL_RESULT_CONSUMED;
        }
        if (byte == '\t' &&
            session->auto_indent_replay_remaining >= 4u) {
            session->auto_indent_replay_remaining -= 4u;
            return PIKA_REPL_RESULT_CONSUMED;
        }
        session->auto_indent_accepted =
            session->auto_indent_replay_remaining ==
                    session->auto_indent_width
                ? 1u
                : 0u;
        session->auto_indent_replay_remaining = 0u;
    }
    if (session->escape_state != 0u) {
        return repl_handle_escape(session, byte);
    }
    if (byte == 0x1bu) {
        session->escape_state = 1u;
        session->escape_length = 1u;
        session->config.buffers.escape[0] = byte;
        return PIKA_REPL_RESULT_NEED_MORE;
    }
    if (byte == 0x00u) {
        return repl_reject_nul(session);
    }
    if (byte == 0x03u) {
        return repl_interrupt_edit(session);
    }
    if (byte == 0x04u) {
        if (session->line_length == 0u &&
            session->block_length == 0u &&
            session->state !=
                PIKA_REPL_STATE_CONTINUATION) {
            session->state = PIKA_REPL_STATE_ENDED;
            return PIKA_REPL_RESULT_ENDED;
        }
        return repl_delete_at_cursor(session);
    }
    if (byte == 0x08u || byte == 0x7fu) {
        return repl_backspace(session);
    }
    if (byte == '\r' || byte == '\n') {
        if (byte == '\r') {
            session->last_input_was_cr = 1u;
        }
        return repl_submit_line(session);
    }
    if (byte == '\t') {
        result = PIKA_REPL_RESULT_NEED_MORE;
        for (index = 0u; index < 4u; ++index) {
            result = repl_insert_byte(session, ' ');
            if (result >= PIKA_REPL_RESULT_RECOVERABLE_ERROR) {
                return result;
            }
        }
        return result;
    }
    if (byte < 0x20u) {
        return repl_reject_control(session, byte);
    }
    return repl_insert_byte(session, byte);
}

PikaReplResult pika_repl_session_init(
    PikaReplSession* session,
    const PikaReplConfig* config,
    const PikaReplIo* io) {
    int history_absent;
    if (session == NULL || session->initialized != 0u ||
        config == NULL || io == NULL ||
        io->write == NULL || config->storage == NULL ||
        config->buffers.line == NULL ||
        config->buffers.line_capacity == 0u ||
        config->buffers.block == NULL ||
        config->buffers.block_capacity == 0u ||
        config->buffers.escape == NULL ||
        config->buffers.escape_capacity < 3u ||
        config->output_byte_limit >
            PIKA_REPL_MAX_OUTPUT_BYTES_PER_COMMAND) {
        return PIKA_REPL_RESULT_FATAL_ERROR;
    }
    if ((config->initial_module == NULL) !=
        (config->initial_execution == NULL)) {
        return PIKA_REPL_RESULT_FATAL_ERROR;
    }
    history_absent =
        config->buffers.history == NULL &&
        config->buffers.history_offsets == NULL &&
        config->buffers.history_lengths == NULL &&
        config->buffers.history_capacity == 0u &&
        config->buffers.history_entry_capacity == 0u;
    if (!history_absent &&
        (config->buffers.history == NULL ||
         config->buffers.history_offsets == NULL ||
         config->buffers.history_lengths == NULL ||
         config->buffers.history_capacity == 0u ||
         config->buffers.history_entry_capacity == 0u)) {
        return PIKA_REPL_RESULT_FATAL_ERROR;
    }
    memset(session, 0, sizeof(*session));
    session->config = *config;
    session->io = *io;
    session->module = config->initial_module;
    if (config->initial_execution != NULL) {
        session->execution = *config->initial_execution;
    }
    session->config.initial_module = NULL;
    session->config.initial_execution = NULL;
    session->initialized = 1u;
    session->state = PIKA_REPL_STATE_READY;
    repl_clear_diagnostic(session);
    if (repl_emit_primary_prompt(session) >=
        PIKA_REPL_RESULT_RECOVERABLE_ERROR) {
        return session->state == PIKA_REPL_STATE_FATAL
                   ? PIKA_REPL_RESULT_FATAL_ERROR
                   : PIKA_REPL_RESULT_RECOVERABLE_ERROR;
    }
    return PIKA_REPL_RESULT_CONSUMED;
}

PikaReplResult pika_repl_feed(
    PikaReplSession* session,
    const uint8_t* bytes,
    size_t length) {
    PikaReplResult combined = PIKA_REPL_RESULT_CONSUMED;
    size_t index;
    if (session == NULL || session->initialized == 0u ||
        (length > 0u && bytes == NULL)) {
        return PIKA_REPL_RESULT_FATAL_ERROR;
    }
    if (session->state == PIKA_REPL_STATE_FATAL) {
        return PIKA_REPL_RESULT_FATAL_ERROR;
    }
    if (session->state == PIKA_REPL_STATE_ENDED) {
        return PIKA_REPL_RESULT_ENDED;
    }
    if (session->state == PIKA_REPL_STATE_IO_FAULT) {
        return PIKA_REPL_RESULT_RECOVERABLE_ERROR;
    }
    for (index = 0u; index < length; ++index) {
        PikaReplResult current =
            repl_process_byte(session, bytes[index]);
        if (current != PIKA_REPL_RESULT_COMMAND_COMPLETE &&
            current != PIKA_REPL_RESULT_ENDED &&
            !(current == PIKA_REPL_RESULT_RECOVERABLE_ERROR &&
              session->state == PIKA_REPL_STATE_READY)) {
            ++session->input_offset;
        }
        if (current > combined) {
            combined = current;
        }
        if (current == PIKA_REPL_RESULT_FATAL_ERROR ||
            current == PIKA_REPL_RESULT_ENDED ||
            session->state == PIKA_REPL_STATE_IO_FAULT) {
            break;
        }
    }
    if (combined == PIKA_REPL_RESULT_CONSUMED &&
        (session->state == PIKA_REPL_STATE_EDITING ||
         session->state == PIKA_REPL_STATE_CONTINUATION ||
         session->escape_state != 0u)) {
        return PIKA_REPL_RESULT_NEED_MORE;
    }
    return combined;
}

PikaReplResult pika_repl_end_input(PikaReplSession* session) {
    uint32_t failure_offset = 0u;
    int utf8_status;
    if (session == NULL || session->initialized == 0u) {
        return PIKA_REPL_RESULT_FATAL_ERROR;
    }
    if (session->state == PIKA_REPL_STATE_FATAL) {
        return PIKA_REPL_RESULT_FATAL_ERROR;
    }
    if (session->state == PIKA_REPL_STATE_ENDED) {
        return PIKA_REPL_RESULT_ENDED;
    }
    if (session->escape_state != 0u) {
        return repl_reject_escape(session, 0);
    }
    utf8_status = repl_utf8_prefix_status(
        session->config.buffers.line,
        session->line_length, &failure_offset);
    if (utf8_status <= 0) {
        return repl_reject_utf8(
            session, failure_offset);
    }
    if (session->line_length != 0u ||
        session->block_length != 0u ||
        session->state == PIKA_REPL_STATE_CONTINUATION) {
        repl_set_diagnostic(
            session, PIKA_REPL_ERROR_INPUT,
            PIKA_STATUS_SYNTAX_ERROR, NULL, 0u, 0u,
            session->input_offset, 1);
        return repl_finish_error(
            session,
            "SyntaxError: incomplete input at end of stream\r\n",
            session->line_length > 0u);
    }
    session->state = PIKA_REPL_STATE_ENDED;
    return PIKA_REPL_RESULT_ENDED;
}

PikaReplResult pika_repl_request_interrupt(
    PikaReplSession* session) {
    if (session == NULL || session->initialized == 0u) {
        return PIKA_REPL_RESULT_FATAL_ERROR;
    }
    if (session->state == PIKA_REPL_STATE_EXECUTING) {
        repl_set_diagnostic(
            session, PIKA_REPL_ERROR_RUNTIME,
            PIKA_STATUS_RUNTIME_ERROR, NULL, 0u, 0u,
            session->input_offset, 1);
        return PIKA_REPL_RESULT_RECOVERABLE_ERROR;
    }
    if (session->state == PIKA_REPL_STATE_ENDED) {
        return PIKA_REPL_RESULT_ENDED;
    }
    if (session->state == PIKA_REPL_STATE_FATAL) {
        return PIKA_REPL_RESULT_FATAL_ERROR;
    }
    return repl_interrupt_edit(session);
}

PikaReplResult pika_repl_session_reset(
    PikaReplSession* session) {
    PikaReplConfig config;
    PikaReplIo io;
    uint16_t io_failures;
    if (session == NULL || session->initialized == 0u) {
        return PIKA_REPL_RESULT_FATAL_ERROR;
    }
    config = session->config;
    io = session->io;
    io_failures = session->consecutive_io_failures;
    pika_execution_state_reset(
        &session->execution, config.storage,
        config.binding_storage);
    pika_compiled_module_destroy(session->module);
    memset(session, 0, sizeof(*session));
    session->config = config;
    session->io = io;
    session->initialized = 1u;
    session->state = PIKA_REPL_STATE_READY;
    session->consecutive_io_failures = io_failures;
    repl_clear_diagnostic(session);
    if (repl_emit_primary_prompt(session) >=
        PIKA_REPL_RESULT_RECOVERABLE_ERROR) {
        return session->state == PIKA_REPL_STATE_FATAL
                   ? PIKA_REPL_RESULT_FATAL_ERROR
                   : PIKA_REPL_RESULT_RECOVERABLE_ERROR;
    }
    return PIKA_REPL_RESULT_CONSUMED;
}

void pika_repl_session_deinit(PikaReplSession* session) {
    if (session == NULL || session->initialized == 0u) {
        return;
    }
    pika_execution_state_reset(
        &session->execution, session->config.storage,
        session->config.binding_storage);
    pika_compiled_module_destroy(session->module);
    memset(session, 0, sizeof(*session));
}

PikaReplState pika_repl_session_state(
    const PikaReplSession* session) {
    return session == NULL || session->initialized == 0u
               ? PIKA_REPL_STATE_FATAL
               : session->state;
}

const PikaReplDiagnostic* pika_repl_last_diagnostic(
    const PikaReplSession* session) {
    return session == NULL || session->initialized == 0u
               ? NULL
               : &session->diagnostic;
}

#endif
