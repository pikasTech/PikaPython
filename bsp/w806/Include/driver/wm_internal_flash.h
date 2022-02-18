#ifndef __WM_INTERNAL_FLASH_H__
#define __WM_INTERNAL_FLASH_H__

#include "wm_hal.h"

typedef struct
{
	HAL_LockTypeDef             Lock;

} FLASH_ProcessTypeDef;

typedef volatile unsigned char vu8;
typedef volatile unsigned short vu16;
typedef volatile unsigned long vu32;

#define M32(adr)	(*((vu32*) (adr)))

#define FLASH                ((FLASH_TypeDef *)FLASH_BASE)


#define INSIDE_FLS_SECTOR_SIZE	0x1000
#define INSIDE_FLS_PAGE_SIZE	256


/******************************0x81FFFFF
 *            USER PARAM
 * ****************************0x8XXXXXX
 *            RUN IMAGE
 * ****************************0x8010400
 *            RUN IMAGE HEADER
 * ****************************0x8010000
 *            SECBOOT IMAGE
 * ****************************0x8002400
 *            SECBOOT IMAGE HEAER
 * ****************************0x8002000
 *            CHIP DATA
 * ****************************0x8000000*/
 
#define INSIDE_FLS_BASE_ADDR		0x8000000UL
#define INSIDE_FLS_SECBOOT_ADDR 	(INSIDE_FLS_BASE_ADDR  + 0x02000)
#define INSIDE_FLS_IMAGE_ADDR		(INSIDE_FLS_BASE_ADDR  + 0x10000)
#define INSIDE_FLS_END_ADDR			(INSIDE_FLS_BASE_ADDR  + 0x1FFFFF)

enum TYPE_FLASH_ID{
	SPIFLASH_MID_GD = 0xC8,
	SPIFLASH_MID_ESMT = 0x1C,
	SPIFLASH_MID_PUYA = 0x85,	
	SPIFLASH_MID_WINBOND = 0xEF,	
	SPIFLASH_MID_FUDANMICRO = 0xA1,
	SPIFLASH_MID_BOYA       = 0x68,
	SPIFLASH_MID_XMC		= 0x20,
	SPIFLASH_MID_XTX        = 0x0B,
	SPIFLASH_MID_TSINGTENG    = 0xEB, /*UNIGROUP TSINGTENG*/	
};

typedef union {
    struct {
        uint32_t encrypt_en: 1;               /*!< bit:  0  write encrypt flag */
        uint32_t code_decrypt:1;              /*!< bit:  1      read code from AHB decrypt flag */
        uint32_t key_ready: 1;                /*!< bit:  2  RO key ready flag */
        uint32_t data_decrypt:1;              /*!< bit:  3      read data from AHB or flash controller decrypt flag */
        uint32_t key_sel: 1;                  /*!< bit:  4   key sel flag */
        uint32_t _reserved0: 27;              /*!< bit:  5.. 31  Reserved */
    } b;                                   /*!< Structure    Access by bit */
    uint32_t w;                            /*!< Type         Access by whole register */
} FLASH_ENCRYPT_CTRL_Type;

#define FLASH_HS				0x00000001

 /** Flash Keys */
#define RDPRT_KEY       0x5AA5
#define FLASH_KEY1      0x57696E6E
#define FLASH_KEY2      0x65724D69
#define FLASH_KEY3		0x63726F21

 /** Flash Control Register definitions */
#define FLASH_PG        0x00000001
#define FLASH_PER       0x00000002
#define FLASH_MER       0x00000004
#define FLASH_STRT			0x00000008
#define FLASH_LOCK   	  0x00000020
#define FLASH_ERRIE     0x00000040
#define FLASH_EOPIE     0x00000080

 /** Flash Status Register definitions */
#define FLASH_BSY       0x00000001
#define FLASH_PGERR     0x00000002
#define FLASH_EOP       0x00000004

#define FLS_PARAM_TYPE_ID      (0)
#define FLS_PARAM_TYPE_SIZE      (1)
#define FLS_PARAM_TYPE_PAGE_SIZE      (2)
#define FLS_PARAM_TYPE_PROG_SIZE      (3)
#define FLS_PARAM_TYPE_SECTOR_SIZE      (4)

#define FLS_FLAG_UNDER_PROTECT      (1<<0)
#define FLS_FLAG_FAST_READ      (1<<1)
#define FLS_FLAG_AAAI      (1<<2)

#define CMD_START_Pos                         8U                                          /*!< CMD start position */
#define CMD_START_Msk                         (1UL << CMD_START_Pos)                         /*!< CMD start Mask */

/**
 * @brief          This function is used to unlock flash protect area [0x0~0x2000].
 *
 * @param	       None	 
 *
 * @return         None
 *
 * @note           None
 */
int wm_flash_unlock(void);

/**
 * @brief          This function is used to lock flash protect area [0x0~0x2000].
 *
 * @param	       None	 
 *
 * @return         None
 *
 * @note           None
 */
int wm_flash_lock(void);

/**
 * @brief          This function is used to read data from the flash.
 *
 * @param[in]      addr                 Specifies the starting address to read from.
 * @param[in]      buf                  Specified the address to save the readback data.
 * @param[in]      len                  Specifies the length of the data to read.
 *
 * @retval         TLS_FLS_STATUS_OK	    if read sucsess
 * @retval         TLS_FLS_STATUS_EIO	    if read fail
 *
 * @note           None
 */
HAL_StatusTypeDef HAL_FLASH_Read(uint32_t addr, uint8_t *buf, uint32_t len);

/**
 * @brief          This function is used to write data into the flash.
 *
 * @param[in]      addr     Specifies the starting address to write to
 * @param[in]      buf      Pointer to a byte array that is to be written
 * @param[in]      len      Specifies the length of the data to be written
 *
 * @retval         TLS_FLS_STATUS_OK	        if write flash success
 * @retval         TLS_FLS_STATUS_EPERM	        if flash struct point is null
 * @retval         TLS_FLS_STATUS_ENODRV	    if flash driver is not installed
 * @retval         TLS_FLS_STATUS_EINVAL	    if argument is invalid
 * @retval         TLS_FLS_STATUS_EIO           if io error
 *
 * @note           None
 */
HAL_StatusTypeDef HAL_FLASH_Write(uint32_t addr, uint8_t *buf, uint32_t len);

#endif