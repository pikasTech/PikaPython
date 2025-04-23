/**
 * @file    vfs_manager.h
 * @brief   Methods that build and manipulate a virtual file system
 *
 * DAPLink Interface Firmware
 * Copyright (c) 2009-2016, ARM Limited, All Rights Reserved
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

#ifndef VFS_MANAGER_USER_H
#define VFS_MANAGER_USER_H

#include <stdint.h>
#include <stdbool.h>

#include "file_stream.h"
#include "error.h"

#ifdef __cplusplus
extern "C" {
#endif

#define VFS_CLUSTER_SIZE        0x1000
#define VFS_SECTOR_SIZE         4096
#define VFS_INVALID_SECTOR      0xFFFFFFFF
#define VFS_FILE_INVALID        0
#define VFS_MAX_FILES           16


typedef void *vfs_file_t;
typedef uint32_t vfs_sector_t;

extern const vfs_filename_t daplink_mode_file_name;

/* Callable from anywhere */

// Enable or disable the virtual filesystem
void vfs_mngr_fs_enable(bool enabled);

// Remount the virtual filesystem
void vfs_mngr_fs_remount(void);


/* Callable only from the thread running the virtual fs */

// Initialize the VFS manager
// Must be called after USB has been initialized (usbd_init())
// Notes: Must only be called from the thread runnning USB
void vfs_mngr_init(bool enabled);

// Run the vfs manager state machine
// Notes: Must only be called from the thread runnning USB
void vfs_mngr_periodic(uint32_t elapsed_ms);

// Return the status of the last transfer or ERROR_SUCCESS
// if none have been performed yet
error_t vfs_mngr_get_transfer_status(void);


/* Use functions */

// Build the filesystem by calling vfs_init and then adding files with vfs_create_file
void vfs_user_build_filesystem(void);

// Called when a file on the filesystem changes
//void vfs_user_file_change_handler(const vfs_filename_t filename, vfs_file_change_t change, vfs_file_t file, vfs_file_t new_file_data);

// Called when VFS is disconnecting
void vfs_user_disconnecting(void);

void file_data_handler(uint32_t sector, const uint8_t *buf, uint32_t num_of_sectors);

stream_type_t file_change_handler(const vfs_filename_t file_name_ext, uint32_t size);

void transfer_finsh_state(error_t status);
#ifdef __cplusplus
}
#endif

#endif
