#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "file_stream.h"
#include "util.h"
#include "validation.h"
#include "flash_decoder.h"
#include "intelhex.h"
#include "target_family.h"
#include "flash_algo.h"
#include "board.h"
#include "hpm_ppor_drv.h"
#include "swd_host.h"
#include "board.h"
#include "hpm_romapi.h"

extern xpi_nor_config_t s_xpi_nor_config;

#undef this
#define this        (*ptThis)

typedef enum {
    STREAM_STATE_CLOSED,
    STREAM_STATE_OPEN,
    STREAM_STATE_END,
    STREAM_STATE_ERROR
} stream_state_t;

typedef bool (*stream_detect_cb_t)(const uint8_t *data, uint32_t size);
typedef error_t (*stream_open_cb_t)(void *state);
typedef error_t (*stream_write_cb_t)(void *state, uint32_t sector,const uint8_t *data, uint32_t size);
typedef error_t (*stream_close_cb_t)(void *state);



typedef struct {
    char filename[13];
    uint32_t fileSize;
} file_info_t;

typedef struct {
    stream_detect_cb_t detect;
    stream_open_cb_t open;
    stream_write_cb_t write;
    stream_close_cb_t close;
} stream_t;



static file_info_t fileInfo;

static bool detect_bin(const uint8_t *data, uint32_t size);


static bool detect_hex(const uint8_t *data, uint32_t size);
static error_t open_hex(void *state);
static error_t write_hex(void *state, uint32_t sector,const uint8_t *data, uint32_t size);
static error_t close_hex(void *state);

static bool detect_rbl(const uint8_t *data, uint32_t size);
static error_t open_rbl(void *state);
static error_t write_rbl(void *state, uint32_t sector,const uint8_t *data, uint32_t size);
static error_t close_rbl(void *state);

static void enter_bootloader(uint8_t *pchDate, uint16_t hwLength);
extern void led_usb_in_activity(void);
extern void led_usb_out_activity(void);

static const stream_t stream[] = {
    {0, 0, 0, 0}, 
    {detect_bin, open_bin, write_bin, close_bin},   // STREAM_TYPE_BIN
    {detect_hex, open_hex, write_hex, close_hex},   // STREAM_TYPE_HEX
    {detect_rbl, open_rbl, write_rbl, close_rbl},   // STREAM_TYPE_RBL
};

static shared_state_t shared_state;
static stream_state_t state = STREAM_STATE_CLOSED;
static stream_t *current_stream = 0;
static uint32_t load_bin_start_address = 0; // Decoded from the flash decoder, the binary buffer data starts at this address
#define USER_DATA_SIZE            192
#define MARK_SIZE                 64
typedef struct {
    char chProjectName[16];
    char chHardWareVersion[16];
    char chSoftBootVersion[16];
    char chSoftAppVersion[16];
    uint8_t trackedSectors[128];
} msgSig_t;
typedef struct {
    union {
        msgSig_t sig;
        uint8_t B[USER_DATA_SIZE];
    } msg_data;
} user_data_t;

user_data_t tUserData = {
    .msg_data.sig.chProjectName = "microlink",
    .msg_data.sig.chHardWareVersion = "V1.0.0",
    .msg_data.sig.chSoftBootVersion = "V1.0.0",
    .msg_data.sig.chSoftAppVersion =  "V1.0.0",
};

stream_type_t stream_start_identify(const uint8_t *data, uint32_t size)
{
    stream_type_t i;

    for (i = STREAM_TYPE_START + 1; i < STREAM_TYPE_COUNT; i++) {
        if (stream[i].detect(data, size)) {
            return i;
        }
    }

    return STREAM_TYPE_NONE;
}


error_t stream_open(stream_type_t stream_type)
{
    error_t status;
    uint32_t val = 0;
    // Stream must not be open already
    if (state != STREAM_STATE_CLOSED) {
        util_assert(0);
        return ERROR_INTERNAL;
    }

    // Stream must be of a supported type
    if (stream_type >= STREAM_TYPE_COUNT) {
        util_assert(0);
        return ERROR_INTERNAL;
    }

    // Initialize all variables
    memset(&shared_state, 0, sizeof(shared_state));
    state = STREAM_STATE_OPEN;
    current_stream = (stream_t *)&stream[stream_type];
    // Initialize the specified stream
    status = current_stream->open(&shared_state);

    if (ERROR_SUCCESS != status) {
        state = STREAM_STATE_ERROR;
    }
    return status;
}

error_t stream_write(uint32_t sector,const uint8_t *data, uint32_t size)
{
    error_t status;

    // Stream must be open already
    if (state != STREAM_STATE_OPEN) {
        util_assert(0);
        return ERROR_INTERNAL;
    }

    // Write to stream
    status = current_stream->write(&shared_state, sector,data, size);
    //led_usb_in_activity();
    //led_usb_out_activity();
    if (ERROR_SUCCESS_DONE == status) {
        state = STREAM_STATE_END;
    } else if ((ERROR_SUCCESS_DONE_OR_CONTINUE == status) || (ERROR_SUCCESS == status)) {
        // Stream should remain in the open state
        util_assert(STREAM_STATE_OPEN == state);
    } else {
        state = STREAM_STATE_ERROR;
    }
    return status;
}

