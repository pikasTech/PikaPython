/* Includes ------------------------------------------------------------------*/
#include "Touch_driver.h"
#include <FreeRTOS.h>
#include <string.h>
#include <task.h>
#include "../PikaStdDevice/pika_hal.h"
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static uint8_t GT911_Config[] = {
    0x81, 0x00, 0x04, 0x58, 0x02, 0x0A, 0x0C,
    0x20, 0x01, 0x08, 0x28, 0x05, 0x50,  // 0x8047 - 0x8053
    0x3C, 0x0F, 0x05, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // 0x8054 - 0x8060
    0x00, 0x89, 0x2A, 0x0B, 0x2D, 0x2B, 0x0F,
    0x0A, 0x00, 0x00, 0x01, 0xA9, 0x03,  // 0x8061 - 0x806D
    0x2D, 0x00, 0x01, 0x00, 0x00, 0x00, 0x03,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x21,  // 0x806E - 0x807A
    0x59, 0x94, 0xC5, 0x02, 0x07, 0x00, 0x00,
    0x04, 0x93, 0x24, 0x00, 0x7D, 0x2C,  // 0x807B - 0x8087
    0x00, 0x6B, 0x36, 0x00, 0x5D, 0x42, 0x00,
    0x53, 0x50, 0x00, 0x53, 0x00, 0x00,  // 0x8088	- 0x8094
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // 0x8095 - 0x80A1
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // 0x80A2 - 0x80AD
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x02, 0x04, 0x06, 0x08, 0x0A,  // 0x80AE - 0x80BA
    0x0C, 0x0E, 0x10, 0x12, 0x14, 0x16, 0xFF,
    0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,  // 0x80BB - 0x80C7
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // 0x80C8 - 0x80D4
    0x02, 0x04, 0x06, 0x08, 0x0A, 0x0F, 0x10,
    0x12, 0x16, 0x18, 0x1C, 0x1D, 0x1E,  // 0x80D5 - 0x80E1
    0x1F, 0x20, 0x21, 0x22, 0x24, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,  // 0x80E2 - 0x80EE
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // 0x80EF - 0x80FB
    0x00, 0x00, 0xD6, 0x01};             // 0x80FC - 0x8100
static GT911_Status_t CommunicationResult;
static uint8_t TxBuffer[200];
static uint8_t RxBuffer[200] = {0};
/* Private function prototypes -----------------------------------------------*/
static void GT911_Reset(void);
static void GT911_CalculateCheckSum(void);
static GT911_Status_t GT911_SetCommandRegister(uint8_t command);
static GT911_Status_t GT911_GetProductID(uint32_t* id);
static GT911_Status_t GT911_SendConfig(void);
static GT911_Status_t GT911_GetStatus(uint8_t* status);
static GT911_Status_t GT911_SetStatus(uint8_t status);

GT911_Status_t GT911_I2C_Read_Mem(uint8_t Addr,
                                  uint16_t mem_addr,
                                  uint8_t* read_data,
                                  uint16_t read_length);

GT911_Status_t GT911_I2C_Write_Mem(uint8_t Addr,
                                   uint16_t mem_addr,
                                   uint8_t* write_data,
                                   uint16_t write_length);
/* API Implementation
 * --------------------------------------------------------*/
GT911_Status_t GT911_Init(GT911_Config_t config) {
    //	Set X resolution
    GT911_Config[1] = config.X_Resolution & 0x00FF;
    GT911_Config[2] = (config.X_Resolution >> 8) & 0x00FF;
    //	Set Y resolution
    GT911_Config[3] = config.Y_Resolution & 0x00FF;
    GT911_Config[4] = (config.Y_Resolution >> 8) & 0x00FF;
    //  Set touch number
    GT911_Config[5] = config.Number_Of_Touch_Support;
    //  set reverse Y
    GT911_Config[6] = 0;
    GT911_Config[6] |= config.ReverseY << 7;
    //  set reverse X
    GT911_Config[6] |= config.ReverseX << 6;
    //  set switch X2Y
    GT911_Config[6] |= config.SwithX2Y << 3;
    //  set Sito
    GT911_Config[6] |= config.SoftwareNoiseReduction << 2;

    // Reset chip
    GT911_Reset();
    // Get product ID
    uint32_t productID = 0;
    CommunicationResult = GT911_GetProductID(&productID);
    __platform_printf("GT911 ID:%d\r\n", productID);
    // Reset chip
    GT911_Reset();
    CommunicationResult = GT911_SendConfig();
    if (CommunicationResult != GT911_OK) {
        return CommunicationResult;
    }
    GT911_SetCommandRegister(0x00);
    return GT911_OK;
}

