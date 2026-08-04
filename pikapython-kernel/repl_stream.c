/* SPEC: PJ2026-050106 REPL v0.3; stream event adapter. */
#include "pika_repl.h"

#if !PIKA_REPL_ENABLE
typedef unsigned char PikaReplDisabledTranslationUnit;
#endif

#if PIKA_REPL_ENABLE

PikaReplResult pika_repl_stream_accept(
    PikaReplSession* session,
    PikaReplStreamEvent event,
    const uint8_t* bytes,
    size_t length) {
    if (event == PIKA_REPL_STREAM_BYTES) {
        return pika_repl_feed(session, bytes, length);
    }
    if (bytes != NULL || length != 0u) {
        return PIKA_REPL_RESULT_FATAL_ERROR;
    }
    if (event == PIKA_REPL_STREAM_END) {
        return pika_repl_end_input(session);
    }
    if (event == PIKA_REPL_STREAM_INTERRUPT) {
        return pika_repl_request_interrupt(session);
    }
    return PIKA_REPL_RESULT_FATAL_ERROR;
}

#endif
