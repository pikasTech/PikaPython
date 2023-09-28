#include "pikaScript.h"
#include "nimble/ble.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
// #include "nimble/host/include/host/ble_gap.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "services/ans/ble_svc_ans.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_log.h"
// #include "ble_uuid.h"
#include "cb_event_id.h"
// TODO:发布的时候怎么将printf隐藏掉
// #define printf __platform_printf
#define printf pika_platform_printf

#define GATT_SVR_SVC_ALERT_UUID               0x1811
// BLE_UUID_base = 0x00000000-0000-1000-8000-00805F9B34FB;

static const char *tag = "NimBLE_BLE";
bool BLE_ONLY = false;                           //只使用BLE,默认否
bool FLASH_FIRST_INIT = true;                    //是否第一次初始化,默认是
bool BLE_INIT   = false;                         //蓝牙是否初始化,默认不是
PikaEventListener *g_pika_ble_listener = NULL;   // 事件监听器
int  g_ble_addr_mode = 0;
bool g_ble_connectable  = 1;
int  g_interval = 0;
uint8_t g_uuid_len = 2;
uint8_t g_uuid[16] = {0x01,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
uint8_t g_all_chr_count = 0;
uint16_t * g_chrs_handle = NULL; 

struct ble_gatt_svc_def* gatt_svr_svcs = NULL; // BLE服务句柄

/* A characteristic that can be subscribed to */
static uint8_t gatt_svr_chr_val;
static uint16_t gatt_svr_chr_val_handle;

/* A custom descriptor */
static uint8_t gatt_svr_dsc_val;

// 函数声明
// GATT 服务端回调函数
static int ble_gatt_svc_access_cb(uint16_t conn_handle, uint16_t attr_handle,struct ble_gatt_access_ctxt *ctxt, void *arg);

// GATT 客户端写回调函数
static int ble_gatt_client_write_cb(uint16_t conn_handle,
                       const struct ble_gatt_error *error,
                       struct ble_gatt_attr *attr,
                       void *arg);

// GATT 客户端读回调函数
static int ble_gatt_client_read_cb(uint16_t conn_handle,
                       const struct ble_gatt_error *error,
                       struct ble_gatt_attr *attr,
                       void *arg);

// GATT mtu_exchange回调函数
static int ble_gatt_mtu_exchange_cb(uint16_t conn_handle,
                            const struct ble_gatt_error *error,
                            uint16_t mtu, void *arg);

// GATT 查找所有服务回调函数
static int ble_gatt_disc_all_svcs_cb(uint16_t conn_handle,
                                 const struct ble_gatt_error *error,
                                 const struct ble_gatt_svc *service,
                                 void *arg);

// GATT 查找所有服务回调函数
static int ble_gatt_disc_svcs_by_uuid_cb(uint16_t conn_handle,
                                 const struct ble_gatt_error *error,
                                 const struct ble_gatt_svc *service,
                                 void *arg);

// GATT 查找所有特性回调函数
static int ble_gatt_disc_all_chrs_cb(uint16_t conn_handle,
                            const struct ble_gatt_error *error,
                            const struct ble_gatt_chr *chr, void *arg);

// GATT 查找特定UUID特性回调函数
static int ble_gatt_disc_chrs_by_uuid_cb(uint16_t conn_handle,
                            const struct ble_gatt_error *error,
                            const struct ble_gatt_chr *chr, void *arg);

// GATT 查找所有描述符回调函数
static int ble_gatt_disc_all_dscs_cb(uint16_t conn_handle,
                            const struct ble_gatt_error *error,
                            uint16_t chr_val_handle,
                            const struct ble_gatt_dsc *dsc, void *arg);

// GAP层回调函数
static int ble_gap_event_cb(struct ble_gap_event *event, void *arg);

// gatt初始化基本服务
void gatt_svr_init(void);

// 从字符串中读取UUID
int read_uuid_from_str(char* buf, int len, ble_uuid_any_t* uuid_struct);

// 反转数组
void data_inver(const void *addr,uint8_t *addr_inver,uint8_t size);

int _bluetooth_BLE_adv(int interval);

// 填充UUID
int fill_UUID(ble_uuid_any_t* UUID,int UUID_bytes, uint8_t* bytes_UUID)
{
    if(UUID_bytes == 2){
        UUID->u.type       = BLE_UUID_TYPE_16;
        UUID->u16.value    = bytes_UUID[0] << 8 | bytes_UUID[1];
    }else if(UUID_bytes == 4) {
        UUID->u.type       = BLE_UUID_TYPE_32;
        UUID->u32.value    = bytes_UUID[0] << 24 | bytes_UUID[1] << 16 | bytes_UUID[2] << 8 | bytes_UUID[3];
    }else{
        UUID->u.type       = BLE_UUID_TYPE_128;
        data_inver(bytes_UUID,UUID->u128.value,16);
    }
    return 0;
}

// 蓝牙任务
void ble_host_task(void *param)
{
    ESP_LOGI(tag, "BLE Host Task Started");
    /* This function will return only when nimble_port_stop() is executed */
    nimble_port_run();
    nimble_port_freertos_deinit();
}

// 获取地址类型
uint8_t get_addr_type(int addr_mode)
{
    uint8_t own_addr_type;
    switch (addr_mode) {
        case 0:
            own_addr_type = BLE_OWN_ADDR_PUBLIC;
            break;
        case 1:
            own_addr_type = BLE_OWN_ADDR_RANDOM;
            break;
        case 2:
            own_addr_type = BLE_OWN_ADDR_RPA_PUBLIC_DEFAULT;
            break;
        case 3:
            own_addr_type = BLE_OWN_ADDR_RPA_RANDOM_DEFAULT;
            break;
    }
    return own_addr_type;
}

// int _bluetooth_BLE_init(PikaObj *self)
int _bluetooth_BLE_init(PikaObj *self)
{
    printf("_bluetooth_BLE___init__\r\n");
    if (FLASH_FIRST_INIT)
    {
        //初始化flash
        esp_err_t ret = nvs_flash_init();
        if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
            ESP_ERROR_CHECK(nvs_flash_erase());
            ret = nvs_flash_init();
        }
        ESP_ERROR_CHECK(ret);
        FLASH_FIRST_INIT = false;
    }
    return 1;
}

pika_bool _bluetooth_BLE_pyi_active(PikaObj *self, pika_bool active)
{
    printf("_bluetooth_BLE_pyi_active\r\n");
    if(active == true){
        //开始任务
        // nimble_port_freertos_init(ble_host_task);
        // 初始化堆栈
        // TODO: 多蓝牙对象时会报错
        // printf("ble_hs_is_enabled : %d\r\n",ble_hs_is_enabled());
        if(!BLE_INIT){
            nimble_port_init();
            BLE_INIT = true;
        }
        return true;
    }else {
        if(BLE_INIT){
            nimble_port_deinit();
            BLE_INIT = false;
        }
        // nimble_port_stop();
        return false;
    }
}

pika_bool _bluetooth_BLE_pyi_check_active(PikaObj *self)
{
    printf("_bluetooth_BLE_pyi_check_active\r\n");
    return BLE_INIT;
}

int _bluetooth_BLE_pyi_test(PikaObj *self)
{
    printf("_bluetooth_BLE_test\r\n");
    return 1;
}

void data_inver(const void *addr,uint8_t *addr_inver,uint8_t size)
{
    const uint8_t *u8p;
    u8p = addr;
    for ( int i = 0; i < size; i++){
        addr_inver[i] =  u8p[size - i - 1];
    }
}

void print_addr(const void *addr)
{
    const uint8_t *u8p;
    u8p = addr;
    printf("%02x:%02x:%02x:%02x:%02x:%02x\r\n",u8p[5], u8p[4], u8p[3], u8p[2], u8p[1], u8p[0]);
}

void print_uuid_128(uint8_t *uuid)
{
    for (size_t i = 0; i < 16; i++)
        {
            printf("%02x",uuid[15 - i]);
        }
        printf("\r\n");
}

void print_data(uint8_t *data,uint8_t count)
{
    for (size_t i = 0; i < count; i++)
        {
            printf("%02x",data[i]);
        }
        printf("\r\n");
}

/**
 * Logs information about a connection to the console.
 */
static void print_conn_desc(struct ble_gap_conn_desc *desc)
{
    MODLOG_DFLT(INFO, "handle=%d our_ota_addr_type=%d our_ota_addr=",
                desc->conn_handle, desc->our_ota_addr.type);
    print_addr(desc->our_ota_addr.val);
    MODLOG_DFLT(INFO, " our_id_addr_type=%d our_id_addr=",
                desc->our_id_addr.type);
    print_addr(desc->our_id_addr.val);
    MODLOG_DFLT(INFO, " peer_ota_addr_type=%d peer_ota_addr=",
                desc->peer_ota_addr.type);
    print_addr(desc->peer_ota_addr.val);
    MODLOG_DFLT(INFO, " peer_id_addr_type=%d peer_id_addr=",
                desc->peer_id_addr.type);
    print_addr(desc->peer_id_addr.val);
    MODLOG_DFLT(INFO, " conn_itvl=%d conn_latency=%d supervision_timeout=%d "
                "encrypted=%d authenticated=%d bonded=%d\r\n",
                desc->conn_itvl, desc->conn_latency,
                desc->supervision_timeout,
                desc->sec_state.encrypted,
                desc->sec_state.authenticated,
                desc->sec_state.bonded);
}