error_t stream_close(void)
{
    error_t status;

    // Stream must not be closed already
    if (STREAM_STATE_CLOSED == state) {
        util_assert(0);
        return ERROR_INTERNAL;
    }

    // Close stream
    status = current_stream->close(&shared_state);

    state = STREAM_STATE_CLOSED;
    return status;
}

bool vfs_user_file_change_handler_hook(const vfs_filename_t filename, uint32_t size)
{
    memcpy(fileInfo.filename, filename, sizeof(fileInfo.filename));
    fileInfo.fileSize = size;
    return false;
}

/* Binary file processing */
static bool detect_bin(const uint8_t *data, uint32_t size)
{
    return validate_bin_nvic(data);
}

error_t open_bin(void *state)
{
    error_t status = ERROR_FAILURE;

    if(ERROR_SUCCESS == get_flash_blob()){
        bin_state_t *bin_state = (bin_state_t *)state;
        memset(bin_state, 0, sizeof(*bin_state));
        status = flash_decoder_open();
    }
    return status;
}


void set_bin_start_address(uint32_t start_address)
{
    load_bin_start_address = start_address;
}

uint32_t get_bin_start_address(void)
{
    return load_bin_start_address;
}
#include "pikaScript.h"
error_t write_bin(void *state, uint32_t sector,const uint8_t *data, uint32_t size)
{
    error_t status;
    led_usb_in_activity();
    led_usb_out_activity();
    bin_state_t *bin_state = (bin_state_t *)state;
    if (bin_state->buf_pos < FLASH_DECODER_MIN_SIZE) {
        flash_decoder_type_t flash_type;
        uint32_t size_left;
        uint32_t copy_size;
        uint32_t start_addr;
        const flash_intf_t *flash_intf;
        // Buffer Data
        size_left = FLASH_DECODER_MIN_SIZE - bin_state->buf_pos;
        copy_size = MIN(size_left, size);
        memcpy(bin_state->vector_buf + bin_state->buf_pos, data, copy_size);
        bin_state->buf_pos += copy_size;

        if (bin_state->buf_pos < FLASH_DECODER_MIN_SIZE) {
            printf(" Not enough data to determine type\r\n");
            return ERROR_SUCCESS;
        }

        data += copy_size;
        size -= copy_size;
        // Determine type
        flash_type = FLASH_DECODER_TYPE_TARGET;

        // Determine flash addresss
        status = flash_decoder_get_flash(flash_type, 0, false, &start_addr, &flash_intf);

        if (ERROR_SUCCESS != status) {
            printf(" flash type unknown\r\n");
            return status;
        }

        if(load_bin_start_address > start_addr){
            bin_state->flash_addr = load_bin_start_address;
        }else{
            bin_state->flash_addr = start_addr;
        }
       // printf(" start_addr = 0x%x,flash_addr = 0x%x\r\n",start_addr,bin_state->flash_addr);
        // Pass on data to the decoder
        status = flash_decoder_write(bin_state->flash_addr, bin_state->vector_buf, bin_state->buf_pos);

        if (ERROR_SUCCESS != status) {
            printf("flash_addr = 0x%x,status = %d\r\n",bin_state->flash_addr,status);
            return status;
        }

        bin_state->flash_addr += bin_state->buf_pos;
    }

    // Write data
    status = flash_decoder_write(bin_state->flash_addr, data, size);

    if (ERROR_SUCCESS != status) {
        printf("flash_addr = 0x%x,status = %d\r\n",bin_state->flash_addr,status);
        return status;
    }

    bin_state->flash_addr += size;


    return ERROR_SUCCESS_DONE_OR_CONTINUE;
}

error_t close_bin(void *state)
{
    error_t status = 0;
    status = flash_decoder_close();
    swd_init_debug();
    extern uint8_t software_reset(void);
    software_reset(); 
    
    return status;
}

/* Hex file processing */

static bool detect_hex(const uint8_t *data, uint32_t size)
{
    return validate_hexfile(data);
}

static error_t open_hex(void *state)
{
    error_t status = ERROR_FAILURE;
    if(ERROR_SUCCESS == get_flash_blob()){
      hex_state_t *hex_state = (hex_state_t *)state;
      memset(hex_state, 0, sizeof(*hex_state));
      reset_hex_parser();
      hex_state->parsing_complete = false;
      status = flash_decoder_open();
    }
    return status;
}

