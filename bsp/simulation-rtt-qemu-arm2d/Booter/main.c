/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-08-19     RT-Thread    first version
 */

#include <rtthread.h>

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>
#include <stdio.h>
#include <stdint.h>
#include "pikaScript.h"
#include "dataStrs.h"
#include "Arm2D_common.h"

#define SAMPLE_UART_NAME       "uart1"    /* 串口设备名称 */
static rt_device_t serial; /* 串口设备句柄 */
static struct rt_semaphore rx_sem; /* 用于接收消息的信号量 */

#define RX_Buff_SIZE 256
Args *rxArgs;
uint8_t isRxOk = 0;
PikaObj *pikaMain;

void rx_single_line_handle(char *line) {

}

/* 接收数据回调函数 */
static rt_err_t uart_input_callback(rt_device_t dev, rt_size_t size) {

}

static void clearBuff(char *buff, uint32_t size) {
	for (int i = 0; i < size; i++) {
		buff[i] = 0;
	}
}

static void uart_init() {
	serial = rt_device_find(SAMPLE_UART_NAME);

	/* 以中断接收及轮询发送模式打开串口设备 */
	rt_device_open(serial, RT_DEVICE_FLAG_INT_RX);

	/* 初始化信号量 */
	rt_sem_init(&rx_sem, "rx_sem", 0, RT_IPC_FLAG_FIFO);

	/* 设置接收回调函数 */
	rt_device_set_rx_indicate(serial, uart_input_callback);
}

int main(void) {
	uart_init();
	pikaMain = pikaScriptInit();

	while (1) {

	}
	return RT_EOK;
}
