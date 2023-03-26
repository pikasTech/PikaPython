#include "os_path.h"
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#include "PikaStdData_List.h"
#include "PikaStdData_Tuple.h"
#endif

#ifdef _WIN32
#define PATH_SEPARATOR '\\'
#define PATH_SEPARATOR_STRING "\\"
#else
#define PATH_SEPARATOR '/'
#define PATH_SEPARATOR_STRING "/"
#endif

#define IS_PATH_SEP(ch) ((ch) == '/' || (ch) == '\\')

// 返回指定路径的绝对路径
char* os_path_abspath(PikaObj* self, char* path) {
    char* abs_path = NULL;

#ifdef _WIN32
    DWORD size = GetFullPathNameA(path, 0, NULL, NULL);
    if (size == 0) {
        // 获取绝对路径失败
        return NULL;
    }

    abs_path = (char*)malloc(size * sizeof(char));
    if (abs_path == NULL) {
        // 内存分配失败
        return NULL;
    }

    DWORD ret_size = GetFullPathNameA(path, size, abs_path, NULL);
    if (ret_size == 0 || ret_size > size) {
        // 获取绝对路径失败
        free(abs_path);
        return NULL;
    }
#else
    char* cwd = getcwd(NULL, 0);
    if (cwd == NULL) {
        // 获取当前工作目录失败
        return NULL;
    }

    abs_path = realpath(path, NULL);
    if (abs_path == NULL) {
        // 获取绝对路径失败
        free(cwd);
        return NULL;
    }

    // 如果路径不是绝对路径，则将其转换为绝对路径
    if (abs_path[0] != '/') {
        char* temp_path =
            (char*)malloc((strlen(cwd) + strlen(abs_path) + 2) * sizeof(char));
        if (temp_path == NULL) {
            // 内存分配失败
            free(cwd);
            free(abs_path);
            return NULL;
        }

        strcpy(temp_path, cwd);
        strcat(temp_path, "/");
        strcat(temp_path, abs_path);

        free(abs_path);
        abs_path = temp_path;
    }

    free(cwd);
#endif
    char* res = obj_cacheStr(self, abs_path);
    free(abs_path);
    return res;
}

// 判断指定路径是否存在
PIKA_BOOL os_path_exists(PikaObj* self, char* path) {
#ifdef _WIN32
    DWORD attr = GetFileAttributesA(path);
    if (attr == INVALID_FILE_ATTRIBUTES) {
        // 获取文件属性失败
        return PIKA_FALSE;
    }

    return PIKA_TRUE;
#else
    struct stat statbuf;
    if (stat(path, &statbuf) == -1) {
        // 获取文件状态失败
        return PIKA_FALSE;
    }

    return PIKA_TRUE;
#endif
}

// 判断指定路径是否为绝对路径
PIKA_BOOL os_path_isabs(PikaObj* self, char* path) {
#ifdef _WIN32
    if (path[0] == '\\' || path[0] == '/') {
        return PIKA_TRUE;
    }

    if (strlen(path) > 1 && path[1] == ':') {
        return PIKA_TRUE;
    }

    return PIKA_FALSE;
#else
    if (path[0] == '/') {
        return PIKA_TRUE;
    }

    return PIKA_FALSE;
#endif
}

// Returns true if the given path is a directory, false otherwise.
PIKA_BOOL os_path_isdir(PikaObj* self, char* path) {
    PIKA_BOOL is_dir = PIKA_FALSE;
#ifdef _WIN32
    DWORD attrs = GetFileAttributes(path);
    if (attrs != INVALID_FILE_ATTRIBUTES) {
        is_dir =
            (attrs & FILE_ATTRIBUTE_DIRECTORY) != 0 ? PIKA_TRUE : PIKA_FALSE;
    }
#else
    struct stat st;
    if (stat(path, &st) == 0) {
        is_dir = S_ISDIR(st.st_mode) ? PIKA_TRUE : PIKA_FALSE;
    }
#endif
    return is_dir;
}

// Returns true if the given path is a regular file, false otherwise.
PIKA_BOOL os_path_isfile(PikaObj* self, char* path) {
    PIKA_BOOL is_file = PIKA_FALSE;
#ifdef _WIN32
    DWORD attrs = GetFileAttributes(path);
    if (attrs != INVALID_FILE_ATTRIBUTES) {
        is_file =
            (attrs & FILE_ATTRIBUTE_DIRECTORY) == 0 ? PIKA_TRUE : PIKA_FALSE;
    }
#else
    struct stat st;
    if (stat(path, &st) == 0) {
        is_file = S_ISREG(st.st_mode) ? PIKA_TRUE : PIKA_FALSE;
    }
#endif
    return is_file;
}

char* os_path_join(PikaObj* self, PikaTuple* paths) {
    size_t total_len = 1;  // Start with a single null terminator
    int num_paths = pikaTuple_getSize(paths);
    for (int i = 0; i < num_paths; i++) {
        const char* path = pikaTuple_getStr(paths, i);
        total_len += strlen(path);
        if (i < num_paths - 1) {
            if (!IS_PATH_SEP(path[strlen(path) - 1])) {
                total_len++;
            }
        }
    }
    char* result = (char*)malloc(total_len);
    if (!result) {
        return NULL;
    }
    result[0] = '\0';
    for (int i = 0; i < num_paths; i++) {
        const char* path = pikaTuple_getStr(paths, i);
        if (!path || !path[0]) {
            continue;
        }
        if (i == 0) {
            // First component
            strncat(result, path, total_len);
        } else {
            // Subsequent components
            if (!IS_PATH_SEP(result[strlen(result) - 1])) {
                strncat(result, PATH_SEPARATOR_STRING, total_len);
            }
            strncat(result, path, total_len);
        }
    }
    char* res = obj_cacheStr(self, result);
    free(result);
    return res;
}