int _bluetooth_BLE_advertise(PikaObj *self, int addr, int interval, pika_bool connectable, 
        uint8_t * adv_data, int adv_data_len, pika_bool adv_data_append, uint8_t * rsp_data, int rsp_data_len)
{
    nimble_port_freertos_init(ble_host_task);
    printf("_bluetooth_BLE_gap_advertise\r\n");
    //  声明并初始化广播结构体
    struct ble_hs_adv_fields fields;
    memset(&fields, 0, sizeof fields);

    fields.flags = BLE_HS_ADV_F_DISC_GEN ;
    if(BLE_ONLY  == true){
        fields.flags |= BLE_HS_ADV_F_BREDR_UNSUP;
    }

    if(adv_data_append)
    {
        fields.tx_pwr_lvl_is_present = 0;
        fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;

        char* name = ble_svc_gap_device_name();
        fields.name = (uint8_t *)name;
        fields.name_len = strlen(name);
        fields.name_is_complete = 1;

        // TODO:UUID修改成可变的,用户在哪修改?
        fields.uuids16 = (ble_uuid16_t[]) {
            BLE_UUID16_INIT(GATT_SVR_SVC_ALERT_UUID)
        };
        fields.num_uuids16 = 1;
        fields.uuids16_is_complete = 1;
        if(adv_data_len > 0)
        {
            fields.mfg_data = (uint8_t *)adv_data;
            fields.mfg_data_len = adv_data_len;
        }
        int rc = ble_gap_adv_set_fields(&fields);
        if (rc != 0) {
        printf("error setting advertisement data; rc=%d\r\n", rc);
        return -1 ;
    }
    }else{
        ble_gap_adv_set_data(adv_data,adv_data_len);
    }
    
    //设置rsp data
    if(rsp_data_len > 0) {
        uint8_t* rsp_data_new = (uint8_t*)malloc(rsp_data_len + 2);
        rsp_data_new[0] = rsp_data_len + 1;
        rsp_data_new[1] = 0xff;
        memcpy(rsp_data_new + 2, rsp_data, rsp_data_len); 
        int rc;
        rc =  ble_gap_adv_rsp_set_data(rsp_data_new,rsp_data_len+2);
        if (rc != 0) {
            printf("error setting advertisement response data; rc=%d\r\n", rc);
            free(rsp_data_new);
            return -1 ;
        }
        free(rsp_data_new);
    }

    g_ble_addr_mode = addr;
    g_ble_connectable = connectable;
    g_interval = interval;

    return _bluetooth_BLE_adv(interval);
}

int _bluetooth_BLE_adv(int interval) //TODO:1. 换个命名 2.再次进行广播，之前的值应该清零？
{
    printf("_bluetooth_BLE_adv\r\n");
    // 声明并初始化广播结构体
    struct ble_gap_adv_params adv_params;
    memset(&adv_params, 0, sizeof(adv_params));

    uint8_t own_addr_type =  get_addr_type(g_ble_addr_mode);
    // 连接模式
    uint8_t connet_mode;
    if(g_ble_connectable == true){
        connet_mode = BLE_GAP_CONN_MODE_UND;
    }else {
        connet_mode = BLE_GAP_CONN_MODE_NON;
    }

    adv_params.conn_mode = connet_mode;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
    if(interval == 0){       // 默认值是104, 65ms
        adv_params.itvl_min = 0;
        adv_params.itvl_max = 0;
    }else if (interval < 35){  //需要大于35，
        adv_params.itvl_min = 35; 
        adv_params.itvl_max = 36; 
    }else{  
        adv_params.itvl_min = interval; 
        adv_params.itvl_max = interval + 1; //只能和adv_params.itvl_min差1
    }

    if(ble_gap_adv_active() == 1){    //是否原有广播任务
        ble_gap_adv_stop();
    }

    int rc = ble_gap_adv_start(own_addr_type, NULL, BLE_HS_FOREVER, &adv_params, ble_gap_event_cb, NULL);
    if(rc != 0)
        return rc;

    uint8_t *  one_bytes_handle;
    one_bytes_handle = (uint8_t *)malloc(g_all_chr_count);
    for (int i = 0; i < g_all_chr_count; i++)
    {
        one_bytes_handle[i] = g_chrs_handle[i];
    }
    
    pika_eventListener_send(g_pika_ble_listener,_IRQ_DIY_REGISTER_HANDLE,
        arg_newObj(New_pikaTupleFrom(
            arg_newInt(_IRQ_DIY_REGISTER_HANDLE),
            arg_newBytes(one_bytes_handle,g_all_chr_count)
            )));

    free(one_bytes_handle);
    return 0;
}

// 停止广播
int _bluetooth_BLE_stop_advertise(PikaObj *self)
{
    printf("_bluetooth_BLE_stop_advertise\r\n");
    if(ble_gap_adv_active() == 1){
        return ble_gap_adv_stop();
    }else{
        return 0;
    }
}

int _bluetooth_BLE_pyi_gap_connect(PikaObj *self, uint8_t* peer_addr, int peer_addr_type, int scan_duration_ms)
{
    // nimble_port_freertos_init(ble_host_task);
    printf("_bluetooth_BLE_gap_connect\r\n");
    int rc = ble_gap_disc_active();
    if (rc != 0) {
        printf("Failed to cancel scan; rc=%d\n", rc);
        return rc;
    }

    uint8_t own_addr_type ;
    rc = ble_hs_id_infer_auto(0, &own_addr_type);
    printf("own_addr_type = %d\r\n", own_addr_type);
    
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "error determining address type; rc=%d\n", rc);
        return rc;
    }

    ble_addr_t addr_peer = {
        .type = peer_addr_type,
    };
    
    data_inver(peer_addr,addr_peer.val,6);
    return ble_gap_connect(own_addr_type,&addr_peer,scan_duration_ms,NULL,ble_gap_event_cb,NULL);
}

int _bluetooth_BLE_pyi_gap_disconnect(PikaObj *self, int conn_handle)
{
    printf("_bluetooth_BLE_gap_disconnect\r\n");
    return ble_gap_terminate(conn_handle, BLE_ERR_REM_USER_CONN_TERM);
}

int _bluetooth_BLE_gap_disc(PikaObj *self, int addr_mode, int duration_ms, int interval, int window, pika_bool active)
{
    nimble_port_freertos_init(ble_host_task);
    printf("_bluetooth_BLE_gap_disc\r\n");
    // 获取地址类型                                             
    uint8_t own_addr_type =  get_addr_type(addr_mode);

    // 声明并初始化结构体实例
    struct ble_gap_disc_params disc_params = {
        .itvl = interval,
        .window = window,
        .passive = ~active,
        .limited = 0,
        .filter_duplicates = 0
    };

    if(ble_gap_disc_active() == 1)
        ble_gap_disc_cancel();

    if (duration_ms == 0){
        return ble_gap_disc(own_addr_type, BLE_HS_FOREVER, &disc_params, ble_gap_event_cb, NULL);
    }else{
        return ble_gap_disc(own_addr_type, duration_ms, &disc_params, ble_gap_event_cb, NULL);
    }
    
}

// 停止扫描
// 不会引发扫描中止事件
int _bluetooth_BLE_gap_stop_disc(PikaObj *self)
{
    printf("_bluetooth_BLE_gap_stop_scan\r\n");
    return ble_gap_disc_cancel();
}

