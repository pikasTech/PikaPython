/*
  Copyright (c) 2009-2017 Dave Gamble and cJSON contributors

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/
#ifndef pika_cJSON__h
#define pika_cJSON__h

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(__WINDOWS__) && \
    (defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32))
#define __WINDOWS__
#endif

#ifdef __WINDOWS__

/* When compiling for windows, we specify a specific calling convention to
avoid issues where we are being called from a project with a different
default calling convention.  For windows you have 3 define options:

pika_cJSON_HIDE_SYMBOLS - Define this in the case where you don't want to
ever dllexport symbols pika_cJSON_EXPORT_SYMBOLS - Define this on library
build when you want to dllexport symbols (default) pika_cJSON_IMPORT_SYMBOLS
- Define this if you want to dllimport symbol

For *nix builds that support visibility attribute, you can define similar
behavior by

setting default visibility to hidden by adding
-fvisibility=hidden (for gcc)
or
-xldscope=hidden (for sun cc)
to CFLAGS

then using the pika_cJSON_API_VISIBILITY flag to "export" the same symbols
the way pika_cJSON_EXPORT_SYMBOLS does

*/

#define pika_cJSON_CDECL __cdecl
#define pika_cJSON_STDCALL __stdcall

/* export symbols by default, this is necessary for copy pasting the C and
 * header file */
#if !defined(pika_cJSON_HIDE_SYMBOLS) && \
    !defined(pika_cJSON_IMPORT_SYMBOLS) && !defined(pika_cJSON_EXPORT_SYMBOLS)
#define pika_cJSON_EXPORT_SYMBOLS
#endif

#if defined(pika_cJSON_HIDE_SYMBOLS)
#define pika_cJSON_PUBLIC(type) type pika_cJSON_STDCALL
#elif defined(pika_cJSON_EXPORT_SYMBOLS)
#define pika_cJSON_PUBLIC(type) __declspec(dllexport) type pika_cJSON_STDCALL
#elif defined(pika_cJSON_IMPORT_SYMBOLS)
#define pika_cJSON_PUBLIC(type) __declspec(dllimport) type pika_cJSON_STDCALL
#endif
#else /* !__WINDOWS__ */
#define pika_cJSON_CDECL
#define pika_cJSON_STDCALL

#if (defined(__GNUC__) || defined(__SUNPRO_CC) || defined(__SUNPRO_C)) && \
    defined(pika_cJSON_API_VISIBILITY)
#define pika_cJSON_PUBLIC(type) __attribute__((visibility("default"))) type
#else
#define pika_cJSON_PUBLIC(type) type
#endif
#endif

/* project version */
#define pika_cJSON_VERSION_MAJOR 1
#define pika_cJSON_VERSION_MINOR 7
#define pika_cJSON_VERSION_PATCH 15

#include <stddef.h>

/* cJSON Types: */
#define pika_cJSON_Invalid (0)
#define pika_cJSON_False (1 << 0)
#define pika_cJSON_True (1 << 1)
#define pika_cJSON_NULL (1 << 2)
#define pika_cJSON_Number (1 << 3)
#define pika_cJSON_String (1 << 4)
#define pika_cJSON_Array (1 << 5)
#define pika_cJSON_Object (1 << 6)
#define pika_cJSON_Raw (1 << 7) /* raw json */

#define pika_cJSON_IsReference 256
#define pika_cJSON_StringIsConst 512

/* The cJSON structure: */
typedef struct cJSON {
    /* next/prev allow you to walk array/object chains. Alternatively, use
     * GetArraySize/GetArrayItem/GetObjectItem */
    struct cJSON* next;
    struct cJSON* prev;
    /* An array or object item will have a child pointer pointing to a chain
     * of the items in the array/object. */
    struct cJSON* child;

    /* The type of the item, as above. */
    int type;

    /* The item's string, if type==pika_cJSON_String  and type ==
     * pika_cJSON_Raw
     */
    char* valuestring;
    /* writing to valueint is DEPRECATED, use pika_cJSON_SetNumberValue
     * instead
     */
    int valueint;
    /* The item's number, if type==pika_cJSON_Number */
    double valuedouble;

    /* The item's name string, if this item is the child of, or is in the
     * list of subitems of an object. */
    char* string;
} cJSON;

