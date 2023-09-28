#define _IRQ_COUNT                         32
#define _IRQ_DIY_MAX_ID                    103

#define _IRQ_CENTRAL_CONNECT               1
#define _IRQ_CENTRAL_DISCONNECT            2
// #define _IRQ_CONNECT                       1
// #define _IRQ_DISCONNECT                    2
#define _IRQ_GATTS_WRITE                   3
#define _IRQ_GATTS_READ_REQUEST            4
#define _IRQ_SCAN_RESULT                   5
#define _IRQ_SCAN_DONE                     6
#define _IRQ_PERIPHERAL_CONNECT            7
#define _IRQ_PERIPHERAL_DISCONNECT         8
#define _IRQ_GATTC_SERVICE_RESULT          9
#define _IRQ_GATTC_SERVICE_DONE            10
#define _IRQ_GATTC_CHARACTERISTIC_RESULT   11
#define _IRQ_GATTC_CHARACTERISTIC_DONE     12
#define _IRQ_GATTC_DESCRIPTOR_RESULT       13
#define _IRQ_GATTC_DESCRIPTOR_DONE         14
#define _IRQ_GATTC_READ_RESULT             15
#define _IRQ_GATTC_READ_DONE               16
#define _IRQ_GATTC_WRITE_DONE              17
#define _IRQ_GATTC_NOTIFY                  18
#define _IRQ_GATTC_INDICATE                19
#define _IRQ_GATTS_INDICATE_DONE           20
#define _IRQ_MTU_EXCHANGED                 21
#define _IRQ_L2CAP_ACCEPT                  22
#define _IRQ_L2CAP_CONNECT                 23
#define _IRQ_L2CAP_DISCONNECT              24
#define _IRQ_L2CAP_RECV                    25
#define _IRQ_L2CAP_SEND_READY              26
#define _IRQ_CONNECTION_UPDATE             27
#define _IRQ_ENCRYPTION_UPDATE             28
#define _IRQ_GET_SECRET                    29
#define _IRQ_SET_SECRET                    30
#define _IRQ_GATTC_SUBSCRIBE               31   // 往后事件均相较mpy新增
#define _IRQ_GATTS_SUBSCRIBE               32
#define _IRQ_TEST                          33

#define _IRQ_DIY_REGISTER_HANDLE            101
#define _IRQ_DIY_NIMBLE_READ                102

#define _GATTS_NO_ERROR                           0x00
#define _GATTS_ERROR_READ_NOT_PERMITTED           0x02
#define _GATTS_ERROR_WRITE_NOT_PERMITTED          0x03
#define _GATTS_ERROR_INSUFFICIENT_AUTHENTICATION  0x05
#define _GATTS_ERROR_INSUFFICIENT_AUTHORIZATION   0x08
#define _GATTS_ERROR_INSUFFICIENT_ENCRYPTION      0x0f