// 注册服务
int _bluetooth_BLE_gatts_register_svcs(PikaObj *self, PikaObj* services_info,int all_chr_count)
{
    // nimble_port_stop();
    printf("_bluetooth_BLE_gatts_register_svcs\r\n");
    g_all_chr_count = all_chr_count;                  //同步全部特性数量
    g_chrs_handle = (uint16_t * )calloc(g_all_chr_count,sizeof(uint16_t));

    if(g_chrs_handle == NULL){
        printf("malloc g_chrs_handle error\r\n");
        return -1;
    }

    size_t service_count , chr_count, dsc_count;
    uint8_t i,j,k;
    uint8_t *chrs_count_per_service;
    uint8_t  UUID_bytes,handle_index = 0;

    struct ble_gatt_chr_def* gatt_svr_chrs;
    struct ble_gatt_dsc_def* gatt_svr_dscs;

    service_count = pikaTuple_getSize(services_info);            //服务的个数,是不确定的
    // printf("services_info service_count = %d\r\n",service_count);

    gatt_svr_svcs = (struct ble_gatt_svc_def*) malloc((service_count + 1) * sizeof(struct ble_gatt_svc_def)); //申请空间
    chrs_count_per_service    = (uint8_t*) malloc((service_count + 1) * sizeof(uint8_t));
    if(gatt_svr_svcs == NULL){
        printf("malloc svcs memory error\r\n");
        return -1;
    }


    gatt_svr_svcs[service_count].type = 0; //服务数量中止

    for (i = 0;i < service_count;i++){                                            //对于每个服务
        PikaObj* service = arg_getObj(pikaTuple_getArg(services_info, i));        //读取服务
        uint8_t* service_bytes_UUID = arg_getBytes(pikaTuple_getArg(service,0));  //获取服务的UUID
        UUID_bytes   = arg_getInt(pikaTuple_getArg(service,1));                   //获取服务的UUID大小
        
        int service_struct_size = pikaTuple_getSize(service);

        if(service_struct_size == 2){ // 无属性
            // 设置服务
            ble_uuid_any_t* service_UUID = (ble_uuid_any_t*) malloc(sizeof(ble_uuid_any_t));
            if(service_UUID == NULL){
                printf("malloc service_UUID memory error\r\n");
                return -1;
            }
            gatt_svr_svcs[i].type = BLE_GATT_SVC_TYPE_PRIMARY;
            gatt_svr_svcs[i].characteristics = NULL;
            gatt_svr_svcs[i].uuid = &(service_UUID->u);
            gatt_svr_svcs[i].includes = NULL; //很关键
            
            fill_UUID(service_UUID,UUID_bytes,service_bytes_UUID);

        }else{ //有属性
            PikaObj* chrs = arg_getObj(pikaTuple_getArg(service, 2));                 //读取属性合集
            chr_count = pikaTuple_getSize(chrs);                                      // 属性的个数,是不确定的

            gatt_svr_chrs = (struct ble_gatt_chr_def*) malloc((chr_count  + 1)* sizeof(struct ble_gatt_chr_def)); //申请空间
            if(gatt_svr_chrs == NULL){
                printf("malloc chrs memory error\r\n");
                return -1;
            }    
            gatt_svr_chrs[chr_count].uuid = 0;    //特性数量中止

            chrs_count_per_service[i] = chr_count; //存储特性数量,方便后续释放空间

            // 设置服务
            ble_uuid_any_t* service_UUID = (ble_uuid_any_t*) malloc(sizeof(ble_uuid_any_t));
            if(service_UUID == NULL){
                printf("malloc service_UUID memory error\r\n");
                return -1;
            }

            gatt_svr_svcs[i].type = BLE_GATT_SVC_TYPE_PRIMARY;
            gatt_svr_svcs[i].characteristics = gatt_svr_chrs;
            gatt_svr_svcs[i].uuid = &(service_UUID->u);
            gatt_svr_svcs[i].includes = NULL; //很关键
            
            fill_UUID(service_UUID,UUID_bytes,service_bytes_UUID);
            // printf("service %d chrs size %d \r\n",i,chr_count);
            for (j = 0;j < chr_count;j++){                                                  // 对于每个属性
                PikaObj* chr = arg_getObj(pikaTuple_getArg(chrs, j));                       //读取属性

                uint8_t * chr_bytes_UUID = arg_getBytes(pikaTuple_getArg(chr,0));           //属性UUID
                uint8_t UUID_bytes = arg_getInt(pikaTuple_getArg(chr,1));                   //属性UUID大小
                uint64_t chr_flags = pikaTuple_getInt(chr,2);                               //属性FLAG   

                int chr_struct_size = pikaTuple_getSize(chr);
                if(chr_struct_size == 3){ // 无描述符
                    // 设置属性
                    ble_uuid_any_t* chr_UUID = (ble_uuid_any_t*) malloc(sizeof(ble_uuid_any_t));
                    if(chr_UUID == NULL){
                        printf("malloc chr_UUID memory error\r\n");
                        return -1;
                    }
                    gatt_svr_chrs[j].uuid        = &(chr_UUID->u);
                    gatt_svr_chrs[j].access_cb   = ble_gatt_svc_access_cb;
                    gatt_svr_chrs[j].arg         = self;
                    gatt_svr_chrs[j].flags       = chr_flags;
                    gatt_svr_chrs[j].descriptors = NULL;
                    gatt_svr_chrs[j].val_handle  = &(g_chrs_handle[handle_index]);//蓝牙同步之后,才会有有效值
                    fill_UUID(chr_UUID,UUID_bytes,chr_bytes_UUID);
                    handle_index++;
                }else{                    // 有描述符
                    PikaObj* dscs = arg_getObj(pikaTuple_getArg(chr, 3));                       // dscs = 描述符合集
                    dsc_count = pikaTuple_getSize(dscs);                                        //描述符的个数，是不确定的
                    gatt_svr_dscs = (struct ble_gatt_dsc_def*) malloc((dsc_count + 1 )* sizeof(struct ble_gatt_dsc_def)); //申请空间
                    if(gatt_svr_dscs == NULL){
                        printf("malloc dscs memory error\r\n");
                        return -1;
                    }

                    gatt_svr_dscs[dsc_count].uuid = 0; //描述符数量中止

                    // 设置属性
                    ble_uuid_any_t* chr_UUID = (ble_uuid_any_t*) malloc(sizeof(ble_uuid_any_t));
                    if(chr_UUID == NULL){
                        printf("malloc chr_UUID memory error\r\n");
                        return -1;
                    }

                    gatt_svr_chrs[j].uuid        = &(chr_UUID->u);
                    gatt_svr_chrs[j].access_cb   = ble_gatt_svc_access_cb;
                    gatt_svr_chrs[j].arg         = self;
                    gatt_svr_chrs[j].flags       = chr_flags;
                    gatt_svr_chrs[j].descriptors = gatt_svr_dscs;
                    gatt_svr_chrs[j].val_handle  = &(g_chrs_handle[handle_index]);//蓝牙同步之后,才会有有效值
                    fill_UUID(chr_UUID,UUID_bytes,chr_bytes_UUID);
                    
                    handle_index++;
                    // printf("chr_UUID_size : %d chr_flags %d dscs size:%d\r\n",UUID_bytes,chr_flags,dsc_count);

                    for(k = 0;k < dsc_count;k++){                        //对于每个描述符
                        PikaObj * dsc = arg_getObj(pikaTuple_getArg(dscs, k));
                        uint8_t * dsc_bytes_UUID = arg_getBytes(pikaTuple_getArg(dsc,0));  
                        uint8_t UUID_bytes = arg_getInt(pikaTuple_getArg(dsc,1));
                        uint16_t dsc_flags = pikaTuple_getInt(dsc, 2);

                        // 设置描述符
                        ble_uuid_any_t* dsc_UUID = (ble_uuid_any_t*) malloc(sizeof(ble_uuid_any_t));
                        
                        if(dsc_UUID == NULL){
                            printf("malloc dsc_UUID memory error\r\n");
                            return -1;
                        }

                        gatt_svr_dscs[k].uuid        = &(dsc_UUID->u);
                        gatt_svr_dscs[k].access_cb   = ble_gatt_svc_access_cb;
                        gatt_svr_dscs[k].arg         = self;
                        gatt_svr_dscs[k].att_flags   = dsc_flags;
                        fill_UUID(dsc_UUID,UUID_bytes,dsc_bytes_UUID);
                    }
                }
            }
        }
    }

    //注册基本服务
    gatt_svr_init(); 

    // 注册服务
    int rc = ble_gatts_count_cfg(gatt_svr_svcs);
    if (rc != 0) {
        return rc;
    }

    rc = ble_gatts_add_svcs(gatt_svr_svcs);
    if (rc != 0) {
        return rc;
    }
    // 释放内存空间的时候需要遍历结构体 TODO:思考一下在何处释放该内存,至少需要在host层同步后释放
    // for (i = 0;i < service_count; i++){  
    //     gatt_svr_chrs = gatt_svr_svcs[i].characteristics;
    //     chr_count = chrs_count_per_service[i];       //FIXME:修改一下获取特性数量的方法
    //     for (j = 0;j < chr_count; j++){
    //         gatt_svr_dscs = gatt_svr_chrs[j].descriptors;
    //         free(gatt_svr_dscs);
    //     } 
    //     free(gatt_svr_chrs);
    // }
    // free(gatt_svr_svcs);
    // gatt_svr_svcs = NULL;
    nimble_port_freertos_init(ble_host_task);
    return rc;
}

// 设置广播数据
// 在设置前需要蓝牙协议栈处于同步状态, nimble_port_freertos_init(ble_host_task)
// 直接发送传入内容, 不按照规范补充格式
// 若要按规范补充则需要调用ble_gap_adv_set_fields(const struct ble_hs_adv_fields *rsp_fields)函数
int _bluetooth_BLE_set_adv_data(PikaObj *self, uint8_t* data, int data_len)
{
    printf("_bluetooth_BLE_set_adv_data\r\n");
    return ble_gap_adv_set_data(data,data_len);
}

// 设置扫描响应数据
// 在设置前需要蓝牙协议栈处于同步状态, nimble_port_freertos_init(ble_host_task)
// 直接发送传入内容, 不按照规范补充格式
// 若要按规范补充则需要调用ble_gap_adv_rsp_set_fields(const struct ble_hs_adv_fields *rsp_fields)函数
// int _bluetooth_BLE_set_rsp_data(PikaObj *self, char* data, int data_len)
int _bluetooth_BLE_set_rsp_data(PikaObj *self, uint8_t* data, int data_len)
{
    printf("_bluetooth_BLE_set_rsp_data\r\n");
    // printf("%hx %hx %hx %hx\r\n",data[0],data[1],data[2],data[3]);
    // printf("%d %d %d %d\r\n",data[0],data[1],data[2],data[3]);
    return  ble_gap_adv_rsp_set_data(data,data_len);
}


char * _bluetooth_BLE_config_mac_get(PikaObj *self)
{
    // nimble_port_freertos_init(ble_host_task);
    printf("_bluetooth_BLE_config_mac_get\r\n");
    uint8_t own_addr_type;
    uint8_t addr_val[6] = {0};
    int rc = ble_hs_id_infer_auto(0, &own_addr_type);
    if (rc != 0) {
        printf("error determining address type; rc=%d\n", rc);
        return obj_cacheStr(self,"");
    }
    rc = ble_hs_id_copy_addr(own_addr_type, addr_val, NULL);
    // print_addr(addr_val);
    char mac[18];
    sprintf(&mac, "%02x:%02x:%02x:%02x:%02x:%02x",addr_val[5], addr_val[4], addr_val[3], addr_val[2], addr_val[1], addr_val[0]);
    // nimble_port_stop();
    return obj_cacheStr(self,mac);
}


// 测试通过
char* _bluetooth_BLE_config_gap_name_get(PikaObj *self){
    printf("_bluetooth_BLE_config_addr_gap_name_get\r\n");
    char *name = ble_svc_gap_device_name();
    return obj_cacheStr(self, name);
}


int _bluetooth_BLE_config_addr_mode_get(PikaObj *self){
    printf("_bluetooth_BLE_config_addr_mode_get\r\n");
    // nimble_port_freertos_init(ble_host_task);
    uint8_t own_addr_type;
    int rc = ble_hs_id_infer_auto(0, &own_addr_type);
    // nimble_port_stop();
    return own_addr_type;
}

int _bluetooth_BLE_config_mtu_get(PikaObj *self){
    printf("_bluetooth_BLE_config_mtu_get\r\n");
    return 0;
}

int _bluetooth_BLE_config_addr_rxbuf_get(PikaObj *self){
    printf("_bluetooth_BLE_config_addr_rxbuf_get\r\n");
    return 0;
}

int _bluetooth_BLE_config_bond_get(PikaObj *self){
    printf("_bluetooth_BLE_config_bond_get\r\n");
    return 0;
}

int _bluetooth_BLE_config_io_get(PikaObj *self)
{
    printf("_bluetooth_BLE_config_io_get\r\n");
    return 0;
}

int _bluetooth_BLE_config_le_secure_get(PikaObj *self)
{
    printf("_bluetooth_BLE_config_le_secure_get\r\n");
    return 0;
}

int _bluetooth_BLE_config_mitm_get(PikaObj *self)
{
    printf("_bluetooth_BLE_config_mitm_get\r\n");
    return 0;
}


int _bluetooth_BLE_config_addr_mode_update(PikaObj *self)
{
    printf("_bluetooth_BLE_config_addr_mode_update\r\n");
    return 0;
}

int _bluetooth_BLE_config_bond_update(PikaObj *self, pika_bool bond)
{
    printf("_bluetooth_BLE_config_bond_update\r\n");
    return 0;
}

// 基本测试通过
int _bluetooth_BLE_config_gap_name_update(PikaObj *self, char* gap_name)
{
    printf("_bluetooth_BLE_config_gap_name_update\r\n");
    return ble_svc_gap_device_name_set(gap_name);
}

// TODO:未找到对应函数
int _bluetooth_BLE_config_io_update(PikaObj *self, int io){
    printf("_bluetooth_BLE_config_io_update\r\n");
    return 0;
}

int _bluetooth_BLE_config_le_secure_update(PikaObj *self, pika_bool le_secure)
{
    ESP_LOGD(tag, "_bluetooth_BLE_config_le_secure_update\r\n");
    // TODO:需要进行判断，若BLE处于广播状态则不能修改
    if(ble_gap_adv_active()){
        ESP_LOGI(tag, "an advertisement procedure is currently in progress\r\n");
        return -1;
    }
    else{
        BLE_ONLY = le_secure;
        ESP_LOGI(tag, "secure update succeed\r\n");
        return 0;
    }
}

// TODO:未找到实现方法
int _bluetooth_BLE_config_mac_update(PikaObj *self)
{
    printf("_bluetooth_BLE_config_mac_update\r\n");
    return 0;
}