typedef struct pika_cJSON_Hooks {
    /* malloc/free are CDECL on Windows regardless of the default calling
     * convention of the compiler, so ensure the hooks allow passing those
     * functions directly. */
    void*(pika_cJSON_CDECL* malloc_fn)(size_t sz);
    void(pika_cJSON_CDECL* free_fn)(void* ptr);
} pika_cJSON_Hooks;

typedef int pika_cJSON_bool;

/* Limits how deeply nested arrays/objects can be before cJSON rejects to parse
 * them. This is to prevent stack overflows. */
#ifndef pika_cJSON_NESTING_LIMIT
#define pika_cJSON_NESTING_LIMIT 1000
#endif

/* returns the version of cJSON as a string */
pika_cJSON_PUBLIC(const char*) pika_cJSON_Version(void);

/* Supply malloc, realloc and free functions to cJSON */
pika_cJSON_PUBLIC(void) pika_cJSON_InitHooks(pika_cJSON_Hooks* hooks);

/* Memory Management: the caller is always responsible to free the results
 * from all variants of pika_cJSON_Parse (with pika_cJSON_Delete) and
 * pika_cJSON_Print (with stdlib free, pika_cJSON_Hooks.free_fn, or
 * pika_cJSON_free as appropriate). The exception is
 * pika_cJSON_PrintPreallocated, where the caller has full responsibility of
 * the buffer. */
/* Supply a block of JSON, and this returns a cJSON object you can
 * interrogate.
 */
pika_cJSON_PUBLIC(cJSON*) pika_cJSON_Parse(const char* value);
pika_cJSON_PUBLIC(cJSON*)
    pika_cJSON_ParseWithLength(const char* value, size_t buffer_length);
/* ParseWithOpts allows you to require (and check) that the JSON is null
 * terminated, and to retrieve the pointer to the final byte parsed. */
/* If you supply a ptr in return_parse_end and parsing fails, then
 * return_parse_end will contain a pointer to the error so will match
 * pika_cJSON_GetErrorPtr(). */
pika_cJSON_PUBLIC(cJSON*)
    pika_cJSON_ParseWithOpts(const char* value,
                             const char** return_parse_end,
                             pika_cJSON_bool require_null_terminated);
pika_cJSON_PUBLIC(cJSON*)
    pika_cJSON_ParseWithLengthOpts(const char* value,
                                   size_t buffer_length,
                                   const char** return_parse_end,
                                   pika_cJSON_bool require_null_terminated);

/* Render a cJSON entity to text for transfer/storage. */
pika_cJSON_PUBLIC(char*) pika_cJSON_Print(const cJSON* item);
/* Render a cJSON entity to text for transfer/storage without any
 * formatting. */
pika_cJSON_PUBLIC(char*) pika_cJSON_PrintUnformatted(const cJSON* item);
/* Render a cJSON entity to text using a buffered strategy. prebuffer is a
 * guess at the final size. guessing well reduces reallocation. fmt=0 gives
 * unformatted, =1 gives formatted */
pika_cJSON_PUBLIC(char*) pika_cJSON_PrintBuffered(const cJSON* item,
                                                  int prebuffer,
                                                  pika_cJSON_bool fmt);
/* Render a cJSON entity to text using a buffer already allocated in memory
 * with given length. Returns 1 on success and 0 on failure. */
/* NOTE: cJSON is not always 100% accurate in estimating how much memory it
 * will use, so to be safe allocate 5 bytes more than you actually need */
pika_cJSON_PUBLIC(pika_cJSON_bool)
    pika_cJSON_PrintPreallocated(cJSON* item,
                                 char* buffer,
                                 const int length,
                                 const pika_cJSON_bool format);
/* Delete a cJSON entity and all subentities. */
pika_cJSON_PUBLIC(void) pika_cJSON_Delete(cJSON* item);

