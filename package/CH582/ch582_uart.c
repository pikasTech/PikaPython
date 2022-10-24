#include "CH58x_common.h"
#include "CH582_UART.h"
#include "ch582_utils.h"

void UART0_DeInit(void)
{
    R8_UART0_FCR = 0x0;
    R8_UART0_LCR = 0x0;
    R8_UART0_IER = 0x0;
}

void UART1_DeInit(void)
{
    R8_UART1_FCR = 0x0;
    R8_UART1_LCR = 0x0;
    R8_UART1_IER = 0x0;
}

void UART2_DeInit(void)
{
    R8_UART2_FCR = 0x0;
    R8_UART2_LCR = 0x0;
    R8_UART2_IER = 0x0;
}

void UART3_DeInit(void)
{
    R8_UART3_FCR = 0x0;
    R8_UART3_LCR = 0x0;
    R8_UART3_IER = 0x0;
}

void CH582_UART_platformEnable(PikaObj *self)
{
    int id = obj_getInt(self, "id");
    int baudrate = obj_getInt(self, "baudRate");
    switch (id)
    {
    case 0: // TX0:PB7 RX0:PB4
        GPIOB_SetBits(GPIO_Pin_7);
        GPIOB_ModeCfg(GPIO_Pin_4, GPIO_ModeIN_PU);
        GPIOB_ModeCfg(GPIO_Pin_7, GPIO_ModeOut_PP_5mA);
        UART0_DefInit();
        UART0_BaudRateCfg(baudrate);
        break;
    case 1: // TX1:PA9 RX1:pa8
        GPIOA_SetBits(GPIO_Pin_9);
        GPIOA_ModeCfg(GPIO_Pin_8, GPIO_ModeIN_PU);
        GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_5mA);
        UART1_DefInit();
        UART1_BaudRateCfg(baudrate);
        break;
    case 2: // TX2:PA7 RX2:PA6
        GPIOA_SetBits(GPIO_Pin_7);
        GPIOA_ModeCfg(GPIO_Pin_6, GPIO_ModeIN_PU);
        GPIOA_ModeCfg(GPIO_Pin_7, GPIO_ModeOut_PP_5mA);
        UART2_DefInit();
        UART2_BaudRateCfg(baudrate);
        break;
    case 3: // TX3:PA5 RX3:PA4
        GPIOA_SetBits(GPIO_Pin_5);
        GPIOA_ModeCfg(GPIO_Pin_4, GPIO_ModeIN_PU);
        GPIOA_ModeCfg(GPIO_Pin_5, GPIO_ModeOut_PP_5mA);
        UART3_DefInit();
        UART3_BaudRateCfg(baudrate);
    default:
        break;
    }
}

void CH582_UART_platformRead(PikaObj *self)
{
    int id = obj_getInt(self, "id");
    int len = obj_getInt(self, "length");
    char *readData = NULL;

    switch (id)
    {
    case 0:
        while (len > 0)
        {
            *readData++ = R8_UART0_RBR;
            len--;
        }
        break;
    case 1:
        while (len > 0)
        {
            *readData++ = R8_UART1_RBR;
            len--;
        }
        break;
    case 2:
        while (len > 0)
        {
            *readData++ = R8_UART2_RBR;
            len--;
        }
        break;
    case 3:
        while (len > 0)
        {
            *readData++ = R8_UART3_RBR;
            len--;
        }
        break;
    default:
        break;
    }
    obj_setBytes(self, "readData", readData, len);
}

void CH582_UART_platformWrite(PikaObj *self)
{
    int id = obj_getInt(self, "id");
    char *data = NULL;
    size_t len = obj_loadBytes(self, "writeData", data);
    switch (id)
    {
    case 0:
        UART0_SendString(data, strGetSize(data));
        break;
    case 1:
        UART1_SendString(data, strGetSize(data));
        break;
    case 2:
        UART2_SendString(data, strGetSize(data));
        break;
    case 3:
        UART3_SendString(data, strGetSize(data));
        break;
    default:
        break;
    }
}

void CH582_UART_platformDisable(PikaObj *self)
{
    int id = obj_getInt(self, "id");
    switch (id)
    {
    case 0:
        UART0_DeInit();
        GPIOB_ModeCfg(GPIO_Pin_4, GPIO_ModeIN_PU);
        GPIOB_ModeCfg(GPIO_Pin_7, GPIO_ModeIN_PU);
        break;
    case 1:
        UART1_DeInit();
        GPIOA_ModeCfg(GPIO_Pin_8, GPIO_ModeIN_PU);
        GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeIN_PU);
        break;
    case 2:
        UART2_DeInit();
        GPIOA_ModeCfg(GPIO_Pin_6, GPIO_ModeIN_PU);
        GPIOA_ModeCfg(GPIO_Pin_7, GPIO_ModeIN_PU);
        break;
    case 3:
        UART3_DeInit();
        GPIOA_ModeCfg(GPIO_Pin_4, GPIO_ModeIN_PU);
        GPIOA_ModeCfg(GPIO_Pin_5, GPIO_ModeIN_PU);
        break;
    default:
        break;
    }
}