// TODO:未找到对应函数
int _bluetooth_BLE_config_mitm_update(PikaObj *self, pika_bool mitm)
{
    printf("_bluetooth_BLE_config_mitm_update\r\n");
    return 0;
}

// TODO:未找到对应函数
int _bluetooth_BLE_config_mtu_update(PikaObj *self, int mtu)
{
    printf("_bluetooth_BLE_config_mtu_update\r\n");
    return 0;
}

// TODO:未找到对应函数
int _bluetooth_BLE_config_rxbuf_update(PikaObj *self, int rxbuf)
{
    printf("_bluetooth_BLE_config_rxbuf_update\r\n");
    return 0;
}


int _bluetooth_BLE_pyi_test2(PikaObj *self)
{
    uint8_t data[2];
    data[0] = 0x00;
    data[1] = 0x00;
    struct os_mbuf *om = ble_hs_mbuf_from_flat(data, 2);
    return ble_gattc_write_no_rsp(1, 38, om);
}

int _bluetooth_BLE_pyi_test3(PikaObj *self, int connhandle, int valuehandle)
{
    printf("_bluetooth_BLE_pyi_test3\r\n");
    
    // uint8_t data[2];
    // data[0] = 0x00;
    // // data[1] = 0x02;
    // struct os_mbuf *om = ble_hs_mbuf_from_flat(data, 2);
    // return  ble_gattc_write_no_rsp(1, 38, om);

    // ble_gatts_notify(1,32);
    printf("ble_hs_is_enabled(void) %d\r\n", ble_hs_is_enabled());
    return 0;
}

int _bluetooth_BLE_test_call_some_name(PikaObj *self)
{
    printf("_bluetooth_BLE_test_call_some_name\r\n");
    return 0;
}

//查找全部服务
int _bluetooth_BLE_gattc_dis_svcs(PikaObj *self, int conn_handle){
    return ble_gattc_disc_all_svcs(conn_handle, ble_gatt_disc_all_svcs_cb, NULL);
}

//通过UUID查找服务
int _bluetooth_BLE_gattc_dis_svcs_by_uuid(PikaObj *self, int conn_handle, uint8_t* uuid,int len){
    ble_uuid_any_t uuid_any = {0};
    // printf("UUID: %02x %02x\r\n",uuid[0],uuid[1]);
    if(len == 2){
        uuid_any.u16.u.type = BLE_UUID_TYPE_16;
        uuid_any.u16.value = uuid[0] << 8 | uuid[1];
        // uuid_any.u16.value = uuid[0] * 256 +  uuid[1];
        // uuid_any.u16.value = 0x1800;
        // uuid_any.u16.value = 0x26;
        // memcpy((uint8_t*)&(uuid_any.u16.value),uuid,2);
    }else if(len == 4){
        uuid_any.u32.u.type = BLE_UUID_TYPE_32;
        uuid_any.u32.value = uuid[0] << 24 | uuid[1] << 16 | uuid[2] << 8 | uuid[3];
    }else{
        // uint8_t data[16];
        uuid_any.u128.u.type = BLE_UUID_TYPE_128;
        data_inver(uuid,uuid_any.u128.value,16);
        
    }
    return ble_gattc_disc_svc_by_uuid(conn_handle,&uuid_any.u,ble_gatt_disc_svcs_by_uuid_cb, NULL);
}

//通过UUID查找全部属性
int _bluetooth_BLE_gattc_dis_chrs(PikaObj *self, int conn_handle, int start_handle, int end_handle){
    return ble_gattc_disc_all_chrs(conn_handle,start_handle,end_handle,ble_gatt_disc_all_chrs_cb,NULL);;
}

//通过UUID查找属性
int _bluetooth_BLE_gattc_dis_chrs_by_uuid(PikaObj *self, int conn_handle, int start_handle, int end_handle, uint8_t* uuid,int len){
    ble_uuid_any_t uuid_any = {0};
    if(len == 2){
        uuid_any.u16.u.type = BLE_UUID_TYPE_16;
        uuid_any.u16.value = uuid[0] << 8 | uuid[1];
    }else if(len == 4){
        uuid_any.u32.u.type = BLE_UUID_TYPE_32;
        uuid_any.u32.value = uuid[0] << 24 | uuid[1] << 16 | uuid[2] << 8 | uuid[3];
    }else{
        uuid_any.u128.u.type = BLE_UUID_TYPE_128;
        data_inver(uuid,uuid_any.u128.value,16);
    }
    return ble_gattc_disc_chrs_by_uuid(conn_handle,start_handle,end_handle,&uuid_any.u,ble_gatt_disc_chrs_by_uuid_cb,NULL);
}

//查找全部描述符
int _bluetooth_BLE_gattc_dis_dscs(PikaObj *self, int conn_handle, int start_handle, int end_handle){
    return ble_gattc_disc_all_dscs(conn_handle,start_handle,end_handle,ble_gatt_disc_all_dscs_cb,NULL);
}

// GATT读属性、描述符
int _bluetooth_BLE_pyi_gattc_read(PikaObj *self, int conn_handle, int value_handle){
    return ble_gattc_read(conn_handle, value_handle,ble_gatt_client_read_cb, NULL);
}

int client_subscribe_vertify_cb(uint16_t conn_handle,
                             const struct ble_gatt_error *error,
                             struct ble_gatt_attr *attr,
                             void *arg){
    uint8_t flags = attr->om->om_data[0];
    uint8_t data_len = attr->om->om_len;
    printf("data =  %02x data_len = %d error = %02x\r\n",flags, data_len,error->status);

    pika_eventListener_send(g_pika_ble_listener,_IRQ_GATTS_SUBSCRIBE,
    arg_newObj(New_pikaTupleFrom(
        arg_newInt(_IRQ_GATTS_SUBSCRIBE),
        arg_newInt(conn_handle),
        arg_newInt(attr->handle - 1),
        arg_newInt(flags),
        arg_newInt(error->status)
        )));
    return 0;
}


int client_subscribe_cb(uint16_t conn_handle,
                             const struct ble_gatt_error *error,
                             struct ble_gatt_attr *attr,
                             void *arg){
    
    // printf("subscribe callback conn_handle %d value handle error %d\r\n", conn_handle,error->att_handle,error->status);
    // printf("ble_gatt_attr_fn handle  %d  offset %d \r\n", attr->handle,attr->offset);
    return ble_gattc_read(conn_handle,attr->handle,client_subscribe_vertify_cb,NULL);
}

int _bluetooth_BLE_pyi_gattc_subscribe(PikaObj *self, int conn_handle, int value_handle, int subscribe){

    uint8_t data[2];
    data[0] = subscribe;
    data[1] = 0x00;
    struct os_mbuf * txom = ble_hs_mbuf_from_flat(data,2);
    return ble_gattc_write(conn_handle, value_handle + 1 ,txom, client_subscribe_cb, NULL);
}

// int _bluetooth_BLE_pyi_gattc_subscribe_indicate(PikaObj *self, int conn_handle, int value_handle, pika_bool subscribe){

//     uint8_t data[2];
//     if(subscribe){
//         data[0] = 0x02;
//     }else{
//         data[0] = 0x00;
//     }
//     data[1] = 0x00;
//     struct os_mbuf * txom = ble_hs_mbuf_from_flat(data,2);
//     return ble_gattc_write(conn_handle, value_handle + 1 ,txom, client_subscribe_cb, NULL);
// }


// int _bluetooth_BLE_pyi_gattc_subscribe_notify(PikaObj *self, int conn_handle, int value_handle, pika_bool subscribe){
//     uint8_t data[2];
//     if(subscribe){
//         data[0] = 0x01;
//     }else{
//         data[0] = 0x00;
//     }
//     data[1] = 0x00;
//     struct os_mbuf * txom = ble_hs_mbuf_from_flat(data,2);
//     return ble_gattc_write(conn_handle, value_handle + 1,txom,client_subscribe_cb, NULL);
// }


// GATT写属性、描述符
//TODO:还有一个相同功能的函数但不知道区别
// ble_gattc_write_no_rsp(uint16_t conn_handle, uint16_t attr_handle, struct os_mbuf *om)
int _bluetooth_BLE_gattc_write_with_no_rsp(PikaObj *self, int conn_handle, int value_handle, char* data, int data_len){
    return ble_gattc_write_no_rsp_flat(conn_handle,value_handle,data,data_len);
}

// GATT写属性、描述符
//TODO:还有一个相同功能的函数但不知道区别
// ble_gattc_write(uint16_t conn_handle, uint16_t attr_handle,struct os_mbuf *txom, ble_gatt_attr_fn *cb, void *cb_arg)
int _bluetooth_BLE_gattc_write_with_rsp(PikaObj *self, int conn_handle, int value_handle, char* data, int data_len){
    return ble_gattc_write_flat(conn_handle,value_handle,data,data_len,ble_gatt_client_write_cb,NULL);
}

int _bluetooth_BLE_gatts_chr_updated(PikaObj *self, int chr_val_handle)
{
    ble_gatts_chr_updated(chr_val_handle);
    return 0;
}


// GATT indicate
int _bluetooth_BLE_pyi_gatts_indicate(PikaObj *self, int conn_handle, int value_handle, uint8_t* data, int data_size)
{
    printf("_bluetooth_BLE_pyi_gatts_indicate\r\n");
    struct os_mbuf *om = ble_hs_mbuf_from_flat(data, data_size);
    if (om == NULL)
    {
        printf("ble_hs_mbuf_from_flat error\r\n");
    }
    printf("conn_handle %d value_handle %d  data_size %d data %02x\r\n", conn_handle, value_handle, data_size,data[0]);
    return ble_gatts_indicate_custom(conn_handle,value_handle,om);
}

int _bluetooth_BLE_pyi_gatts_notify(PikaObj *self, int conn_handle, int value_handle, uint8_t* data, int data_size)
{
    printf("_bluetooth_BLE_pyi_gatts_notify\r\n");
    struct os_mbuf *om = ble_hs_mbuf_from_flat(data, data_size);
    if (om == NULL)
    {
        printf("ble_hs_mbuf_from_flat error\r\n");
    }
    printf("conn_handle %d value_handle %d  data_size %d data %02x\r\n", conn_handle, value_handle, data_size,data[0]);

    return ble_gatts_notify_custom(conn_handle,value_handle,om);
}

int _bluetooth_BLE_pyi_gatts_show_local(PikaObj *self)
{
    printf("_bluetooth_BLE_pyi_gatts_show_local");
    ble_gatts_show_local();
    return 0;
}