/* Returns the number of items in an array (or object). */
pika_cJSON_PUBLIC(int) pika_cJSON_GetArraySize(const cJSON* array);
/* Retrieve item number "index" from array "array". Returns NULL if
 * unsuccessful. */
pika_cJSON_PUBLIC(cJSON*)
    pika_cJSON_GetArrayItem(const cJSON* array, int index);
/* Get item "string" from object. Case insensitive. */
pika_cJSON_PUBLIC(cJSON*) pika_cJSON_GetObjectItem(const cJSON* const object,
                                                   const char* const string);
pika_cJSON_PUBLIC(cJSON*)
    pika_cJSON_GetObjectItemCaseSensitive(const cJSON* const object,
                                          const char* const string);
pika_cJSON_PUBLIC(pika_cJSON_bool)
    pika_cJSON_HasObjectItem(const cJSON* object, const char* string);
/* For analysing failed parses. This returns a pointer to the parse error.
 * You'll probably need to look a few chars back to make sense of it.
 * Defined when pika_cJSON_Parse() returns 0. 0 when pika_cJSON_Parse()
 * succeeds. */
pika_cJSON_PUBLIC(const char*) pika_cJSON_GetErrorPtr(void);

/* Check item type and return its value */
pika_cJSON_PUBLIC(char*) pika_cJSON_GetStringValue(const cJSON* const item);
pika_cJSON_PUBLIC(double) pika_cJSON_GetNumberValue(const cJSON* const item);

/* These functions check the type of an item */
pika_cJSON_PUBLIC(pika_cJSON_bool)
    pika_cJSON_IsInvalid(const cJSON* const item);
pika_cJSON_PUBLIC(pika_cJSON_bool) pika_cJSON_IsFalse(const cJSON* const item);
pika_cJSON_PUBLIC(pika_cJSON_bool) pika_cJSON_IsTrue(const cJSON* const item);
pika_cJSON_PUBLIC(pika_cJSON_bool) pika_cJSON_IsBool(const cJSON* const item);
pika_cJSON_PUBLIC(pika_cJSON_bool) pika_cJSON_IsNull(const cJSON* const item);
pika_cJSON_PUBLIC(pika_cJSON_bool) pika_cJSON_IsNumber(const cJSON* const item);
pika_cJSON_PUBLIC(pika_cJSON_bool) pika_cJSON_IsString(const cJSON* const item);
pika_cJSON_PUBLIC(pika_cJSON_bool) pika_cJSON_IsArray(const cJSON* const item);
pika_cJSON_PUBLIC(pika_cJSON_bool) pika_cJSON_IsObject(const cJSON* const item);
pika_cJSON_PUBLIC(pika_cJSON_bool) pika_cJSON_IsRaw(const cJSON* const item);

/* These calls create a cJSON item of the appropriate type. */
pika_cJSON_PUBLIC(cJSON*) pika_cJSON_CreateNull(void);
pika_cJSON_PUBLIC(cJSON*) pika_cJSON_CreateTrue(void);
pika_cJSON_PUBLIC(cJSON*) pika_cJSON_CreateFalse(void);
pika_cJSON_PUBLIC(cJSON*) pika_cJSON_CreateBool(pika_cJSON_bool boolean);
pika_cJSON_PUBLIC(cJSON*) pika_cJSON_CreateNumber(double num);
pika_cJSON_PUBLIC(cJSON*) pika_cJSON_CreateString(const char* string);
/* raw json */
pika_cJSON_PUBLIC(cJSON*) pika_cJSON_CreateRaw(const char* raw);
pika_cJSON_PUBLIC(cJSON*) pika_cJSON_CreateArray(void);
pika_cJSON_PUBLIC(cJSON*) pika_cJSON_CreateObject(void);

/* Create a string where valuestring references a string so
 * it will not be freed by pika_cJSON_Delete */
pika_cJSON_PUBLIC(cJSON*) pika_cJSON_CreateStringReference(const char* string);
/* Create an object/array that only references it's elements so
 * they will not be freed by pika_cJSON_Delete */
pika_cJSON_PUBLIC(cJSON*) pika_cJSON_CreateObjectReference(const cJSON* child);
pika_cJSON_PUBLIC(cJSON*) pika_cJSON_CreateArrayReference(const cJSON* child);

