#include "SWM320.h"

void serial_init(void)
{
    UART_InitStructure UART_initStruct;

    PORT_Init(PORTC, PIN2, FUNMUX0_UART0_RXD, 1); //GPIOC.2配置为UART0输入引脚
    PORT_Init(PORTC, PIN3, FUNMUX1_UART0_TXD, 0); //GPIOC.3配置为UART0输出引脚

    UART_initStruct.Baudrate = 115200;
    UART_initStruct.DataBits = UART_DATA_8BIT;
    UART_initStruct.Parity = UART_PARITY_NONE;
    UART_initStruct.StopBits = UART_STOP_1BIT;
    UART_initStruct.RXThresholdIEn = 0;
    UART_initStruct.TXThresholdIEn = 0;
    UART_initStruct.TimeoutIEn = 0;
    UART_Init(UART0, &UART_initStruct);
    UART_Open(UART0);
}

/****************************************************************************************************************************************** 
* 函数名称: fputc()
* 功能说明: printf()使用此函数完成实际的串口打印动作
* 输    入: int ch  要打印的字符
*           FILE *f 文件句柄
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
int fputc(int ch, FILE *f)
{
    UART_WriteByte(UART0, ch);

    while (UART_IsTXBusy(UART0))
        ;

    return ch;
}
