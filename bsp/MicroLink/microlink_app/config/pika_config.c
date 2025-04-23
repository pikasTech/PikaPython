#include "pikascript.h"
#include "board.h"
#include "ff.h"
#include "PikaObj.h"
extern void reboot(void);
typedef FIL _INNER_FILE;
int __fmodeflags(const char* mode);

void pika_platform_enable_irq_handle(void)
{

}
void pika_platform_disable_irq_handle(void)
{

}

void pika_platform_reboot(void) 
{
   reboot();
}

int64_t pika_platform_get_tick(void) 
{
    return hpm_csr_get_core_cycle();
}

FILE* pika_platform_fopen(const char* filename, const char* modes)
{
    FRESULT res;
    int flags;

    /* Check for valid initial mode character */
    if (!strchr("rwa", *modes)) {
        return 0;
    }

    /* Compute the flags to pass to open() */
    flags = __fmodeflags(modes);

    _INNER_FILE *_f = pika_platform_malloc(sizeof(*_f));
    if(NULL == _f){
        return NULL;
    }
    res = f_open(_f, filename, flags);
    if (res) {
        pika_platform_free(_f);
        return NULL;
    }

    return (FILE *)_f;
}

size_t pika_platform_fwrite(const void* ptr,
                                      size_t size,
                                      size_t n,
                                      FILE* stream) 
{
    _INNER_FILE* _f = (_INNER_FILE*)stream;
    size_t len = 0;
    f_write(_f, ptr, n * size, &len);
    return len;

}


size_t pika_platform_fread(void* ptr,
                                     size_t size,
                                     size_t n,
                                     FILE* stream)
{
    _INNER_FILE* _f = (_INNER_FILE*)stream;
    size_t len = 0;
    f_read(_f, ptr, n * size, &len);
    return len;
}

int pika_platform_fclose(FILE* stream) 
{
    _INNER_FILE* _f = (_INNER_FILE*)stream;
    f_close(_f);
    pika_platform_free(_f);
    return 0;
}


int pika_platform_fseek(FILE* stream, long offset, int whence) 
{
    _INNER_FILE* _f = (_INNER_FILE*)stream;
    DWORD fatfs_offset;
    switch (whence) {
        case SEEK_SET:
            fatfs_offset = offset;
            break;
        case SEEK_CUR:
            fatfs_offset = f_tell(_f) + offset;
            break;
        case SEEK_END:
            fatfs_offset = f_size(_f) + offset;
            break;
        default:
            return -1; // Invalid argument
    }
    if (f_lseek(_f, fatfs_offset) == FR_OK) {
        return 0;
    } else {
        return -1;
    }
}

long pika_platform_ftell(FILE* stream) 
{
    _INNER_FILE* _f = (_INNER_FILE*)stream;
    return f_tell(_f);
}

char* pika_platform_getcwd(char* buf, size_t size) 
{
    // FatFS doesn't directly provide a getcwd function. You might need 
    // to manage the current directory yourself or return a default if it's 
    // not crucial for your application.
    strncpy(buf, "/", size);
    return buf;
}

int pika_platform_chdir(const char* path) 
{
    // FatFS doesn't directly provide a chdir function. You might need 
    // to manage the current directory yourself.
    return -1; // Not implemented
}

int pika_platform_rmdir(const char* pathname) {
    return f_unlink(pathname);
}

int pika_platform_mkdir(const char* pathname, int mode) {
    return f_mkdir(pathname);
}

int pika_platform_remove(const char* pathname) {
    return f_unlink(pathname);
}

int pika_platform_rename(const char* oldpath, const char* newpath) {
    return f_rename(oldpath, newpath);
}


char** pika_platform_listdir(const char* path, int* count) {
    DIR dir;
    static FILINFO fno;
    FRESULT res;
    char **filelist = NULL;
    int idx = 0, capacity = 10; // start with a capacity of 10, then grow if necessary

    res = f_opendir(&dir, path); // Open the directory
    if (res == FR_OK) {
        filelist = pika_platform_malloc(capacity * sizeof(char*));
        if(!filelist) return NULL; // Failed to allocate memory

        for (;;) {
            res = f_readdir(&dir, &fno); // Read a directory item
            if (res != FR_OK || fno.fname[0] == 0) break; // Break on error or end of dir

            if (idx == capacity) { // Grow the list if necessary
                capacity *= 2;
                char **newlist = pika_platform_realloc(filelist, capacity * sizeof(char*));
                if(!newlist) {
                    // Free any previously allocated memory
                    for(int i = 0; i < idx; i++)
                        pika_platform_free(filelist[i]);
                    pika_platform_free(filelist);
                    *count = 0;
                    return NULL; // Failed to allocate more memory
                }
                filelist = newlist;
            }

            filelist[idx] = pika_platform_strdup(fno.fname);
            if(!filelist[idx]) {
                // Free any previously allocated memory
                for(int i = 0; i < idx; i++)
                    pika_platform_free(filelist[i]);
                pika_platform_free(filelist);
                *count = 0;
                return NULL; // Failed to allocate memory for filename
            }
            idx++;
        }
        f_closedir(&dir);
    }

    *count = idx;
    return filelist;
}

int pika_platform_path_exists(const char *path){
    FRESULT res;
    FILINFO fno;
    
    res = f_stat(path, &fno);
    if (res != FR_OK) {
        return 0; // Path does not exist
    }
    return 1;
}

int pika_platform_path_isdir(const char *path){
		int is_dir = 0;
    FRESULT res;
    FILINFO fno;

    res = f_stat(path, &fno);
    if (res == FR_OK) {
        is_dir = (fno.fattrib & AM_DIR) ? 1 : 0;
    }else{
				return 0;
		}
    return is_dir;
}

int pika_platform_path_isfile(const char *path){
    int is_file = 0;
    FRESULT res;
    FILINFO fno;

    res = f_stat(path, &fno);
    if (res == FR_OK) {
        is_file = (fno.fattrib & AM_DIR) == 0 ? 1 : 0;
    }else{
				return 0;
		}
    return is_file;
}

int __fmodeflags(const char* mode) 
{
    int flags = 0; 

    if (strchr(mode, '+')) {
        flags = FA_WRITE | FA_READ; 
    }

    if (*mode == 'r') {
        if (!(flags & FA_WRITE)) { 
            flags |= FA_READ;
        }
    } else if (*mode == 'w') {
        flags |= FA_CREATE_ALWAYS | FA_WRITE; 
    } else if (*mode == 'a') {
        flags |= FA_OPEN_APPEND | FA_WRITE; 
    }

    return flags;
}