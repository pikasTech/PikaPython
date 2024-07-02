#include "_flashdb_TSDB.h"
#include "_flashdb_TSL.h"
#include <stdio.h>
#include "flashdb.h"

// #include "fdb_def.h"
#define PIKA_USING_FLASHDB1 1
#if PIKA_USING_FLASHDB1
// #include <pthread.h>
#include "flashdb.h"
#define FDB_LOG_TAG "[main]"

#define _FDBBUFFS (Args*)obj_getPtr(self, "FDBBUFFS")
#define strdup(x) strsCopy(_FDBBUFFS, x)

/* TSDB object */
// struct fdb_tsdb tsdb = { 0 };
/* counts for simulated timestamp */
// static int counts = 0;

// extern void tsdb_basic_sample(fdb_tsdb_t tsdb);
// extern void tsdb_type_string_sample(fdb_tsdb_t tsdb);
// extern void tsdb_type_blob_sample(fdb_tsdb_t tsdb);
// extern void tsdb_sample(fdb_tsdb_t tsdb);

/*
static void lock(fdb_db_t db)
{
    pthread_mutex_lock((pthread_mutex_t *)db->user_data);
}

static void unlock(fdb_db_t db)
{
    pthread_mutex_unlock((pthread_mutex_t *)db->user_data);
}
*/
static fdb_time_t get_time(void) {
    // ms to s
    return pika_platform_get_tick() / 1000;
}
#endif

/* TSDB object */

typedef struct _fdb_tsdb_context {
    struct fdb_tsdb tsdb;
    pika_bool path_inited;
} fdb_tsdb_context;

#define _OBJ2TSDB_CONTEXT(x) ((fdb_tsdb_context*)obj_getStruct(x, "tsdbctx"))
#define _OBJ2TSDB(x) (&_OBJ2TSDB_CONTEXT(x)->tsdb)

typedef struct fdb_tsdb FDB_TSDB;
typedef struct fdb_default_kv_node FDB_DEFAULT_KV_NODE;

/*
Arg* _flashdb_TSDB_blob_make(PikaObj *self, Arg* blob, Arg* value_buf, int
buf_len){ return NULL;
}

Arg* _flashdb_TSDB_blob_read(PikaObj *self, Arg* db, Arg* blob){
  return NULL;
}

int _flashdb_TSDB_kv_del(PikaObj *self, Arg* tsdb, char* key){
  return 0;
}

Arg* _flashdb_TSDB_kv_get(PikaObj *self, Arg* tsdb, char* key){
  return NULL;
}
*/

int _flashdb_TSDB_tsl_append(PikaObj* self, Arg* blob_in) {
    fdb_err_t res = FDB_NO_ERR;
    FDB_TSDB* tsdb = _OBJ2TSDB(self);

    ArgType argt_blob_in = arg_getType(blob_in);
    if (argt_blob_in != ARG_TYPE_BYTES) {
        pika_platform_printf("blob must be bytes but got:%d", argt_blob_in);
    }
    size_t len = arg_getBytesSize(blob_in);
    uint8_t* bytes = (uint8_t*)arg_getBytes(blob_in);

    struct fdb_blob blob;
    blob.size = len;
    blob.buf = bytes;

    res = fdb_tsl_append(tsdb, &blob);
    return res;
}

int _flashdb_TSDB_set_default(PikaObj* self, Arg* tsdb) {
    return 0;
}

int _flashdb_TSDB_control(PikaObj* self, int cmd, Arg* arg) {
    return -1;
}

void _flashdb_TSDB_deinit(PikaObj* self) {
    fdb_tsdb_deinit(_OBJ2TSDB(self));
}

struct _kvdb_foreach_context {
    struct fdb_default_kv_node* def_kv_table;
    PikaObj* self;
};

void _flashdb_TSDB___init__(PikaObj* self,
                            char* name,
                            char* path,
                            int max_len,
                            Arg* user_data) {
    pika_platform_printf("tsdb_init \n");
    if (NULL == _OBJ2TSDB_CONTEXT(self)) {
        Args* buffs = New_strBuff();
        obj_setPtr(self, "FDBBUFFS", buffs);
        // create tsdb context if not exist
        fdb_tsdb_context tsdb_initial = {
            .tsdb = {0},
            .path_inited = pika_false,
        };
        obj_setStruct(self, "tsdbctx", tsdb_initial);
    }
    fdb_tsdb_context* tsdb_context = _OBJ2TSDB_CONTEXT(self);
    fdb_tsdb_t tsdb_this = &tsdb_context->tsdb;
    fdb_err_t result;
    if (!tsdb_context->path_inited) {
        pika_bool file_mode = pika_true;
        uint32_t sec_size = 4096, db_size = sec_size * 4;

        fdb_tsdb_control(tsdb_this, FDB_TSDB_CTRL_SET_SEC_SIZE, &sec_size);
        fdb_tsdb_control(tsdb_this, FDB_TSDB_CTRL_SET_MAX_SIZE, &db_size);
        /* enable file mode */
        fdb_tsdb_control(tsdb_this, FDB_TSDB_CTRL_SET_FILE_MODE, &file_mode);
        /* create database directory */
        pika_platform_mkdir(path, 0777);
        tsdb_context->path_inited = pika_true;
    }
    // int len =pikaDict_getSize(default_kv_in);

    result = fdb_tsdb_init(tsdb_this, strdup(name), strdup(path),
                           pika_platform_get_tick, max_len, NULL);

    if (result != FDB_NO_ERR) {
        obj_setSysOut(self, "tsdb_init fail");
        obj_setErrorCode(self, result);
    }
}