GT911_Status_t GT911_ReadTouch(TouchCordinate_t* cordinate,
                               uint8_t* number_of_cordinate) {
    uint8_t StatusRegister;
    GT911_Status_t Result = GT911_NotResponse;
    Result = GT911_GetStatus(&StatusRegister);
    if (Result != GT911_OK) {
        return Result;
    }
    if ((StatusRegister & 0x80) != 0) {
        *number_of_cordinate = StatusRegister & 0x0F;
        if (*number_of_cordinate != 0) {
            for (uint8_t i = 0; i < *number_of_cordinate; i++) {
                TxBuffer[0] = ((GOODIX_POINT1_X_ADDR + (i * 8)) & 0xFF00) >> 8;
                TxBuffer[1] = (GOODIX_POINT1_X_ADDR + (i * 8)) & 0xFF;
                GT911_I2C_Write(GOODIX_ADDRESS, TxBuffer, 2);
                GT911_I2C_Read(GOODIX_ADDRESS, RxBuffer, 6);
                cordinate[i].x = RxBuffer[0];
                cordinate[i].x = (RxBuffer[1] << 8) + cordinate[i].x;
                cordinate[i].y = RxBuffer[2];
                cordinate[i].y = (RxBuffer[3] << 8) + cordinate[i].y;
            }
        }
        GT911_SetStatus(0);
    }
    return GT911_OK;
}

// Private functions Implementation
// ---------------------------------------------------------*/
static void GT911_Reset(void) {
    GT911_INT_Output();
    GT911_RST_Control(0);
    GT911_Delay(20);
    GT911_INT_Control(0);
    GT911_Delay(50);
    GT911_RST_Control(1);
    GT911_Delay(100);
    GT911_INT_Input();
    GT911_Delay(100);
}

static void GT911_CalculateCheckSum(void) {
    GT911_Config[184] = 0;
    for (uint8_t i = 0; i < 184; i++) {
        GT911_Config[184] += GT911_Config[i];
    }
    GT911_Config[184] = (~GT911_Config[184]) + 1;
}

static GT911_Status_t GT911_SetCommandRegister(uint8_t command) {
    TxBuffer[0] = (GOODIX_REG_COMMAND & 0xFF00) >> 8;
    TxBuffer[1] = GOODIX_REG_COMMAND & 0xFF;
    TxBuffer[2] = command;
    return GT911_I2C_Write(GOODIX_ADDRESS, TxBuffer, 3);
}

static GT911_Status_t GT911_GetProductID(uint32_t* id) {
    TxBuffer[0] = (GOODIX_REG_ID & 0xFF00) >> 8;
    TxBuffer[1] = GOODIX_REG_ID & 0xFF;
    GT911_Status_t Result = GT911_NotResponse;
    Result = GT911_I2C_Write(GOODIX_ADDRESS, TxBuffer, 2);
    if (Result == GT911_OK) {
        Result = GT911_I2C_Read(GOODIX_ADDRESS, RxBuffer, 4);
        if (Result == GT911_OK) {
            memcpy(id, RxBuffer, 4);
        }
    }
    return Result;
}

