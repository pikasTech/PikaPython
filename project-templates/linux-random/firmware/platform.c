#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#include "pika_platform.h"

static uint32_t pika_linux_time_ticks = 1000u;
static struct termios pika_linux_input_mode;
static int pika_linux_input_mode_saved;

static void pika_linux_restore_terminal(void) {
    if (pika_linux_input_mode_saved) {
        (void)tcsetattr(STDIN_FILENO, TCSANOW, &pika_linux_input_mode);
        pika_linux_input_mode_saved = 0;
    }
}

static PikaStatus pika_linux_prepare_terminal(void) {
    struct termios mode;
    if (!isatty(STDIN_FILENO) || pika_linux_input_mode_saved) {
        return PIKA_STATUS_OK;
    }
    if (tcgetattr(STDIN_FILENO, &mode) != 0) {
        return PIKA_STATUS_IO_ERROR;
    }
    pika_linux_input_mode = mode;
    mode.c_iflag &= (tcflag_t)~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    mode.c_oflag &= (tcflag_t)~OPOST;
    mode.c_cflag |= CS8;
    mode.c_lflag &= (tcflag_t)~(ECHO | ICANON | IEXTEN | ISIG);
    mode.c_cc[VMIN] = 1;
    mode.c_cc[VTIME] = 0;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &mode) != 0) {
        return PIKA_STATUS_IO_ERROR;
    }
    pika_linux_input_mode_saved = 1;
    (void)atexit(pika_linux_restore_terminal);
    return PIKA_STATUS_OK;
}

PikaStatus pika_platform_write(const char* data, size_t size) {
    if (data == NULL && size != 0u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (size > 0u && fwrite(data, 1u, size, stdout) != size) {
        return PIKA_STATUS_IO_ERROR;
    }
    (void)fflush(stdout);
    return PIKA_STATUS_OK;
}

PikaStatus pika_platform_repl_read(
    uint8_t* data,
    size_t capacity,
    size_t* received) {
    int value;
    if (data == NULL || received == NULL || capacity == 0u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (pika_linux_prepare_terminal() != PIKA_STATUS_OK) {
        return PIKA_STATUS_IO_ERROR;
    }
    value = fgetc(stdin);
    if (value == EOF) {
        *received = 0u;
        return PIKA_STATUS_IO_ERROR;
    }
    data[0] = (uint8_t)value;
    *received = 1u;
    return PIKA_STATUS_OK;
}

PikaStatus pika_platform_time_ticks_ms(uint32_t* ticks) {
    if (ticks == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    *ticks = pika_linux_time_ticks;
    pika_linux_time_ticks += 5u;
    return PIKA_STATUS_OK;
}

PikaStatus pika_platform_time_sleep_ms(uint32_t duration) {
    pika_linux_time_ticks += duration;
    return PIKA_STATUS_OK;
}
