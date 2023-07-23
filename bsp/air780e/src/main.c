#include "common_api.h"
#include "luat_rtos.h"
#include "luat_debug.h"
#include "luat_uart.h"
#include "pikascript.h"

#define USER_UART_ID LUAT_VUART_ID_0
#define RX_BUF_LEN 1024

typedef struct
{
    char buf[RX_BUF_LEN];
    int pos;
    int len;
}rx_buf;

static volatile rx_buf s_rx_buf = {.pos = 0, .len = 0};
static luat_rtos_task_handle pika_main_handle;

void luat_usb_recv_cb(int uart_id, uint32_t data_len){
    s_rx_buf.len += luat_uart_read(uart_id, &s_rx_buf.buf[s_rx_buf.len], (data_len > (1024 - s_rx_buf.len)?(1024 - s_rx_buf.len):data_len));
}

int pika_platform_putchar(char ch) {
    return luat_uart_write(USER_UART_ID, &ch, 1);
}

char pika_platform_getchar(void) {
	while (s_rx_buf.pos >= s_rx_buf.len)
	{
		s_rx_buf.pos = 0;
		s_rx_buf.len = 0;
		luat_rtos_task_sleep(1);
	}

	return s_rx_buf.buf[s_rx_buf.pos++];;
}

static void pika_main(void *param)
{
	PikaObj * pikaMain = pikaScriptInit();

    luat_uart_t uart = {
        .id = USER_UART_ID,
    };

    luat_uart_setup(&uart);

    luat_uart_ctrl(LUAT_VUART_ID_0, LUAT_UART_SET_RECV_CALLBACK, luat_usb_recv_cb);

	while(1)
	{
		pikaScriptShell(pikaMain);
	}
}


void pika_main_init(void)
{
	luat_rtos_task_create(&pika_main_handle, 4 * 1204, 50, "pika_main", pika_main, NULL, 32);
}

INIT_TASK_EXPORT(pika_main_init, "0");