static GT911_Status_t GT911_SendConfig(void) {
    GT911_CalculateCheckSum();
    TxBuffer[0] = (GOODIX_REG_CONFIG_DATA & 0xFF00) >> 8;
    TxBuffer[1] = GOODIX_REG_CONFIG_DATA & 0xFF;
    memcpy(&TxBuffer[2], GT911_Config, sizeof(GT911_Config));
    return GT911_I2C_Write(GOODIX_ADDRESS, TxBuffer, sizeof(GT911_Config) + 2);
}

static GT911_Status_t GT911_GetStatus(uint8_t* status) {
    TxBuffer[0] = (GOODIX_READ_COORD_ADDR & 0xFF00) >> 8;
    TxBuffer[1] = GOODIX_READ_COORD_ADDR & 0xFF;
    GT911_Status_t Result = GT911_NotResponse;
    Result = GT911_I2C_Write(GOODIX_ADDRESS, TxBuffer, 2);
    if (Result == GT911_OK) {
        Result = GT911_I2C_Read(GOODIX_ADDRESS, RxBuffer, 1);
        if (Result == GT911_OK) {
            *status = RxBuffer[0];
        }
    }
    return Result;
}

static GT911_Status_t GT911_SetStatus(uint8_t status) {
    TxBuffer[0] = (GOODIX_READ_COORD_ADDR & 0xFF00) >> 8;
    TxBuffer[1] = GOODIX_READ_COORD_ADDR & 0xFF;
    TxBuffer[2] = status;
    return GT911_I2C_Write(GOODIX_ADDRESS, TxBuffer, 3);
}

static pika_dev* TOUCH_INT = NULL;
static pika_dev* TOUCH_RST = NULL;
static pika_dev* TOUCH_IIC = NULL;

void GT911_INT_Input(void) {
    if (NULL == TOUCH_INT) {
        TOUCH_INT = pika_hal_open(PIKA_HAL_GPIO, "P17");
    }
    pika_hal_GPIO_config cfg = {0};
    cfg.dir = PIKA_HAL_GPIO_DIR_IN;
    cfg.pull = PIKA_HAL_GPIO_PULL_NONE;
    pika_hal_ioctl(TOUCH_INT, PIKA_HAL_IOCTL_CONFIG, &cfg);
    pika_hal_ioctl(TOUCH_INT, PIKA_HAL_IOCTL_ENABLE);
}

void GT911_INT_Output(void) {
    if (NULL == TOUCH_INT) {
        TOUCH_INT = pika_hal_open(PIKA_HAL_GPIO, "P17");
    }
    pika_hal_GPIO_config cfg = {0};
    cfg.dir = PIKA_HAL_GPIO_DIR_OUT;
    cfg.pull = PIKA_HAL_GPIO_PULL_NONE;
    pika_hal_ioctl(TOUCH_INT, PIKA_HAL_IOCTL_CONFIG, &cfg);
    pika_hal_ioctl(TOUCH_INT, PIKA_HAL_IOCTL_ENABLE);
}

void GT911_RST_Control(bool high_or_low) {
    if (NULL == TOUCH_RST) {
        TOUCH_RST = pika_hal_open(PIKA_HAL_GPIO, "P5");
        pika_hal_GPIO_config cfg = {0};
        cfg.dir = PIKA_HAL_GPIO_DIR_OUT;
        pika_hal_ioctl(TOUCH_RST, PIKA_HAL_IOCTL_CONFIG, &cfg);
        pika_hal_ioctl(TOUCH_RST, PIKA_HAL_IOCTL_ENABLE);
    }
    uint32_t val = 0;
    switch (high_or_low) {
        case true:
            val = 1;
            pika_hal_write(TOUCH_RST, &val, sizeof(uint32_t));
            break;
        case false:
            val = 0;
            pika_hal_write(TOUCH_RST, &val, sizeof(uint32_t));
            break;
    }
}

void GT911_INT_Control(bool high_or_low) {
    if (NULL == TOUCH_INT) {
        GT911_INT_Output();
    }
    uint32_t val = 0;
    switch (high_or_low) {
        case true:
            val = 1;
            pika_hal_write(TOUCH_INT, &val, sizeof(uint32_t));
            break;
        case false:
            val = 0;
            pika_hal_write(TOUCH_INT, &val, sizeof(uint32_t));
            break;
    }
}

