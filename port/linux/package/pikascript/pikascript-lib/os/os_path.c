#include "os_path.h"
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#endif
#include "PikaStdData_List.h"
#include "PikaStdData_Tuple.h"

#ifdef _WIN32
#define PATH_SEPARATOR '\\'
#define PATH_SEPARATOR_STRING "\\"
#else
#define PATH_SEPARATOR '/'
#define PATH_SEPARATOR_STRING "/"
#endif

#define IS_PATH_SEP(ch) ((ch) == '/' || (ch) == '\\')

char* os_path_abspath(PikaObj* self, char* path) {
    char* abs_path = NULL;

#ifdef _WIN32
    DWORD size = GetFullPathNameA(path, 0, NULL, NULL);
    if (size == 0) {
        return NULL;
    }

    abs_path = (char*)malloc(size * sizeof(char));
    if (abs_path == NULL) {
        return NULL;
    }

    DWORD ret_size = GetFullPathNameA(path, size, abs_path, NULL);
    if (ret_size == 0 || ret_size > size) {
        free(abs_path);
        return NULL;
    }
#else
    char* cwd = getcwd(NULL, 0);
    if (cwd == NULL) {
        return NULL;
    }

    abs_path = realpath(path, NULL);
    if (abs_path == NULL) {
        free(cwd);
        return NULL;
    }

    if (abs_path[0] != '/') {
        char* temp_path =
            (char*)malloc((strlen(cwd) + strlen(abs_path) + 2) * sizeof(char));
        if (temp_path == NULL) {
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

PIKA_BOOL os_path_exists(PikaObj* self, char* path) {
#ifdef _WIN32
    DWORD attr = GetFileAttributesA(path);
    if (attr == INVALID_FILE_ATTRIBUTES) {
        return PIKA_FALSE;
    }

    return PIKA_TRUE;
#else
    struct stat statbuf;
    if (stat(path, &statbuf) == -1) {
        return PIKA_FALSE;
    }

    return PIKA_TRUE;
#endif
}

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
        free(dirname);
        return res;
    }
}

int _os_path_split(char* path, char** folder, char** file) {
    if (path == NULL || folder == NULL || file == NULL) {
        return -1;
    }
    char* p = strrchr(path, PATH_SEPARATOR);
    if (p) {
        size_t idx = p - path;
        *folder = pika_platform_malloc(idx + 2);
        if (*folder == NULL) {
            return -1;
        }
        strncpy(*folder, path, idx + 1);
        (*folder)[idx] = '\0';
        *file = strdup(p + 1);
        if (*file == NULL) {
            pika_platform_free(*folder);
            *folder = NULL;
            return -1;
        }
        return 0;
    } else {
        *folder = strdup(path);
        if (*folder == NULL) {
            return -1;
        }
        *file = strdup("");
        if (*file == NULL) {
            pika_platform_free(*folder);
            *folder = NULL;
            return -1;
        }
        return 0;
    }
}

int _os_path_splitext(char* path, char** file, char** ext) {
    char* p = strrchr(path, '.');
    if (p) {
        size_t idx = p - path;
        *file = malloc(idx + 1);
        if (!(*file)) {
            return -1;
        }
        strncpy(*file, path, idx);
        (*file)[idx] = '\0';
        *ext = strdup(p);
        if (!(*ext)) {
            pika_platform_free(*file);
            *file = NULL;
            return -1;
        }
        return 0;
    } else {
        *file = strdup(path);
        if (!(*file)) {
            return -1;
        }
        *ext = strdup("");
        if (!(*ext)) {
            free(*file);
            *file = NULL;
            return -1;
        }
        return 0;
    }
}

PikaObj* os_path_split(PikaObj* self, char* path) {
    char* folder = NULL;
    char* file = NULL;
    PikaObj* tuple = NULL;

    if (0 != _os_path_split(path, &folder, &file)) {
        goto __exit;
    }

    tuple = New_pikaTupleFromVarArgs(arg_newStr(folder), arg_newStr(file));
    pika_platform_free(folder);
    pika_platform_free(file);

    return tuple;
__exit:
    if (tuple) {
        obj_deinit(tuple);
    }
    if (folder) {
        pika_platform_free(folder);
    }
    if (file) {
        pika_platform_free(file);
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
        goto __exit;
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
        pika_platform_free(file);
    }
    if (ext) {
        pika_platform_free(ext);
    }
    return NULL;
}
