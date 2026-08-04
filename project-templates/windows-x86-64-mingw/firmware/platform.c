#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "pika_platform.h"

static HANDLE pika_windows_input_handle;
static DWORD pika_windows_input_mode;
static int pika_windows_input_mode_saved;
static uint8_t pika_windows_pending[8];
static size_t pika_windows_pending_offset;
static size_t pika_windows_pending_size;
static WCHAR pika_windows_high_surrogate;

static void pika_windows_restore_console(void) {
    if (pika_windows_input_mode_saved) {
        (void)SetConsoleMode(
            pika_windows_input_handle, pika_windows_input_mode);
        pika_windows_input_mode_saved = 0;
    }
}

static PikaStatus pika_windows_prepare_console(void) {
    DWORD mode;
    HANDLE input = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
    if (input != INVALID_HANDLE_VALUE &&
        GetConsoleMode(input, &mode) != 0) {
        (void)SetConsoleCP(CP_UTF8);
        if (!pika_windows_input_mode_saved) {
            pika_windows_input_handle = input;
            pika_windows_input_mode = mode;
            pika_windows_input_mode_saved = 1;
            (void)atexit(pika_windows_restore_console);
        }
        mode &= (DWORD)~(
            ENABLE_ECHO_INPUT |
            ENABLE_LINE_INPUT |
            ENABLE_PROCESSED_INPUT);
        if (SetConsoleMode(input, mode) == 0) {
            return PIKA_STATUS_IO_ERROR;
        }
    }
    if (output != INVALID_HANDLE_VALUE &&
        GetConsoleMode(output, &mode) != 0) {
        (void)SetConsoleOutputCP(CP_UTF8);
    }
    return PIKA_STATUS_OK;
}

static void pika_windows_queue_bytes(
    const uint8_t* bytes,
    size_t size) {
    memcpy(pika_windows_pending, bytes, size);
    pika_windows_pending_offset = 0u;
    pika_windows_pending_size = size;
}

static int pika_windows_queue_key(const KEY_EVENT_RECORD* key) {
    static const uint8_t up[] = {0x1bu, '[', 'A'};
    static const uint8_t down[] = {0x1bu, '[', 'B'};
    static const uint8_t right[] = {0x1bu, '[', 'C'};
    static const uint8_t left[] = {0x1bu, '[', 'D'};
    uint8_t byte;
    WCHAR wide[2];
    int wide_size = 1;
    int encoded;
    if (!key->bKeyDown) {
        return 0;
    }
    switch (key->wVirtualKeyCode) {
        case VK_UP:
            pika_windows_queue_bytes(up, sizeof(up));
            return 1;
        case VK_DOWN:
            pika_windows_queue_bytes(down, sizeof(down));
            return 1;
        case VK_RIGHT:
            pika_windows_queue_bytes(right, sizeof(right));
            return 1;
        case VK_LEFT:
            pika_windows_queue_bytes(left, sizeof(left));
            return 1;
        case VK_BACK:
            byte = 0x08u;
            pika_windows_queue_bytes(&byte, 1u);
            return 1;
        case VK_DELETE:
            byte = 0x7fu;
            pika_windows_queue_bytes(&byte, 1u);
            return 1;
        case VK_RETURN:
            byte = '\r';
            pika_windows_queue_bytes(&byte, 1u);
            return 1;
        default:
            break;
    }
    if ((key->dwControlKeyState & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)) != 0u &&
        (key->wVirtualKeyCode == 'C' || key->wVirtualKeyCode == 'D')) {
        byte = key->wVirtualKeyCode == 'C' ? 0x03u : 0x04u;
        pika_windows_queue_bytes(&byte, 1u);
        return 1;
    }
    if (key->uChar.UnicodeChar == 0u) {
        return 0;
    }
    wide[0] = key->uChar.UnicodeChar;
    if (wide[0] >= 0xd800u && wide[0] <= 0xdbffu) {
        pika_windows_high_surrogate = wide[0];
        return 0;
    }
    if (wide[0] >= 0xdc00u && wide[0] <= 0xdfffu &&
        pika_windows_high_surrogate != 0u) {
        wide[1] = wide[0];
        wide[0] = pika_windows_high_surrogate;
        pika_windows_high_surrogate = 0u;
        wide_size = 2;
    } else {
        pika_windows_high_surrogate = 0u;
    }
    encoded = WideCharToMultiByte(
        CP_UTF8, WC_ERR_INVALID_CHARS, wide, wide_size,
        (char*)pika_windows_pending,
        (int)sizeof(pika_windows_pending), NULL, NULL);
    if (encoded <= 0) {
        return 0;
    }
    pika_windows_pending_offset = 0u;
    pika_windows_pending_size = (size_t)encoded;
    return 1;
}

PikaStatus pika_platform_write(const char* data, size_t size) {
    static int console_prepared;
    if (data == NULL && size != 0u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (!console_prepared) {
        if (pika_windows_prepare_console() != PIKA_STATUS_OK) {
            return PIKA_STATUS_IO_ERROR;
        }
        console_prepared = 1;
    }
    if (size > 0u && fwrite(data, 1u, size, stdout) != size) {
        return PIKA_STATUS_IO_ERROR;
    }
    return fflush(stdout) == 0 ? PIKA_STATUS_OK : PIKA_STATUS_IO_ERROR;
}

PikaStatus pika_platform_repl_read(
    uint8_t* data,
    size_t capacity,
    size_t* received) {
    DWORD read_bytes = 0u;
    DWORD mode;
    HANDLE input;
    if (data == NULL || received == NULL || capacity == 0u ||
        capacity > (size_t)UINT32_MAX) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (pika_windows_prepare_console() != PIKA_STATUS_OK) {
        return PIKA_STATUS_IO_ERROR;
    }
    input = GetStdHandle(STD_INPUT_HANDLE);
    if (input == NULL || input == INVALID_HANDLE_VALUE) {
        return PIKA_STATUS_IO_ERROR;
    }
    if (pika_windows_pending_offset < pika_windows_pending_size) {
        data[0] = pika_windows_pending[pika_windows_pending_offset++];
        *received = 1u;
        return PIKA_STATUS_OK;
    }
    if (GetConsoleMode(input, &mode) != 0) {
        INPUT_RECORD record;
        for (;;) {
            if (ReadConsoleInputW(input, &record, 1u, &read_bytes) == 0) {
                return PIKA_STATUS_IO_ERROR;
            }
            if (record.EventType == KEY_EVENT &&
                pika_windows_queue_key(&record.Event.KeyEvent)) {
                data[0] = pika_windows_pending[pika_windows_pending_offset++];
                *received = 1u;
                return PIKA_STATUS_OK;
            }
        }
    }
    if (ReadFile(input, data, 1u, &read_bytes, NULL) == 0) {
        if (GetLastError() == ERROR_BROKEN_PIPE) {
            *received = 0u;
            return PIKA_STATUS_OK;
        }
        return PIKA_STATUS_IO_ERROR;
    }
    *received = (size_t)read_bytes;
    return PIKA_STATUS_OK;
}

PikaStatus pika_platform_time_ticks_ms(uint32_t* ticks) {
    if (ticks == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    *ticks = (uint32_t)GetTickCount64();
    return PIKA_STATUS_OK;
}

PikaStatus pika_platform_time_sleep_ms(uint32_t duration) {
    Sleep((DWORD)duration);
    return PIKA_STATUS_OK;
}