void GT911_Delay(uint16_t ms) {
    // HAL_Delay(ms);
    vTaskDelay(ms / portTICK_PERIOD_MS);
}

GT911_Status_t GT911_I2C_Init(void) {
    if (NULL == TOUCH_IIC) {
        TOUCH_IIC = pika_hal_open(PIKA_HAL_IIC, "IIC3");
        pika_hal_IIC_config cfg = {0};
        cfg.slave_addr = GOODIX_ADDRESS;
        cfg.speed = 400000;
        cfg.timeout = 10000;
        pika_hal_ioctl(TOUCH_IIC, PIKA_HAL_IOCTL_CONFIG, &cfg);
        pika_hal_ioctl(TOUCH_IIC, PIKA_HAL_IOCTL_ENABLE);
    }
    return GT911_OK;
}

GT911_Status_t GT911_I2C_Write(uint8_t Addr,
                               uint8_t* write_data,
                               uint16_t write_length) {
    if (NULL == TOUCH_IIC) {
        GT911_I2C_Init();
    }
    pika_hal_IIC_config cfg = {0};
    cfg.slave_addr = Addr;
    cfg.mem_addr_ena = PIKA_HAL_IIC_MEM_ADDR_ENA_DISABLE;
    pika_hal_ioctl(TOUCH_IIC, PIKA_HAL_IOCTL_CONFIG, &cfg);
    pika_hal_write(TOUCH_IIC, write_data, write_length);
    return GT911_OK;
}

GT911_Status_t GT911_I2C_Write_Mem(uint8_t Addr,
                                   uint16_t mem_addr,
                                   uint8_t* write_data,
                                   uint16_t write_length) {
    if (NULL == TOUCH_IIC) {
        GT911_I2C_Init();
    }
    pika_hal_IIC_config cfg = {0};
    cfg.slave_addr = Addr;
    cfg.mem_addr = mem_addr;
    cfg.mem_addr_ena = PIKA_HAL_IIC_MEM_ADDR_ENA_ENABLE;
    cfg.mem_addr_size = sizeof(uint16_t);
    pika_hal_ioctl(TOUCH_IIC, PIKA_HAL_IOCTL_CONFIG, &cfg);
    pika_hal_write(TOUCH_IIC, write_data, write_length);
    return GT911_OK;
}

GT911_Status_t GT911_I2C_Read(uint8_t Addr,
                              uint8_t* read_data,
                              uint16_t read_length) {
    if (NULL == TOUCH_IIC) {
        GT911_I2C_Init();
    }
    pika_hal_IIC_config cfg = {0};
    cfg.slave_addr = Addr;
    cfg.mem_addr_ena = PIKA_HAL_IIC_MEM_ADDR_ENA_DISABLE;
    pika_hal_ioctl(TOUCH_IIC, PIKA_HAL_IOCTL_CONFIG, &cfg);
    pika_hal_read(TOUCH_IIC, read_data, read_length);
    return GT911_OK;
}

GT911_Status_t GT911_I2C_Read_Mem(uint8_t Addr,
                                  uint16_t mem_addr,
                                  uint8_t* read_data,
                                  uint16_t read_length) {
    if (NULL == TOUCH_IIC) {
        GT911_I2C_Init();
    }
    pika_hal_IIC_config cfg = {0};
    cfg.slave_addr = Addr;
    cfg.mem_addr = mem_addr;
    cfg.mem_addr_ena = PIKA_HAL_IIC_MEM_ADDR_ENA_ENABLE;
    cfg.mem_addr_size = sizeof(uint16_t);
    pika_hal_ioctl(TOUCH_IIC, PIKA_HAL_IOCTL_CONFIG, &cfg);
    pika_hal_read(TOUCH_IIC, read_data, read_length);
    return GT911_OK;
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF
 * FILE****/