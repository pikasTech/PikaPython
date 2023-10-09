#include "_flashdb_FlashDB.h"
#include <stdio.h>
#include "flashdb.h"
#include "pikaScript.h"
#include "_flashdb_kvdb_t.h"

//#include "fdb_def.h"
#define PIKA_USING_FLASHDB1 1
#if PIKA_USING_FLASHDB1
//#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "flashdb.h"
#define FDB_LOG_TAG "[main]"
/*
//static pthread_mutex_t kv_locker, ts_locker;
static uint32_t boot_count = 0;
static time_t boot_time[10] = {0, 1, 2, 3};
// default KV nodes 
static struct fdb_default_kv_node default_kv_table[] = {
        {"username", "armink", 0}, // string KV 
        {"password", "123456", 0}, // string KV 
        {"boot_count", &boot_count, sizeof(boot_count)}, // int type KV 
        {"boot_time", &boot_time, sizeof(boot_time)},    // int array type KV 
};
*/
/* KVDB object */
//static struct fdb_kvdb kvdb = { 0 };
/* TSDB object */
struct fdb_tsdb tsdb = { 0 };
/* counts for simulated timestamp */
static int counts = 0;

extern void kvdb_basic_sample(fdb_kvdb_t kvdb);
extern void kvdb_type_string_sample(fdb_kvdb_t kvdb);
extern void kvdb_type_blob_sample(fdb_kvdb_t kvdb);
extern void tsdb_sample(fdb_tsdb_t tsdb);

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
static fdb_time_t get_time(void)
{
    return time(NULL);
}
#endif

/* KVDB object */
static struct fdb_kvdb kvdb1 = {0};
typedef struct fdb_kvdb FDB_KVDB;
typedef struct fdb_default_kv_node FDB_DEFAULT_KV_NODE;
FDB_DEFAULT_KV_NODE* def_kv_table;
int def_kv_table_idx;

void _flashdb_FlashDB___init__(PikaObj *self){
  printf("fix me");
#if PIKA_USING_FLASHDB1
        __platform_printf(" FDB_LOG_TAG :%s \r\n", FDB_LOG_TAG);
    fdb_err_t result;
    bool file_mode = true;
    uint32_t sec_size = 4096, db_size = sec_size * 4;

#ifdef FDB_USING_KVDB
    { /* KVDB Sample */
        //struct fdb_default_kv default_kv;

        //default_kv.kvs = default_kv_table;
        //default_kv.num = sizeof(default_kv_table) / sizeof(default_kv_table[0]);
        /* set the lock and unlock function if you want */
  //      pthread_mutex_init(&kv_locker, NULL);
  //      fdb_kvdb_control(&kvdb, FDB_KVDB_CTRL_SET_LOCK, (void *)lock);
  //      fdb_kvdb_control(&kvdb, FDB_KVDB_CTRL_SET_UNLOCK, (void *)unlock);
        /* set the sector and database max size */
        fdb_kvdb_control(&kvdb1, FDB_KVDB_CTRL_SET_SEC_SIZE, &sec_size);
        fdb_kvdb_control(&kvdb1, FDB_KVDB_CTRL_SET_MAX_SIZE, &db_size);
        /* enable file mode */
        fdb_kvdb_control(&kvdb1, FDB_KVDB_CTRL_SET_FILE_MODE, &file_mode);
        /* create database directory */
        mkdir("fdb_kvdb1", 0777);
        return;

        /* Key-Value database initialization
         *
         *       &kvdb: database object
         *       "env": database name
         * "fdb_kvdb1": The flash partition name base on FAL. Please make sure it's in FAL partition table.
         *              Please change to YOUR partition name.
         * &default_kv: The default KV nodes. It will auto add to KVDB when first initialize successfully.
         *  &kv_locker: The locker object.
         */
        //result = fdb_kvdb_init(&kvdb, "env", "fdb_kvdb1", &default_kv, &kv_locker);
    uint32_t* boot_count = (uint32_t*) malloc(sizeof(uint32_t));
    time_t* boot_time = (time_t*) malloc(10* sizeof(time_t));
    *boot_count=0;
    boot_time[0]=0;
    boot_time[1]=1;

  struct fdb_default_kv_node* def_kv_table =(struct fdb_default_kv_node*) malloc(4 * sizeof(struct fdb_default_kv_node));
  def_kv_table[0].key = strdup("username");
  def_kv_table[0].value= strdup("armink");
  def_kv_table[0].value_len = 0;
  def_kv_table[1].key = strdup("password");
  def_kv_table[1].value= strdup("123456");
  def_kv_table[1].value_len = 0;
  def_kv_table[2].key = strdup("boot_count");
  def_kv_table[2].value= boot_count;
  def_kv_table[2].value_len = sizeof(*boot_count);
  def_kv_table[3].key = strdup("boot_time");
  def_kv_table[3].value= boot_time;
  def_kv_table[3].value_len = sizeof(*boot_time);
      struct fdb_default_kv default_kv;

       default_kv.kvs = def_kv_table;
        default_kv.num = 4; 

        result = fdb_kvdb_init(&kvdb1, "env", "fdb_kvdb1", &default_kv, NULL);

        if (result != FDB_NO_ERR) {
            return -1;
        }

        /* run basic KV samples */
        kvdb_basic_sample(&kvdb1);
        /* run string KV samples */
        kvdb_type_string_sample(&kvdb1);
        /* run blob KV samples */
        kvdb_type_blob_sample(&kvdb1);
    }
#endif /* FDB_USING_KVDB */
#endif

}