static error_t write_hex(void *state, uint32_t sector,const uint8_t *data, uint32_t size)
{
    error_t status = ERROR_SUCCESS;
    hex_state_t *hex_state = (hex_state_t *)state;
    hexfile_parse_status_t parse_status = HEX_PARSE_UNINIT;
    uint32_t bin_start_address = 0; // Decoded from the hex file, the binary buffer data starts at this address
    uint32_t bin_buf_written = 0;   // The amount of data in the binary buffer starting at address above
    uint32_t block_amt_parsed = 0;  // amount of data parsed in the block on the last call
    led_usb_in_activity();
    led_usb_out_activity();
    while (1) {
        // try to decode a block of hex data into bin data
        parse_status = parse_hex_blob(data, size, &block_amt_parsed, hex_state->bin_buffer, sizeof(hex_state->bin_buffer), &bin_start_address, &bin_buf_written);

        // the entire block of hex was decoded. This is a simple state
        if (HEX_PARSE_OK == parse_status) {
            if (bin_buf_written > 0) {
                status = flash_decoder_write(bin_start_address, hex_state->bin_buffer, bin_buf_written);
            }

            break;
        } else if (HEX_PARSE_UNALIGNED == parse_status) {
            if (bin_buf_written > 0) {
                status = flash_decoder_write(bin_start_address, hex_state->bin_buffer, bin_buf_written);

                if (ERROR_SUCCESS != status) {
                    break;
                }
            }

            // incrememntal offset to finish the block
            size -= block_amt_parsed;
            data += block_amt_parsed;
        } else if (HEX_PARSE_EOF == parse_status) {
            if (bin_buf_written > 0) {
                status = flash_decoder_write(bin_start_address, hex_state->bin_buffer, bin_buf_written);
            }

            if (ERROR_SUCCESS == status) {
                status = ERROR_SUCCESS_DONE;
            }

            break;
        } else if (HEX_PARSE_CKSUM_FAIL == parse_status) {
            status = ERROR_HEX_CKSUM;
            break;
        } else if ((HEX_PARSE_UNINIT == parse_status) || (HEX_PARSE_FAILURE == parse_status)) {
            util_assert(HEX_PARSE_UNINIT != parse_status);
            status = ERROR_HEX_PARSER;
            break;
        }
    }

    return status;
}

static error_t close_hex(void *state)
{
    error_t status;
    status = flash_decoder_close();
    swd_init_debug();
    extern uint8_t software_reset(void);
    software_reset(); 
    return status;
}

/* RBL file processing */



static bool detect_rbl(const uint8_t *data, uint32_t size)
{
    return validate_rblfile(data);
}

static error_t open_rbl(void *state)
{
    rbl_state_t *rbl_state = (rbl_state_t *)state;
    error_t status = ERROR_SUCCESS;
    rbl_state->tracked_sector_count = 0;  // 清空记录
    return status;
}

static error_t write_rbl(void *state, uint32_t sector,const uint8_t *data, uint32_t size)
{
    rbl_state_t *rbl_state = (rbl_state_t *)state;
    if (rbl_state->tracked_sector_count < FAT_ON_CHIP_FLASH_BLOCK_NUM) {
        rbl_state->tracked_sectors[rbl_state->tracked_sector_count++] = sector;
    } 
    return ERROR_SUCCESS_DONE_OR_CONTINUE;
}

static error_t close_rbl(void *state)
{
    error_t status = ERROR_SUCCESS;
    rbl_state_t *rbl_state = (rbl_state_t *)state;
    //for (uint32_t j = 0; j < rbl_state->tracked_sector_count; j++) {
    //    printf("Sector %u\n", rbl_state->tracked_sectors[j]);
    //} 
    memcpy(tUserData.msg_data.sig.trackedSectors, rbl_state->tracked_sectors, rbl_state->tracked_sector_count);
    enter_bootloader((uint8_t *)&tUserData,sizeof(tUserData)); 
    return status;
}

void enter_bootloader(uint8_t *pchDate, uint16_t hwLength)
{
    uint32_t wData = 0;

    if (rom_xpi_nor_erase(BOARD_APP_XPI_NOR_XPI_BASE, xpi_xfer_channel_auto, &s_xpi_nor_config,(FAT_ON_CHIP_FLASH_OFFSET + (APP_PART_ADDR + APP_PART_SIZE - (3*MARK_SIZE) - (USER_DATA_SIZE))  - 0x80000000 ), USER_DATA_SIZE) != status_success) {
        return;
    }
    if (rom_xpi_nor_program(BOARD_APP_XPI_NOR_XPI_BASE, xpi_xfer_channel_auto, &s_xpi_nor_config, (uint32_t *)pchDate, (APP_PART_ADDR + APP_PART_SIZE - (3*MARK_SIZE) - (USER_DATA_SIZE) - 0x80000000), USER_DATA_SIZE) != status_success) {
        return;
    }

    if (rom_xpi_nor_program(BOARD_APP_XPI_NOR_XPI_BASE, xpi_xfer_channel_auto, &s_xpi_nor_config, (uint32_t *)&wData, (APP_PART_ADDR + APP_PART_SIZE - MARK_SIZE - 0x80000000), sizeof(wData)) != status_success) {
        return;
    }
    ppor_sw_reset(HPM_PPOR, 5000);
}






