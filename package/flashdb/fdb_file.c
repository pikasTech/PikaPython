/*
 * Copyright (c) 2020, Armink, <armink.ztl@gmail.com>
 * Copyright (c) 2020, enkiller, <462747508@qq.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include "fdb_low_lvl.h"
#include "flashdb.h"

#define FDB_LOG_TAG "[file]"

#ifdef FDB_USING_FILE_MODE

#define DB_PATH_MAX 256

static void get_db_file_path(fdb_db_t db,
                             uint32_t addr,
                             char* path,
                             size_t size) {
#define DB_NAME_MAX 8

    /* from db_name.fdb.0 to db_name.fdb.n */
    char file_name[DB_NAME_MAX + 4 + 10];
    uint32_t sec_addr = FDB_ALIGN_DOWN(addr, db->sec_size);
    int index = sec_addr / db->sec_size;

    snprintf(file_name, sizeof(file_name), "%.*s.fdb.%d", DB_NAME_MAX, db->name,
             index);
    if (strlen(db->storage.dir) + 1 + strlen(file_name) >= size) {
        /* path is too long */
        FDB_ASSERT(0)
    }
    snprintf(path, size, "%s/%s", db->storage.dir, file_name);
}

#if defined(FDB_USING_FILE_POSIX_MODE)
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#if !defined(_MSC_VER)
#include <unistd.h>
#endif

static int open_db_file(fdb_db_t db, uint32_t addr, pika_bool clean) {
    uint32_t sec_addr = FDB_ALIGN_DOWN(addr, db->sec_size);
    int fd = db->cur_file;
    char path[DB_PATH_MAX];

    if (sec_addr != db->cur_sec || fd <= 0 || clean) {
        get_db_file_path(db, addr, path, DB_PATH_MAX);

        if (fd > 0) {
            close(fd);
            fd = -1;
        }
        if (clean) {
            /* clean the old file */
            fd = open(path, O_RDWR | O_CREAT | O_TRUNC, 0777);
            if (fd <= 0) {
                FDB_INFO("Error: open (%s) file failed.\n", path);
            } else {
                close(fd);
                fd = -1;
            }
        }
        /* open the database file */
        fd = open(path, O_RDWR, 0777);
        db->cur_sec = sec_addr;
    }
    db->cur_file = fd;

    return db->cur_file;
}

fdb_err_t _fdb_file_read(fdb_db_t db, uint32_t addr, void* buf, size_t size) {
    fdb_err_t result = FDB_NO_ERR;
    int fd = open_db_file(db, addr, pika_false);
    if (fd > 0) {
        /* get the offset address is relative to the start of the current file
         */
        addr = addr % db->sec_size;
        if ((lseek(fd, addr, SEEK_SET) != addr) ||
            (read(fd, buf, size) != size))
            result = FDB_READ_ERR;
    } else {
        result = FDB_READ_ERR;
    }
    return result;
}

fdb_err_t _fdb_file_write(fdb_db_t db,
                          uint32_t addr,
                          const void* buf,
                          size_t size,
                          pika_bool sync) {
    fdb_err_t result = FDB_NO_ERR;
    int fd = open_db_file(db, addr, pika_false);
    if (fd > 0) {
        /* get the offset address is relative to the start of the current file
         */
        addr = addr % db->sec_size;
        if ((lseek(fd, addr, SEEK_SET) != addr) ||
            (write(fd, buf, size) != size))
            result = FDB_WRITE_ERR;
        if (sync) {
            fsync(fd);
        }
    } else {
        result = FDB_WRITE_ERR;
    }
    return result;
}