/*
Arg* _flashdb_FlashDB_blob_make(PikaObj *self, Arg* blob, Arg* value_buf, int buf_len){
  return NULL;
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
PikaObj* _flashdb_FlashDB_kv_get_blob(PikaObj *self, PikaObj* kvdb_in, char* key, int size){
  struct fdb_blob blob;
  FDB_KVDB* kvdb = (FDB_KVDB*) obj_getPtr(kvdb_in, "kvdb");
  printf("get_blob kvdb:%p\n", kvdb);
  blob.size = size;
  uint8_t* buf = (uint8_t*) pikaMalloc(size+1);
  if (!buf) {
    printf("alloc fail\n");
    return NULL;
  }   
  blob.buf = buf;
  size_t len =fdb_kv_get_blob(kvdb, key, &blob); 
  if (len != size) {
    printf("size error\n");
    pikaFree(buf, size+1);
    return NULL;
  }
  PikaList* list = New_pikaList();
  for (int i=0; i< len; i++) {
    pikaList_append(list, arg_newInt(buf[i]));
  }
  pikaFree(buf, size+1);
  return list;
}

Arg* _flashdb_FlashDB_kv_get_obj(PikaObj *self, Arg* kvdb, char* key, Arg* kv){
  return NULL;
}

Arg* _flashdb_FlashDB_kv_iterate(PikaObj *self, Arg* kvdb, Arg* itr){
  return NULL;
}

Arg* _flashdb_FlashDB_kv_iterator_init(PikaObj *self, Arg* kvdb, Arg* itr){
  return NULL;
}

void _flashdb_FlashDB_kv_print(PikaObj *self, PikaObj* kvdb_in) {
  //FDB_KVDB* kvdb = (FDB_KVDB*) obj_getPtr(kvdb_in,"kvdb");
  FDB_KVDB* kvdb = (FDB_KVDB*) obj_getPtr(kvdb_in, "kvdb");
  printf("kv_print kvdb:%p\n", kvdb);
  fdb_kv_print(kvdb);
}

int _flashdb_FlashDB_kv_set(PikaObj *self, Arg* kvdb, char* key, char* value){
  return 0;
}

int _flashdb_FlashDB_kv_set_blob(PikaObj *self, PikaObj* kvdb_in, char* key, Arg* blob_in){
  fdb_err_t res=FDB_NO_ERR;
   
  ArgType argt_kvdb_in = arg_getType(kvdb_in);
  if (argt_kvdb_in != ARG_TYPE_OBJECT) {
    printf("kvdb must be object but got:%d\n", argt_kvdb_in);
    return 0;
  }
  PikaObj* kvdb_obj = arg_getObj(kvdb_in);
  printf("kvdb_obj:%p\n", kvdb_obj);

  FDB_KVDB* kvdb = (FDB_KVDB*) obj_getPtr(kvdb_obj,"kvdb");
  printf("kvdb:%p\n", kvdb);

  ArgType argt_blob_in = arg_getType(blob_in);
  if (argt_blob_in != ARG_TYPE_BYTES) {
    printf("blob must be bytes but got:%d", argt_blob_in);
  }
  size_t len = arg_getBytesSize(blob_in);
  uint8_t* bytes = (uint8_t*) arg_getBytes(blob_in);
  printf("blob len:%d\n", len);

  struct fdb_blob blob;
  blob.size = len;
  blob.buf = bytes;


  //res = fdb_kv_set_blob(kvdb, key, &blob);
  
  return res;
  /*
  struct fdb_blob blob;
  PikaObj* kvdb_obj = arg_getObj(kvdb_in);
  FDB_KVDB* kvdb = (FDB_KVDB*) obj_getPtr(kvdb_obj,"kvdb");
  size_t len = arg_getBytesSize(blob_in);
  uint8_t* bytes = (uint8_t*) arg_getBytes(blob_in);
  blob.size = len;
  blob.buf = bytes;
  //res = fdb_kv_set_blob(kvdb, key, &blob);
  */
  return res;
}

int _flashdb_FlashDB_kv_set_default(PikaObj *self, Arg* kvdb){
  return 0;
}

Arg* _flashdb_FlashDB_kv_to_blob(PikaObj *self, Arg* kv, Arg* blob){
  return NULL;
}

int _flashdb_FlashDB_kvdb_control(PikaObj *self, Arg* kvdb, int cmd, Arg* arg){
  return 0;
}