// 未找到正确的使用方法与效果
// TODO:待验证
int _bluetooth_BLE_pyi_gattc_exchange_mtu(PikaObj *self, int conn_handle){
    return ble_gattc_exchange_mtu(conn_handle,ble_gatt_mtu_exchange_cb,NULL);
}

// 回调函数注册
void _bluetooth_BLE_setCallback(PikaObj *self, Arg* cb)
{
    printf("_bluetooth_BLE_setCallback\r\n");
    if (g_pika_ble_listener == NULL) {
        pika_eventListener_init(&g_pika_ble_listener);
        printf("g_pika_ble_listener init\r\n");
    }

    uint32_t i = 0;
    for ( i = 0; i < _IRQ_COUNT + 1; i++){
        pika_eventListener_registEventCallback(g_pika_ble_listener,i,cb);
    }
    for ( i = 101; i < _IRQ_DIY_MAX_ID + 1; i++){
        pika_eventListener_registEventCallback(g_pika_ble_listener,i,cb);
    }
}

// 基本服务注册
void gatt_svr_init(void)
{
    ble_svc_gap_init();
    ble_svc_gatt_init();
    ble_svc_ans_init();
}

// 从字符串中读取UUID 结构体
int read_uuid_from_str(char* buf, int len, ble_uuid_any_t* uuid_struct)
{
    unsigned int a[16];
    if (len == 4){
        sscanf(buf, "%02x%02x", &a[0],&a[1]);
        uuid_struct->u16.u.type = BLE_UUID_TYPE_16;
        uuid_struct->u16.value = (a[0] << 8) | a[1];
        return 16;
    }
    else if (len == 8){
        sscanf(buf, "%02x%02x%02x%02x", &a[0],&a[1],&a[2],&a[3]);
        uuid_struct->u32.u.type = BLE_UUID_TYPE_32;
        uuid_struct->u32.value  = (a[0] << 24) | (a[1] << 16) | (a[2] << 8) | a[3]; 
        return 32;
    }
    else if (len == 36){
        sscanf(buf, "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",\
         &a[15],&a[14],&a[13],&a[12],&a[11],&a[10],&a[9],&a[8],&a[7],&a[6],&a[5],&a[4],&a[3],&a[2],&a[1],&a[0]);
        // // &a[0],&a[1],&a[2],&a[3],&a[4],&a[5],&a[6],&a[7],&a[8],&a[9],&a[10],&a[11],&a[12],&a[13],&a[14],&a[15]);
        uuid_struct->u128.u.type = BLE_UUID_TYPE_128;
        memcpy(uuid_struct->u128.value,a,16);
        return 128;
    }
    return 0;
}


// GATT层: 服务端 回调函数
static int ble_gatt_svc_access_cb(uint16_t conn_handle, uint16_t attr_handle,
                struct ble_gatt_access_ctxt *ctxt, void *arg){
    const ble_uuid_t *uuid;
    int rc;
    printf("ble_gatt_svc_access_cb\r\n");
    
    switch (ctxt->op) {                       
        case BLE_GATT_ACCESS_OP_READ_CHR:       //读属性值
            if (conn_handle != BLE_HS_CONN_HANDLE_NONE) {
                printf("Characteristic read: conn_handle=%d attr_handle=%d\r\n",conn_handle, attr_handle);
                PikaObj * self = arg;
                obj_setArg(self, "data", arg_newObj(New_pikaTupleFrom(
                                                arg_newInt(_IRQ_GATTS_READ_REQUEST),
                                                arg_newInt(conn_handle),
                                                arg_newInt(attr_handle)
                                            )));

                obj_run(self, "res = _callback(data)");
                Arg* res = obj_getArg(self, "res");
                PikaObj* tuple = arg_getObj(res);
                int res_int    = arg_getInt(pikaTuple_getArg(tuple,0)); 
                int byte_count = arg_getInt(pikaTuple_getArg(tuple,1)); 
                uint8_t * value = (uint8_t * )calloc(byte_count,sizeof(uint8_t));
                value = arg_getBytes(pikaTuple_getArg(tuple,2));

                printf("res_int %d byte_count %d data %02x\r\n",res_int,byte_count,value[0]);
                // int res_int = arg_getInt(res); 
                if(res_int == _GATTS_NO_ERROR){ 
                    rc = os_mbuf_append(ctxt->om, value,byte_count);
                    return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
                }
                return 0;
            } else {
                printf("Characteristic read by NimBLE stack; attr_handle=%d\n",attr_handle);
                PikaObj * self = arg;
                obj_setArg(self, "data", arg_newObj(New_pikaTupleFrom(
                                                arg_newInt(_IRQ_DIY_NIMBLE_READ),
                                                arg_newInt(attr_handle)
                                            )));
                obj_run(self, "res = _callback(data)");
                Arg* res = obj_getArg(self, "res");
                PikaObj* tuple = arg_getObj(res);
                int byte_count = arg_getInt(pikaTuple_getArg(tuple,0)); 
                uint8_t * value = (uint8_t * )calloc(byte_count,sizeof(uint8_t));
                value = arg_getBytes(pikaTuple_getArg(tuple,1));

                printf("byte_count %d data %02x\r\n",byte_count,value[0]);
                rc = os_mbuf_append(ctxt->om,value,byte_count);
                return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
            }
            return 0;
        case BLE_GATT_ACCESS_OP_WRITE_CHR:              //写属性值
            if (conn_handle != BLE_HS_CONN_HANDLE_NONE) {
                printf("Characteristic write; conn_handle=%d attr_handle=%d\r\n", conn_handle, attr_handle);
                uint16_t total_length       =  *(ctxt->om->om_databuf);
                uint8_t * value = (uint8_t*)calloc(total_length,sizeof(uint8_t));
                
                // uint16_t current_buf_length =  ctxt->om->om_len;
                uint16_t current_buf_length;
                uint16_t remain_length      = total_length;
                struct os_mbuf* buf_pointer = ctxt->om;
                while (true){
                    current_buf_length = buf_pointer->om_len;
                    memcpy((value + (total_length - remain_length)), buf_pointer->om_data,current_buf_length); 
                    remain_length -= current_buf_length;

                    if(remain_length > 0)
                        buf_pointer = buf_pointer->om_next.sle_next;
                    else
                        break;
                }

                // printf("ctxt->om->om_pkthdr_len = %d\r\n",ctxt->om->om_pkthdr_len);
                // printf("ctxt->om->om_len = %d\r\n",ctxt->om->om_next.sle_next->om_len);
                // printf("ctxt->om->om_databuf[0] = %02x\r\n",*(ctxt->om->om_databuf));
                // printf("ctxt->om->om_databuf[1] = %02x\r\n",*(ctxt->om->om_databuf+1));
                // printf("ctxt->om->om_databuf[2] = %02x\r\n",*(ctxt->om->om_databuf+2));
                pika_eventListener_send(g_pika_ble_listener,_IRQ_GATTS_WRITE,
                    arg_newObj(New_pikaTupleFrom(
                            arg_newInt(_IRQ_GATTS_WRITE),
                            arg_newInt(conn_handle), 
                            arg_newInt(attr_handle),
                            arg_newBytes(value,total_length) 
                            )));
            } else {
                printf("Characteristic write by NimBLE stack; attr_handle=%d\r\n",attr_handle);
            }
            return 0;

        case BLE_GATT_ACCESS_OP_READ_DSC:     //读描述符(回调函数与属性值先不做区分) TODO:读不到描述符吧？
            if (conn_handle != BLE_HS_CONN_HANDLE_NONE) {
                printf("Descriptor read; conn_handle=%d attr_handle=%d\r\n",conn_handle, attr_handle);

                Arg* res = pika_eventListener_sendAwaitResult(g_pika_ble_listener,_IRQ_GATTS_READ_REQUEST,
                    arg_newObj(New_pikaTupleFrom(
                            arg_newInt(_IRQ_GATTS_READ_REQUEST),
                            arg_newInt(conn_handle),
                            arg_newInt(attr_handle)
                            )));
                
                int res_int = arg_getInt(res);  
                if(res_int == _GATTS_NO_ERROR){ //允许读,如何返回被拒绝访问解决结果?
                    gatt_svr_dsc_val = 0xFF;
                    rc = os_mbuf_append(ctxt->om,
                                &gatt_svr_dsc_val,
                                sizeof(gatt_svr_chr_val));
                    return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
                }

            } else {
                printf("Descriptor read by NimBLE stack; attr_handle=%d\r\n",attr_handle);
            }
            return 0;

        case BLE_GATT_ACCESS_OP_WRITE_DSC:      //写描述符
            if (conn_handle != BLE_HS_CONN_HANDLE_NONE) {
                printf("Descriptor write; conn_handle=%d attr_handle=%d\r\n",conn_handle, attr_handle);
                pika_eventListener_send(g_pika_ble_listener,_IRQ_GATTS_WRITE,
                    arg_newObj(New_pikaTupleFrom(
                            arg_newInt(_IRQ_GATTS_WRITE),
                            arg_newInt(conn_handle),
                            arg_newInt(attr_handle)
                            )));
            } else {
                printf("Descriptor read by NimBLE stack; attr_handle=%d\r\n",attr_handle);
            }
            return 0;
    }
    return 0;
}


// GATT层：客户端读服务回调函数
static int ble_gatt_client_read_cb(uint16_t conn_handle,
                       const struct ble_gatt_error *error,
                       struct ble_gatt_attr *attr,
                       void *arg)
{
    printf("Read complete for the subscribable characteristic; "
            "status=%d conn_handle=%d\r\n", error->status, conn_handle);
    //读取成功
    // TODO:读完之后不会引起其他事件，所以要done 和result一起触发
    if (error->status == 0) {
        //读到数据
        // printf("om_data:%d\r\n",*(attr->om->om_data));
        // printf("om_flags:%d\r\n",attr->om->om_flags);
        // printf("om_pkthdr_len:%d\r\n",attr->om->om_pkthdr_len);
        // printf("om_data:%d\r\n",attr->om->om_len);
        // print_mbuf(attr->om); //TODO:该函数无引用，但在blecentn能够使用
        pika_eventListener_send(g_pika_ble_listener,_IRQ_GATTC_READ_RESULT,
            arg_newObj(New_pikaTupleFrom(
                    arg_newInt(_IRQ_GATTC_READ_RESULT),
                    arg_newInt(conn_handle),
                    arg_newInt(attr->handle), //应该是value handle
                    arg_newBytes(attr->om->om_data,attr->om->om_len) 
                    )));

        pika_eventListener_send(g_pika_ble_listener,_IRQ_GATTC_READ_DONE ,
        arg_newObj(New_pikaTupleFrom(
                arg_newInt(_IRQ_GATTC_READ_DONE),
                arg_newInt(conn_handle),
                arg_newInt(attr->handle),
                arg_newInt(error->status) 
                )));
    }else{
        pika_eventListener_send(g_pika_ble_listener,_IRQ_GATTC_READ_DONE ,
        arg_newObj(New_pikaTupleFrom(
                arg_newInt(_IRQ_GATTC_READ_DONE),
                arg_newInt(conn_handle),
                arg_newInt(-1),
                arg_newInt(error->status) 
                )));
    }
    return 0;
}