char* os_path_basename(PikaObj* self, char* path) {
    char* sep_pos = strrchr(path, PATH_SEPARATOR);
    if (sep_pos == NULL) {
        return obj_cacheStr(self, path);
    } else {
        return obj_cacheStr(self, sep_pos + 1);
    }
}

char* os_path_dirname(PikaObj* self, char* path) {
    char* sep_pos = strrchr(path, PATH_SEPARATOR);
    if (sep_pos == NULL) {
        return obj_cacheStr(self, ".");
    } else if (sep_pos == path) {
        return obj_cacheStr(self, PATH_SEPARATOR_STRING);
    } else {
        int dirname_len = sep_pos - path;
        char* dirname = malloc(dirname_len + 1);
        memcpy(dirname, path, dirname_len);
        dirname[dirname_len] = '\0';
        char* res = obj_cacheStr(self, dirname);
        free (dirname);
        return res;
    }
}

int _os_path_split(char* path, char** folder, char** file) {
    if (path == NULL || folder == NULL || file == NULL) {
        return -1;
    }
    char* p = strrchr(path, PATH_SEPARATOR);
    if (p) {
        /* 字符串最后一个路径分隔符的位置 */
        size_t idx = p - path;
        /* 获取最后一个路径分隔符之前的路径 */
        *folder = malloc(idx + 2);
        if (*folder == NULL) {
            return -1;
        }
        strncpy(*folder, path, idx + 1);
        (*folder)[idx + 1] = '\0';
        /* 获取最后一个路径分隔符之后的文件名 */
        *file = strdup(p + 1);
        if (*file == NULL) {
            free(*folder);
            return -1;
        }
        return 0;
    } else {
        /* 如果路径没有分隔符，则返回路径本身和空字符串 */
        *folder = strdup(path);
        if (*folder == NULL) {
            return -1;
        }
        *file = strdup("");
        if (*file == NULL) {
            free(*folder);
            return -1;
        }
        return 0;
    }
}

int _os_path_splitext(char* path, char** file, char** ext) {
    char* p = strrchr(path, '.');
    if (p) {
        /* 字符串最后一个点的位置 */
        size_t idx = p - path;
        /* 获取点之前的路径 */
        *file = malloc(idx + 1);
        if (!(*file)) {
            /* 内存分配失败 */
            return -1;
        }
        strncpy(*file, path, idx);
        (*file)[idx] = '\0';
        /* 获取点之后的扩展名 */
        *ext = strdup(p);
        if (!(*ext)) {
            /* 内存分配失败 */
            free(*file);
            return -1;
        }
        return 0;
    } else {
        /* 如果没有扩展名，则返回路径本身和空字符串 */
        *file = strdup(path);
        if (!(*file)) {
            /* 内存分配失败 */
            return -1;
        }
        *ext = strdup("");
        if (!(*ext)) {
            /* 内存分配失败 */
            free(*file);
            return -1;
        }
        return 0;
    }
}

PikaObj* os_path_split(PikaObj* self, char* path) {
    char* folder = NULL;
    char* file = NULL;
    PikaObj* tuple = NULL;
    Arg* aFolder = NULL;
    Arg* aFile = NULL;

    if (0 != _os_path_split(path, &folder, &file)) {
        goto __exit;  // 发生错误，跳转到 __exit 处做资源回收
    }

    tuple = newNormalObj(New_PikaStdData_Tuple);
    PikaStdData_Tuple___init__(tuple);

    aFolder = arg_newStr(folder);
    aFile = arg_newStr(file);

    PikaStdData_List_append(tuple, aFolder);
    PikaStdData_List_append(tuple, aFile);

    arg_deinit(aFolder);
    arg_deinit(aFile);
    free(folder);
    free(file);

    return tuple;
__exit:
    if (aFolder) {
        arg_deinit(aFolder);
    }
    if (aFile) {
        arg_deinit(aFile);
    }
    if (tuple) {
        obj_deinit(tuple);
    }
    if (folder) {
        free(folder);
    }
    if (file) {
        free(file);
    }
    return NULL;
}

PikaObj* os_path_splitext(PikaObj* self, char* path) {
    char* file = NULL;
    char* ext = NULL;
    PikaObj* tuple = NULL;
    Arg* aFile = NULL;
    Arg* aExt = NULL;

    if (0 != _os_path_splitext(path, &file, &ext)) {
        goto __exit;  // 发生错误，跳转到 __exit 处做资源回收
    }

    tuple = newNormalObj(New_PikaStdData_Tuple);
    PikaStdData_Tuple___init__(tuple);
    aFile = arg_newStr(file);
    aExt = arg_newStr(ext);
    PikaStdData_List_append(tuple, aFile);
    PikaStdData_List_append(tuple, aExt);

    arg_deinit(aFile);
    arg_deinit(aExt);
    free(file);
    free(ext);

    return tuple;

__exit:
    if (aFile) {
        arg_deinit(aFile);
    }
    if (aExt) {
        arg_deinit(aExt);
    }
    if (tuple) {
        obj_deinit(tuple);
    }
    if (file) {
        free(file);
    }
    if (ext) {
        free(ext);
    }
    return NULL;
}