/* These utilities create an Array of count items.
 * The parameter count cannot be greater than the number of elements in the
 * number array, otherwise array access will be out of bounds.*/
pika_cJSON_PUBLIC(cJSON*)
    pika_cJSON_CreateIntArray(const int* numbers, int count);
pika_cJSON_PUBLIC(cJSON*)
    pika_cJSON_CreateFloatArray(const double* numbers, int count);
pika_cJSON_PUBLIC(cJSON*)
    pika_cJSON_CreateDoubleArray(const double* numbers, int count);
pika_cJSON_PUBLIC(cJSON*)
    pika_cJSON_CreateStringArray(const char* const* strings, int count);

/* Append item to the specified array/object. */
pika_cJSON_PUBLIC(pika_cJSON_bool)
    pika_cJSON_AddItemToArray(cJSON* array, cJSON* item);
pika_cJSON_PUBLIC(pika_cJSON_bool)
    pika_cJSON_AddItemToObject(cJSON* object, const char* string, cJSON* item);
/* Use this when string is definitely const (i.e. a literal, or as good as),
 * and will definitely survive the cJSON object. WARNING: When this function
 * was used, make sure to always check that (item->type &
 * pika_cJSON_StringIsConst) is zero before writing to `item->string` */
pika_cJSON_PUBLIC(pika_cJSON_bool)
    pika_cJSON_AddItemToObjectCS(cJSON* object,
                                 const char* string,
                                 cJSON* item);
/* Append reference to item to the specified array/object. Use this when you
 * want to add an existing cJSON to a new cJSON, but don't want to corrupt
 * your existing cJSON. */
pika_cJSON_PUBLIC(pika_cJSON_bool)
    pika_cJSON_AddItemReferenceToArray(cJSON* array, cJSON* item);
pika_cJSON_PUBLIC(pika_cJSON_bool)
    pika_cJSON_AddItemReferenceToObject(cJSON* object,
                                        const char* string,
                                        cJSON* item);

/* Remove/Detach items from Arrays/Objects. */
pika_cJSON_PUBLIC(cJSON*)
    pika_cJSON_DetachItemViaPointer(cJSON* parent, cJSON* const item);
pika_cJSON_PUBLIC(cJSON*)
    pika_cJSON_DetachItemFromArray(cJSON* array, int which);
pika_cJSON_PUBLIC(void) pika_cJSON_DeleteItemFromArray(cJSON* array, int which);
pika_cJSON_PUBLIC(cJSON*)
    pika_cJSON_DetachItemFromObject(cJSON* object, const char* string);
pika_cJSON_PUBLIC(cJSON*)
    pika_cJSON_DetachItemFromObjectCaseSensitive(cJSON* object,
                                                 const char* string);
pika_cJSON_PUBLIC(void)
    pika_cJSON_DeleteItemFromObject(cJSON* object, const char* string);
pika_cJSON_PUBLIC(void)
    pika_cJSON_DeleteItemFromObjectCaseSensitive(cJSON* object,
                                                 const char* string);

/* Update array items. */
pika_cJSON_PUBLIC(pika_cJSON_bool) pika_cJSON_InsertItemInArray(
    cJSON* array,
    int which,
    cJSON* newitem); /* Shifts pre-existing items to the right. */
pika_cJSON_PUBLIC(pika_cJSON_bool)
    pika_cJSON_ReplaceItemViaPointer(cJSON* const parent,
                                     cJSON* const item,
                                     cJSON* replacement);
pika_cJSON_PUBLIC(pika_cJSON_bool)
    pika_cJSON_ReplaceItemInArray(cJSON* array, int which, cJSON* newitem);
pika_cJSON_PUBLIC(pika_cJSON_bool)
    pika_cJSON_ReplaceItemInObject(cJSON* object,
                                   const char* string,
                                   cJSON* newitem);
pika_cJSON_PUBLIC(pika_cJSON_bool)
    pika_cJSON_ReplaceItemInObjectCaseSensitive(cJSON* object,
                                                const char* string,
                                                cJSON* newitem);