void _flashdb_TSDB___del__(PikaObj* self) {
    Args* buffs = _FDBBUFFS;
    if (NULL != buffs) {
        args_deinit(_FDBBUFFS);
    }
    if (NULL != _OBJ2TSDB_CONTEXT(self)) {
        fdb_tsdb_deinit(_OBJ2TSDB(self));
    }
}

void _flashdb_TSDB_CTRL___init__(PikaObj* self) {
    obj_setInt(self, "SET_SEC_SIZE", FDB_TSDB_CTRL_SET_SEC_SIZE);
    obj_setInt(self, "GET_SEC_SIZE", FDB_TSDB_CTRL_GET_SEC_SIZE);
    obj_setInt(self, "SET_LOCK", FDB_TSDB_CTRL_SET_LOCK);
    obj_setInt(self, "SET_UNLOCK", FDB_TSDB_CTRL_SET_UNLOCK);
    obj_setInt(self, "SET_FILE_MODE", FDB_TSDB_CTRL_SET_FILE_MODE);
    obj_setInt(self, "SET_MAX_SIZE", FDB_TSDB_CTRL_SET_MAX_SIZE);
    obj_setInt(self, "SET_NOT_FORMAT", FDB_TSDB_CTRL_SET_NOT_FORMAT);
}

#define _OBJ2TSL(self) obj_getPtr(self, "TSL")
#define _OBJSETTSL(self, tsl) obj_setPtr(self, "TSL", tsl)

int64_t _flashdb_TSL_get_time(PikaObj* self) {
    fdb_tsl_t tsl = _OBJ2TSL(self);
    if (NULL == tsl) {
        return -1;
    }
    return tsl->time;
}

fdb_blob_t blob_alloc(fdb_blob_t blob) {
    uint8_t* buf = (uint8_t*)pikaMalloc(blob->saved.len + 1);
    if (!buf) {
        pika_platform_printf("alloc fail\n");
        return NULL;
    }
    blob->buf = buf;
    blob->size = blob->saved.len;
    return blob;
}

int fdb_blob_free(fdb_blob_t blob) {
    if (blob) {
        pikaFree(blob->buf, blob->size + 1);
        blob->buf = NULL;
        blob->size = 0;
    }
    return 0;
}

Arg* _flashdb_TSL_to_blob(PikaObj* self) {
    fdb_tsl_t tsl = _OBJ2TSL(self);
    fdb_tsdb_t tsdb = obj_getPtr(self, "tsdb");
    if (NULL == tsl) {
        return NULL;
    }
    struct fdb_blob blob;
    fdb_tsl_to_blob(tsl, &blob);
    if (NULL == blob_alloc(&blob)) {
        return NULL;
    }
    fdb_blob_read((fdb_db_t)tsdb, &blob);
    Arg* res = arg_newBytes((uint8_t*)blob.buf, blob.size);
    fdb_blob_free(&blob);
    return res;
}

typedef struct _tsdb_foreach_context {
    Arg* callback;
    Arg* user_data;
    fdb_tsdb_t tsdb;
} tsdb_foreach_context;

PikaObj* New__flashdb_TSDB(Args* args);
pika_bool _flashdb_TSL_iter_callback(fdb_tsl_t tsl, void* arg) {
    tsdb_foreach_context* context = (tsdb_foreach_context*)arg;
    Arg* callback = context->callback;
    Arg* user_data = context->user_data;
    PikaObj* tsl_obj = newNormalObj(New__flashdb_TSL);
    _OBJSETTSL(tsl_obj, tsl);
    obj_setPtr(tsl_obj, "tsdb", context->tsdb);
    Arg* ret = pika_runFunction2(arg_copy(callback), arg_newObj(tsl_obj),
                                 arg_copy(user_data));

    if (NULL == ret) {
        return pika_true;
    }
    pika_bool res = arg_getBool(ret);
    arg_deinit(ret);
    return res;
}

int _flashdb_TSDB_tsl_iter(PikaObj* self, Arg* callback, Arg* user_data) {
    fdb_tsdb_t tsdb = _OBJ2TSDB(self);
    tsdb_foreach_context context = {
        .callback = callback,
        .user_data = user_data,
        .tsdb = tsdb,
    };
    fdb_tsl_iter(tsdb, _flashdb_TSL_iter_callback, &context);
    return 0;
}

#undef strudp