void _flashdb_FlashDB_kvdb_deinit(PikaObj *self, Arg* kvdb){

}

int32_t  _flashdb_foreach(PikaObj* self, Arg* keyEach, Arg* valEach, void* context) {
  printf("each\n");
  char* key = arg_getStr(keyEach);
  printf("key:%s\n", key);
  ArgType argt_val = arg_getType(valEach);
  printf("val type:%d\n", argt_val);
  struct fdb_default_kv_node* def_kv_table =(struct fdb_default_kv_node*) context;
  if (argt_val==ARG_TYPE_STRING) {
    char* val = arg_getStr(valEach);
    printf("val:%s\n", val);
    
    def_kv_table[def_kv_table_idx].key = strdup(key);
    def_kv_table[def_kv_table_idx].value= strdup(val);
    def_kv_table[def_kv_table_idx].value_len = 0;
    def_kv_table_idx++;
    
  } else if (argt_val==ARG_TYPE_BYTES) {
    char buff_item[16] = {0};
    size_t bytes_size = arg_getBytesSize(valEach);
    uint8_t* bytes = arg_getBytes(valEach);
    uint8_t* pbytes = (uint8_t*) malloc(bytes_size * sizeof(uint8_t));
    memcpy(pbytes, bytes, bytes_size);
    printf("byte size:%d\n", bytes_size);
    for (size_t i=0; i < bytes_size; i++) {
      printf("%02x", bytes[i]);
    }
    printf("\n");
    
    def_kv_table[def_kv_table_idx].key = strdup(key);
    def_kv_table[def_kv_table_idx].value= pbytes;
    def_kv_table[def_kv_table_idx].value_len = bytes_size;
    def_kv_table_idx++;
    
  }
  return 0;
}
  
struct fdb_default_kv default_kv;


PikaObj* _flashdb_FlashDB_kvdb_init(PikaObj *self, char* name, char* path, PikaObj* default_kv_in, Arg* user_data){
  printf("kvdb_init \n");
  fdb_err_t result;

  int len =pikaDict_getSize(default_kv_in);
  printf("len:%d\n", len);
  /*
  def_kv_table =(FDB_DEFAULT_KV_NODE*) malloc(len * sizeof(struct fdb_default_kv_node));
  def_kv_table_idx=0;

  pikaDict_forEach(default_kv_in,  _flashdb_foreach, NULL);
*/
/*
   uint32_t* boot_count = (uint32_t*) malloc(sizeof(uint32_t));
    time_t* boot_time = (time_t*) malloc(10* sizeof(time_t));
    *boot_count=0;
    boot_time[0]=0;
    boot_time[1]=1;
    */

  struct fdb_default_kv_node* def_kv_table =(struct fdb_default_kv_node*) malloc(4 * sizeof(struct fdb_default_kv_node));
  /*
  def_kv_table[0].key = strdup("username");
  def_kv_table[0].value= strdup("armink");
  def_kv_table[0].value_len = 0;
  def_kv_table[1].key = strdup("password");
  def_kv_table[1].value= strdup("123456");
  def_kv_table[1].value_len = 0;
  def_kv_table[2].key = strdup("boot_count");
  def_kv_table[2].value= boot_count;
  def_kv_table[2].value_len = sizeof(*boot_count);
  def_kv_table[3].key = strdup("boot_time");
  def_kv_table[3].value= boot_time;
  def_kv_table[3].value_len = sizeof(*boot_time);
*/
  //def_kv_table =(FDB_DEFAULT_KV_NODE*) malloc(len * sizeof(struct fdb_default_kv_node));
  def_kv_table_idx=0;
  pikaDict_forEach(default_kv_in,  _flashdb_foreach, def_kv_table);

  struct fdb_default_kv default_kv;

  default_kv.kvs = def_kv_table;
  default_kv.num = 4; 

  printf("name:'%s'\n",name);
  printf("path:'%s'\n", path);
  //result = fdb_kvdb_init(&kvdb1, "env", "fdb_kvdb1", &default_kv, NULL);
  result = fdb_kvdb_init(&kvdb1, strdup(name), strdup(path), &default_kv, NULL);

  //result = fdb_kvdb_init(&kvdb1, name, path, &default_kv, NULL);
  printf("kvdb init result:%d\n", result);
 

  if (result != FDB_NO_ERR)
  {
            return NULL;
  }
  //PikaObj* kvdb_obj = New_PikaObj();
  PikaObj* kvdb_obj = newNormalObj(New__flashdb_kvdb_t);
  args_setStruct(kvdb_obj->list, "kvdb_struct", kvdb1);
  FDB_KVDB* pkvdb = args_getStruct(kvdb_obj->list, "kvdb_struct");
  //obj_setStruct(kvdb_obj, "kvdb", kvdb1);
  obj_setPtr(kvdb_obj, "kvdb", pkvdb);
  return kvdb_obj;
}


