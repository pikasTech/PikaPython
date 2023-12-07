#include "_flashdb_FlashDB.h"
#include <stdio.h>
#include "_flashdb_kvdb_t.h"
#include "flashdb.h"
#include "pikaScript.h"

// #include "fdb_def.h"
#define PIKA_USING_FLASHDB1 1
#if PIKA_USING_FLASHDB1
// #include <pthread.h>
#include "flashdb.h"
#define FDB_LOG_TAG "[main]"

#define _FDBBUFFS (Args*)obj_getPtr(self, "FDBBUFFS")
#define strdup(x) strsCopy(_FDBBUFFS, x)

pika_bool g_kvdb_path_inited = pika_false;

/* TSDB object */
// struct fdb_tsdb tsdb = { 0 };
/* counts for simulated timestamp */
// static int counts = 0;

// extern void kvdb_basic_sample(fdb_kvdb_t kvdb);
// extern void kvdb_type_string_sample(fdb_kvdb_t kvdb);
// extern void kvdb_type_blob_sample(fdb_kvdb_t kvdb);
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

/* KVDB object */
static struct fdb_kvdb g_kvdb = {0};
typedef struct fdb_kvdb FDB_KVDB;
typedef struct fdb_default_kv_node FDB_DEFAULT_KV_NODE;
int g_def_kv_table_idx;

void _flashdb_FlashDB___init__(PikaObj* self) {
    Args* buffs = New_strBuff();
    obj_setPtr(self, "FDBBUFFS", buffs);
}

/*
Arg* _flashdb_FlashDB_blob_make(PikaObj *self, Arg* blob, Arg* value_buf, int
buf_len){ return NULL;
}

Arg* _flashdb_FlashDB_blob_read(PikaObj *self, Arg* db, Arg* blob){
  return NULL;
}

int _flashdb_FlashDB_kv_del(PikaObj *self, Arg* kvdb, char* key){
  return 0;
}

Arg* _flashdb_FlashDB_kv_get(PikaObj *self, Arg* kvdb, char* key){
  return NULL;
}
*/
PikaObj* _flashdb_FlashDB_kv_get_blob(PikaObj* self,
                                      PikaObj* kvdb_in,
                                      char* key,
                                      int size) {
    struct fdb_blob blob;
    FDB_KVDB* kvdb = (FDB_KVDB*)obj_getPtr(kvdb_in, "kvdb");
    blob.size = size;
    uint8_t* buf = (uint8_t*)pikaMalloc(size + 1);
    if (!buf) {
        pika_platform_printf("alloc fail\n");
        return NULL;
    }
    blob.buf = buf;
    size_t len = fdb_kv_get_blob(kvdb, key, &blob);
    if (len != size) {
        pika_platform_printf("size error\n");
        pikaFree(buf, size + 1);
        return NULL;
    }
    PikaList* list = New_pikaList();
    for (int i = 0; i < len; i++) {
        pikaList_append(list, arg_newInt(buf[i]));
    }
    pikaFree(buf, size + 1);
    return list;
}

Arg* _flashdb_FlashDB_kv_get_obj(PikaObj* self, Arg* kvdb, char* key, Arg* kv) {
    return NULL;
}

Arg* _flashdb_FlashDB_kv_iterate(PikaObj* self, Arg* kvdb, Arg* itr) {
    return NULL;
}

Arg* _flashdb_FlashDB_kv_iterator_init(PikaObj* self, Arg* kvdb, Arg* itr) {
    return NULL;
}

void _flashdb_FlashDB_kv_print(PikaObj* self, PikaObj* kvdb_in) {
    FDB_KVDB* kvdb = (FDB_KVDB*)obj_getPtr(kvdb_in, "kvdb");
    fdb_kv_print(kvdb);
}

int _flashdb_FlashDB_kv_set(PikaObj* self, Arg* kvdb, char* key, char* value) {
    return 0;
}

int _flashdb_FlashDB_kv_set_blob(PikaObj* self,
                                 PikaObj* kvdb_in,
                                 char* key,
                                 Arg* blob_in) {
    fdb_err_t res = FDB_NO_ERR;
    FDB_KVDB* kvdb = (FDB_KVDB*)obj_getPtr(kvdb_in, "kvdb");

    ArgType argt_blob_in = arg_getType(blob_in);
    if (argt_blob_in != ARG_TYPE_BYTES) {
        pika_platform_printf("blob must be bytes but got:%d", argt_blob_in);
    }
    size_t len = arg_getBytesSize(blob_in);
    uint8_t* bytes = (uint8_t*)arg_getBytes(blob_in);

    struct fdb_blob blob;
    blob.size = len;
    blob.buf = bytes;

    res = fdb_kv_set_blob(kvdb, key, &blob);

    return res;
}