// GATT层：客户端写服务回调函数
static int ble_gatt_client_write_cb(uint16_t conn_handle,
                        const struct ble_gatt_error *error,
                        struct ble_gatt_attr *attr,
                        void *arg)
{
    const struct peer_chr *chr;
    const struct peer *peer;
    int rc;

    MODLOG_DFLT(INFO,
                "Write to the custom subscribable characteristic complete; "
                "status=%d conn_handle=%d attr_handle=%d\n",
                error->status, conn_handle, attr->handle);
    if (error->status == 0) {       
        //读到数据
        pika_eventListener_send(g_pika_ble_listener,_IRQ_GATTC_WRITE_DONE,
            arg_newObj(New_pikaTupleFrom(
                    arg_newInt(_IRQ_GATTC_WRITE_DONE ),
                    arg_newInt(conn_handle),
                    arg_newInt(attr->handle),
                    arg_newInt(error->status) 
                    )));
    }else{
        pika_eventListener_send(g_pika_ble_listener,_IRQ_GATTC_WRITE_DONE,
            arg_newObj(New_pikaTupleFrom(
                arg_newInt(_IRQ_GATTC_WRITE_DONE),
                arg_newInt(conn_handle),
                arg_newInt(-1),
                arg_newInt(error->status) 
                )));
    }
    return 0;
}

//GATT层 MTU exchange 回调函数
static int ble_gatt_mtu_exchange_cb(uint16_t conn_handle,
                            const struct ble_gatt_error *error,
                            uint16_t mtu, void *arg)
{
    if(error->status == 0){
        pika_eventListener_send(g_pika_ble_listener,_IRQ_MTU_EXCHANGED ,
        arg_newObj(New_pikaTupleFrom(
                arg_newInt(_IRQ_MTU_EXCHANGED),
                arg_newInt(conn_handle),
                arg_newInt(mtu)
                )));
    }
    return 0;
}


