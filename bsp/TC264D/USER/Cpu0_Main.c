/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2020,逐飞科技
 * All rights reserved.
 * 技术讨论QQ群：三群：824575535
 *
 * 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
 * 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
 *
 * @file       		main
 * @company	   		成都逐飞科技有限公司
 * @author     		逐飞科技(QQ3184284598)
 * @version    		查看doc内version文件 版本说明
 * @Software 		ADS v1.2.2
 * @Target core		TC264D
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2020-3-23
 ********************************************************************************************************************/


#include "headfile.h"
#include "pikascript.h"
#pragma section all "cpu0_dsram"
//将本语句与#pragma section all restore语句之间的全局变量都放在CPU0的RAM中


//工程导入到软件之后，应该选中工程然后点击refresh刷新一下之后再编译
//工程默认设置为关闭优化，可以自己右击工程选择properties->C/C++ Build->Setting
//然后在右侧的窗口中找到C/C++ Compiler->Optimization->Optimization level处设置优化等级
//一般默认新建立的工程都会默认开2级优化，因此大家也可以设置为2级优化

//对于TC系列默认是不支持中断嵌套的，希望支持中断嵌套需要在中断内使用enableInterrupts();来开启中断嵌套
//简单点说实际上进入中断后TC系列的硬件自动调用了disableInterrupts();来拒绝响应任何的中断，因此需要我们自己手动调用enableInterrupts();来开启中断的响应。

uint8 uart_buff;

int core0_main(void)
{
	get_clk();//获取时钟频率  务必保留
	//用户在此处调用各种初始化函数等
    uart_init(UART_0, 115200, UART0_TX_P14_0, UART0_RX_P14_1);
    enableInterrupts();
	uart_putstr(UART_0, "\n---uart test---\n");
	gpio_init(P20_8, GPO, 0, PUSHPULL);
    //在本库中，对于串口的接收与发送都是通过中断完成的，因此想要正常的使用串口功能务必保证中断是开启的，也就是调用了enableInterrupts()
	//串口的中断函数全部都在isr.c中。

	//串口中断相关的配置参数都在isr_config.h中
	//可配置参数有 ERU_CH0_CH4_INT_SERVICE 和 ERU_CH0_CH4_INT_PRIO
	//UART0_INT_SERVICE 中断服务者，表示改中断由谁处理，0:CPU0 1:CPU1 3:DMA  不可设置为其他值
	//UART0_TX_INT_PRIO 发送中断优先级 优先级范围1-255 越大优先级越高 与平时使用的单片机不一样
	//UART0_RX_INT_PRIO	接收中断优先级
	//UART0_ER_INT_PRIO 错误中断优先级
    
    //其他串口中断参数同理

	//需要特备注意的是  不可以有优先级相同的中断函数 每个中断的优先级都必须是不一样的
    
    //等待所有核心初始化完毕
	IfxCpu_emitEvent(&g_cpuSyncEvent);
	IfxCpu_waitEvent(&g_cpuSyncEvent, 0xFFFF);
	enableInterrupts();
	// PikaObj* pikaMain = pikaScriptInit();
	pikaScriptShell(pikaScriptInit());
	//注意 从V1.1.6版本之后  printf打印的信息从串口输出具体可以学习库例程6-Printf_Demo
    //注意 从V1.1.6版本之后  printf打印的信息从串口输出具体可以学习库例程6-Printf_Demo
    //注意 从V1.1.6版本之后  printf打印的信息从串口输出具体可以学习库例程6-Printf_Demo
	while (TRUE)
	{
		//查询是否有数据，如果有则接收
    	if(uart_query(UART_0, &uart_buff))
    	{
    		//将收到的数据，再发出去
    		uart_putchar(UART_0, uart_buff);
    		gpio_toggle(P20_8);
    	}

    	//使用usb转ttl连接单片机的串口，在电脑上打开串口助手，任意发送字符，
    	//单片机收到后会返回发送的字符，串口助手就会显示刚才发送的东西
	}
}

char __platform_getchar()
{
    while(TRUE){
        if(uart_query(UART_0, &uart_buff))
        {
            return uart_buff;
        }
    }
}

#pragma section all restore