int _flashdb_FlashDB_kv_set_default(PikaObj* self, Arg* kvdb) {
    return 0;
}

Arg* _flashdb_FlashDB_kv_to_blob(PikaObj* self, Arg* kv, Arg* blob) {
    return NULL;
}

int _flashdb_FlashDB_kvdb_control(PikaObj* self, int cmd, Arg* arg) {
    return 0;
}

void _flashdb_FlashDB_kvdb_deinit(PikaObj* self) {
    fdb_kvdb_deinit(&g_kvdb);
}

struct _flashdb_foreach_context {
    struct fdb_default_kv_node* def_kv_table;
    PikaObj* self;
};
int32_t _flashdb_foreach(PikaObj* self_dict,
                         Arg* keyEach,
                         Arg* valEach,
                         void* context) {
    char* key = arg_getStr(keyEach);
    ArgType argt_val = arg_getType(valEach);
    struct _flashdb_foreach_context* foreach_context =
        (struct _flashdb_foreach_context*)context;
    struct fdb_default_kv_node* def_kv_table = foreach_context->def_kv_table;
    PikaObj* self = foreach_context->self;

    if (argt_val == ARG_TYPE_STRING) {
        char* val = arg_getStr(valEach);

        def_kv_table[g_def_kv_table_idx].key = strdup(key);
        def_kv_table[g_def_kv_table_idx].value = strdup(val);
        def_kv_table[g_def_kv_table_idx].value_len = 0;
        g_def_kv_table_idx++;

    } else if (argt_val == ARG_TYPE_BYTES) {
        size_t bytes_size = arg_getBytesSize(valEach);
        uint8_t* bytes = arg_getBytes(valEach);
        uint8_t* pbytes =
            (uint8_t*)args_getBuff(_FDBBUFFS, bytes_size * sizeof(uint8_t));
        memcpy(pbytes, bytes, bytes_size);
        /*
        for (size_t i=0; i < bytes_size; i++) {
          pika_platform_printf("%02x", bytes[i]);
        }
        pika_platform_printf("\n");
        */
        def_kv_table[g_def_kv_table_idx].key = strdup(key);
        def_kv_table[g_def_kv_table_idx].value = pbytes;
        def_kv_table[g_def_kv_table_idx].value_len = bytes_size;
        g_def_kv_table_idx++;
    }
    return 0;
}

PikaObj* _flashdb_FlashDB_kvdb_init(PikaObj* self,
                                    char* name,
                                    char* path,
                                    PikaObj* default_kv_in,
                                    Arg* user_data) {
    pika_platform_printf("kvdb_init \n");

    fdb_err_t result;
    if (!g_kvdb_path_inited) {
        pika_bool file_mode = pika_true;
        uint32_t sec_size = 4096, db_size = sec_size * 4;

        fdb_kvdb_control(&g_kvdb, FDB_KVDB_CTRL_SET_SEC_SIZE, &sec_size);
        fdb_kvdb_control(&g_kvdb, FDB_KVDB_CTRL_SET_MAX_SIZE, &db_size);
        /* enable file mode */
        fdb_kvdb_control(&g_kvdb, FDB_KVDB_CTRL_SET_FILE_MODE, &file_mode);
        /* create database directory */
        pika_platform_mkdir(path, 0777);
        g_kvdb_path_inited = pika_true;
    }
    // int len =pikaDict_getSize(default_kv_in);

    struct fdb_default_kv_node* def_kv_table =
        (struct fdb_default_kv_node*)args_getBuff(
            _FDBBUFFS, (4 * sizeof(struct fdb_default_kv_node)));
    g_def_kv_table_idx = 0;

    struct _flashdb_foreach_context context = {
        .def_kv_table = def_kv_table,
        .self = self,
    };
    pikaDict_forEach(default_kv_in, _flashdb_foreach, &context);

    struct fdb_default_kv default_kv;

    default_kv.kvs = def_kv_table;
    default_kv.num = 4;

    result =
        fdb_kvdb_init(&g_kvdb, strdup(name), strdup(path), &default_kv, NULL);

    if (result != FDB_NO_ERR) {
        return NULL;
    }

    PikaObj* kvdb_obj = newNormalObj(New__flashdb_kvdb_t);
    obj_setPtr(kvdb_obj, "kvdb", &g_kvdb);
    return kvdb_obj;
}

void _flashdb_FlashDB___del__(PikaObj* self) {
    Args* buffs = _FDBBUFFS;
    if (NULL != buffs) {
        args_deinit(_FDBBUFFS);
    }
}

#undef strudp