// GAP层：广播事件回调函数
static int ble_gap_event_cb(struct ble_gap_event *event, void *arg)
{
    struct ble_gap_conn_desc desc;
    int rc;
    uint8_t addr[6];

    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT: 
        printf("connection %s; status=%d ",
            event->connect.status == 0 ? "established" : "failed", event->connect.status);
        printf("\r\n");
        if (event->connect.status == 0) {
            rc = ble_gap_conn_find(event->connect.conn_handle, &desc);
            assert(rc == 0);
            print_conn_desc(&desc);
            data_inver(desc.peer_ota_addr.val,&addr,6);

            if(desc.role == BLE_GAP_ROLE_SLAVE){
                pika_eventListener_send(g_pika_ble_listener,_IRQ_CENTRAL_CONNECT,
                            arg_newObj(New_pikaTupleFrom(
                                    arg_newInt(_IRQ_CENTRAL_CONNECT),
                                    arg_newInt(event->connect.conn_handle),
                                    arg_newInt(desc.peer_id_addr.type),
                                    arg_newBytes(addr,6)
                                    )));
            } else if (desc.role == BLE_GAP_ROLE_MASTER){
                pika_eventListener_send(g_pika_ble_listener,_IRQ_PERIPHERAL_CONNECT  ,
                     arg_newObj(New_pikaTupleFrom(
                    arg_newInt(_IRQ_PERIPHERAL_CONNECT),
                    arg_newInt(event->connect.conn_handle),
                    arg_newInt(desc.peer_id_addr.type),
                    arg_newBytes(addr,6)
                    )));
            }
            
        }else if (event->connect.status != 0) {
            /* Connection failed; resume advertising. */
            // bleprph_advertise();
            // TODO: 重新广播，或通知客户端
            if(desc.role == BLE_GAP_ROLE_SLAVE){
                _bluetooth_BLE_adv(g_interval);
                printf("Connection failed; resume advertising.");\
            }
        }
        return 0;

    case BLE_GAP_EVENT_DISCONNECT: //断开连接
        printf("disconnect; reason=%d \r\n", event->disconnect.reason);
        // print_conn_desc(&event->disconnect.conn);

        data_inver(event->disconnect.conn.peer_ota_addr.val,&addr,6);
        if(event->disconnect.conn.role == BLE_GAP_ROLE_SLAVE){
            pika_eventListener_send(g_pika_ble_listener,_IRQ_CENTRAL_DISCONNECT,
                    arg_newObj(New_pikaTupleFrom(
                            arg_newInt(_IRQ_CENTRAL_DISCONNECT),
                            arg_newInt(event->disconnect.conn.conn_handle),
                            arg_newInt(desc.peer_id_addr.type),
                            arg_newBytes(addr,6)
                            )));
        }else if(event->disconnect.conn.role == BLE_GAP_ROLE_MASTER){
            pika_eventListener_send(g_pika_ble_listener,_IRQ_PERIPHERAL_DISCONNECT ,
                                arg_newObj(New_pikaTupleFrom(
                                        arg_newInt(_IRQ_PERIPHERAL_DISCONNECT),
                                        arg_newInt(event->disconnect.conn.conn_handle),
                                        arg_newInt(desc.peer_id_addr.type),
                                        arg_newBytes(addr,6)
                                        )));
        }
        return 0;

    case BLE_GAP_EVENT_CONN_UPDATE: //返回结果
        /* The central has updated the connection parameters. */
        printf("connection updated; status=%d \r\n",event->conn_update.status);
        rc = ble_gap_conn_find(event->conn_update.conn_handle, &desc);
        assert(rc == 0);
        print_conn_desc(&desc);
        return 0;

    case BLE_GAP_EVENT_CONN_UPDATE_REQ :
        // MicroPython : conn_handle, conn_interval, conn_latency, supervision_timeout, status 
        printf("BLE_GAP_EVENT_CONN_UPDATE_REQ\r\n");
        pika_eventListener_send(g_pika_ble_listener,_IRQ_CONNECTION_UPDATE,
                    arg_newObj(New_pikaTupleFrom(
                            arg_newInt(_IRQ_CONNECTION_UPDATE),
                            arg_newInt(event->conn_update_req.conn_handle),
                            arg_newInt(event->conn_update_req.peer_params->itvl_min),
                            arg_newInt(event->conn_update_req.peer_params->latency),
                            arg_newInt(event->conn_update_req.peer_params->supervision_timeout)//,
                            // arg_newInt(event->conn_update.status) TODO:status在上一事件中
                            )));
         return 0;

    case BLE_GAP_EVENT_L2CAP_UPDATE_REQ :
        printf("BLE_GAP_EVENT_L2CAP_UPDATE_REQ\r\n");
        return 0;

    case BLE_GAP_EVENT_TERM_FAILURE:
        printf("BLE_GAP_EVENT_TERM_FAILURE; conn_handle=%d reason=%d\r\n",event->term_failure.conn_handle,event->term_failure.status);
        return 0;
    
    case BLE_GAP_EVENT_DISC: //扫描发现
        struct ble_gap_conn_desc desc;
        struct ble_hs_adv_fields fields;
        int rc;
        rc = ble_hs_adv_parse_fields(&fields, event->disc.data,event->disc.length_data);
        if (rc != 0) {
            return 0;
        }
        data_inver(event->disc.addr.val,addr,6);
        pika_eventListener_send(g_pika_ble_listener,_IRQ_SCAN_RESULT,
            arg_newObj(New_pikaTupleFrom(
                    arg_newInt(_IRQ_SCAN_RESULT),
                    arg_newInt(event->disc.addr.type),
                    arg_newBytes(addr,6),
                    arg_newInt(event->disc.event_type),
                    arg_newInt(event->disc.rssi),
                    arg_newBytes(event->disc.data,event->disc.length_data)
                    )));
        return 0;

    case BLE_GAP_EVENT_DISC_COMPLETE: // 扫描结束
    // MicroPython None
        printf("discovery complete; reason=%d\r\n",event->disc_complete.reason);
        pika_eventListener_send(g_pika_ble_listener,_IRQ_SCAN_DONE,
            arg_newObj(New_pikaTupleFrom(
                    arg_newInt(_IRQ_SCAN_DONE),
                    arg_newInt(event->disc_complete.reason)
                    )));
        return 0;

    case BLE_GAP_EVENT_ADV_COMPLETE: //广播完成
    // MicroPython 没有这个事件
        printf("advertise complete; reason=%d\r\n",event->adv_complete.reason);
        return 0;

    case BLE_GAP_EVENT_ENC_CHANGE:
    // 暂时不理
        /* Encryption has been enabled or disabled for this connection. */
        printf("encryption change event; status=%d \r\n",event->enc_change.status);
        rc = ble_gap_conn_find(event->enc_change.conn_handle, &desc);
        assert(rc == 0);
        print_conn_desc(&desc);
        MODLOG_DFLT(INFO, "\n");
        return 0;

    case BLE_GAP_EVENT_PASSKEY_ACTION :
    // 暂时不理
        printf("PASSKEY_ACTION_EVENT started \r\n");
        struct ble_sm_io pkey = {0};
        // int key = 0;

        if (event->passkey.params.action == BLE_SM_IOACT_DISP) {
            pkey.action = event->passkey.params.action;
            pkey.passkey = 123456; // This is the passkey to be entered on peer
            ESP_LOGI(tag, "Enter passkey %" PRIu32 "on the peer side", pkey.passkey);
            rc = ble_sm_inject_io(event->passkey.conn_handle, &pkey);
            ESP_LOGI(tag, "ble_sm_inject_io result: %d\n", rc);
        } else if (event->passkey.params.action == BLE_SM_IOACT_NUMCMP) {
            ESP_LOGI(tag, "Passkey on device's display: %" PRIu32 , event->passkey.params.numcmp);
            ESP_LOGI(tag, "Accept or reject the passkey through console in this format -> key Y or key N");
            pkey.action = event->passkey.params.action;
            // if (scli_receive_key(&key)) {
            //     pkey.numcmp_accept = key;
            // } else {
            //     pkey.numcmp_accept = 0;
            //     ESP_LOGE(tag, "Timeout! Rejecting the key");
            // }
            rc = ble_sm_inject_io(event->passkey.conn_handle, &pkey);
            ESP_LOGI(tag, "ble_sm_inject_io result: %d\n", rc);
        } else if (event->passkey.params.action == BLE_SM_IOACT_OOB) {
            static uint8_t tem_oob[16] = {0};
            pkey.action = event->passkey.params.action;
            for (int i = 0; i < 16; i++) {
                pkey.oob[i] = tem_oob[i];
            }
            rc = ble_sm_inject_io(event->passkey.conn_handle, &pkey);
            ESP_LOGI(tag, "ble_sm_inject_io result: %d\n", rc);
        } else if (event->passkey.params.action == BLE_SM_IOACT_INPUT) {
            ESP_LOGI(tag, "Enter the passkey through console in this format-> key 123456");
            pkey.action = event->passkey.params.action;
            // if (scli_receive_key(&key)) {
            //     pkey.passkey = key;
            // } else {
            //     pkey.passkey = 0;
            //     ESP_LOGE(tag, "Timeout! Passing 0 as the key");
            // }
            rc = ble_sm_inject_io(event->passkey.conn_handle, &pkey);
            ESP_LOGI(tag, "ble_sm_inject_io result: %d\n", rc);
        }
        return 0;

    case BLE_GAP_EVENT_NOTIFY_RX: // 客户端
        printf("received %s; conn_handle=%d attr_handle=%d attr_len=%d",
                event->notify_rx.indication ? "indication" : "notification",
                event->notify_rx.conn_handle,
                event->notify_rx.attr_handle,
                OS_MBUF_PKTLEN(event->notify_rx.om));
        printf("\r\n");
        if(event->notify_rx.indication == 1){ // indication
            // MicroPython : conn_handle, value_handle, notify_data
            uint16_t len = event->notify_rx.om->om_len;
            char *indic_str = (char *)malloc(len + 1);
            memcpy(indic_str, event->notify_rx.om->om_data, len);
            indic_str[len] = '\0';

            pika_eventListener_send(g_pika_ble_listener,_IRQ_GATTC_INDICATE,
                arg_newObj(New_pikaTupleFrom(
                        arg_newInt(_IRQ_GATTC_INDICATE),
                        arg_newInt(event->notify_rx.conn_handle),
                        arg_newInt(event->notify_rx.attr_handle),
                        arg_newStr(indic_str)
                        )));
            free(indic_str);
        }
        else {  // notify
            uint16_t len = event->notify_rx.om->om_len;
            char *indic_str = (char *)malloc(len + 1);
            memcpy(indic_str, event->notify_rx.om->om_data, len);
            indic_str[len] = '\0';

            pika_eventListener_send(g_pika_ble_listener,_IRQ_GATTC_NOTIFY,
                arg_newObj(New_pikaTupleFrom(
                        arg_newInt(_IRQ_GATTC_NOTIFY),
                        arg_newInt(event->notify_rx.conn_handle),
                        arg_newInt(event->notify_rx.attr_handle),
                        arg_newStr(indic_str)
                        )));
            free(indic_str);
        }
        
        return 0;

    case BLE_GAP_EVENT_NOTIFY_TX: //通知发送完成
        printf("notify_tx event conn_handle=%d attr_handle=%d status=%d is_indication=%d\r\n",
                    event->notify_tx.conn_handle,
                    event->notify_tx.attr_handle,
                    event->notify_tx.status,
                    event->notify_tx.indication);
        if(event->notify_tx.indication == 1){ // indication
            // MicroPython : conn_handle, value_handle, notify_data
            pika_eventListener_send(g_pika_ble_listener,_IRQ_GATTS_INDICATE_DONE,
                arg_newObj(New_pikaTupleFrom(
                        arg_newInt(_IRQ_GATTS_INDICATE_DONE),
                        arg_newInt(event->notify_tx.conn_handle),
                        arg_newInt(event->notify_tx.attr_handle),
                        arg_newInt(event->notify_tx.status)
                        )));
        }        
        return 0;
    case BLE_GAP_EVENT_SUBSCRIBE://订阅 客户端向服务端订阅的事件(新增)
        printf("subscribe event; conn_handle=%d attr_handle=%d "
                    "reason=%d prevn=%d curn=%d previ=%d curi=%d\r\n",
                    event->subscribe.conn_handle,
                    event->subscribe.attr_handle,
                    event->subscribe.reason,
                    event->subscribe.prev_notify,
                    event->subscribe.cur_notify,
                    event->subscribe.prev_indicate,
                    event->subscribe.cur_indicate);
        pika_eventListener_send(g_pika_ble_listener,_IRQ_GATTC_SUBSCRIBE,
        arg_newObj(New_pikaTupleFrom(
                arg_newInt(_IRQ_GATTC_SUBSCRIBE),
                arg_newInt(event->subscribe.conn_handle),
                arg_newInt(event->subscribe.attr_handle),
                arg_newInt(event->subscribe.reason),
                arg_newInt(event->subscribe.cur_notify),
                arg_newInt(event->subscribe.cur_indicate)
                )));
        return 0;

    case BLE_GAP_EVENT_MTU:
        printf("mtu update event; conn_handle=%d cid=%d mtu=%d\r\n",
                    event->mtu.conn_handle,
                    event->mtu.channel_id,
                    event->mtu.value);
        return 0;

    case BLE_GAP_EVENT_IDENTITY_RESOLVED:
            printf("BLE_GAP_EVENT_IDENTITY_RESOLVED\r\n");
        return 0;

    case BLE_GAP_EVENT_REPEAT_PAIRING:
        /* We already have a bond with the peer, but it is attempting to
         * establish a new secure link.  This app sacrifices security for
         * convenience: just throw away the old bond and accept the new link.
         */
        printf("BLE_GAP_EVENT_REPEAT_PAIRING\r\n");


        /* Delete the old bond. */
        rc = ble_gap_conn_find(event->repeat_pairing.conn_handle, &desc);
        assert(rc == 0);
        ble_store_util_delete_peer(&desc.peer_id_addr);

        /* Return BLE_GAP_REPEAT_PAIRING_RETRY to indicate that the host should
         * continue with the pairing operation.
         */
        return BLE_GAP_REPEAT_PAIRING_RETRY;

        case BLE_GAP_EVENT_PHY_UPDATE_COMPLETE:
            printf("BLE_GAP_EVENT_PHY_UPDATE_COMPLETE\r\n");
            return 0;

        case BLE_GAP_EVENT_EXT_DISC:
            printf("BLE_GAP_EVENT_EXT_DISC\r\n");
            return 0;

        case BLE_GAP_EVENT_PERIODIC_SYNC:
            printf("BLE_GAP_EVENT_PERIODIC_SYNC\r\n");
            return 0;

        case BLE_GAP_EVENT_PERIODIC_REPORT:
            printf("BLE_GAP_EVENT_PERIODIC_REPORT\r\n");
            return 0;

        case BLE_GAP_EVENT_PERIODIC_SYNC_LOST:
            printf("BLE_GAP_EVENT_PERIODIC_SYNC_LOST\r\n");
            return 0;

        case BLE_GAP_EVENT_SCAN_REQ_RCVD:
            printf("BLE_GAP_EVENT_SCAN_REQ_RCVD\r\n");
            return 0;

        case BLE_GAP_EVENT_PERIODIC_TRANSFER:
            printf("BLE_GAP_EVENT_PERIODIC_TRANSFER\r\n");
            return 0;

        case BLE_GAP_EVENT_PATHLOSS_THRESHOLD:
            printf("BLE_GAP_EVENT_PATHLOSS_THRESHOLD\r\n");
            return 0;

        case BLE_GAP_EVENT_TRANSMIT_POWER:
            printf("BLE_GAP_EVENT_TRANSMIT_POWER\r\n");
            return 0;

        case BLE_GAP_EVENT_SUBRATE_CHANGE:// 这个是啥事件？
            printf("BLE_GAP_EVENT_SUBRATE_CHANGE\r\n");
            return 0;
    }
    return 0;
}

// GATT 查找所有服务回调函数
static int ble_gatt_disc_all_svcs_cb(uint16_t conn_handle,
                                 const struct ble_gatt_error *error,
                                 const struct ble_gatt_svc *service,
                                 void *arg){
    printf("ble_gatt_disc_all_svcs_cb\r\n");
    if(error->status == 0)
    {
        uint8_t data_len = service->uuid.u.type;
        uint8_t *uuid;
        if(data_len == 16){ // = 16 
            uint16_t value = service->uuid.u16.value;
            uuid    = (uint8_t *)malloc(2);
            uuid[0] = (uint8_t)(value >> 8);        // 取高位字节
            uuid[1] = (uint8_t) value;             // 取低位字节
        }else if(data_len == 32){ // 32
            uint16_t value = service->uuid.u32.value;
            uuid    = (uint8_t *)malloc(4);
            uuid[0] = (uint8_t)(value >> 24); // 取高位字节
            uuid[1] = (uint8_t)(value >> 16); // 取次高位字节
            uuid[2] = (uint8_t)(value >> 8);  // 取次低位字节
            uuid[3] = (uint8_t)value;         // 取低位字节
        }else{ // 128 
            uuid    = (uint8_t *)malloc(16);
            data_inver(service->uuid.u128.value,uuid,16);
        }

        pika_eventListener_send(g_pika_ble_listener,_IRQ_GATTC_SERVICE_RESULT,
                arg_newObj(New_pikaTupleFrom(
                    arg_newInt(_IRQ_GATTC_SERVICE_RESULT),
                    arg_newInt(conn_handle),
                    arg_newInt(service->start_handle),
                    arg_newInt(service->end_handle),
                    arg_newBytes(uuid,data_len/8)
                )));
        free(uuid);
        return 0;
    }else if(error->status == 14){
        pika_eventListener_send(g_pika_ble_listener,_IRQ_GATTC_SERVICE_DONE,
        arg_newObj(New_pikaTupleFrom(
                arg_newInt(_IRQ_GATTC_SERVICE_DONE),
                arg_newInt(conn_handle),
                arg_newInt(0)
                )));
        return 0;
    }else{
        pika_eventListener_send(g_pika_ble_listener,_IRQ_GATTC_SERVICE_DONE,
        arg_newObj(New_pikaTupleFrom(
                arg_newInt(_IRQ_GATTC_SERVICE_DONE),
                arg_newInt(conn_handle),
                arg_newInt(error->status)
                )));
        return 0;
    }
}

