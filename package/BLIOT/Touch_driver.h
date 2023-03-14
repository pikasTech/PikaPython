/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GT911_H_
#define __GT911_H_

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
/* Exported macro ------------------------------------------------------------*/
// GOODIX REGISTER ADDRESSES
//
#define GOODIX_ADDRESS 0x5D

// Write only registers
#define GOODIX_REG_COMMAND 0x8040
#define GOODIX_REG_ESD_CHECK 0x8041
#define GOODIX_REG_PROXIMITY_EN 0x8042

// Read/write registers
// The version number of the configuration file
#define GOODIX_REG_CONFIG_DATA 0x8047
// X output maximum value (LSB 2 bytes)
#define GOODIX_REG_MAX_X 0x8048
// Y output maximum value (LSB 2 bytes)
#define GOODIX_REG_MAX_Y 0x804A
// Maximum number of output contacts: 1~5 (4 bit value 3:0, 7:4 is reserved)
#define GOODIX_REG_MAX_TOUCH 0x804C

// Module switch 1
// 7:6 Reserved, 5:4 Stretch rank, 3 X2Y, 2 SITO (Single sided ITO touch
// screen), 1:0 INT Trigger mode */
#define GOODIX_REG_MOD_SW1 0x804D
// Module switch 2
// 7:1 Reserved, 0 Touch key */
#define GOODIX_REG_MOD_SW2 0x804E

// Number of debuffs fingers press/release
#define GOODIX_REG_SHAKE_CNT 0x804F

// X threshold
#define GOODIX_REG_X_THRESHOLD 0x8057

// Configuration update fresh
#define GOODIX_REG_CONFIG_FRESH 0x8100

// ReadOnly registers (device and coordinates info)
// Product ID (LSB 4 bytes, GT9110: 0x06 0x00 0x00 0x09)
#define GOODIX_REG_ID 0x8140
// Firmware version (LSB 2 bytes)
#define GOODIX_REG_FW_VER 0x8144

// Current output X resolution (LSB 2 bytes)
#define GOODIX_READ_X_RES 0x8146
// Current output Y resolution (LSB 2 bytes)
#define GOODIX_READ_Y_RES 0x8148
// Module vendor ID
#define GOODIX_READ_VENDOR_ID 0x814A

#define GOODIX_READ_COORD_ADDR 0x814E

#define GOODIX_POINT1_X_ADDR 0x8150
#define GOODIX_POINT1_Y_ADDR 0x8152

/* Commands for REG_COMMAND */
// 0: read coordinate state
#define GOODIX_CMD_READ 0x00
// 1: difference value original value
#define GOODIX_CMD_DIFFVAL 0x01
// 2: software reset
#define GOODIX_CMD_SOFTRESET 0x02
// 3: Baseline update
#define GOODIX_CMD_BASEUPDATE 0x03
// 4: Benchmark calibration
#define GOODIX_CMD_CALIBRATE 0x04
// 5: Off screen (send other invalid)
#define GOODIX_CMD_SCREEN_OFF 0x05

/* Exported types ------------------------------------------------------------*/
typedef enum {
    GT911_OK = 0,
    GT911_Error = 1,
    GT911_NotResponse = 2
} GT911_Status_t;

typedef struct {
    uint16_t X_Resolution;
    uint16_t Y_Resolution;
    uint8_t Number_Of_Touch_Support;
    bool ReverseX;
    bool ReverseY;
    bool SwithX2Y;
    bool SoftwareNoiseReduction;

} GT911_Config_t;

typedef struct {
    uint16_t x;
    uint16_t y;
} TouchCordinate_t;

/* Exported constants --------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
GT911_Status_t GT911_Init(GT911_Config_t config);
GT911_Status_t GT911_ReadTouch(TouchCordinate_t* cordinate,
                               uint8_t* number_of_cordinate);

// User method implementation prototype
// ----------------------------------------*/
void GT911_INT_Input(void);
void GT911_INT_Output(void);
void GT911_RST_Control(bool high_or_low);
void GT911_INT_Control(bool high_or_low);
void GT911_Delay(uint16_t ms);
GT911_Status_t GT911_I2C_Init(void);
GT911_Status_t GT911_I2C_Write(uint8_t Addr,
                               uint8_t* write_data,
                               uint16_t write_length);
GT911_Status_t GT911_I2C_Read(uint8_t Addr,
                              uint8_t* read_data,
                              uint16_t read_length);

#endif /* __GT911_H_ */