/* Duplicate a cJSON item */
pika_cJSON_PUBLIC(cJSON*)
    pika_cJSON_Duplicate(const cJSON* item, pika_cJSON_bool recurse);
/* Duplicate will create a new, identical cJSON item to the one you pass, in
 * new memory that will need to be released. With recurse!=0, it will
 * duplicate any children connected to the item. The item->next and ->prev
 * pointers are always zero on return from Duplicate. */
/* Recursively compare two cJSON items for equality. If either a or b is
 * NULL or invalid, they will be considered unequal. case_sensitive
 * determines if object keys are treated case sensitive (1) or case
 * insensitive (0) */
pika_cJSON_PUBLIC(pika_cJSON_bool)
    pika_cJSON_Compare(const cJSON* const a,
                       const cJSON* const b,
                       const pika_cJSON_bool case_sensitive);

/* Minify a strings, remove blank characters(such as ' ', '\t', '\r', '\n')
 * from strings. The input pointer json cannot point to a read-only address
 * area, such as a string constant, but should point to a readable and
 * writable address area. */
pika_cJSON_PUBLIC(void) pika_cJSON_Minify(char* json);

/* Helper functions for creating and adding items to an object at the same
 * time. They return the added item or NULL on failure. */
pika_cJSON_PUBLIC(cJSON*)
    pika_cJSON_AddNullToObject(cJSON* const object, const char* const name);
pika_cJSON_PUBLIC(cJSON*)
    pika_cJSON_AddTrueToObject(cJSON* const object, const char* const name);
pika_cJSON_PUBLIC(cJSON*)
    pika_cJSON_AddFalseToObject(cJSON* const object, const char* const name);
pika_cJSON_PUBLIC(cJSON*)
    pika_cJSON_AddBoolToObject(cJSON* const object,
                               const char* const name,
                               const pika_cJSON_bool boolean);
pika_cJSON_PUBLIC(cJSON*) pika_cJSON_AddNumberToObject(cJSON* const object,
                                                       const char* const name,
                                                       const double number);
pika_cJSON_PUBLIC(cJSON*)
    pika_cJSON_AddStringToObject(cJSON* const object,
                                 const char* const name,
                                 const char* const string);
pika_cJSON_PUBLIC(cJSON*) pika_cJSON_AddRawToObject(cJSON* const object,
                                                    const char* const name,
                                                    const char* const raw);
pika_cJSON_PUBLIC(cJSON*)
    pika_cJSON_AddObjectToObject(cJSON* const object, const char* const name);
pika_cJSON_PUBLIC(cJSON*)
    pika_cJSON_AddArrayToObject(cJSON* const object, const char* const name);

/* When assigning an integer value, it needs to be propagated to valuedouble
 * too. */
#define pika_cJSON_SetIntValue(object, number)                        \
    ((object) ? (object)->valueint = (object)->valuedouble = (number) \
              : (number))
/* helper for the pika_cJSON_SetNumberValue macro */
pika_cJSON_PUBLIC(double)
    pika_cJSON_SetNumberHelper(cJSON* object, double number);
#define pika_cJSON_SetNumberValue(object, number)                          \
    ((object != NULL) ? pika_cJSON_SetNumberHelper(object, (double)number) \
                      : (number))
/* Change the valuestring of a pika_cJSON_String object, only takes effect
 * when type of object is pika_cJSON_String */
pika_cJSON_PUBLIC(char*)
    pika_cJSON_SetValuestring(cJSON* object, const char* valuestring);

/* Macro for iterating over an array or object */
#define pika_cJSON_ArrayForEach(element, array)                              \
    for (element = (array != NULL) ? (array)->child : NULL; element != NULL; \
         element = element->next)

/* malloc/free objects using the malloc/free functions that have been set
 * with pika_cJSON_InitHooks */
pika_cJSON_PUBLIC(void*) pika_cJSON_malloc(size_t size);
pika_cJSON_PUBLIC(void) pika_cJSON_free(void* object);

#ifdef __cplusplus
}
#endif

#endif