// GATT 查找特定UUID服务回调函数
static int ble_gatt_disc_svcs_by_uuid_cb(uint16_t conn_handle,
                                 const struct ble_gatt_error *error,
                                 const struct ble_gatt_svc *service,
                                 void *arg){

    printf("ble_gatt_disc_svcs_by_uuid_cb\r\n");
    printf("error: %d,att handle %d\r\n",error->status,error->att_handle);
    // printf("start handle: %d,end handle %d\r\n",service->start_handle,service->end_handle);
    if(error->status == 0)
    {
        uint8_t data_len = service->uuid.u.type;
        uint8_t *uuid;
        if(data_len == 16){ // = 16 
            uint16_t value = service->uuid.u16.value;
            uuid    = (uint8_t *)malloc(2);
            uuid[0] = (uint8_t)(value >> 8);        // 取高位字节
            uuid[1] = (uint8_t) value;             // 取低位字节
        }else if(data_len == 32){ // 32
            uint16_t value = service->uuid.u32.value;
            uuid    = (uint8_t *)malloc(4);
            uuid[0] = (uint8_t)(value >> 24); // 取高位字节
            uuid[1] = (uint8_t)(value >> 16); // 取次高位字节
            uuid[2] = (uint8_t)(value >> 8);  // 取次低位字节
            uuid[3] = (uint8_t)value;         // 取低位字节
        }else{ // 128 
            uuid    = (uint8_t *)malloc(16);
            data_inver(service->uuid.u128.value,uuid,16);
        }

        pika_eventListener_send(g_pika_ble_listener,_IRQ_GATTC_SERVICE_RESULT,
                arg_newObj(New_pikaTupleFrom(
                    arg_newInt(_IRQ_GATTC_SERVICE_RESULT),
                    arg_newInt(conn_handle),
                    arg_newInt(service->start_handle),
                    arg_newInt(service->end_handle),
                    arg_newBytes(uuid,data_len/8)
                )));
        free(uuid);
        return 0;
    }else if(error->status == 14){
        pika_eventListener_send(g_pika_ble_listener,_IRQ_GATTC_SERVICE_DONE,
        arg_newObj(New_pikaTupleFrom(
                arg_newInt(_IRQ_GATTC_SERVICE_DONE),
                arg_newInt(conn_handle),
                arg_newInt(0)
                )));
        return 0;
    }else{
        pika_eventListener_send(g_pika_ble_listener,_IRQ_GATTC_SERVICE_DONE,
        arg_newObj(New_pikaTupleFrom(
                arg_newInt(_IRQ_GATTC_SERVICE_DONE),
                arg_newInt(conn_handle),
                arg_newInt(error->status)
                )));
        return 0;
    }
}

// GATT 查找所有特性回调函数
static int ble_gatt_disc_all_chrs_cb(uint16_t conn_handle,
                            const struct ble_gatt_error *error,
                            const struct ble_gatt_chr *chr, void *arg){
        
    printf("ble_gatt_disc_all_chrs_cb\r\n");
    if(error->status == 0){
        uint8_t data_len = chr->uuid.u.type;
        uint8_t *uuid;
        if(data_len == 16){ // = 16 
            uint16_t value = chr->uuid.u16.value;
            uuid    = (uint8_t *)malloc(2);
            uuid[0] = (uint8_t)(value >> 8);        // 取高位字节
            uuid[1] = (uint8_t) value;             // 取低位字节
        }else if(data_len == 32){ // 32
            uint16_t value = chr->uuid.u32.value;
            uuid    = (uint8_t *)malloc(4);
            uuid[0] = (uint8_t)(value >> 24); // 取高位字节
            uuid[1] = (uint8_t)(value >> 16); // 取次高位字节
            uuid[2] = (uint8_t)(value >> 8);  // 取次低位字节
            uuid[3] = (uint8_t)value;         // 取低位字节
        }else{ // 128 
            uuid    = (uint8_t *)malloc(16);
            data_inver(chr->uuid.u128.value,uuid,16);
        }
        pika_eventListener_send(g_pika_ble_listener,_IRQ_GATTC_CHARACTERISTIC_RESULT,
                arg_newObj(New_pikaTupleFrom(
                    arg_newInt(_IRQ_GATTC_CHARACTERISTIC_RESULT),
                    arg_newInt(conn_handle),
                    arg_newInt(chr->def_handle),  // 定义的句柄
                    arg_newInt(chr->val_handle),  // 值的句柄
                    arg_newInt(chr->properties),
                    arg_newBytes(uuid,data_len/8)
                )));
        free(uuid);
        return 0;
    }else if(error->status == 14){
        pika_eventListener_send(g_pika_ble_listener,_IRQ_GATTC_CHARACTERISTIC_DONE,
        arg_newObj(New_pikaTupleFrom(
                arg_newInt(_IRQ_GATTC_CHARACTERISTIC_DONE),
                arg_newInt(conn_handle),
                arg_newInt(0)
                )));
        return 0;
    }else{
        pika_eventListener_send(g_pika_ble_listener,_IRQ_GATTC_CHARACTERISTIC_DONE,
        arg_newObj(New_pikaTupleFrom(
                arg_newInt(_IRQ_GATTC_CHARACTERISTIC_DONE),
                arg_newInt(conn_handle),
                arg_newInt(error->status)
                )));
        return 0;
    }
}

// GATT 查找特定UUID特性回调函数
static int ble_gatt_disc_chrs_by_uuid_cb(uint16_t conn_handle,
                            const struct ble_gatt_error *error,
                            const struct ble_gatt_chr *chr, void *arg){
    printf("ble_gatt_disc_chrs_by_uuid_cb\r\n");
    if(error->status == 0){
        uint8_t data_len = chr->uuid.u.type;
        uint8_t *uuid;
        if(data_len == 16){ // = 16 
            uint16_t value = chr->uuid.u16.value;
            uuid    = (uint8_t *)malloc(2);
            uuid[0] = (uint8_t)(value >> 8);        // 取高位字节
            uuid[1] = (uint8_t) value;             // 取低位字节
        }else if(data_len == 32){ // 32
            uint16_t value = chr->uuid.u32.value;
            uuid    = (uint8_t *)malloc(4);
            uuid[0] = (uint8_t)(value >> 24); // 取高位字节
            uuid[1] = (uint8_t)(value >> 16); // 取次高位字节
            uuid[2] = (uint8_t)(value >> 8);  // 取次低位字节
            uuid[3] = (uint8_t)value;         // 取低位字节
        }else{ // 128 
            uuid    = (uint8_t *)malloc(16);
            data_inver(chr->uuid.u128.value,uuid,16);
        }
        pika_eventListener_send(g_pika_ble_listener,_IRQ_GATTC_CHARACTERISTIC_RESULT,
                arg_newObj(New_pikaTupleFrom(
                    arg_newInt(_IRQ_GATTC_CHARACTERISTIC_RESULT),
                    arg_newInt(conn_handle),
                    arg_newInt(chr->def_handle),  
                    arg_newInt(chr->val_handle),
                    arg_newInt(chr->properties),
                    arg_newBytes(uuid,data_len/8)
                )));
        free(uuid);
        return 0;
    }else if(error->status == 14){
        pika_eventListener_send(g_pika_ble_listener,_IRQ_GATTC_CHARACTERISTIC_DONE,
        arg_newObj(New_pikaTupleFrom(
                arg_newInt(_IRQ_GATTC_CHARACTERISTIC_DONE),
                arg_newInt(conn_handle),
                arg_newInt(0)
                )));
        return 0;
    }else{
        pika_eventListener_send(g_pika_ble_listener,_IRQ_GATTC_CHARACTERISTIC_DONE,
        arg_newObj(New_pikaTupleFrom(
                arg_newInt(_IRQ_GATTC_CHARACTERISTIC_DONE),
                arg_newInt(conn_handle),
                arg_newInt(error->status)
                )));
        return 0;
    }
}

// GATT 查找所有描述符回调函数
// TODO:把描述符也显示了
static int ble_gatt_disc_all_dscs_cb(uint16_t conn_handle,
                            const struct ble_gatt_error *error,
                            uint16_t chr_val_handle,
                            const struct ble_gatt_dsc *dsc,
                            void *arg){
    printf("ble_gatt_disc_all_dscs_cb\r\n");
    if(error->status == 0){        
        uint8_t data_len = dsc->uuid.u.type;
        uint8_t *uuid;
        if(data_len == 16){ // = 16 
            uint16_t value = dsc->uuid.u16.value;
            uuid    = (uint8_t *)malloc(2);
            uuid[0] = (uint8_t)(value >> 8);        // 取高位字节
            uuid[1] = (uint8_t) value;             // 取低位字节
        }else if(data_len == 32){ // 32
            uint16_t value = dsc->uuid.u32.value;
            uuid    = (uint8_t *)malloc(4);
            uuid[0] = (uint8_t)(value >> 24); // 取高位字节
            uuid[1] = (uint8_t)(value >> 16); // 取次高位字节
            uuid[2] = (uint8_t)(value >> 8);  // 取次低位字节
            uuid[3] = (uint8_t)value;         // 取低位字节
        }else{ // 128 
            uuid    = (uint8_t *)malloc(16);
            data_inver(dsc->uuid.u128.value,uuid,16);
        }
        pika_eventListener_send(g_pika_ble_listener,_IRQ_GATTC_DESCRIPTOR_RESULT,
                arg_newObj(New_pikaTupleFrom(
                    arg_newInt(_IRQ_GATTC_DESCRIPTOR_RESULT),
                    arg_newInt(conn_handle),
                    // arg_newInt(chr_val_handle),
                    arg_newInt(dsc->handle),  
                    arg_newBytes(uuid,data_len/8)
                )));
        free(uuid);
        return 0;
    }else if(error->status == 14){
        pika_eventListener_send(g_pika_ble_listener,_IRQ_GATTC_DESCRIPTOR_DONE,
        arg_newObj(New_pikaTupleFrom(
                arg_newInt(_IRQ_GATTC_DESCRIPTOR_DONE),
                arg_newInt(conn_handle),
                arg_newInt(0)
                )));
        return 0;
    }else{
        pika_eventListener_send(g_pika_ble_listener,_IRQ_GATTC_DESCRIPTOR_DONE,
        arg_newObj(New_pikaTupleFrom(
                arg_newInt(_IRQ_GATTC_DESCRIPTOR_DONE),
                arg_newInt(conn_handle),
                arg_newInt(error->status)
                )));
        return 0;
    }   
    return 0;
}