fdb_err_t _fdb_file_erase(fdb_db_t db, uint32_t addr, size_t size) {
    fdb_err_t result = FDB_NO_ERR;
    int fd = open_db_file(db, addr, pika_true);
    if (fd > 0) {
#define BUF_SIZE 32
        uint8_t buf[BUF_SIZE];
        size_t i;
        lseek(fd, 0, SEEK_SET);
        for (i = 0; i * BUF_SIZE < size; i++) {
            memset(buf, 0xFF, BUF_SIZE);
            write(fd, buf, BUF_SIZE);
        }
        memset(buf, 0xFF, BUF_SIZE);
        write(fd, buf, size - i * BUF_SIZE);
        fsync(fd);
    } else {
        result = FDB_ERASE_ERR;
    }
    return result;
}
#elif defined(FDB_USING_FILE_LIBC_MODE)
static FILE* open_db_file(fdb_db_t db, uint32_t addr, pika_bool clean) {
    uint32_t sec_addr = FDB_ALIGN_DOWN(addr, db->sec_size);

    if (sec_addr != db->cur_sec || db->cur_file == NULL || clean) {
        char path[DB_PATH_MAX];

        get_db_file_path(db, addr, path, DB_PATH_MAX);

        if (db->cur_file) {
            pika_platform_fclose(db->cur_file);
            db->cur_file = 0;
        }

        if (clean) {
            /* clean the old file */
            db->cur_file = pika_platform_fopen(path, "wb+");
            if (db->cur_file == NULL) {
                FDB_INFO("Error: open (%s) file failed.\n", path);
            } else {
                pika_platform_fclose(db->cur_file);
                db->cur_file = 0;
            }
        }

        /* open the database file to read and write */
        db->cur_file = pika_platform_fopen(path, "rb+");
        db->cur_sec = sec_addr;
    }

    return db->cur_file;
}

fdb_err_t _fdb_file_read(fdb_db_t db, uint32_t addr, void* buf, size_t size) {
    fdb_err_t result = FDB_NO_ERR;
    FILE* fp = open_db_file(db, addr, pika_false);
    if (fp) {
        addr = addr % db->sec_size;
        if ((pika_platform_fseek(fp, addr, SEEK_SET) != 0)) {
            printf("Error: seek (%s) file failed.\n", db->name);
            result = FDB_READ_ERR;
        }
        size_t sizer = pika_platform_fread(buf, 1, size, fp);
        if (sizer != size) {
            printf("Error: read (%s) file failed.\n", db->name);
            result = FDB_READ_ERR;
        }
    } else {
        result = FDB_READ_ERR;
    }
    return result;
}

fdb_err_t _fdb_file_write(fdb_db_t db,
                          uint32_t addr,
                          const void* buf,
                          size_t size,
                          pika_bool sync) {
    fdb_err_t result = FDB_NO_ERR;
    FILE* fp = open_db_file(db, addr, pika_false);
    if (fp) {
        addr = addr % db->sec_size;
        if ((pika_platform_fseek(fp, addr, SEEK_SET) != 0)) {
            printf("Error: seek (%s) file failed.\n", db->name);
            result = FDB_READ_ERR;
        }
        size_t sizew = pika_platform_fwrite(buf, 1, size, fp);
        if (sizew != size) {
            printf("Error: write (%s) file failed.\n", db->name);
            result = FDB_WRITE_ERR;
        }
        if (sync) {
            pika_platform_fflush(fp);
        }
    } else {
        result = FDB_READ_ERR;
    }

    return result;
}

fdb_err_t _fdb_file_erase(fdb_db_t db, uint32_t addr, size_t size) {
    fdb_err_t result = FDB_NO_ERR;

    FILE* fp = open_db_file(db, addr, pika_true);
    if (fp != NULL) {
#define BUF_SIZE 32
        uint8_t buf[BUF_SIZE];
        size_t i;
        pika_platform_fseek(fp, 0, SEEK_SET);
        for (i = 0; i * BUF_SIZE < size; i++) {
            memset(buf, 0xFF, BUF_SIZE);
            size_t sizew = pika_platform_fwrite(buf, 1, BUF_SIZE, fp);
            if (sizew != BUF_SIZE) {
                FDB_PRINT("Error: write (%s) file failed.\n", db->name);
                result = FDB_WRITE_ERR;
                break;
            }
        }
        memset(buf, 0xFF, BUF_SIZE);
        size_t sizew = pika_platform_fwrite(buf, 1, size - i * BUF_SIZE, fp);
        if (sizew != size - i * BUF_SIZE) {
            FDB_PRINT("Error: write (%s) file failed.\n", db->name);
            result = FDB_WRITE_ERR;
        }
        pika_platform_fflush(fp);
    } else {
        result = FDB_ERASE_ERR;
    }
    return result;
}
#endif /* defined(FDB_USING_FILE_LIBC_MODE) */

#endif /* FDB_USING_FILE_MODE */
