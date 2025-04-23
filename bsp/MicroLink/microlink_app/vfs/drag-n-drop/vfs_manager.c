/**
 * @file    vfs_manager.c
 * @brief   Implementation of vfs_manager.h
 *
 * DAPLink Interface Firmware
 * Copyright (c) 2009-2020 Arm Limited, All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include "vfs_manager.h"
#include "util.h"
#include "compiler.h"
#include "error.h"
#include "board.h"
#include "hpm_ppor_drv.h"

// Set to 1 to enable debugging
#define DEBUG_VFS_MANAGER     0

#if DEBUG_VFS_MANAGER
#define vfs_mngr_printf    printf
#else
#define vfs_mngr_printf(...)
#endif

#define INVALID_TIMEOUT_MS  0xFFFFFFFF
#define MAX_EVENT_TIME_MS   60000

#define CONNECT_DELAY_MS 0
#define RECONNECT_DELAY_MS 2500    // Must be above 1s for windows (more for linux)
// TRANSFER_IN_PROGRESS
#define DISCONNECT_DELAY_TRANSFER_TIMEOUT_MS 20000
// TRANSFER_CAN_BE_FINISHED
#define DISCONNECT_DELAY_TRANSFER_IDLE_MS 500
// TRANSFER_NOT_STARTED || TRASNFER_FINISHED
#define DISCONNECT_DELAY_MS 500

typedef enum {
    TRANSFER_NOT_STARTED,
    TRANSFER_IN_PROGRESS,
    TRANSFER_CAN_BE_FINISHED,
    TRASNFER_FINISHED,
} transfer_state_t;

typedef struct {
    vfs_file_t file_to_program;     // A pointer to the directory entry of the file being programmed
    vfs_sector_t start_sector;      // Start sector of the file being programmed by stream
    vfs_sector_t file_start_sector; // Start sector of the file being programmed by vfs
    vfs_sector_t file_next_sector;  // Expected next sector of the file
    vfs_sector_t last_ooo_sector;   // Last out of order sector within the file
    uint32_t size_processed;        // The number of bytes processed by the stream
    uint32_t file_size;             // Size of the file indicated by root dir.  Only allowed to increase
    uint32_t size_transferred;      // The number of bytes transferred
    transfer_state_t transfer_state;// Transfer state
    bool stream_open;               // State of the stream
    bool stream_started;            // Stream processing started. This only gets reset remount
    bool stream_finished;           // Stream processing is done. This only gets reset remount
    bool stream_optional_finish;    // True if the stream processing can be considered done
    bool file_info_optional_finish; // True if the file transfer can be considered done
    bool transfer_timeout;          // Set if the transfer was finished because of a timeout. This only gets reset remount
    stream_type_t stream;           // Current stream or STREAM_TYPE_NONE is stream is closed.  This only gets reset remount
} file_transfer_state_t;

typedef enum {
    VFS_MNGR_STATE_DISCONNECTED,
    VFS_MNGR_STATE_RECONNECTING,
    VFS_MNGR_STATE_CONNECTED
} vfs_mngr_state_t;


static error_t  __attribute__((section(".noinit"))) fail_reason = ERROR_SUCCESS;
static file_transfer_state_t file_transfer_state;

// These variables can be access from multiple threads
// so access to them must be synchronized
static vfs_mngr_state_t vfs_state;
static vfs_mngr_state_t vfs_state_next;
static uint32_t time_usb_idle;

// Synchronization functions
static void sync_init(void);
static void sync_assert_usb_thread(void);
static void sync_lock(void);
static void sync_unlock(void);

static bool changing_state(void);
static void build_filesystem(void);

static bool ready_for_state_change(void);
static void abort_remount(void);

static void transfer_update_file_info(vfs_file_t file, uint32_t start_sector, uint32_t size, stream_type_t stream);
static void transfer_reset_file_info(void);
static void transfer_stream_open(stream_type_t stream, uint32_t start_sector);
static void transfer_stream_data(uint32_t sector, const uint8_t *data, uint32_t size);

// Handler for file data arriving over USB.  This function is responsible
// for detecting the start of a BIN/HEX file and performing programming
void file_data_handler(uint32_t sector, const uint8_t *buf, uint32_t num_of_sectors)
{
    stream_type_t stream;
    uint32_t size;

    // this is the key for starting a file write - we dont care what file types are sent
    //  just look for something unique (NVIC table, hex, srec, etc) until root dir is updated
    if (!file_transfer_state.stream_started) {
        // look for file types we can program
        stream = stream_start_identify((uint8_t *)buf, VFS_SECTOR_SIZE * num_of_sectors);

        if (STREAM_TYPE_NONE != stream) {
            transfer_stream_open(stream, sector);
        }
    }

    if (file_transfer_state.stream_started) {
        // This sector could be part of the file so record it
        size = VFS_SECTOR_SIZE * num_of_sectors;
        file_transfer_state.size_transferred += size;

        transfer_stream_data(sector, buf, size);
    }
}

// Update the tranfer state with new information
static void transfer_stream_open(stream_type_t stream, uint32_t start_sector)
{
    error_t status;
    util_assert(!file_transfer_state.stream_open);
    util_assert(start_sector != VFS_INVALID_SECTOR);
    vfs_mngr_printf("vfs_manager transfer_update_stream_open(stream=%i, start_sector=%i)\r\n",
                    stream, start_sector);

    // Initialize the starting sector if it has not been set
    if (0 == file_transfer_state.start_sector) {
        file_transfer_state.start_sector = start_sector;

        if (start_sector != VFS_INVALID_SECTOR) {
            vfs_mngr_printf("    start_sector=%i\r\n", start_sector);
        }
    }

    // Initialize the stream if it has not been set
    if (STREAM_TYPE_START == file_transfer_state.stream) {
        file_transfer_state.stream = stream;

        if (stream != STREAM_TYPE_START) {
            vfs_mngr_printf("    stream=%i\r\n", stream);
        }
    }

    // Open stream
    status = stream_open(stream);
    vfs_mngr_printf("    stream_open stream=%i ret %i\r\n", stream, status);

    if (ERROR_SUCCESS == status) {

        file_transfer_state.stream_open = true;
        file_transfer_state.stream_started = true;
    }
}

// Update the tranfer state with new information
static void transfer_stream_data(uint32_t sector, const uint8_t *data, uint32_t size)
{
    error_t status;
    vfs_mngr_printf("vfs_manager transfer_stream_data(sector=%i, size=%i)\r\n", sector, size);
    vfs_mngr_printf("    size processed=0x%x, data=%x,%x,%x,%x,...\r\n",
                    file_transfer_state.size_processed, data[0], data[1], data[2], data[3]);

    if (file_transfer_state.stream_finished) {
        util_assert(0);
        return;
    }

    util_assert(size % VFS_SECTOR_SIZE == 0);
    util_assert(file_transfer_state.stream_open);
    status = stream_write(sector,(uint8_t *)data, size);
    vfs_mngr_printf("    stream_write ret=%i\r\n", status);

    if (ERROR_SUCCESS_DONE == status) {
        // Override status so ERROR_SUCCESS_DONE
        // does not get passed into transfer_update_state
        status = stream_close();
        vfs_mngr_printf("    stream_close ret=%i\r\n", status);
        file_transfer_state.stream_open = false;
        file_transfer_state.stream_finished = true;
        file_transfer_state.stream_optional_finish = true;

    } else if (ERROR_SUCCESS_DONE_OR_CONTINUE == status) {
        status = ERROR_SUCCESS;
        file_transfer_state.stream_optional_finish = true;
    } else {
        file_transfer_state.stream_optional_finish = false;
    }

    file_transfer_state.size_processed += size;
}

// Check if the current transfer is still in progress, done, or if an error has occurred
void transfer_finsh_state(error_t status)
{
      if (file_transfer_state.stream_open) {
          error_t close_status;
          close_status = stream_close();
          vfs_mngr_printf("    stream closed ret=%i\r\n", close_status);
          file_transfer_state.stream_open = false;
          file_transfer_state.stream_started = false;
          file_transfer_state.start_sector = 0;
      }

}