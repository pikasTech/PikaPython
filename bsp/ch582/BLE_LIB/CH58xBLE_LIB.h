/********************************** (C) COPYRIGHT ******************************
 * File Name         : CH58xBLE_LIB.H
 * Author            : WCH
 * Version           : V1.20
 * Date              : 2022/01/10
 * Description       : head file
 *******************************************************************************/

/******************************************************************************/
#ifndef __CH58xBLE_LIB_H
#define __CH58xBLE_LIB_H

#ifdef __cplusplus
extern "C"
{
#endif
#include  "stdint.h"

#ifndef int8
typedef signed char             int8;
#endif
#ifndef int16
typedef short                   int16;
#endif
#ifndef BOOL
typedef unsigned char           BOOL;
#endif
#ifndef s8
typedef signed char             s8;
#endif
#ifndef s16
typedef signed short            s16;
#endif
#ifndef s32
typedef signed long             s32;
#endif
#ifndef u8
typedef unsigned char           u8;
#endif
#ifndef u16
typedef unsigned short          u16;
#endif
#ifndef u32
typedef unsigned long           u32;
#endif
#ifndef u64
typedef unsigned long long      u64;
#endif
#ifndef u8C
typedef const unsigned char     u8C;
#endif

#ifndef u8V
typedef unsigned char volatile  u8V;
#endif
#ifndef u32V
typedef unsigned long volatile  u32V;
#endif

#ifndef uint8
typedef unsigned char           uint8;
#endif
#ifndef uint16
typedef unsigned short          uint16;
#endif
#ifndef uint32
typedef unsigned long           uint32;
#endif
#ifndef TRUE
  #define TRUE                  1
#endif
#ifndef FALSE
  #define FALSE                 0
#endif
#ifndef SUCCESS
#define  SUCCESS                0x00
#endif
#ifndef NULL
  #define NULL                  0
#endif
#ifndef CONST
#define CONST                   const
#endif
#ifndef  bStatus_t
typedef unsigned char                   bStatus_t;
#endif
#ifndef  tmosTaskID
typedef unsigned char                   tmosTaskID;
#endif
#ifndef  tmosEvents
typedef unsigned short                  tmosEvents;
#endif
#ifndef  tmosTimer
typedef unsigned long                   tmosTimer;
#endif
// Define function type that generate a random seed callback
typedef uint32_t (*pfnSrandCB)( void );
// Define function type that switch to idle mode callback
typedef uint32_t (*pfnSleepCB)( uint32_t );
// Define function type that run RC 32K clock calibration callback
typedef void (*pfnLSECalibrationCB)( void );
// Define function type that get temperature callback
typedef uint16_t (*pfnTempSampleCB)( void );
// Define function type that connect event complete callback. timeUs - the time relative to next connect event ( only effect in single connection).
typedef void (*pfnConnectEventCB)( uint32_t timeUs ); // Call LL_ConnectEventRegister to init
// Define function type that library status callback.
typedef void (*pfnLibStatusErrorCB)( uint8_t code, uint32_t status );
// Define function type that process event
typedef tmosEvents (*pTaskEventHandlerFn)( tmosTaskID taskID, tmosEvents event );
// Define function type that read flash
typedef uint32_t (*pfnFlashReadCB)( uint32_t addr, uint32_t num, uint32_t *pBuf );
// Define function type that write flash
typedef uint32_t (*pfnFlashWriteCB)( uint32_t addr, uint32_t num, uint32_t *pBuf );
// Define function type that get system clock count
typedef uint32_t (*pfnGetSysClock)( void );

/* BLE library config struct */
typedef struct tag_ble_config
{
    uint32_t MEMAddr;               // library memory start address
    uint16_t MEMLen;                // library memory size
    uint32_t SNVAddr;               // SNV flash start address,must be data-flash area or NULL(bonding information will not be saved)
    uint16_t SNVBlock;              // SNV flash block size ( default 512 )
    uint8_t SNVNum;                 // SNV flash block number ( default 1 )
    uint8_t BufNumber;              // Maximum number of sent and received packages cached by the controller( default 5 )
                                    // Must be greater than the number of connections.
    uint16_t BufMaxLen;             // Maximum length (in octets) of the data portion of each HCI data packet,ATT_MTU = BufMaxLen-4,Range[23,ATT_MAX_MTU_SIZE].( default 27 )
                                    // ATT_MTU = BufMaxLen-4,Range[23,ATT_MAX_MTU_SIZE]
    uint8_t TxNumEvent;             // Maximum number of TX data in a connection event ( default 1 )
    uint8_t RxNumEvent;             // Maximum number of RX data in a connection event ( default equal to BufNumber )
    uint8_t TxPower;                // Transmit power level( default LL_TX_POWEER_0_DBM(0dBm) )
    uint8_t WakeUpTime;             // Wake up time value in one RTC count ( default 45 )
    uint8_t SelRTCClock;            // RTC clock select LSE,LSI(32768Hz or 32000Hz)( default:0 LSE,1: LSI(32000Hz),2:LSI(32768Hz))
                                    // bit7: select connect timer.0:RTC timer 1:system clock timer(must disable sleep)
    uint8_t ConnectNumber;          // Connect number,lower two bits are peripheral number,followed by central number
    uint8_t WindowWidening;         // Wait rf start window
    uint8_t WaitWindow;             // Wait event arrive window
    uint8_t MacAddr[6];             // MAC address,little-endian( factory default )
    pfnSrandCB srandCB;             // Register a program that generate a random seed
    pfnSleepCB sleepCB;             // Register a program that set idle mode
    pfnTempSampleCB tsCB;           // Register a program that read the current temperature,determine whether calibration is need
    pfnLSECalibrationCB rcCB;       // Register a program that RC32K clock calibration
    pfnLibStatusErrorCB staCB;      // Register a program that library status callback
    pfnFlashReadCB readFlashCB;     // Register a program that read flash
    pfnFlashWriteCB writeFlashCB;   // Register a program that write flash
} bleConfig_t; // Library initialization call BLE_LibInit function

/* BLE pa control config struct */
typedef struct tag_ble_pa_control_config
{
    uint32_t txEnableGPIO;        // tx enable gpio register
    uint32_t txDisableGPIO;       // tx disable gpio register
    uint32_t tx_pin;              // tx pin define
    uint32_t rxEnableGPIO;        // rx enable gpio register
    uint32_t rxDisableGPIO;       // rx disable gpio register
    uint32_t rx_pin;              // tx pin define
} blePaControlConfig_t;

// defined for all task
#define SYS_EVENT_MSG                   (0x8000)  // A message is waiting event
#define INVALID_TASK_ID                 0xFF      // Task ID isn't setup properly
#define TASK_NO_TASK                    0xFF

typedef struct
{
    uint8_t event;
    uint8_t status;
} tmos_event_hdr_t;

/*********************************************************************
 * GLOBAL MACROS
 */
#define VER_FILE  "CH58x_BLE_LIB_V1.2"
extern const uint8_t VER_LIB[];  // LIB version
#define SYSTEM_TIME_MICROSEN            625   // unit of process event timer is 625us
#define MS1_TO_SYSTEM_TIME(x)  ((x)*1000/SYSTEM_TIME_MICROSEN)   // transform unit in ms to unit in 625us ( attentional bias )
/* takes a byte out of a uint32_t : var - uint32_t,  ByteNum - byte to take out (0 - 3) */
#define BREAK_UINT32( var, ByteNum ) (uint8_t)((uint32_t)(((var) >>((ByteNum) * 8)) & 0x00FF))
#define HI_UINT16(a) (((a) >> 8) & 0xFF)
#define LO_UINT16(a) ((a) & 0xFF)
#define HI_UINT8(a)  (((a) >> 4) & 0x0F)
#define LO_UINT8(a)  ((a) & 0x0F)
#define BUILD_UINT32(Byte0, Byte1, Byte2, Byte3) \
          ((uint32_t)(((uint32_t)(Byte0) & 0x00FF) \
          + (((uint32_t)(Byte1) & 0x00FF) << 8) \
          + (((uint32_t)(Byte2) & 0x00FF) << 16) \
          + (((uint32_t)(Byte3) & 0x00FF) << 24)))
#define BUILD_UINT16(loByte, hiByte) ((uint16_t)(((loByte) & 0x00FF)|(((hiByte) & 0x00FF)<<8)))

#define ACTIVE_LOW        !
#define ACTIVE_HIGH       !!            // double negation forces result to be '1'

#ifndef BV
#define BV(n)      (1 << (n))
#endif

#ifndef BF
#define BF(x,b,s)  (((x) & (b)) >> (s))
#endif

#ifndef MIN
#define MIN(n,m)   (((n) < (m)) ? (n) : (m))
#endif

#ifndef MAX
#define MAX(n,m)   (((n) < (m)) ? (m) : (n))
#endif

#ifndef ABS
#define ABS(n)     (((n) < 0) ? -(n) : (n))
#endif

/* TxPower define(Accuracy:¡À1dBm) */
#define LL_TX_POWEER_MINUS_16_DBM       0x01
#define LL_TX_POWEER_MINUS_11_DBM       0x03
#define LL_TX_POWEER_MINUS_7_DBM        0x05
#define LL_TX_POWEER_MINUS_5_DBM        0x07
#define LL_TX_POWEER_MINUS_3_DBM        0x09
#define LL_TX_POWEER_MINUS_1_DBM        0x0B
#define LL_TX_POWEER_0_DBM              0x0D
#define LL_TX_POWEER_1_DBM              0x0F
#define LL_TX_POWEER_2_DBM              0x13
#define LL_TX_POWEER_3_DBM              0x17
#define LL_TX_POWEER_4_DBM              0x1D
#define LL_TX_POWEER_5_DBM              0x29
#define LL_TX_POWEER_6_DBM              0x3D

/* ERR_LIB_INIT define */
#define ERR_LLE_IRQ_HANDLE              0x01
#define ERR_MEM_ALLOCATE_SIZE           0x02
#define ERR_SET_MAC_ADDR                0x03
#define ERR_GAP_ROLE_CONFIG             0x04
#define ERR_CONNECT_NUMBER_CONFIG       0x05
#define ERR_SNV_ADDR_CONFIG             0x06
#define ERR_CLOCK_SELECT_CONFIG         0x07

//! Default Public and Random Address Length
#define B_ADDR_LEN                      6
//! Random Number Size
#define B_RANDOM_NUM_SIZE               8
//! Default key length
#define KEYLEN                          16
//! Maximum Advertising Packet Length
#define B_MAX_ADV_LEN                   31     // maximum legacy advertising packet length
#define B_MAX_ADV_EXT_LEN               460    // maximum extended advertising packet length
#define B_MAX_ADV_PERIODIC_LEN          460    // maximum periodic advertising packet length

#define FAILURE                         0x01   //!< Failure
#define INVALIDPARAMETER                0x02   //!< Invalid request field
#define INVALID_TASK                    0x03   //!< Task ID isn't setup properly
#define MSG_BUFFER_NOT_AVAIL            0x04   //!< No buffer is available.
#define INVALID_MSG_POINTER             0x05   //!< No message pointer.
#define INVALID_EVENT_ID                0x06   //!< Invalid event id.
#define INVALID_TIMEOUT                 0x07   //!< Invalid timeout.
#define NO_TIMER_AVAIL                  0x08   //!< No event is available.
#define NV_OPER_FAILED                  0x0A   //!< read a data item to NV failed.
#define INVALID_MEM_SIZE                0x0B   //!< The tokens take up too much space and don't fit into Advertisement data and Scan Response Data

/** BLE_STATUS_VALUES BLE Default BLE Status Values
 * returned as bStatus_t
 */
#define bleInvalidTaskID                INVALID_TASK  //!< Task ID isn't setup properly
#define bleEecKeyRequestRejected        0x06   //!< key missing
#define bleNotReady                     0x10   //!< Not ready to perform task
#define bleAlreadyInRequestedMode       0x11   //!< Already performing that task
#define bleIncorrectMode                0x12   //!< Not setup properly to perform that task
#define bleMemAllocError                0x13   //!< Memory allocation error occurred
#define bleNotConnected                 0x14   //!< Can't perform function when not in a connection
#define bleNoResources                  0x15   //!< There are no resource available
#define blePending                      0x16   //!< Waiting
#define bleTimeout                      0x17   //!< Timed out performing function
#define bleInvalidRange                 0x18   //!< A parameter is out of range
#define bleLinkEncrypted                0x19   //!< The link is already encrypted
#define bleProcedureComplete            0x1A   //!< The Procedure is completed
#define bleInvalidMtuSize               0x1B   //!< SDU size is larger than peer MTU.

/********************************LinkDB****************************************/
// Special case connection handles
#define INVALID_CONNHANDLE              0xFFFF // Invalid connection handle, used for no connection handle
#define LOOPBACK_CONNHANDLE             0xFFFE // Loopback connection handle, used to loopback a message
// Link state flags
#define LINK_NOT_CONNECTED              0x00   // Link isn't connected
#define LINK_CONNECTED                  0x01   // Link is connected
#define LINK_AUTHENTICATED              0x02   // Link is authenticated
#define LINK_BOUND                      0x04   // Link is bonded
#define LINK_ENCRYPTED                  0x10   // Link is encrypted
// Link Database Status callback changeTypes
#define LINKDB_STATUS_UPDATE_NEW        0      // New connection created
#define LINKDB_STATUS_UPDATE_REMOVED    1      // Connection was removed
#define LINKDB_STATUS_UPDATE_STATEFLAGS 2      // Connection state flag changed
/*******************************gattUUID***************************************/
/**
 * GATT Services
 */
#define GAP_SERVICE_UUID                0x1800 // Generic Access Profile
#define GATT_SERVICE_UUID               0x1801 // Generic Attribute Profile

/**
 * GATT Declarations
 */
#define GATT_PRIMARY_SERVICE_UUID       0x2800 // Primary Service
#define GATT_SECONDARY_SERVICE_UUID     0x2801 // Secondary Service
#define GATT_INCLUDE_UUID               0x2802 // Include
#define GATT_CHARACTER_UUID             0x2803 // Characteristic

/**
 * GATT Descriptors
 */
#define GATT_CHAR_EXT_PROPS_UUID        0x2900 // Characteristic Extended Properties
#define GATT_CHAR_USER_DESC_UUID        0x2901 // Characteristic User Description
#define GATT_CLIENT_CHAR_CFG_UUID       0x2902 // Client Characteristic Configuration
#define GATT_SERV_CHAR_CFG_UUID         0x2903 // Server Characteristic Configuration
#define GATT_CHAR_FORMAT_UUID           0x2904 // Characteristic Presentation Format
#define GATT_CHAR_AGG_FORMAT_UUID       0x2905 // Characteristic Aggregate Format
#define GATT_VALID_RANGE_UUID           0x2906 // Valid Range
#define GATT_EXT_REPORT_REF_UUID        0x2907 // External Report Reference Descriptor
#define GATT_REPORT_REF_UUID            0x2908 // Report Reference Descriptor

/**
 * GATT Characteristics
 */
#define DEVICE_NAME_UUID                0x2A00 // Device Name
#define APPEARANCE_UUID                 0x2A01 // Appearance
#define PERI_PRIVACY_FLAG_UUID          0x2A02 // Peripheral Privacy Flag
#define RECONNECT_ADDR_UUID             0x2A03 // Reconnection Address
#define PERI_CONN_PARAM_UUID            0x2A04 // Peripheral Preferred Connection Parameters
#define SERVICE_CHANGED_UUID            0x2A05 // Service Changed
#define CENTRAL_ADDRESS_RESOLUTION_UUID 0x2AA6 // Central Address Resolution

/**
 * GATT Service UUIDs
 */
#define IMMEDIATE_ALERT_SERV_UUID       0x1802  // Immediate Alert
#define LINK_LOSS_SERV_UUID             0x1803  // Link Loss
#define TX_PWR_LEVEL_SERV_UUID          0x1804  // Tx Power
#define CURRENT_TIME_SERV_UUID          0x1805  // Current Time Service
#define REF_TIME_UPDATE_SERV_UUID       0x1806  // Reference Time Update Service
#define NEXT_DST_CHANGE_SERV_UUID       0x1807  // Next DST Change Service
#define GLUCOSE_SERV_UUID               0x1808  // Glucose
#define THERMOMETER_SERV_UUID           0x1809  // Health Thermometer
#define DEVINFO_SERV_UUID               0x180A  // Device Information
#define NWA_SERV_UUID                   0x180B  // Network Availability
#define HEARTRATE_SERV_UUID             0x180D  // Heart Rate
#define PHONE_ALERT_STS_SERV_UUID       0x180E  // Phone Alert Status Service
#define BATT_SERV_UUID                  0x180F  // Battery Service
#define BLOODPRESSURE_SERV_UUID         0x1810  // Blood Pressure
#define ALERT_NOTIF_SERV_UUID           0x1811  // Alert Notification Service
#define HID_SERV_UUID                   0x1812  // Human Interface Device
#define SCAN_PARAM_SERV_UUID            0x1813  // Scan Parameters
#define RSC_SERV_UUID                   0x1814  // Running Speed and Cadence
#define CSC_SERV_UUID                   0x1816  // Cycling Speed and Cadence
#define CYCPWR_SERV_UUID                0x1818  // Cycling Power
#define LOC_NAV_SERV_UUID               0x1819  // Location and Navigation

/**
 * GATT Characteristic UUIDs
 */
#define ALERT_LEVEL_UUID                0x2A06  // Alert Level
#define TX_PWR_LEVEL_UUID               0x2A07  // Tx Power Level
#define DATE_TIME_UUID                  0x2A08  // Date Time
#define DAY_OF_WEEK_UUID                0x2A09  // Day of Week
#define DAY_DATE_TIME_UUID              0x2A0A  // Day Date Time
#define EXACT_TIME_256_UUID             0x2A0C  // Exact Time 256
#define DST_OFFSET_UUID                 0x2A0D  // DST Offset
#define TIME_ZONE_UUID                  0x2A0E  // Time Zone
#define LOCAL_TIME_INFO_UUID            0x2A0F  // Local Time Information
#define TIME_WITH_DST_UUID              0x2A11  // Time with DST
#define TIME_ACCURACY_UUID              0x2A12  // Time Accuracy
#define TIME_SOURCE_UUID                0x2A13  // Time Source
#define REF_TIME_INFO_UUID              0x2A14  // Reference Time Information
#define TIME_UPDATE_CTRL_PT_UUID        0x2A16  // Time Update Control Point
#define TIME_UPDATE_STATE_UUID          0x2A17  // Time Update State
#define GLUCOSE_MEAS_UUID               0x2A18  // Glucose Measurement
#define BATT_LEVEL_UUID                 0x2A19  // Battery Level
#define TEMP_MEAS_UUID                  0x2A1C  // Temperature Measurement
#define TEMP_TYPE_UUID                  0x2A1D  // Temperature Type
#define IMEDIATE_TEMP_UUID              0x2A1E  // Intermediate Temperature
#define MEAS_INTERVAL_UUID              0x2A21  // Measurement Interval
#define BOOT_KEY_INPUT_UUID             0x2A22  // Boot Keyboard Input Report
#define SYSTEM_ID_UUID                  0x2A23  // System ID
#define MODEL_NUMBER_UUID               0x2A24  // Model Number String
#define SERIAL_NUMBER_UUID              0x2A25  // Serial Number String
#define FIRMWARE_REV_UUID               0x2A26  // Firmware Revision String
#define HARDWARE_REV_UUID               0x2A27  // Hardware Revision String
#define SOFTWARE_REV_UUID               0x2A28  // Software Revision String
#define MANUFACTURER_NAME_UUID          0x2A29  // Manufacturer Name String
#define IEEE_11073_CERT_DATA_UUID       0x2A2A  // IEEE 11073-20601 Regulatory Certification Data List
#define CURRENT_TIME_UUID               0x2A2B  // Current Time
#define SCAN_REFRESH_UUID               0x2A31  // Scan Refresh
#define BOOT_KEY_OUTPUT_UUID            0x2A32  // Boot Keyboard Output Report
#define BOOT_MOUSE_INPUT_UUID           0x2A33  // Boot Mouse Input Report
#define GLUCOSE_CONTEXT_UUID            0x2A34  // Glucose Measurement Context
#define BLOODPRESSURE_MEAS_UUID         0x2A35  // Blood Pressure Measurement
#define IMEDIATE_CUFF_PRESSURE_UUID     0x2A36  // Intermediate Cuff Pressure
#define HEARTRATE_MEAS_UUID             0x2A37  // Heart Rate Measurement
#define BODY_SENSOR_LOC_UUID            0x2A38  // Body Sensor Location
#define HEARTRATE_CTRL_PT_UUID          0x2A39  // Heart Rate Control Point
#define NETWORK_AVAIL_UUID              0x2A3E  // Network Availability
#define ALERT_STATUS_UUID               0x2A3F  // Alert Status
#define RINGER_CTRL_PT_UUID             0x2A40  // Ringer Control Point
#define RINGER_SETTING_UUID             0x2A41  // Ringer Setting
#define ALERT_CAT_ID_BMASK_UUID         0x2A42  // Alert Category ID Bit Mask
#define ALERT_CAT_ID_UUID               0x2A43  // Alert Category ID
#define ALERT_NOTIF_CTRL_PT_UUID        0x2A44  // Alert Notification Control Point
#define UNREAD_ALERT_STATUS_UUID        0x2A45  // Unread Alert Status
#define NEW_ALERT_UUID                  0x2A46  // New Alert
#define SUP_NEW_ALERT_CAT_UUID          0x2A47  // Supported New Alert Category
#define SUP_UNREAD_ALERT_CAT_UUID       0x2A48  // Supported Unread Alert Category
#define BLOODPRESSURE_FEATURE_UUID      0x2A49  // Blood Pressure Feature
#define HID_INFORMATION_UUID            0x2A4A  // HID Information
#define REPORT_MAP_UUID                 0x2A4B  // Report Map
#define HID_CTRL_PT_UUID                0x2A4C  // HID Control Point
#define REPORT_UUID                     0x2A4D  // Report
#define PROTOCOL_MODE_UUID              0x2A4E  // Protocol Mode
#define SCAN_INTERVAL_WINDOW_UUID       0x2A4F  // Scan Interval Window
#define PNP_ID_UUID                     0x2A50  // PnP ID
#define GLUCOSE_FEATURE_UUID            0x2A51  // Glucose Feature
#define RECORD_CTRL_PT_UUID             0x2A52  // Record Access Control Point
#define RSC_MEAS_UUID                   0x2A53  // RSC Measurement
#define RSC_FEATURE_UUID                0x2A54  // RSC Feature
#define SC_CTRL_PT_UUID                 0x2A55  // SC Control Point
#define CSC_MEAS_UUID                   0x2A5B  // CSC Measurement
#define CSC_FEATURE_UUID                0x2A5C  // CSC Feature
#define SENSOR_LOC_UUID                 0x2A5D  // Sensor Location
#define CYCPWR_MEAS_UUID                0x2A63  // Cycling Power Measurement
#define CYCPWR_VECTOR_UUID              0x2A64  // Cycling Power Vector
#define CYCPWR_FEATURE_UUID             0x2A65  // Cycling Power Feature
#define CYCPWR_CTRL_PT_UUID             0x2A66  // Cycling Power Control Point
#define LOC_SPEED_UUID                  0x2A67  // Location and Speed
#define NAV_UUID                        0x2A68  // Navigation
#define POS_QUALITY_UUID                0x2A69  // Position Quality
#define LN_FEATURE_UUID                 0x2A6A  // LN Feature
#define LN_CTRL_PT_UUID                 0x2A6B  // LN Control Point  
#define ELE_UUID                        0x2A6C  // Elevation
#define PRESSURE_UUID                   0x2A6D  // Pressure
#define TEMP_UUID                       0x2A6E  // Temperature
#define HUMI_UUID                       0x2A6F  // Humidity
#define TRUE_WIND_SPEED_UUID            0x2A70  // True Wind Speed
#define TRUE_WIND_DIRECTION_UUID        0x2A71  // True Wind Direction
#define URI_UUID                        0x2AB6  // URI
#define MEDIA_STATE_UUID                0x2BA3  // Media State
#define MEDIA_CTRL_PT_UUID              0x2BA4  // Media Control Point
#define MEDIA_CTRL_PT_OS_UUID           0x2BA5  // Media Control Point Opcodes Supported
#define CALL_STATE_UUID                 0x2BBD  // Call State
#define CALL_CTRL_PT_UUID               0x2BBE  // Call Control Point
#define CALL_CTRL_PT_OO_UUID            0x2BBF  // Call Control Point Optional Opcodes
#define TERM_REASON_UUID                0x2BC0  // Termination Reason
#define INCOMING_CALL_UUID              0x2BC1  // Incoming Call
#define MUTE_UUID                       0x2BC3  // Mute

/**
 * GATT Unit UUIDs
 */
#define GATT_UNITLESS_UUID                      0x2700  // unitless
#define GATT_UNIT_LENGTH_METER_UUID             0x2701  // m, m
#define GATT_UNIT_MASS_KGRAM_UUID               0x2702  // kg, kg
#define GATT_UNIT_TIME_SECOND_UUID              0x2703  // s, s
#define GATT_UNIT_ELECTRIC_CURRENT_A_UUID       0x2704  // A, A
#define GATT_UNIT_THERMODYN_TEMP_K_UUID         0x2705  // K, K
#define GATT_UNIT_AMOUNT_SUBSTANCE_M_UUID       0x2706  // mol, mol
#define GATT_UNIT_LUMINOUS_INTENSITY_C_UUID     0x2707  // cd, cd

#define GATT_UNIT_AREA_SQ_MTR_UUID              0x2710  // m^2, m^2
#define GATT_UNIT_VOLUME_CUBIC_MTR_UUID         0x2711  // m^3, m^3
#define GATT_UNIT_VELOCITY_MPS_UUID             0x2712  // m/s, m s^-1
#define GATT_UNIT_ACCELERATION_MPS_SQ_UUID      0x2713  // m/s^2, m s^-2
#define GATT_UNIT_WAVENUMBER_RM_UUID            0x2714  // ? m^-1
#define GATT_UNIT_DENSITY_KGPCM_UUID            0x2715  // p, kg m^-3
#define GATT_UNIT_SURFACE_DENSITY_KGPSM_UUID    0x2716  // pA, kg m^-2
#define GATT_UNIT_SPECIFIC_VOLUME_CMPKG_UUID    0x2717  // v, m^3 kg^-1
#define GATT_UNIT_CURRENT_DENSITY_APSM_UUID     0x2718  // j, A m^-2
#define GATT_UNIT_MAG_FIELD_STRENGTH_UUID       0x2719  // H, A m
#define GATT_UNIT_AMOUNT_CONC_MPCM_UUID         0x271A  // c, mol m^-3
#define GATT_UNIT_MASS_CONC_KGPCM_UUID          0x271B  // c, kg m^-3
#define GATT_UNIT_LUMINANCE_CPSM_UUID           0x271C  // Lv, cd m^-2
#define GATT_UNIT_REFRACTIVE_INDEX_UUID         0x271D  // n, 1
#define GATT_UNIT_RELATIVE_PERMEABLILTY_UUID    0x271E  // u, 1
#define GATT_UNIT_PLANE_ANGLE_RAD_UUID          0x2720  // rad, m m-1
#define GATT_UNIT_SOLID_ANGLE_STERAD_UUID       0x2721  // sr, m2 m-2
#define GATT_UNIT_FREQUENCY_HTZ_UUID            0x2722  // Hz, s-1
#define GATT_UNIT_FORCE_NEWTON_UUID             0x2723  // N, m kg s-2
#define GATT_UNIT_PRESSURE_PASCAL_UUID          0x2724  // Pa, N/m2 = m2 kg s-2
#define GATT_UNIT_ENERGY_JOULE_UUID             0x2725  // J, N m = m2 kg s-2
#define GATT_UNIT_POWER_WATT_UUID               0x2726  // W, J/s = m2 kg s-3
#define GATT_UNIT_E_CHARGE_C_UUID               0x2727  // C, sA
#define GATT_UNIT_E_POTENTIAL_DIF_V_UUID        0x2728  // V, W/A = m2 kg s-3 A-1

#define GATT_UNIT_CELSIUS_TEMP_DC_UUID          0x272F  // oC, t/oC = T/K - 273.15

#define GATT_UNIT_TIME_MINUTE_UUID              0x2760  // min, 60 s
#define GATT_UNIT_TIME_HOUR_UUID                0x2761  // h, 3600 s
#define GATT_UNIT_TIME_DAY_UUID                 0x2762  // d, 86400 s
#define GATT_UNIT_PLANE_ANGLE_DEGREE_UUID       0x2763  // o, (pi/180) rad
#define GATT_UNIT_PLANE_ANGLE_MINUTE_UUID       0x2764  // ', (pi/10800) rad
#define GATT_UNIT_PLANE_ANGLE_SECOND_UUID       0x2765  // '', (pi/648000) rad
#define GATT_UNIT_AREA_HECTARE_UUID             0x2766  // ha, 10^4 m^2
#define GATT_UNIT_VOLUME_LITRE_UUID             0x2767  // l, 10^-3 m^3
#define GATT_UNIT_MASS_TONNE_UUID               0x2768  // t, 10^3 kg

#define GATT_UINT_LENGTH_YARD_UUID              0x27A0  // yd, 0.9144 m
#define GATT_UNIT_LENGTH_PARSEC_UUID            0x27A1  // pc, 3.085678 ?1016 m
#define GATT_UNIT_LENGTH_INCH_UUID              0x27A2  // in, 0.0254 m
#define GATT_UNIT_LENGTH_FOOT_UUID              0x27A3  // ft, 0.3048 m
#define GATT_UNIT_LENGTH_MILE_UUID              0x27A4  // mi, 1609.347 m
#define GATT_UNIT_PRESSURE_PFPSI_UUID           0x27A5  // psi, 6.894757 ?103 Pa
#define GATT_UNIT_VELOCITY_KMPH_UUID            0x27A6  // km/h, 0.2777778 m^s-1
#define GATT_UNIT_VELOCITY_MPH_UUID             0x27A7  // mi/h, 0.44704 m^ s-1
#define GATT_UNIT_ANGULAR_VELOCITY_RPM_UUID     0x27A8  // r/min, 0.1047198 rad s-1
#define GATT_UNIT_ENERGY_GCAL_UUID              0x27A9  // energy (gram calorie)
#define GATT_UNIT_ENERGY_KCAL_UUID              0x27AA  // kcal, 4190.02 J
#define GATT_UNIT_ENERGY_KWH_UUID               0x27AB  // kWh, 3600000 J
#define GATT_UNIT_THERMODYN_TEMP_DF_UUID        0x27AC  // oF, t/oF = T/K ?1.8 - 459.67
#define GATT_UNIT_PERCENTAGE_UUID               0x27AD  // percentage,%
#define GATT_UNIT_PER_MILE_UUID                 0x27AE  // per mille
#define GATT_UNIT_PERIOD_BPM_UUID               0x27AF  // period (beats per minute),BPM
#define GATT_UNIT_E_CHARGE_AH_UUID              0x27B0  // electric charge (ampere hours)
#define GATT_UNIT_MASS_DENSITY_MGPD_UUID        0x27B1  // mass density (milligram per decilitre)
#define GATT_UNIT_MASS_DENSITY_MMPL_UUID        0x27B2  // mass density (millimole per litre)
#define GATT_UNIT_TIME_YEAR_UUID                0x27B3  // time (year)
#define GATT_UNIT_TIME_MONTH_UUID               0x27B4  // time (month)

/*********************************Messages IDs*********************************/
// GATT - Messages IDs
#define GATT_MSG_EVENT                  0xB0 //!< Incoming GATT message
#define GATT_SERV_MSG_EVENT             0xB1 //!< Incoming GATT ServApp message
// GAP - Messages IDs
#define GAP_MSG_EVENT                   0xD0 //!< Incoming GAP message
/************************************ATT***************************************/
#define ATT_MTU_SIZE                    23   //!< Minimum ATT MTU size
#define ATT_MAX_MTU_SIZE                512  //!< Maximum ATT MTU size
// ATT Methods 
#define ATT_ERROR_RSP                   0x01 //!< ATT Error Response
#define ATT_EXCHANGE_MTU_REQ            0x02 //!< ATT Exchange MTU Request
#define ATT_EXCHANGE_MTU_RSP            0x03 //!< ATT Exchange MTU Response
#define ATT_FIND_INFO_REQ               0x04 //!< ATT Find Information Request
#define ATT_FIND_INFO_RSP               0x05 //!< ATT Find Information Response
#define ATT_FIND_BY_TYPE_VALUE_REQ      0x06 //!< ATT Find By Type Value Request
#define ATT_FIND_BY_TYPE_VALUE_RSP      0x07 //!< ATT Find By Type Value Response
#define ATT_READ_BY_TYPE_REQ            0x08 //!< ATT Read By Type Request
#define ATT_READ_BY_TYPE_RSP            0x09 //!< ATT Read By Type Response
#define ATT_READ_REQ                    0x0a //!< ATT Read Request
#define ATT_READ_RSP                    0x0b //!< ATT Read Response
#define ATT_READ_BLOB_REQ               0x0c //!< ATT Read Blob Request
#define ATT_READ_BLOB_RSP               0x0d //!< ATT Read Blob Response
#define ATT_READ_MULTI_REQ              0x0e //!< ATT Read Multiple Request
#define ATT_READ_MULTI_RSP              0x0f //!< ATT Read Multiple Response
#define ATT_READ_BY_GRP_TYPE_REQ        0x10 //!< ATT Read By Group Type Request
#define ATT_READ_BY_GRP_TYPE_RSP        0x11 //!< ATT Read By Group Type Response
#define ATT_WRITE_REQ                   0x12 //!< ATT Write Request
#define ATT_WRITE_RSP                   0x13 //!< ATT Write Response
#define ATT_PREPARE_WRITE_REQ           0x16 //!< ATT Prepare Write Request
#define ATT_PREPARE_WRITE_RSP           0x17 //!< ATT Prepare Write Response
#define ATT_EXECUTE_WRITE_REQ           0x18 //!< ATT Execute Write Request
#define ATT_EXECUTE_WRITE_RSP           0x19 //!< ATT Execute Write Response
#define ATT_HANDLE_VALUE_NOTI           0x1b //!< ATT Handle Value Notification
#define ATT_HANDLE_VALUE_IND            0x1d //!< ATT Handle Value Indication
#define ATT_HANDLE_VALUE_CFM            0x1e //!< ATT Handle Value Confirmation

#define ATT_WRITE_CMD                   0x52 //!< ATT Write Command
#define ATT_SIGNED_WRITE_CMD            0xD2 //!< ATT Signed Write Command

//  ATT Error Codes
#define ATT_ERR_INVALID_HANDLE          0x01 //!< The attribute handle given was not valid on this server
#define ATT_ERR_READ_NOT_PERMITTED      0x02 //!< The attribute cannot be read
#define ATT_ERR_WRITE_NOT_PERMITTED     0x03 //!< The attribute cannot be written
#define ATT_ERR_INVALID_PDU             0x04 //!< The attribute PDU was invalid
#define ATT_ERR_INSUFFICIENT_AUTHEN     0x05 //!< The attribute requires authentication before it can be read or written
#define ATT_ERR_UNSUPPORTED_REQ         0x06 //!< Attribute server does not support the request received from the client
#define ATT_ERR_INVALID_OFFSET          0x07 //!< Offset specified was past the end of the attribute
#define ATT_ERR_INSUFFICIENT_AUTHOR     0x08 //!< The attribute requires authorization before it can be read or written
#define ATT_ERR_PREPARE_QUEUE_FULL      0x09 //!< Too many prepare writes have been queued
#define ATT_ERR_ATTR_NOT_FOUND          0x0a //!< No attribute found within the given attribute handle range
#define ATT_ERR_ATTR_NOT_LONG           0x0b //!< The attribute cannot be read using the Read Blob Request
#define ATT_ERR_INSUFFICIENT_KEY_SIZE   0x0c //!< The Encryption Key Size used for encrypting this link is insufficient
#define ATT_ERR_INVALID_VALUE_SIZE      0x0d //!< The attribute value length is invalid for the operation
#define ATT_ERR_UNLIKELY                0x0e //!< The attribute request that was requested has encountered an error that was very unlikely, and therefore could not be completed as requested
#define ATT_ERR_INSUFFICIENT_ENCRYPT    0x0f //!< The attribute requires encryption before it can be read or written
#define ATT_ERR_UNSUPPORTED_GRP_TYPE    0x10 //!< The attribute type is not a supported grouping attribute as defined by a higher layer specification
#define ATT_ERR_INSUFFICIENT_RESOURCES  0x11 //!< Insufficient Resources to complete the request
#define ATT_ERR_INVALID_VALUE           0x80 //!< The attribute value is invalid for the operation

/*********************************************************************
 * ATT Find By Type Value Response macros
 */
// Attribute Handle and Group End Handle pair indexes
#define ATT_ATTR_HANDLE_IDX( i )        ( (i) * (2 + 2) )
#define ATT_GRP_END_HANDLE_IDX( i )     ( ATT_ATTR_HANDLE_IDX( (i) ) + 2 )

#define ATT_ATTR_HANDLE( info, i )      ( BUILD_UINT16( (info)[ATT_ATTR_HANDLE_IDX((i))], \
                                                        (info)[ATT_ATTR_HANDLE_IDX((i))+1] ) )
#define ATT_GRP_END_HANDLE( info, i )   ( BUILD_UINT16( (info)[ATT_GRP_END_HANDLE_IDX((i))], \
                                                        (info)[ATT_GRP_END_HANDLE_IDX((i))+1] ) )
/** @defgroup ATT_MSG_EVENT_DEFINES ATT Message Event IDs
 * @{
 */
#define ATT_FLOW_CTRL_VIOLATED_EVENT    0x7E //!< Sent when ATT flow control is violated on a connection.  This event is sent as an TMOS message defined as attFlowCtrlViolatedEvt_t.
#define ATT_MTU_UPDATED_EVENT           0x7F //!< Sent when MTU is updated for a connection.  This event is sent as an TMOS message defined as attMtuUpdatedEvt_t.
/** @} End ATT_MSG_EVENT_DEFINES */

/*** Opcode fields: bitmasks ***/
// Size of 16-bit Bluetooth UUID
#define ATT_BT_UUID_SIZE                2
// Size of 128-bit UUID
#define ATT_UUID_SIZE                   16
/******************************** GATT ***********************************/

// GATT Attribute Access Permissions Bit Fields
#define GATT_PERMIT_READ                0x01 //!< Attribute is Readable
#define GATT_PERMIT_WRITE               0x02 //!< Attribute is Writable
#define GATT_PERMIT_AUTHEN_READ         0x04 //!< Read requires Authentication
#define GATT_PERMIT_AUTHEN_WRITE        0x08 //!< Write requires Authentication
#define GATT_PERMIT_AUTHOR_READ         0x10 //!< Read requires Authorization
#define GATT_PERMIT_AUTHOR_WRITE        0x20 //!< Write requires Authorization
#define GATT_PERMIT_ENCRYPT_READ        0x40 //!< Read requires Encryption
#define GATT_PERMIT_ENCRYPT_WRITE       0x80 //!< Write requires Encryption

// GATT Characteristic Properties Bit Fields
#define GATT_PROP_BCAST                 0x01 //!< Permits broadcasts of the Characteristic Value
#define GATT_PROP_READ                  0x02 //!< Permits reads of the Characteristic Value
#define GATT_PROP_WRITE_NO_RSP          0x04 //!< Permits writes of the Characteristic Value without response
#define GATT_PROP_WRITE                 0x08 //!< Permits writes of the Characteristic Value with response
#define GATT_PROP_NOTIFY                0x10 //!< Permits notifications of a Characteristic Value without acknowledgement
#define GATT_PROP_INDICATE              0x20 //!< Permits indications of a Characteristic Value with acknowledgement
#define GATT_PROP_AUTHEN                0x40 //!< Permits signed writes to the Characteristic Value
#define GATT_PROP_EXTENDED              0x80 //!< Additional characteristic properties are defined in the Characteristic Extended Properties Descriptor

// GATT local read or write operation
#define GATT_LOCAL_READ                 0xFF
#define GATT_LOCAL_WRITE                0xFE

// GATT Encryption Key Size Limits
#define GATT_MIN_ENCRYPT_KEY_SIZE       7  //!< GATT Minimum Encryption Key Size
#define GATT_MAX_ENCRYPT_KEY_SIZE       16 //!< GATT Maximum Encryption Key Size

// Attribute handle definitions
#define GATT_INVALID_HANDLE             0x0000 // Invalid attribute handle
#define GATT_MIN_HANDLE                 0x0001 // Minimum attribute handle
#define GATT_MAX_HANDLE                 0xFFFF // Maximum attribute handle

#define GATT_MAX_MTU                    0xFFFF // Maximum MTU size

// Attribute Access Permissions
#define gattPermitRead( a )             ( (a) & GATT_PERMIT_READ )
#define gattPermitWrite( a )            ( (a) & GATT_PERMIT_WRITE )
#define gattPermitAuthenRead( a )       ( (a) & GATT_PERMIT_AUTHEN_READ )
#define gattPermitAuthenWrite( a )      ( (a) & GATT_PERMIT_AUTHEN_WRITE )
#define gattPermitAuthorRead( a )       ( (a) & GATT_PERMIT_AUTHOR_READ )
#define gattPermitAuthorWrite( a )      ( (a) & GATT_PERMIT_AUTHOR_WRITE )
#define gattPermitEncryptRead( a )      ( (a) & GATT_PERMIT_ENCRYPT_READ )
#define gattPermitEncryptWrite( a )     ( (a) & GATT_PERMIT_ENCRYPT_WRITE )

// Check for different UUID types
#define gattPrimaryServiceType( t )     ( ATT_CompareUUID( primaryServiceUUID, ATT_BT_UUID_SIZE, (t).uuid, (t).len ) )
#define gattSecondaryServiceType( t )   ( ATT_CompareUUID( secondaryServiceUUID, ATT_BT_UUID_SIZE, (t).uuid, (t).len ) )
#define gattCharacterType( t )          ( ATT_CompareUUID( characterUUID, ATT_BT_UUID_SIZE, (t).uuid, (t).len ) )
#define gattIncludeType( t )            ( ATT_CompareUUID( includeUUID, ATT_BT_UUID_SIZE, (t).uuid, (t).len ) )
#define gattServiceType( t )            ( gattPrimaryServiceType( (t) ) || gattSecondaryServiceType( (t) ) )
#define GATT_CONNECT_NUM                (3)
#define GATT_MAX_NUM_CONN               (GATT_CONNECT_NUM+1)

// GATT Client Characteristic Configuration Bit Fields
#define GATT_CLIENT_CFG_NOTIFY          0x0001 //!< The Characteristic Value shall be notified
#define GATT_CLIENT_CFG_INDICATE        0x0002 //!< The Characteristic Value shall be indicated

#define GATT_CFG_NO_OPERATION           0x0000 // No operation

// All profile services bit fields
#define GATT_ALL_SERVICES               0xFFFFFFFF

// The number of attribute records in a given attribute table
#define GATT_NUM_ATTRS( attrs )         ( sizeof( attrs ) / sizeof( gattAttribute_t ) )

// The handle of a service is the handle of the first attribute
#define GATT_SERVICE_HANDLE( attrs )    ( (attrs)[0].handle )

// The handle of the first included service (i = 1) is the value of the second attribute
#define GATT_INCLUDED_HANDLE( attrs, i ) ( *((uint16_t *)((attrs)[(i)].pValue)) )

// Client Characteristic Configuration table (from CCC attribute value pointer)
#define GATT_CCC_TBL( pValue )          ( (gattCharCfg_t *)(*((PTR_TYPE)(&pValue))))

/************************************ GAP *************************************/
#define GAP_MSG_EVENT_DEFINES                        //!< GAP type of command
#define GAP_DEVICE_INIT_DONE_EVENT              0x00 //!< Sent when the Device Initialization is complete.  This event is sent as an tmos message defined as gapDeviceInitDoneEvent_t.
#define GAP_DEVICE_DISCOVERY_EVENT              0x01 //!< Sent when the Device Discovery Process is complete. This event is sent as an tmos message defined as gapDevDiscEvent_t.
#define GAP_ADV_DATA_UPDATE_DONE_EVENT          0x02 //!< Sent when the Advertising Data or SCAN_RSP Data has been updated. This event is sent as an tmos message defined as gapAdvDataUpdateEvent_t.
#define GAP_MAKE_DISCOVERABLE_DONE_EVENT        0x03 //!< Sent when the Make Discoverable Request is complete. This event is sent as an tmos message defined as gapMakeDiscoverableRspEvent_t.
#define GAP_END_DISCOVERABLE_DONE_EVENT         0x04 //!< Sent when the Advertising has ended. This event is sent as an tmos message defined as gapEndDiscoverableRspEvent_t.
#define GAP_LINK_ESTABLISHED_EVENT              0x05 //!< Sent when the Establish Link Request is complete. This event is sent as an tmos message defined as gapEstLinkReqEvent_t.
#define GAP_LINK_TERMINATED_EVENT               0x06 //!< Sent when a connection was terminated. This event is sent as an tmos message defined as gapTerminateLinkEvent_t.
#define GAP_LINK_PARAM_UPDATE_EVENT             0x07 //!< Sent when an Update Parameters Event is received. This event is sent as an tmos message defined as gapLinkUpdateEvent_t.
#define GAP_RANDOM_ADDR_CHANGED_EVENT           0x08 //!< Sent when a random address was changed. This event is sent as an tmos message defined as gapRandomAddrEvent_t.
#define GAP_SIGNATURE_UPDATED_EVENT             0x09 //!< Sent when the device's signature counter is updated. This event is sent as an tmos message defined as gapSignUpdateEvent_t.
#define GAP_AUTHENTICATION_COMPLETE_EVENT       0x0A //!< Sent when the Authentication (pairing) process is complete. This event is sent as an tmos message defined as gapAuthCompleteEvent_t.
#define GAP_PASSKEY_NEEDED_EVENT                0x0B //!< Sent when a Passkey is needed.  This is part of the pairing process. This event is sent as an tmos message defined as gapPasskeyNeededEvent_t.
#define GAP_SLAVE_REQUESTED_SECURITY_EVENT      0x0C //!< Sent when a Slave Security Request is received. This event is sent as an tmos message defined as gapSlaveSecurityReqEvent_t.
#define GAP_DEVICE_INFO_EVENT                   0x0D //!< Sent during the Device Discovery Process when a device is discovered. This event is sent as an tmos message defined as gapDeviceInfoEvent_t.
#define GAP_BOND_COMPLETE_EVENT                 0x0E //!< Sent when the bonding process is complete. This event is sent as an tmos message defined as gapBondCompleteEvent_t.
#define GAP_PAIRING_REQ_EVENT                   0x0F //!< Sent when an unexpected Pairing Request is received. This event is sent as an tmos message defined as gapPairingReqEvent_t.
#define GAP_DIRECT_DEVICE_INFO_EVENT            0x10 //!< Sent when a direct Advertising Data is received. This event is sent as an tmos message defined as gapDirectDeviceInfoEvent_t.
#define GAP_PHY_UPDATE_EVENT                    0x11 //!< Sent when a PHY Update Event is received. This event is sent as an tmos message defined as gapLinkUpdateEvent_t.
#define GAP_EXT_ADV_DEVICE_INFO_EVENT           0x12 //!< Sent when a Extended Advertising Data is received. This event is sent as an tmos message defined as gapExtAdvDeviceInfoEvent_t.
#define GAP_MAKE_PERIODIC_ADV_DONE_EVENT        0x13 //!< Sent when the Set Periodic Advertising enable is complete. This event is sent as an tmos message defined as gapMakePeriodicRspEvent_t.
#define GAP_END_PERIODIC_ADV_DONE_EVENT         0x14 //!< Sent when the Set Periodic Advertising disable is complete. This event is sent as an tmos message defined as gapEndPeriodicRspEvent_t.
#define GAP_SYNC_ESTABLISHED_EVENT              0x15 //!< Sent when a Periodic Advertising Sync Establish is complete. This event is sent as an tmos message defined as gapSyncEstablishedEvent_t.
#define GAP_PERIODIC_ADV_DEVICE_INFO_EVENT      0x16 //!< Sent when a Periodic Advertising Data is received. This event is sent as an tmos message defined as gapPeriodicAdvDeviceInfoEvent_t.
#define GAP_SYNC_LOST_EVENT                     0x17 //!< Sent when a Periodic Advertising Sync was lost. This event is sent as an tmos message defined as gapSyncLostEvent_t.
#define GAP_SCAN_REQUEST_EVENT                  0x19 //!< Sent when a SCAN_REQ PDU or an AUX_SCAN_REQ PDU has been received by the advertiser. This event is sent as an tmos message defined as gapScanReqReseiveEvent_t.

// GAP_PROFILE_ROLE_DEFINES GAP Profile Roles
#define GAP_PROFILE_BROADCASTER                 0x01 //!< A device that sends advertising events only.
#define GAP_PROFILE_OBSERVER                    0x02 //!< A device that receives advertising events only.
#define GAP_PROFILE_PERIPHERAL                  0x04 //!< A device that accepts the establishment of an LE physical link using the connection establishment procedure
#define GAP_PROFILE_CENTRAL                     0x08 //!< A device that supports the Central role initiates the establishment of a physical connection

// GAP Status Return Values - returned as bStatus_t
#define bleGAPUserCanceled                      0x30  //!< The user canceled the task
#define bleGAPConnNotAcceptable                 0x31  //!< The connection was not accepted
#define bleGAPBondRejected                      0x32  //!< The bond information was rejected.
#define bleGAPExpiredCanceled                   0x33  //!< The duration has expired

#define GAP_DEVICE_NAME_LEN                     21 // Excluding null-terminate char

// option defined
#define LISTEN_PERIODIC_ADVERTISING_MODE        (1<<0) //!< used to determine whether the Periodic Advertiser List is used
#define REPORTING_INITIALLY_DISABLED            (1<<1) //!< 0: Reporting initially enabled 1: Reporting initially disabled
#define DUPLICATE_FILTERING_INITIALLY_ENABLED   (1<<2) //!< 0: Duplicate filtering initially disabled 1: Duplicate filtering initially enabled

/*-------------------------------------------------------------------
 * CONSTANTS
 */
/** @defgroup GAP_CONN_HANDLE_DEFINES GAP Special Connection Handles
 * Used by GAP_TerminateLinkReq()
 * @{
 */
#define GAP_CONNHANDLE_INIT                     0xFFFE  //!< terminates a link create
#define GAP_CONNHANDLE_ALL                      0xFFFF  //!< terminates all links for the matching task ID.
/** @} End GAP_CONN_HANDLE_DEFINES */

// Privacy Flag States
#define GAP_PRIVACY_DISABLED                    0x00
#define GAP_PRIVACY_ENABLED                     0x01

// GAP GATT Server Parameters used with GGS Get/Set Parameter and Application's Callback functions
#define GGS_DEVICE_NAME_ATT                     0   // RW  uint8_t[GAP_DEVICE_NAME_LEN]
#define GGS_APPEARANCE_ATT                      1   // RW  uint16_t
#define GGS_PERI_PRIVACY_FLAG_ATT               2   // RW  uint8_t
#define GGS_RECONNCT_ADDR_ATT                   3   // RW  uint8_t[B_ADDR_LEN]
#define GGS_PERI_CONN_PARAM_ATT                 4   // RW  sizeof(gapPeriConnectParams_t)
#define GGS_PERI_PRIVACY_FLAG_PROPS             5   // RW  uint8_t
#define GGS_W_PERMIT_DEVICE_NAME_ATT            6   // W   uint8_t
#define GGS_W_PERMIT_APPEARANCE_ATT             7   // W   uint8_t
#define GGS_W_PERMIT_PRIVACY_FLAG_ATT           8   // W   uint8_t
#define GGS_CENT_ADDR_RES_ATT                   9   // RW  uint8_t
// GAP Services bit fields
#define GAP_SERVICE                             0x00000001

// GAP_PARAMETER_ID_DEFINES GAP Parameter IDs
// Timers
#define TGAP_GEN_DISC_ADV_MIN                   0   //!< Minimum time to remain advertising, when in Discoverable mode.Default 0-turns off the timeout. (n * 0.625 mSec).
#define TGAP_LIM_ADV_TIMEOUT                    1   //!< Maximum time to remain advertising, when in Limited Discoverable mode.Default 180 seconds. (n * 1 seconds)
#define TGAP_DISC_SCAN                          2   //!< Minimum time to perform scanning,Setting this parameter to 0 turns off the timeout.Default 10.24seconds. (n * 0.625 mSec)

// when in General Discovery process
#define TGAP_DISC_ADV_INT_MIN                   3   //!< Minimum advertising interval.Default 160. (n * 0.625 mSec)
#define TGAP_DISC_ADV_INT_MAX                   4   //!< Maximum advertising interval.Default 160. (n * 0.625 mSec)
#define TGAP_DISC_SCAN_INT                      5   //!< Scan interval used during Link Layer Scanning state.Default 16. (n * 0.625 mSec)
#define TGAP_DISC_SCAN_WIND                     6   //!< Scan window used during Link Layer Scanning state.Default 16. (n * 0.625 mSec)

// when in Connection Establishment process(1M PHY)
#define TGAP_CONN_EST_INT_MIN                   7   //!< Minimum Link Layer connection interval.Default 80. (n * 1.25 mSec)
#define TGAP_CONN_EST_INT_MAX                   8   //!< Maximum Link Layer connection interval.Default 80. (n * 1.25 mSec)
#define TGAP_CONN_EST_SCAN_INT                  9   //!< Scan interval used during Link Layer Initiating state.Default 16. (n * 0.625 mSec)
#define TGAP_CONN_EST_SCAN_WIND                 10  //!< Scan window used during Link Layer Initiating state.Default 16. (n * 0.625 mSec)
#define TGAP_CONN_EST_HIGH_SCAN_INT             11  //!< Scan interval used during Link Layer Initiating state, high duty scan cycle scan parameters (n * 0.625 mSec)
#define TGAP_CONN_EST_HIGH_SCAN_WIND            12  //!< Scan window used during Link Layer Initiating state, high duty scan cycle scan parameters (n * 0.625 mSec)
#define TGAP_CONN_EST_SUPERV_TIMEOUT            13  //!< Link Layer connection supervision timeout.Default 2000. (n * 10 mSec)
#define TGAP_CONN_EST_LATENCY                   14  //!< Link Layer connection slave latency.Default 0. (in number of connection events)
#define TGAP_CONN_EST_MIN_CE_LEN                15  //!< Local informational parameter about minimum length of connection needed.Default 0. (n * 0.625 mSec)
#define TGAP_CONN_EST_MAX_CE_LEN                16  //!< Local informational parameter about maximum length of connection needed.Default 0. (n * 0.625 mSec)

// Proprietary
#define TGAP_PRIVATE_ADDR_INT                   17  //!< Minimum Time Interval between private (resolvable) address changes.Default 15. (n * 1 minute)
#define TGAP_SM_TIMEOUT                         18  //!< SM Message Timeout (milliseconds). Default 30 seconds.
#define TGAP_SM_MIN_KEY_LEN                     19  //!< SM Minimum Key Length supported. Default 7.
#define TGAP_SM_MAX_KEY_LEN                     20  //!< SM Maximum Key Length supported. Default 16.
#define TGAP_FILTER_ADV_REPORTS                 21  //!< Filter duplicate advertising reports. Default TRUE.
#define TGAP_SCAN_RSSI_MIN                      22  //!< Minimum RSSI required for scan advertising to be reported to the app. Default -127.
#define TGAP_REJECT_CONN_PARAMS                 23  //!< Whether or not to reject Connection Parameter Update Request received on Central device. Default FALSE.
#define TGAP_AUTH_TASK_ID                       24  //!< Task ID override for Task Authentication control (for stack internal use only)

// v5.x
#define TGAP_ADV_TX_POWER                       25  //!< Indicates the maximum power level Range: -127 ¡Ü N ¡Ü +126 Units: dBm.Default 127(Host has no preference).
#define TGAP_ADV_PRIMARY_PHY                    26  //!< Indicates the PHY on which the advertising packets are transmitted on the primary advertising channel.LE 1M/LE Coded.Default GAP_PHY_VAL_LE_1M.
#define TGAP_ADV_SECONDARY_PHY                  27  //!< LE 1M/LE 2M/LE Coded. Default GAP_PHY_VAL_LE_1M.
#define TGAP_ADV_SECONDARY_MAX_SKIP             28  //!< Maximum advertising events the Controller can skip before sending the AUX_ADV_IND packets on the secondary advertising channel. Default 0.
#define TGAP_ADV_ADVERTISING_SID                29  //!< Value of the Advertising SID subfield in the ADI field of the PDU Range:0-15. Default 0.
#define TGAP_ADV_SCAN_REQ_NOTIFY                30  //!< Scan request notifications enabled.Default 0-disabled.
#define TGAP_ADV_ADVERTISING_DURATION           31  //!< Advertising duration Range: 0x0001 ¨C 0xFFFF Time = N * 10ms. Default 0-No advertising duration.
#define TGAP_ADV_MAX_EVENTS                     32  //!< indicates the maximum number of extended advertising events.Range: 0x00 ¨C 0xFF. Default 0(No maximum number of advertising events).

// when in General Discovery process
#define TGAP_DISC_SCAN_PHY                      33  //!< LE 1M/LE Coded. Default GAP_PHY_BIT_LE_1M.
#define TGAP_DISC_SCAN_CODED_INT                34  //!< Scan interval used during Link Layer coded Scanning state, when in General Discovery process (n * 0.625 mSec)
#define TGAP_DISC_SCAN_CODED_WIND               35  //!< Scan window used during Link Layer coded Scanning state, when in General Discovery process (n * 0.625 mSec)
#define TGAP_DISC_SCAN_DURATION                 36  //!< Scan duration Range: 0x0001 ¨C 0xFFFF Time = N * 10 ms. Default 0-Scan continuously until explicitly disable.
#define TGAP_DISC_SCAN_PERIOD                   37  //!< Time interval from when the Controller started its last Scan_Duration until it begins the subsequent Scan_Duration.
                                                    //!< Default 0 Periodic scanning disabled.

// when in Connection Establishment process(2M PHY)
#define TGAP_CONN_EST_INT_PHY                   38  //!< LE 1M/LE Coded. Default GAP_PHY_BIT_LE_1M.
#define TGAP_CONN_EST_2M_INT_MIN                39  //!< Minimum Link Layer connection interval.Default 80. (n * 1.25 mSec)
#define TGAP_CONN_EST_2M_INT_MAX                40  //!< Maximum Link Layer connection interval.Default 80. (n * 1.25 mSec)
#define TGAP_CONN_EST_2M_SUPERV_TIMEOUT         41  //!< Link Layer connection supervision timeout.Default 2000. (n * 10 mSec)
#define TGAP_CONN_EST_2M_LATENCY                42  //!< Link Layer connection slave latency.Default 0. (in number of connection events)
#define TGAP_CONN_EST_2M_MIN_CE_LEN             43  //!< Local informational parameter about minimum length of connection needed.Default 0. (n * 0.625 mSec)
#define TGAP_CONN_EST_2M_MAX_CE_LEN             44  //!< Local informational parameter about maximum length of connection needed.Default 0. (n * 0.625 mSec)

// when in Connection Establishment process(Coded PHY)
#define TGAP_CONN_EST_CODED_INT_MIN             45  //!< Minimum Link Layer connection interval.Default 80. (n * 1.25 mSec)
#define TGAP_CONN_EST_CODED_INT_MAX             46  //!< Maximum Link Layer connection interval.Default 80. (n * 1.25 mSec)
#define TGAP_CONN_EST_CODED_SCAN_INT            47  //!< Scan interval used during Link Layer Initiating state.Default 16. (n * 0.625 mSec)
#define TGAP_CONN_EST_CODED_SCAN_WIND           48  //!< Scan window used during Link Layer Initiating state.Default 16. (n * 0.625 mSec)
#define TGAP_CONN_EST_CODED_HIGH_SCAN_INT       49  //!< Scan interval used during Link Layer Initiating state, high duty scan cycle scan parameters (n * 0.625 mSec)
#define TGAP_CONN_EST_CODED_HIGH_SCAN_WIND      50  //!< Scan window used during Link Layer Initiating state, high duty scan cycle scan parameters (n * 0.625 mSec)
#define TGAP_CONN_EST_CODED_SUPERV_TIMEOUT      51  //!< Link Layer connection supervision timeout.Default 2000. (n * 10 mSec)
#define TGAP_CONN_EST_CODED_LATENCY             52  //!< Link Layer connection slave latency.Default 0. (in number of connection events)
#define TGAP_CONN_EST_CODED_MIN_CE_LEN          53  //!< Local informational parameter about minimum length of connection needed.Default 0. (n * 0.625 mSec)
#define TGAP_CONN_EST_CODED_MAX_CE_LEN          54  //!< Local informational parameter about maximum length of connection needed.Default 0. (n * 0.625 mSec)

// periodic advertising
#define TGAP_PERIODIC_ADV_INT_MIN               55  //!< Minimum periodic advertising interval.Range: 0x0006 to 0xFFFF.Default 160. (n * 1.25 mSec)
#define TGAP_PERIODIC_ADV_INT_MAX               56  //!< Maximum periodic advertising interval.Range: 0x0006 to 0xFFFF.Default 160. (n * 1.25 mSec)
#define TGAP_PERIODIC_ADV_PROPERTIES            57  //!< Include TxPower in the periodic advertising PDU.

#define TGAP_PARAMID_MAX                        58  //!< ID MAX-valid Parameter ID

// GAP_DEVDISC_MODE_DEFINES GAP Device Discovery Modes
#define DEVDISC_MODE_NONDISCOVERABLE            0x00  //!< No discoverable setting
#define DEVDISC_MODE_GENERAL                    0x01  //!< General Discoverable devices
#define DEVDISC_MODE_LIMITED                    0x02  //!< Limited Discoverable devices
#define DEVDISC_MODE_ALL                        0x03  //!< Not filtered

// GAP_ADDR_TYPE_DEFINES GAP Address Types
#define ADDRTYPE_PUBLIC                         0x00  //!< Use the BD_ADDR
#define ADDRTYPE_STATIC                         0x01  //!< Static address
#define ADDRTYPE_PRIVATE_NONRESOLVE             0x02  //!< Generate Non-Resolvable Private Address
#define ADDRTYPE_PRIVATE_RESOLVE                0x03  //!< Generate Resolvable Private Address

// GAP_ADVERTISEMENT_TYPE_DEFINES GAP Advertising Event Types
#define GAP_ADTYPE_ADV_IND                      0x00  //!< Connectable undirected event typet
#define GAP_ADTYPE_ADV_HDC_DIRECT_IND           0x01  //!< Connectable high duty cycle directed event type
#define GAP_ADTYPE_ADV_SCAN_IND                 0x02  //!< Scannable undirected event type
#define GAP_ADTYPE_ADV_NONCONN_IND              0x03  //!< Non-Connectable undirected event type
#define GAP_ADTYPE_ADV_LDC_DIRECT_IND           0x04  //!< Connectable low duty cycle directed event type
//v5.x
#define GAP_ADTYPE_EXT_CONN_DIRECT              0x05  //!< extend Connectable directed event type
#define GAP_ADTYPE_EXT_SCAN_UNDIRECT            0x06  //!< extend Scannable undirected event type
#define GAP_ADTYPE_EXT_NONCONN_NONSCAN_UNDIRECT 0x07 //!< extend Non-Connectable and Non-Scannable undirected event type
#define GAP_ADTYPE_EXT_CONN_UNDIRECT            0x08  //!< extend Connectable undirected event type
#define GAP_ADTYPE_EXT_SCAN_DIRECT              0x09  //!< extend Scannable directed event type
#define GAP_ADTYPE_EXT_NONCONN_NONSCAN_DIRECT   0x0A  //!< extend Non-Connectable and Non-Scannable directed event type

// GAP_ADVERTISEMENT_TYPE_DEFINES GAP Advertising PHY VAL TYPE(GAP_PHY_VAL_TYPE)
#define GAP_PHY_VAL_TYPE
#define GAP_PHY_VAL_LE_1M                       0x01
#define GAP_PHY_VAL_LE_2M                       0x02
#define GAP_PHY_VAL_LE_CODED                    0x03
#define GAP_PHY_VAL_LE_CODED_S2                 0x04

// GAP_ADVERTISEMENT_TYPE_DEFINES GAP Scan PHY VAL TYPE(GAP_PHY_BIT_TYPE)
#define GAP_PHY_BIT_TYPE
#define GAP_PHY_BIT_LE_1M                       (1<<0)
#define GAP_PHY_BIT_LE_2M                       (1<<1)
#define GAP_PHY_BIT_LE_CODED                    (1<<2)
#define GAP_PHY_BIT_ALL                         (GAP_PHY_BIT_LE_1M|GAP_PHY_BIT_LE_2M|GAP_PHY_BIT_LE_CODED)
#define GAP_PHY_BIT_LE_CODED_S2                 (1<<3)

// GAP_ADVERTISEMENT_TYPE_DEFINES GAP Periodic Advertising Properties
#define GAP_PERI_PROPERTIES_INCLUDE_TXPOWER     (1<<6)


// GAP Advertising Report Event Types
#define GAP_ADVERTISEMENT_REPORT_TYPE_DEFINES
// bit0 to 4 ADVERTISEMENT_TYPE:defined for gapExtAdvDeviceInfoEvent_t Advertisement data type
#define GAP_ADRPT_ADV_IND                       0x00  //!< Connectable undirected advertisement
#define GAP_ADRPT_ADV_DIRECT_IND                0x01  //!< Connectable directed advertisement
#define GAP_ADRPT_ADV_SCAN_IND                  0x02  //!< Scannable undirected advertisement
#define GAP_ADRPT_ADV_NONCONN_IND               0x03  //!< Non-Connectable undirected advertisement
#define GAP_ADRPT_SCAN_RSP                      0x04  //!< Scan Response
#define GAP_ADRPT_EXT_CONN_DIRECT               0x05  //!< extend Connectable directed report type
#define GAP_ADRPT_EXT_SCAN_UNDIRECT             0x06  //!< extend Scannable undirected report type
#define GAP_ADRPT_EXT_NONCONN_NONSCAN_UNDIRECT  0x07  //!< extend Non-Connectable and Non-Scannable undirected report type
#define GAP_ADRPT_EXT_CONN_UNDIRECT             0x08  //!< extend Connectable undirected report type
#define GAP_ADRPT_EXT_SCAN_DIRECT               0x09  //!< extend Scannable directed report type
#define GAP_ADRPT_EXT_NONCONN_NONSCAN_DIRECT    0x0A  //!< extend Non-Connectable and Non-Scannable directed report type
#define GAP_ADRPT_EXT_SCAN_RESPONSE             0x0B  //!< extend Scan Response report type
// bit5 to 6 Data status:defined for gapExtAdvDeviceInfoEvent_t Advertisement data type
#define GAP_ADRPT_EXT_DATA_MASK                 (3<<5)
#define GAP_ADRPT_EXT_DATA_COMPLETE             (0<<5) //!< Complete
#define GAP_ADRPT_EXT_DATA_INCOMPLETE           (1<<5) //!< more data to come
#define GAP_ADRPT_EXT_DATA_LAST                 (2<<5) //!< Incomplete, data truncated, no more to come

// GAP_EXTEND_ADVERTISEMENT_REPORT_TYPE_DEFINES GAP Extend Advertising Report Event Types
#define GAP_ADRPT_ADV_CONNECTABLE               (1<<0)
#define GAP_ADRPT_ADV_SCANNABLE                 (1<<1)
#define GAP_ADRPT_ADV_DITECTED                  (1<<2)
#define GAP_ADRPT_SCAN_RESPONSE                 (1<<3)

// GAP_FILTER_POLICY_DEFINES GAP Advertiser Filter Scan Parameters
#define GAP_FILTER_POLICY_ALL                   0x00  //!< Allow Scan Request from Any, Allow Connect Request from Any (default).
#define GAP_FILTER_POLICY_WHITE_SCAN            0x01  //!< Allow Scan Request from White List Only, Allow Connect from Any
#define GAP_FILTER_POLICY_WHITE_CON             0x02  //!< Allow Scan Request from Any, Connect from White List Only
#define GAP_FILTER_POLICY_WHITE                 0x03  //!< Allow Scan Request and Connect from White List Only

//! Maximum Pairing Passcode/Passkey value.  Range of a passkey can be 0 - 999,999.
#define GAP_PASSCODE_MAX                        999999

/** Sign Counter Initialized - Sign counter hasn't been used yet.  Used when setting up
 *  a connection's signing information.
 */
#define GAP_INIT_SIGN_COUNTER                   0xFFFFFFFF

// GAP_ADVCHAN_DEFINES GAP Advertisement Channel Map
#define GAP_ADVCHAN_37  0x01  //!< Advertisement Channel 37
#define GAP_ADVCHAN_38  0x02  //!< Advertisement Channel 38
#define GAP_ADVCHAN_39  0x04  //!< Advertisement Channel 39
#define GAP_ADVCHAN_ALL (GAP_ADVCHAN_37 | GAP_ADVCHAN_38 | GAP_ADVCHAN_39) //!< All Advertisement Channels Enabled

// GAP_ADTYPE_DEFINES GAP Advertisement Data Types
#define GAP_ADTYPE_FLAGS                        0x01 //!< Discovery Mode: @ref GAP_ADTYPE_FLAGS_MODES
#define GAP_ADTYPE_16BIT_MORE                   0x02 //!< Service: More 16-bit UUIDs available
#define GAP_ADTYPE_16BIT_COMPLETE               0x03 //!< Service: Complete list of 16-bit UUIDs
#define GAP_ADTYPE_32BIT_MORE                   0x04 //!< Service: More 32-bit UUIDs available
#define GAP_ADTYPE_32BIT_COMPLETE               0x05 //!< Service: Complete list of 32-bit UUIDs
#define GAP_ADTYPE_128BIT_MORE                  0x06 //!< Service: More 128-bit UUIDs available
#define GAP_ADTYPE_128BIT_COMPLETE              0x07 //!< Service: Complete list of 128-bit UUIDs
#define GAP_ADTYPE_LOCAL_NAME_SHORT             0x08 //!< Shortened local name
#define GAP_ADTYPE_LOCAL_NAME_COMPLETE          0x09 //!< Complete local name
#define GAP_ADTYPE_POWER_LEVEL                  0x0A //!< TX Power Level: -127 to +127 dBm
#define GAP_ADTYPE_OOB_CLASS_OF_DEVICE          0x0D //!< Simple Pairing OOB Tag: Class of device (3 octets)
#define GAP_ADTYPE_OOB_SIMPLE_PAIRING_HASHC     0x0E //!< Simple Pairing OOB Tag: Simple Pairing Hash C (16 octets)
#define GAP_ADTYPE_OOB_SIMPLE_PAIRING_RANDR     0x0F //!< Simple Pairing OOB Tag: Simple Pairing Randomizer R (16 octets)
#define GAP_ADTYPE_SM_TK                        0x10 //!< Security Manager TK Value
#define GAP_ADTYPE_SM_OOB_FLAG                  0x11 //!< Security Manager OOB Flags
#define GAP_ADTYPE_SLAVE_CONN_INTERVAL_RANGE    0x12 //!< Min and Max values of the connection interval (2 octets Min, 2 octets Max) (0xFFFF indicates no conn interval min or max)
#define GAP_ADTYPE_SIGNED_DATA                  0x13 //!< Signed Data field
#define GAP_ADTYPE_SERVICES_LIST_16BIT          0x14 //!< Service Solicitation: list of 16-bit Service UUIDs
#define GAP_ADTYPE_SERVICES_LIST_128BIT         0x15 //!< Service Solicitation: list of 128-bit Service UUIDs
#define GAP_ADTYPE_SERVICE_DATA                 0x16 //!< Service Data - 16-bit UUID
#define GAP_ADTYPE_PUBLIC_TARGET_ADDR           0x17 //!< Public Target Address
#define GAP_ADTYPE_RANDOM_TARGET_ADDR           0x18 //!< Random Target Address
#define GAP_ADTYPE_APPEARANCE                   0x19 //!< Appearance
#define GAP_ADTYPE_ADV_INTERVAL                 0x1A //!< Advertising Interval
#define GAP_ADTYPE_LE_BD_ADDR                   0x1B //!< LE Bluetooth Device Address
#define GAP_ADTYPE_LE_ROLE                      0x1C //!< LE Role
#define GAP_ADTYPE_SIMPLE_PAIRING_HASHC_256     0x1D //!< Simple Pairing Hash C-256
#define GAP_ADTYPE_SIMPLE_PAIRING_RANDR_256     0x1E //!< Simple Pairing Randomizer R-256
#define GAP_ADTYPE_SERVICE_DATA_32BIT           0x20 //!< Service Data - 32-bit UUID
#define GAP_ADTYPE_SERVICE_DATA_128BIT          0x21 //!< Service Data - 128-bit UUID
#define GAP_ADTYPE_URI                          0x24 //!< URI
#define GAP_ADTYPE_SUPPORTED_FEATURES           0x27 //!< LE Supported Features
#define GAP_ADTYPE_CHANNEL_MAP_UPDATE           0x28 //!< Channel Map Update Indication
#define GAP_ADTYPE_BIG_INFO                     0x2C //!< BIGInfo
#define GAP_ADTYPE_BROADCAST_CODE               0x2D //!< Broadcast_Code
#define GAP_ADTYPE_3D_INFO_DATA                 0x3D //!< 3D Information Data
#define GAP_ADTYPE_MANUFACTURER_SPECIFIC        0xFF //!< Manufacturer Specific Data: first 2 octets contain the Company Identifier Code followed by the additional manufacturer specific data

// GAP_ADTYPE_FLAGS_MODES GAP ADTYPE Flags Discovery Modes
#define GAP_ADTYPE_FLAGS_LIMITED                0x01 //!< Discovery Mode: LE Limited Discoverable Mode
#define GAP_ADTYPE_FLAGS_GENERAL                0x02 //!< Discovery Mode: LE General Discoverable Mode
#define GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED    0x04 //!< Discovery Mode: BR/EDR Not Supported

// GAP_APPEARANCE_VALUES GAP Appearance Values
#define GAP_APPEARE_UNKNOWN                     0x0000 //!< Unknown
#define GAP_APPEARE_GENERIC_PHONE               0x0040 //!< Generic Phone
#define GAP_APPEARE_GENERIC_COMPUTER            0x0080 //!< Generic Computer
#define GAP_APPEARE_GENERIC_WATCH               0x00C0 //!< Generic Watch
#define GAP_APPEARE_WATCH_SPORTS                0x00C1 //!< Watch: Sports Watch
#define GAP_APPEARE_GENERIC_CLOCK               0x0100 //!< Generic Clock
#define GAP_APPEARE_GENERIC_DISPLAY             0x0140 //!< Generic Display
#define GAP_APPEARE_GENERIC_RC                  0x0180 //!< Generic Remote Control
#define GAP_APPEARE_GENERIC_EYE_GALSSES         0x01C0 //!< Generic Eye-glasses
#define GAP_APPEARE_GENERIC_TAG                 0x0200 //!< Generic Tag
#define GAP_APPEARE_GENERIC_KEYRING             0x0240 //!< Generic Keyring
#define GAP_APPEARE_GENERIC_MEDIA_PLAYER        0x0280 //!< Generic Media Player
#define GAP_APPEARE_GENERIC_BARCODE_SCANNER     0x02C0 //!< Generic Barcode Scanner
#define GAP_APPEARE_GENERIC_THERMOMETER         0x0300 //!< Generic Thermometer
#define GAP_APPEARE_GENERIC_THERMO_EAR          0x0301 //!< Thermometer: Ear
#define GAP_APPEARE_GENERIC_HR_SENSOR           0x0340 //!< Generic Heart rate Sensor
#define GAP_APPEARE_GENERIC_HRS_BELT            0x0341 //!< Heart Rate Sensor: Heart Rate Belt
#define GAP_APPEARE_GENERIC_BLOOD_PRESSURE      0x0380 //!< Generic Blood Pressure
#define GAP_APPEARE_GENERIC_BP_ARM              0x0381 //!< Blood Pressure: Arm
#define GAP_APPEARE_GENERIC_BP_WRIST            0x0382 //!< Blood Pressure: Wrist
#define GAP_APPEARE_GENERIC_HID                 0x03C0 //!< Generic Human Interface Device (HID)
#define GAP_APPEARE_HID_KEYBOARD                0x03C1 //!< HID Keyboard
#define GAP_APPEARE_HID_MOUSE                   0x03C2 //!< HID Mouse
#define GAP_APPEARE_HID_JOYSTIC                 0x03C3 //!< HID Joystick
#define GAP_APPEARE_HID_GAMEPAD                 0x03C4 //!< HID Gamepad
#define GAP_APPEARE_HID_DIGITIZER_TYABLET       0x03C5 //!< HID Digitizer Tablet
#define GAP_APPEARE_HID_DIGITAL_CARDREADER      0x03C6 //!< HID Card Reader
#define GAP_APPEARE_HID_DIGITAL_PEN             0x03C7 //!< HID Digital Pen
#define GAP_APPEARE_HID_BARCODE_SCANNER         0x03C8 //!< HID Barcode Scanner

/************************************gapRole***********************************/
// GAPROLE_PROFILE_PARAMETERS GAP Role Manager Parameters
#define GAPROLE_PROFILEROLE                     0x300  //!< Reading this parameter will return GAP Role type. Read Only. Size is uint8_t.
#define GAPROLE_IRK                             0x301  //!< Identity Resolving Key. Read/Write. Size is uint8_t[KEYLEN]. Default is all 0, which means that the IRK will be randomly generated.
#define GAPROLE_SRK                             0x302  //!< Signature Resolving Key. Read/Write. Size is uint8_t[KEYLEN]. Default is all 0, which means that the SRK will be randomly generated.
#define GAPROLE_SIGNCOUNTER                     0x303  //!< Sign Counter. Read/Write. Size is uint32_t. Default is 0.
#define GAPROLE_BD_ADDR                         0x304  //!< Device's Address. Read Only. Size is uint8_t[B_ADDR_LEN]. This item is read from the controller.
#define GAPROLE_ADVERT_ENABLED                  0x305  //!< Enable/Disable Advertising. Read/Write. Size is uint8_t. Default is TRUE=Enabled.
#define GAPROLE_ADVERT_DATA                     0x306  //!< Advertisement Data. Read/Write. Max size is B_MAX_ADV_EXT_LEN. Default to all 0.
#define GAPROLE_SCAN_RSP_DATA                   0x307  //!< Scan Response Data. Read/Write. Max size is B_MAX_ADV_EXT_LEN. Defaults to all 0.
#define GAPROLE_ADV_EVENT_TYPE                  0x308  //!< Advertisement Type. Read/Write. Size is uint8_t.  Default is GAP_ADTYPE_ADV_IND.
#define GAPROLE_ADV_DIRECT_TYPE                 0x309  //!< Direct Advertisement Address Type. Read/Write. Size is uint8_t. Default is ADDRTYPE_PUBLIC.
#define GAPROLE_ADV_DIRECT_ADDR                 0x30A  //!< Direct Advertisement Address. Read/Write. Size is uint8_t[B_ADDR_LEN]. Default is NULL.
#define GAPROLE_ADV_CHANNEL_MAP                 0x30B  //!< Which channels to advertise on. Read/Write Size is uint8_t. Default is GAP_ADVCHAN_ALL
#define GAPROLE_ADV_FILTER_POLICY               0x30C  //!< Filter Policy. Ignored when directed advertising is used. Read/Write. Size is uint8_t. Default is GAP_FILTER_POLICY_ALL.
#define GAPROLE_STATE                           0x30D  //!< Reading this parameter will return GAP Peripheral Role State. Read Only. Size is uint8_t.
#define GAPROLE_MAX_SCAN_RES                    0x30E  //!< Maximum number of discover scan results to receive. Default is 0 = unlimited.
#define GAPROLE_MIN_CONN_INTERVAL               0x311  //!< Minimum Connection Interval to allow (n * 1.25ms).  Range: 7.5 msec to 4 seconds (0x0006 to 0x0C80). Read/Write. Size is uint16_t. Default is 7.5 milliseconds (0x0006).
#define GAPROLE_MAX_CONN_INTERVAL               0x312  //!< Maximum Connection Interval to allow (n * 1.25ms).  Range: 7.5 msec to 4 seconds (0x0006 to 0x0C80). Read/Write. Size is uint16_t. Default is 4 seconds (0x0C80).
// v5.x
#define GAPROLE_PHY_TX_SUPPORTED                0x313  //!< The transmitter PHYs that the Host prefers the Controller to use.Default is GAP_PHY_BIT_ALL
#define GAPROLE_PHY_RX_SUPPORTED                0x314  //!< The receiver PHYs that the Host prefers the Controller to use.Default is GAP_PHY_BIT_ALL
#define GAPROLE_PERIODIC_ADVERT_DATA            0x315  //!< Periodic advertisement Data. Read/Write. Max size is B_MAX_ADV_PERIODIC_LEN. Default to all 0.
#define GAPROLE_PERIODIC_ADVERT_ENABLED         0x316  //!< bit0:Enable/Disable Periodic Advertising. Read/Write. Size is uint8_t. Default is FALSE=Disable.
                                                       //!< bit1:Include the ADI field in AUX_SYNC_IND PDUs

/************************************GAPBOND***********************************/
// GAPBOND_PROFILE_PARAMETERS GAP Bond Manager Parameters
#define GAPBOND_PERI_PAIRING_MODE               0x400  //!< Pairing Mode: @ref  GAPBOND_PAIRING_MODE_DEFINES. Read/Write. Size is uint8_t. Default is GAPBOND_PAIRING_MODE_WAIT_FOR_REQ.
#define GAPBOND_PERI_MITM_PROTECTION            0x401  //!< Man-In-The-Middle (MITM) basically turns on Passkey protection in the pairing algorithm. Read/Write. Size is uint8_t. Default is 0(disabled).
#define GAPBOND_PERI_IO_CAPABILITIES            0x402  //!< I/O capabilities.  Read/Write. Size is uint8_t. Default is GAPBOND_IO_CAP_DISPLAY_ONLY @ref GAPBOND_IO_CAP_DEFINES.
#define GAPBOND_PERI_OOB_ENABLED                0x403  //!< OOB data available for pairing algorithm. Read/Write. Size is uint8_t. Default is 0(disabled).
#define GAPBOND_PERI_OOB_DATA                   0x404  //!< OOB Data. Read/Write. size uint8_t[16]. Default is all 0's.
#define GAPBOND_PERI_BONDING_ENABLED            0x405  //!< Request Bonding during the pairing process if enabled.  Read/Write. Size is uint8_t. Default is 0(disabled).
#define GAPBOND_PERI_KEY_DIST_LIST              0x406  //!< The key distribution list for bonding.  size is uint8_t.  @ref GAPBOND_KEY_DIST_DEFINES. Default is sEncKey, sIdKey, mIdKey, mSign enabled.
#define GAPBOND_PERI_DEFAULT_PASSCODE           0x407  //!< The default passcode for MITM protection. size is uint32_t. Range is 0 - 999,999. Default is 0.
#define GAPBOND_CENT_PAIRING_MODE               0x408  //!< Pairing Mode: @ref  GAPBOND_PAIRING_MODE_DEFINES. Read/Write. Size is uint8_t. Default is GAPBOND_PAIRING_MODE_WAIT_FOR_REQ.
#define GAPBOND_CENT_MITM_PROTECTION            0x409  //!< Man-In-The-Middle (MITM) basically turns on Passkey protection in the pairing algorithm. Read/Write. Size is uint8_t. Default is 0(disabled).
#define GAPBOND_CENT_IO_CAPABILITIES            0x40A  //!< I/O capabilities.  Read/Write. Size is uint8_t. Default is GAPBOND_IO_CAP_DISPLAY_ONLY @ref GAPBOND_IO_CAP_DEFINES.
#define GAPBOND_CENT_OOB_ENABLED                0x40B  //!< OOB data available for pairing algorithm. Read/Write. Size is uint8_t. Default is 0(disabled).
#define GAPBOND_CENT_OOB_DATA                   0x40C  //!< OOB Data. Read/Write. size uint8_t[16]. Default is all 0's.
#define GAPBOND_CENT_BONDING_ENABLED            0x40D  //!< Request Bonding during the pairing process if enabled.  Read/Write. Size is uint8_t. Default is 0(disabled).
#define GAPBOND_CENT_KEY_DIST_LIST              0x40E  //!< The key distribution list for bonding.  size is uint8_t.  @ref GAPBOND_KEY_DIST_DEFINES. Default is sEncKey, sIdKey, mIdKey, mSign enabled.
#define GAPBOND_CENT_DEFAULT_PASSCODE           0x40F  //!< The default passcode for MITM protection. size is uint32_t. Range is 0 - 999,999. Default is 0.
#define GAPBOND_ERASE_ALLBONDS                  0x410  //!< Erase all of the bonded devices. Write Only. No Size.
#define GAPBOND_AUTO_FAIL_PAIRING               0x411  //!< TEST MODE (DO NOT USE) to automatically send a Pairing Fail when a Pairing Request is received. Read/Write. size is uint8_t. Default is 0 (disabled).
#define GAPBOND_AUTO_FAIL_REASON                0x412  //!< TEST MODE (DO NOT USE) Pairing Fail reason when auto failing. Read/Write. size is uint8_t. Default is 0x05 (SMP_PAIRING_FAILED_NOT_SUPPORTED).
#define GAPBOND_KEYSIZE                         0x413  //!< Key Size used in pairing. Read/Write. size is uint8_t. Default is 16.
#define GAPBOND_AUTO_SYNC_WL                    0x414  //!< Clears the White List adds to it each unique address stored by bonds in NV. Read/Write. Size is uint8_t. Default is FALSE.
#define GAPBOND_BOND_COUNT                      0x415  //!< Gets the total number of bonds stored in NV. Read Only. Size is uint8_t. Default is 0 (no bonds).
#define GAPBOND_BOND_FAIL_ACTION                0x416  //!< Possible actions Central may take upon an unsuccessful bonding. Write Only. Size is uint8_t. Default is 0x02 (Terminate link upon unsuccessful bonding).
#define GAPBOND_ERASE_SINGLEBOND                0x417  //!< Erase a single bonded device. Write only. Must provide address type followed by device address.
#define GAPBOND_BOND_AUTO                       0x418  //!< Auto save bonds into FLASH. Write Only. size is uint8_t. Default is 1(enabled).
#define GAPBOND_BOND_UPDATE                     0x419  //!< Save current bonds into FLASH. Write Only. No Size.
#define GAPBOND_DISABLE_SINGLEBOND              0x41A  //!< Disable a single bonded device. Write only. Must provide address type followed by device address.
#define GAPBOND_ENABLE_SINGLEBOND               0x41B  //!< Ensable a single bonded device. Write only. Must provide address type followed by device address.
#define GAPBOND_DISABLE_ALLBONDS                0x41C  //!< Disable all of the bonded devices. Write Only. No Size.
#define GAPBOND_ENABLE_ALLBONDS                 0x41D  //!< Ensable all of the bonded devices. Write Only. No Size.
#define GAPBOND_ERASE_AUTO                      0x41E  //!< Auto erase all of the bonded devices when the maximum number is reached.Size is uint8_t. Default is 1(enabled).
#define GAPBOND_AUTO_SYNC_RL                    0x41F  //!< Clears the Resolving List adds to it each unique address stored by bonds in NV. Read/Write. Size is uint8_t. Default is FALSE.

// GAPBOND_PAIRING_MODE_DEFINES GAP Bond Manager Pairing Modes
#define GAPBOND_PAIRING_MODE_NO_PAIRING         0x00  //!< Pairing is not allowed
#define GAPBOND_PAIRING_MODE_WAIT_FOR_REQ       0x01  //!< Wait for a pairing request or slave security request
#define GAPBOND_PAIRING_MODE_INITIATE           0x02  //!< Don't wait, initiate a pairing request or slave security request

// GAPBOND_IO_CAP_DEFINES GAP Bond Manager I/O Capabilities
#define GAPBOND_IO_CAP_DISPLAY_ONLY             0x00  //!< Display Only Device
#define GAPBOND_IO_CAP_DISPLAY_YES_NO           0x01  //!< Display and Yes and No Capable
#define GAPBOND_IO_CAP_KEYBOARD_ONLY            0x02  //!< Keyboard Only
#define GAPBOND_IO_CAP_NO_INPUT_NO_OUTPUT       0x03  //!< No Display or Input Device
#define GAPBOND_IO_CAP_KEYBOARD_DISPLAY         0x04  //!< Both Keyboard and Display Capable

// GAPBOND_KEY_DIST_DEFINES GAP Bond Manager Key Distribution
#define GAPBOND_KEYDIST_SENCKEY                 0x01  //!< Slave Encryption Key
#define GAPBOND_KEYDIST_SIDKEY                  0x02  //!< Slave IRK and ID information
#define GAPBOND_KEYDIST_SSIGN                   0x04  //!< Slave CSRK
#define GAPBOND_KEYDIST_SLINK                   0x08  //!< Slave Link Key
#define GAPBOND_KEYDIST_MENCKEY                 0x10  //!< Master Encrypton Key
#define GAPBOND_KEYDIST_MIDKEY                  0x20  //!< Master IRK and ID information
#define GAPBOND_KEYDIST_MSIGN                   0x40  //!< Master CSRK
#define GAPBOND_KEYDIST_MLINK                   0x80  //!< Master Link Key

// GAPBOND_PAIRING_STATE_DEFINES GAP Bond Manager Pairing States
#define GAPBOND_PAIRING_STATE_STARTED           0x00  //!< Pairing started
#define GAPBOND_PAIRING_STATE_COMPLETE          0x01  //!< Pairing complete
#define GAPBOND_PAIRING_STATE_BONDED            0x02  //!< Devices bonded
#define GAPBOND_PAIRING_STATE_BOND_SAVED        0x03  //!< Bonding record saved in NV

// SMP_PAIRING_FAILED_DEFINES Pairing failure status values
#define SMP_PAIRING_FAILED_PASSKEY_ENTRY_FAILED 0x01 //!< The user input of the passkey failed, for example, the user cancelled the operation.
#define SMP_PAIRING_FAILED_OOB_NOT_AVAIL        0x02 //!< The OOB data is not available
#define SMP_PAIRING_FAILED_AUTH_REQ             0x03 //!< The pairing procedure can't be performed as authentication requirements can't be met due to IO capabilities of one or both devices
#define SMP_PAIRING_FAILED_CONFIRM_VALUE        0x04 //!< The confirm value doesn't match the calculated compare value
#define SMP_PAIRING_FAILED_NOT_SUPPORTED        0x05 //!< Pairing isn't supported by the device
#define SMP_PAIRING_FAILED_ENC_KEY_SIZE         0x06 //!< The resultant encryption key size is insufficient for the security requirements of this device.
#define SMP_PAIRING_FAILED_CMD_NOT_SUPPORTED    0x07 //!< The SMP command received is not supported on this device.
#define SMP_PAIRING_FAILED_UNSPECIFIED          0x08 //!< Pairing failed due to an unspecified reason
#define SMP_PAIRING_FAILED_REPEATED_ATTEMPTS    0x09 //!< Pairing or authentication procedure is disallowed because too little time has elapsed since the last pairing request or security request.

// GAPBOND_BONDING_FAILURE_DEFINES Bonding Failure Actions
#define GAPBOND_FAIL_NO_ACTION                  0x00 //!< Take no action upon unsuccessful bonding
#define GAPBOND_FAIL_INITIATE_PAIRING           0x01 //!< Initiate pairing upon unsuccessful bonding
#define GAPBOND_FAIL_TERMINATE_LINK             0x02 //!< Terminate link upon unsuccessful bonding
#define GAPBOND_FAIL_TERMINATE_ERASE_BONDS      0x03 //!< Terminate link and erase all existing bonds on device upon unsuccessful bonding

#define GAP_BOND_REC_ID_OFFSET                  0 //!< NV ID for the main bonding record
#define GAP_BOND_LOCAL_LTK_OFFSET               1 //!< NV ID for the bonding record's local LTK information
#define GAP_BOND_DEV_LTK_OFFSET                 2 //!< NV ID for the bonding records' device LTK information
#define GAP_BOND_DEV_IRK_OFFSET                 3 //!< NV ID for the bonding records' device IRK
#define GAP_BOND_DEV_CSRK_OFFSET                4 //!< NV ID for the bonding records' device CSRK
#define GAP_BOND_DEV_SIGN_COUNTER_OFFSET        5 //!< NV ID for the bonding records' device Sign Counter
#define GAP_BOND_REC_IDS                        6

// Bonding NV Items -   START  0x20   
#define BLE_NVID_GAP_BOND_START                 0x20  //!< Start of the GAP Bond Manager's NV IDs

// GATT Configuration NV Items - START  0x70 
#define BLE_NVID_GATT_CFG_START                 0x70  //!< Start of the GATT Configuration NV IDs

// RF BOND Configuration 
#define BLE_NVID_BOND_RF_START                  0x60  //!< Start of the RF BOND NV IDs

// Macros to calculate the index/offset in to NV space
#define calcNvID(Idx, offset)           (((((Idx) * GAP_BOND_REC_IDS) + (offset))) + BLE_NVID_GAP_BOND_START)
#define mainRecordNvID(bondIdx)         (calcNvID((bondIdx), GAP_BOND_REC_ID_OFFSET))
#define localLTKNvID(bondIdx)           (calcNvID((bondIdx), GAP_BOND_LOCAL_LTK_OFFSET))
#define devLTKNvID(bondIdx)             (calcNvID((bondIdx), GAP_BOND_DEV_LTK_OFFSET))
#define devIRKNvID(bondIdx)             (calcNvID((bondIdx), GAP_BOND_DEV_IRK_OFFSET))
#define devCSRKNvID(bondIdx)            (calcNvID((bondIdx), GAP_BOND_DEV_CSRK_OFFSET))
#define devSignCounterNvID(bondIdx)     (calcNvID((bondIdx), GAP_BOND_DEV_SIGN_COUNTER_OFFSET))
// Macros to calculate the GATT index/offset in to NV space
#define gattCfgNvID(Idx)                ((Idx) + BLE_NVID_GATT_CFG_START)

// Structure of NV data for the connected device's encryption information
typedef struct
{
    uint8_t LTK[KEYLEN];              // Long Term Key (LTK)
    uint16_t div;                     // LTK eDiv
    uint8_t rand[B_RANDOM_NUM_SIZE];  // LTK random number
    uint8_t keySize;                  // LTK key size
} gapBondLTK_t;

// Structure of NV data for the connected device's address information
typedef struct
{
    uint8_t publicAddr[B_ADDR_LEN];     // Master's address
    uint8_t reconnectAddr[B_ADDR_LEN];  // Privacy Reconnection Address
    uint16_t stateFlags; // State flags: SM_AUTH_STATE_AUTHENTICATED & SM_AUTH_STATE_BONDING
    uint8_t bondsToDelete;
} gapBondRec_t;

// Structure of NV data for the connected device's characteristic configuration
typedef struct
{
    uint16_t attrHandle;  // attribute handle
    uint8_t value;        // attribute value for this device
} gapBondCharCfg_t;

/*********************************************************************
 * TYPEDEFS
 */
typedef struct
{
    uint8_t srk[KEYLEN];   // Signature Resolving Key
    uint32_t signCounter;  // Sign Counter
} linkSec_t;

typedef struct
{
    uint8_t ltk[KEYLEN];             // Long Term Key
    uint16_t div;                    // Diversifier
    uint8_t rand[B_RANDOM_NUM_SIZE]; // random number
    uint8_t keySize;                 // LTK Key Size
    uint8_t gapBondInvalid;
} encParams_t;

typedef struct
{
    uint8_t taskID;            // Application that controls the link
    uint16_t connectionHandle; // Controller connection handle
    uint8_t stateFlags;        // LINK_CONNECTED, LINK_AUTHENTICATED...
    uint8_t addrType;          // Address type of connected device
    uint8_t addr[B_ADDR_LEN];  // Other Device's address
    uint8_t connRole;          // Connection formed as Master or Slave
    uint16_t connInterval;     // The connection's interval (n * 1.23 ms)
    uint16_t connLatency;
    uint16_t connTimeout;
    uint16_t MTU;              // The connection's MTU size
    linkSec_t sec;             // Connection Security related items
    encParams_t *pEncParams;   // pointer to LTK, ediv, rand. if needed.
    uint16_t smEvtID;
    void *pPairingParams;
    void *pAuthLink;
} linkDBItem_t;
// function pointer used to register for a status callback
typedef void (*pfnLinkDBCB_t)( uint16_t connectionHandle, uint8_t changeType );
// function pointer used to perform specialized link database searches
typedef void (*pfnPerformFuncCB_t)( linkDBItem_t *pLinkItem );

/**
 * Attribute Type format (2 or 16 octet UUID).
 */
typedef struct
{
    uint8_t len;                 //!< Length of UUID (2 or 16)
    uint8_t uuid[ATT_UUID_SIZE]; //!< 16 or 128 bit UUID
} attAttrType_t;

/**
 * Attribute Type format (2-octet Bluetooth UUID).
 */
typedef struct
{
    uint8_t len;                    //!< Length of UUID (2)
    uint8_t uuid[ATT_BT_UUID_SIZE]; //!< 16 bit UUID
} attAttrBtType_t;

/**
 * Error Response format.
 */
typedef struct
{
    uint8_t reqOpcode;  //!< Request that generated this error response
    uint16_t handle;    //!< Attribute handle that generated error response
    uint8_t errCode;    //!< Reason why the request has generated error response
} attErrorRsp_t;

/**
 * Exchange MTU Request format.
 */
typedef struct
{
    uint16_t clientRxMTU;  //!< Client receive MTU size
} attExchangeMTUReq_t;

/**
 * Exchange MTU Response format.
 */
typedef struct
{
    uint16_t serverRxMTU;  //!< Server receive MTU size
} attExchangeMTURsp_t;

/**
 * Find Information Request format.
 */
typedef struct
{
    uint16_t startHandle;  //!< First requested handle number (must be first field)
    uint16_t endHandle;    //!< Last requested handle number
} attFindInfoReq_t;

/**
 * Find Information Response format.
 */
typedef struct
{
    uint16_t numInfo;  //!< Number of attribute handle-UUID pairs found
    uint8_t format;    //!< Format of information data
    uint8_t *pInfo;    //!< Information data whose format is determined by format field (4 to ATT_MTU_SIZE-2)
} attFindInfoRsp_t;

/**
 * Find By Type Value Request format.
 */
typedef struct
{
    uint16_t startHandle;   //!< First requested handle number (must be first field)
    uint16_t endHandle;     //!< Last requested handle number
    attAttrBtType_t type;  //!< 2-octet UUID to find
    uint16_t len;           //!< Length of value
    uint8_t *pValue;        //!< Attribute value to find (0 to ATT_MTU_SIZE-7)
} attFindByTypeValueReq_t;

/**
 * Find By Type Value Response format.
 */
typedef struct
{
    uint16_t numInfo;      //!< Number of handles information found
    uint8_t *pHandlesInfo; //!< List of 1 or more handles information (4 to ATT_MTU_SIZE-1)
} attFindByTypeValueRsp_t;

/**
 * Read By Type Request format.
 */
typedef struct
{
    uint16_t startHandle;  //!< First requested handle number (must be first field)
    uint16_t endHandle;    //!< Last requested handle number
    attAttrType_t type;   //!< Requested type (2 or 16 octet UUID)
} attReadByTypeReq_t;

/**
 * Read By Type Response format.
 */
typedef struct
{
    uint16_t numPairs;   //!< Number of attribute handle-UUID pairs found
    uint16_t len;        //!< Size of each attribute handle-value pair
    uint8_t *pDataList;  //!< List of 1 or more attribute handle-value pairs (2 to ATT_MTU_SIZE-2)
} attReadByTypeRsp_t;

/**
 * Read Request format.
 */
typedef struct
{
    uint16_t handle;  //!< Handle of the attribute to be read (must be first field)
} attReadReq_t;

/**
 * Read Response format.
 */
typedef struct
{
    uint16_t len;     //!< Length of value
    uint8_t *pValue;  //!< Value of the attribute with the handle given (0 to ATT_MTU_SIZE-1)
} attReadRsp_t;

/**
 * Read Blob Req format.
 */
typedef struct
{
    uint16_t handle;  //!< Handle of the attribute to be read (must be first field)
    uint16_t offset;  //!< Offset of the first octet to be read
} attReadBlobReq_t;

/**
 * Read Blob Response format.
 */
typedef struct
{
    uint16_t len;    //!< Length of value
    uint8_t *pValue; //!< Part of the value of the attribute with the handle given (0 to ATT_MTU_SIZE-1)
} attReadBlobRsp_t;

/**
 * Read Multiple Request format.
 */
typedef struct
{
    uint8_t *pHandles; //!< Set of two or more attribute handles (4 to ATT_MTU_SIZE-1) - must be first field
    uint16_t numHandles; //!< Number of attribute handles
} attReadMultiReq_t;

/**
 * Read Multiple Response format.
 */
typedef struct
{
    uint16_t len;     //!< Length of values
    uint8_t *pValues; //!< Set of two or more values (0 to ATT_MTU_SIZE-1)
} attReadMultiRsp_t;

/**
 * Read By Group Type Request format.
 */
typedef struct
{
    uint16_t startHandle; //!< First requested handle number (must be first field)
    uint16_t endHandle;   //!< Last requested handle number
    attAttrType_t type;  //!< Requested group type (2 or 16 octet UUID)
} attReadByGrpTypeReq_t;

/**
 * Read By Group Type Response format.
 */
typedef struct
{
    uint16_t numGrps;   //!< Number of attribute handle, end group handle and value sets found
    uint16_t len;       //!< Length of each attribute handle, end group handle and value set
    uint8_t *pDataList; //!< List of 1 or more attribute handle, end group handle and value (4 to ATT_MTU_SIZE-2)
} attReadByGrpTypeRsp_t;

/**
 * Write Request format.
 */
typedef struct
{
    uint16_t handle;  //!< Handle of the attribute to be written (must be first field)
    uint16_t len;     //!< Length of value
    uint8_t *pValue;  //!< Value of the attribute to be written (0 to ATT_MTU_SIZE-3)
    uint8_t sig;     //!< Authentication Signature status (not included (0), valid (1), invalid (2))
    uint8_t cmd;     //!< Command Flag
} attWriteReq_t;

/**
 * Prepare Write Request format.
 */
typedef struct
{
    uint16_t handle;  //!< Handle of the attribute to be written (must be first field)
    uint16_t offset;  //!< Offset of the first octet to be written
    uint16_t len;     //!< Length of value
    uint8_t *pValue; //!< Part of the value of the attribute to be written (0 to ATT_MTU_SIZE-5) - must be allocated
} attPrepareWriteReq_t;

/**
 * Prepare Write Response format.
 */
typedef struct
{
    uint16_t handle; //!< Handle of the attribute that has been read
    uint16_t offset; //!< Offset of the first octet to be written
    uint16_t len;    //!< Length of value
    uint8_t *pValue; //!< Part of the value of the attribute to be written (0 to ATT_MTU_SIZE-5)
} attPrepareWriteRsp_t;

/**
 * Execute Write Request format.
 */
typedef struct
{
    uint8_t flags;  //!< 0x00 - cancel all prepared writes.
                  //!< 0x01 - immediately write all pending prepared values.
} attExecuteWriteReq_t;

/**
 * Handle Value Notification format.
 */
typedef struct
{
    uint16_t handle; //!< Handle of the attribute that has been changed (must be first field)
    uint16_t len;    //!< Length of value
    uint8_t *pValue; //!< Current value of the attribute (0 to ATT_MTU_SIZE-3)
} attHandleValueNoti_t;

/**
 * Handle Value Indication format.
 */
typedef struct
{
    uint16_t handle; //!< Handle of the attribute that has been changed (must be first field)
    uint16_t len;    //!< Length of value
    uint8_t *pValue; //!< Current value of the attribute (0 to ATT_MTU_SIZE-3)
} attHandleValueInd_t;

/**
 * ATT Flow Control Violated Event message format.  This message is sent to the
 * app by the local ATT Server or Client when a sequential ATT Request-Response
 * or Indication-Confirmation protocol flow control is violated for a connection.
 * All subsequent ATT Requests and Indications received by the local ATT Server
 * and Client respectively will be dropped.
 *
 * This message is to inform the app (that has registered with GAP by calling 
 * GAP_RegisterForMsgs()) in case it wants to drop the connection.
 */
typedef struct
{
    uint8_t opcode;        //!< opcode of message that caused flow control violation
    uint8_t pendingOpcode; //!< opcode of pending message
} attFlowCtrlViolatedEvt_t;

/**
 * ATT MTU Updated Event message format.  This message is sent to the app
 * by the local ATT Server or Client when the ATT MTU size is updated for a
 * connection. The default ATT MTU size is 23 octets.
 *
 * This message is to inform the app (that has registered with GAP by calling 
 * GAP_RegisterForMsgs()) about the new ATT MTU size negotiated for a connection.
 */
typedef struct
{
    uint16_t MTU; //!< new MTU size
} attMtuUpdatedEvt_t;

/**
 * ATT Message format. It's a union of all attribute protocol messages and
 * locally-generated events used between the attribute protocol and upper
 * layer profile/application.
 */
typedef union
{
    // Request messages
    attExchangeMTUReq_t exchangeMTUReq;           //!< ATT Exchange MTU Request
    attFindInfoReq_t findInfoReq;                 //!< ATT Find Information Request
    attFindByTypeValueReq_t findByTypeValueReq;   //!< ATT Find By Type Value Request
    attReadByTypeReq_t readByTypeReq;             //!< ATT Read By Type Request
    attReadReq_t readReq;                         //!< ATT Read Request
    attReadBlobReq_t readBlobReq;                 //!< ATT Read Blob Request
    attReadMultiReq_t readMultiReq;               //!< ATT Read Multiple Request
    attReadByGrpTypeReq_t readByGrpTypeReq;       //!< ATT Read By Group Type Request
    attWriteReq_t writeReq;                       //!< ATT Write Request
    attPrepareWriteReq_t prepareWriteReq;         //!< ATT Prepare Write Request
    attExecuteWriteReq_t executeWriteReq;         //!< ATT Execute Write Request

    // Response messages
    attErrorRsp_t errorRsp;                       //!< ATT Error Response
    attExchangeMTURsp_t exchangeMTURsp;           //!< ATT Exchange MTU Response
    attFindInfoRsp_t findInfoRsp;                 //!< ATT Find Information Response
    attFindByTypeValueRsp_t findByTypeValueRsp;   //!< ATT Find By Type Value Response
    attReadByTypeRsp_t readByTypeRsp;             //!< ATT Read By Type Response
    attReadRsp_t readRsp;                         //!< ATT Read Response
    attReadBlobRsp_t readBlobRsp;                 //!< ATT Read Blob Response
    attReadMultiRsp_t readMultiRsp;               //!< ATT Read Multiple Response
    attReadByGrpTypeRsp_t readByGrpTypeRsp;       //!< ATT Read By Group Type Response
    attPrepareWriteRsp_t prepareWriteRsp;         //!< ATT Prepare Write Response

    // Indication and Notification messages
    attHandleValueNoti_t handleValueNoti;         //!< ATT Handle Value Notification
    attHandleValueInd_t handleValueInd;           //!< ATT Handle Value Indication

    // Locally-generated event messages
    attFlowCtrlViolatedEvt_t flowCtrlEvt;         //!< ATT Flow Control Violated Event
    attMtuUpdatedEvt_t mtuEvt;                    //!< ATT MTU Updated Event
} attMsg_t;

/**
 * GATT Find By Type Value Request format.
 */
typedef struct
{
    uint16_t startHandle;  //!< First requested handle number (must be first field)
    uint16_t endHandle;    //!< Last requested handle number
    attAttrType_t value;   //!< Primary service UUID value (2 or 16 octets)
} gattFindByTypeValueReq_t;

/**
 * GATT Read By Type Request format.
 */
typedef struct
{
    uint8_t discCharsByUUID;  //!< Whether this is a GATT Discover Characteristics by UUID sub-procedure
    attReadByTypeReq_t req; //!< Read By Type Request
} gattReadByTypeReq_t;

/**
 * GATT Write Long Request format. Do not change the order of the members.
 */
typedef struct
{
    uint8_t reliable; //!< Whether reliable writes requested (always FALSE for Write Long)
    attPrepareWriteReq_t req;  //!< ATT Prepare Write Request
    uint16_t lastOffset; //!< Offset of last Prepare Write Request sent
} gattWriteLongReq_t;

/**
 * GATT Reliable Writes Request format. Do not change the order of the members.
 */
typedef struct
{
    uint8_t reliable; //!< Whether reliable writes requested (always TRUE for Reliable Writes)
    attPrepareWriteReq_t *pReqs; //!< Array of Prepare Write Requests (must be allocated)
    uint8_t numReqs;             //!< Number of Prepare Write Requests
    uint8_t index;               //!< Index of last Prepare Write Request sent
    uint8_t flags;               //!< 0x00 - cancel all prepared writes.
                                 //!< 0x01 - immediately write all pending prepared values.
} gattReliableWritesReq_t;

/**
 * GATT Message format. It's a union of all attribute protocol/profile messages
 * and locally-generated events used between the attribute protocol/profile and
 * upper layer application.
 */
typedef union
{
    // Request messages
    attExchangeMTUReq_t exchangeMTUReq;             //!< ATT Exchange MTU Request
    attFindInfoReq_t findInfoReq;                   //!< ATT Find Information Request
    attFindByTypeValueReq_t findByTypeValueReq;     //!< ATT Find By Type Value Request
    attReadByTypeReq_t readByTypeReq;               //!< ATT Read By Type Request
    attReadReq_t readReq;                           //!< ATT Read Request
    attReadBlobReq_t readBlobReq;                   //!< ATT Read Blob Request
    attReadMultiReq_t readMultiReq;                 //!< ATT Read Multiple Request
    attReadByGrpTypeReq_t readByGrpTypeReq;         //!< ATT Read By Group Type Request
    attWriteReq_t writeReq;                         //!< ATT Write Request
    attPrepareWriteReq_t prepareWriteReq;           //!< ATT Prepare Write Request
    attExecuteWriteReq_t executeWriteReq;           //!< ATT Execute Write Request
    gattFindByTypeValueReq_t gattFindByTypeValueReq;//!< GATT Find By Type Value Request
    gattReadByTypeReq_t gattReadByTypeReq;          //!< GATT Read By Type Request
    gattWriteLongReq_t gattWriteLongReq;            //!< GATT Long Write Request
    gattReliableWritesReq_t gattReliableWritesReq;  //!< GATT Reliable Writes Request

    // Response messages
    attErrorRsp_t errorRsp;                         //!< ATT Error Response
    attExchangeMTURsp_t exchangeMTURsp;             //!< ATT Exchange MTU Response
    attFindInfoRsp_t findInfoRsp;                   //!< ATT Find Information Response
    attFindByTypeValueRsp_t findByTypeValueRsp;     //!< ATT Find By Type Value Response
    attReadByTypeRsp_t readByTypeRsp;               //!< ATT Read By Type Response
    attReadRsp_t readRsp;                           //!< ATT Read Response
    attReadBlobRsp_t readBlobRsp;                   //!< ATT Read Blob Response
    attReadMultiRsp_t readMultiRsp;                 //!< ATT Read Multiple Response
    attReadByGrpTypeRsp_t readByGrpTypeRsp;         //!< ATT Read By Group Type Response
    attPrepareWriteRsp_t prepareWriteRsp;           //!< ATT Prepare Write Response

    // Indication and Notification messages
    attHandleValueNoti_t handleValueNoti;           //!< ATT Handle Value Notification
    attHandleValueInd_t handleValueInd;             //!< ATT Handle Value Indication

    // Locally-generated event messages
    attFlowCtrlViolatedEvt_t flowCtrlEvt;           //!< ATT Flow Control Violated Event
    attMtuUpdatedEvt_t mtuEvt;                      //!< ATT MTU Updated Event
} gattMsg_t;

/**
 * GATT tmos GATT_MSG_EVENT message format. This message is used to forward an
 * incoming attribute protocol/profile message up to upper layer application.
 */
typedef struct
{
    tmos_event_hdr_t hdr;  //!< GATT_MSG_EVENT and status
    uint16_t connHandle;   //!< Connection message was received on
    uint8_t method;        //!< Type of message
    gattMsg_t msg;         //!< Attribute protocol/profile message
} gattMsgEvent_t;

/**
 * GATT Attribute Type format.
 */
typedef struct
{
    uint8_t len;           //!< Length of UUID (2 or 16)
    const uint8_t *uuid;   //!< Pointer to UUID
} gattAttrType_t;

/**
 * GATT Attribute format.
 */
typedef struct attAttribute_t
{
    gattAttrType_t type;   //!< Attribute type (2 or 16 octet UUIDs)
    uint8_t permissions;   //!< Attribute permissions
    uint16_t handle;       //!< Attribute handle - assigned internally by attribute server
    uint8_t *pValue;       //!< Attribute value - encoding of the octet array is defined in
                           //!< the applicable profile. The maximum length of an attribute
                           //!< value shall be 512 octets.
} gattAttribute_t;

/**
 * GATT Service format.
 */
typedef struct
{
    uint16_t numAttrs;  //!< Number of attributes in attrs
    uint8_t encKeySize; //!< Minimum encryption key size required by service (7-16 bytes)

    /** Array of attribute records.
     *  note: The list must start with a Service attribute followed by
     *        all attributes associated with this Service attribute.
     */
    gattAttribute_t *attrs;
} gattService_t;

/**
 * @brief   Callback function prototype to read an attribute value.
 *
 * @note    blePending can be returned ONLY for the following
 *          read operations:
 *          - Read Request: ATT_READ_REQ
 *          - Read Blob Request: ATT_READ_BLOB_REQ
 *
 * @note    If blePending is returned then it's the responsibility of the application to respond to 
 *          ATT_READ_REQ and ATT_READ_BLOB_REQ message with ATT_READ_RSP and ATT_READ_BLOB_RSP 
 *          message respectively.
 *
 * @note    Payload 'pValue' used with ATT_READ_RSP and ATT_READ_BLOB_RSP must be allocated using GATT_bm_alloc().
 *
 * @param   connHandle - connection request was received on
 * @param   pAttr - pointer to attribute
 * @param   pValue - pointer to data to be read (to be returned)
 * @param   pLen - length of data (to be returned)
 * @param   offset - offset of the first octet to be read
 * @param   maxLen - maximum length of data to be read
 * @param   method - type of read message
 *
 * @return  SUCCESS: Read was successfully.<BR>
 *          blePending: A response is pending for this client.<BR>
 *          Error, otherwise: ref ATT_ERR_CODE_DEFINES.<BR>
 */
typedef uint8_t (*pfnGATTReadAttrCB_t)( uint16_t connHandle, gattAttribute_t *pAttr, uint8_t *pValue,
    uint16_t *pLen, uint16_t offset, uint16_t maxLen, uint8_t method );
    
/**
 * @brief   Callback function prototype to write an attribute value.
 *
 * @note    blePending can be returned ONLY for the following
 *          write operations:
 *          - Write Request: ATT_WRITE_REQ
 *          - Write Command: ATT_WRITE_CMD
 *          - Write Long: ATT_EXECUTE_WRITE_REQ
 *          - Reliable Writes: Multiple ATT_PREPARE_WRITE_REQ followed by one final ATT_EXECUTE_WRITE_REQ
 *
 * @note    If blePending is returned then it's the responsibility of the application to 1) respond to 
 *          ATT_WRITE_REQ and ATT_EXECUTE_WRITE_REQ message with ATT_WRITE_RSP and ATT_EXECUTE_WRITE_RSP
 *          message respectively, and 2) free each request payload 'pValue' using BM_free().
 *
 * @note    Write Command (ATT_WRITE_CMD) does NOT require a response message.
 *
 * @param   connHandle - connection request was received on
 * @param   pAttr - pointer to attribute
 * @param   pValue - pointer to data to be written
 * @param   pLen - length of data
 * @param   offset - offset of the first octet to be written
 * @param   method - type of write message
 *
 * @return  SUCCESS: Write was successfully.<BR>
 *          blePending: A response is pending for this client.<BR>
 *          Error, otherwise: ref ATT_ERR_CODE_DEFINES.<BR>
 */
typedef uint8_t (*pfnGATTWriteAttrCB_t)( uint16_t connHandle, gattAttribute_t *pAttr, uint8_t *pValue,
    uint16_t len, uint16_t offset, uint8_t method );
    
/**
 * @brief   Callback function prototype to authorize a Read or Write operation
 *          on a given attribute.
 *
 * @param   connHandle - connection request was received on
 * @param   pAttr - pointer to attribute
 * @param   opcode - request opcode (ATT_READ_REQ or ATT_WRITE_REQ)
 *
 * @return  SUCCESS: Operation authorized.<BR>
 *          ATT_ERR_INSUFFICIENT_AUTHOR: Authorization required.<BR>
 */
typedef bStatus_t (*pfnGATTAuthorizeAttrCB_t)( uint16_t connHandle, gattAttribute_t *pAttr,
    uint8_t opcode );

/**
 * GATT Structure for Client Characteristic Configuration.
 */
typedef struct
{
    uint16_t connHandle; //!< Client connection handle
    uint8_t value;      //!< Characteristic configuration value for this client
} gattCharCfg_t;

/**
 * GATT Structure for service callback functions - must be setup by the application
 * and used when GATTServApp_RegisterService() is called.
 */
typedef struct
{
    pfnGATTReadAttrCB_t pfnReadAttrCB;      //!< Read callback function pointer
    pfnGATTWriteAttrCB_t pfnWriteAttrCB;    //!< Write callback function pointer
    pfnGATTAuthorizeAttrCB_t pfnAuthorizeAttrCB; //!< Authorization callback function pointer
} gattServiceCBs_t;

/*************************************gap**************************************/
/**
 * GAP event header format.
 */
typedef struct
{
    tmos_event_hdr_t hdr;             //!< GAP_MSG_EVENT and status
    uint8_t opcode;                   //!< GAP type of command. Ref: @ref GAP_MSG_EVENT_DEFINES
} gapEventHdr_t;

/**
 * GAP_DEVICE_INIT_DONE_EVENT message format.  This message is sent to the
 * app when the Device Initialization is done [initiated by calling
 * GAP_DeviceInit()].
 */
typedef struct
{
    tmos_event_hdr_t hdr;              //!< GAP_MSG_EVENT and status
    uint8_t opcode;                    //!< GAP_DEVICE_INIT_DONE_EVENT
    uint8_t devAddr[B_ADDR_LEN];       //!< Device's BD_ADDR
    uint16_t dataPktLen;               //!< HC_LE_Data_Packet_Length
    uint8_t numDataPkts;               //!< HC_Total_Num_LE_Data_Packets
} gapDeviceInitDoneEvent_t;

/**
 * GAP_SIGNATURE_UPDATED_EVENT message format.  This message is sent to the
 * app when the signature counter has changed.  This message is to inform the
 * application in case it wants to save it to be restored on reboot or reconnect.
 * This message is sent to update a connection's signature counter and to update
 * this device's signature counter.  If devAddr == BD_ADDR, then this message pertains
 * to this device.
 */
typedef struct
{
    tmos_event_hdr_t hdr;             //!< GAP_MSG_EVENT and status
    uint8_t opcode;                   //!< GAP_SIGNATURE_UPDATED_EVENT
    uint8_t addrType;                 //!< Device's address type for devAddr
    uint8_t devAddr[B_ADDR_LEN];      //!< Device's BD_ADDR, could be own address
    uint32_t signCounter;             //!< new Signed Counter
} gapSignUpdateEvent_t;

/**
 * GAP_DEVICE_INFO_EVENT message format.  This message is sent to the
 * app during a Device Discovery Request, when a new advertisement or scan
 * response is received.
 */
typedef struct
{
    tmos_event_hdr_t hdr;             //!< GAP_MSG_EVENT and status
    uint8_t opcode;                   //!< GAP_DEVICE_INFO_EVENT
    uint8_t eventType;                //!< Advertisement Type: @ref GAP_ADVERTISEMENT_REPORT_TYPE_DEFINES
    uint8_t addrType;                 //!< address type: @ref GAP_ADDR_TYPE_DEFINES
    uint8_t addr[B_ADDR_LEN];         //!< Address of the advertisement or SCAN_RSP
    int8_t rssi;                      //!< Advertisement or SCAN_RSP RSSI
    uint8_t dataLen;                  //!< Length (in bytes) of the data field (evtData)
    uint8_t *pEvtData;                //!< Data field of advertisement or SCAN_RSP
} gapDeviceInfoEvent_t;

/**
 * GAP_DIRECT_DEVICE_INFO_EVENT message format.  This message is sent to the
 * app during a Device Discovery Request, when a new advertisement or scan
 * response is received.
 */
typedef struct
{
    tmos_event_hdr_t hdr;             //!< GAP_MSG_EVENT and status
    uint8_t opcode;                   //!< GAP_DIRECT_DEVICE_INFO_EVENT
    uint8_t eventType;                //!< Advertisement Type: @ref GAP_ADVERTISEMENT_REPORT_TYPE_DEFINES
    uint8_t addrType;                 //!< address type: @ref GAP_ADDR_TYPE_DEFINES
    uint8_t addr[B_ADDR_LEN];         //!< Address of the advertisement or SCAN_RSP
    uint8_t directAddrType;           //!< public or random address type
    uint8_t directAddr[B_ADDR_LEN];   //!< device address
    int8_t rssi;                      //!< Advertisement or SCAN_RSP RSSI
} gapDirectDeviceInfoEvent_t;

/**
 * GAP_EXT_ADV_DEVICE_INFO_EVENT message format.  This message is sent to the
 * app during a Device Discovery Request, when a new advertisement or scan
 * response is received.
 */
typedef struct
{
    tmos_event_hdr_t hdr;              //!< GAP_MSG_EVENT and status
    uint8_t opcode;                    //!< GAP_EXT_ADV_DEVICE_INFO_EVENT
    uint8_t eventType;                 //!< Advertisement Type: @ref GAP_ADVERTISEMENT_REPORT_TYPE_DEFINES
    uint8_t addrType;                  //!< address type: @ref GAP_ADDR_TYPE_DEFINES
    uint8_t addr[B_ADDR_LEN];          //!< Address of the advertisement or SCAN_RSP
    uint8_t primaryPHY;                //!< Advertiser PHY on the primary advertising channel
    uint8_t secondaryPHY;              //!< Advertiser PHY on the secondary advertising channel
    uint8_t advertisingSID;            //!< Value of the Advertising SID subfield in the ADI field of the PDU
    uint8_t txPower;                   //!< Advertisement or SCAN_RSP power
    int8_t rssi;                       //!< Advertisement or SCAN_RSP RSSI
    uint16_t periodicAdvInterval;      //!< the interval of periodic advertising
    uint8_t directAddressType;         //!< public or random address type
    uint8_t directAddress[B_ADDR_LEN]; //!< device address
    uint8_t dataLen;                   //!< Length (in bytes) of the data field (evtData)
    uint8_t *pEvtData;                 //!< Data field of advertisement or SCAN_RSP
} gapExtAdvDeviceInfoEvent_t;

/**
 * Type of device discovery (Scan) to perform.
 */
typedef struct
{
    uint8_t taskID;       //!< Requesting App's Task ID, used to return results
    uint8_t mode;         //!< Discovery Mode: @ref GAP_DEVDISC_MODE_DEFINES
    uint8_t activeScan;   //!< TRUE for active scanning
    uint8_t whiteList;    //!< TRUE to only allow advertisements from devices in the white list.
} gapDevDiscReq_t;

/**
 * Type of device.
 */
typedef struct
{
    uint8_t eventType; //!< Indicates advertising event type used by the advertiser: @ref GAP_ADVERTISEMENT_REPORT_TYPE_DEFINES
    uint8_t addrType;         //!< Address Type: @ref GAP_ADDR_TYPE_DEFINES
    uint8_t addr[B_ADDR_LEN]; //!< Device's Address
} gapDevRec_t;

/**
 * GAP_DEVICE_DISCOVERY_EVENT message format. This message is sent to the
 * Application after a scan is performed.
 */
typedef struct
{
    tmos_event_hdr_t hdr;    //!< GAP_MSG_EVENT and status
    uint8_t opcode;          //!< GAP_DEVICE_DISCOVERY_EVENT
    uint8_t numDevs;         //!< Number of devices found during scan
    gapDevRec_t *pDevList;   //!< array of device records
} gapDevDiscEvent_t;

/**
 * GAP_MAKE_DISCOVERABLE_DONE_EVENT message format.  This message is sent to the
 * app when the Advertise config is complete.
 */
typedef struct
{
    tmos_event_hdr_t hdr; //!< GAP_MSG_EVENT and status
    uint8_t opcode; //!< GAP_MAKE_DISCOVERABLE_DONE_EVENT
} gapMakeDiscoverableRspEvent_t;

/**
 * GAP_END_DISCOVERABLE_DONE_EVENT message format.  This message is sent to the
 * app when the Advertising has stopped.
 */
typedef struct
{
    tmos_event_hdr_t hdr; //!< GAP_MSG_EVENT and status
    uint8_t opcode; //!< GAP_END_DISCOVERABLE_DONE_EVENT
} gapEndDiscoverableRspEvent_t;

/**
 * GAP_PERIODIC_ADVERTISING_DONE_EVENT message format.  This message is sent to the
 * app when the Periodic Advertising config is complete.
 */
typedef struct
{
    tmos_event_hdr_t hdr; //!< GAP_MSG_EVENT and status
    uint8_t opcode; //!< GAP_PERIODIC_ADVERTISING_DONE_EVENT
} gapMakePeriodicRspEvent_t;

/**
 * GAP_END_PERIODIC_ADV_DONE_EVENT message format.  This message is sent to the
 * app when the Periodic Advertising disable is complete.
 */
typedef struct
{
    tmos_event_hdr_t hdr; //!< GAP_MSG_EVENT and status
    uint8_t opcode; //!< GAP_END_PERIODIC_ADV_DONE_EVENT
} gapEndPeriodicRspEvent_t;

/**
 * GAP_SYNC_ESTABLISHED_EVENT message format.  This message is sent to the
 * app when the Periodic Advertising Sync Establish is complete.
 */
typedef struct
{
    tmos_event_hdr_t hdr;        //!< GAP_MSG_EVENT and status
    uint8_t opcode;              //!< GAP_SYNC_ESTABLISHED_EVENT
    uint8_t status;              //!< Periodic advertising sync status
    uint16_t syncHandle;         //!< Identifying the periodic advertising train
    uint8_t advertisingSID;      //!< Value of the Advertising SID subfield in the ADI field of the PDU
    uint8_t devAddrType;         //!< Device address type: @ref GAP_ADDR_TYPE_DEFINES
    uint8_t devAddr[B_ADDR_LEN]; //!< Device address of sync
    uint8_t advertisingPHY;      //!< Advertiser PHY
    uint16_t periodicInterval;   //!< Periodic advertising interval
    uint8_t clockAccuracy;       //!< Clock Accuracy
} gapSyncEstablishedEvent_t;

/**
 * GAP_PERIODIC_ADV_DEVICE_INFO_EVENT message format.  This message is sent to the
 * app during Periodic Advertising Sync, when received a Periodic Advertising packet
 */
typedef struct
{
    tmos_event_hdr_t hdr;         //!< GAP_MSG_EVENT and status
    uint8_t opcode;               //!< GAP_PERIODIC_ADV_DEVICE_INFO_EVENT
    uint16_t syncHandle;          //!< Identifying the periodic advertising train
    int8_t txPower;               //!< Periodic advertising tx power,Units: dBm
    int8_t rssi;                  //!< Periodic advertising rssi,Units: dBm
    uint8_t unUsed;
    uint8_t dataStatus;           //!< Data complete
    uint8_t dataLength;           //!< Length (in bytes) of the data field (evtData)
    uint8_t *pEvtData;            //!< Data field of periodic advertising data
} gapPeriodicAdvDeviceInfoEvent_t;

/**
 * GAP_SYNC_LOST_EVENT message format.  This message is sent to the
 * app when the Periodic Advertising Sync timeout period.
 */
typedef struct
{
    tmos_event_hdr_t hdr;          //!< GAP_MSG_EVENT and status
    uint8_t opcode;                //!< GAP_SYNC_LOST_EVENT
    uint16_t syncHandle;           //!< Identifying the periodic advertising train
} gapSyncLostEvent_t;

/**
 * GAP_SCAN_REQUEST_EVENT message format.  This message is sent to the
 * app when the advertiser receives a SCAN_REQ PDU or an AUX_SCAN_REQ PDU
 */
typedef struct
{
    tmos_event_hdr_t hdr;           //!< GAP_MSG_EVENT and status
    uint8_t opcode;                 //!< GAP_SCAN_REQUEST_EVENT
    uint8_t advHandle;              //!< identifying the periodic advertising train
    uint8_t scannerAddrType;        //!< the type of the address
    uint8_t scannerAddr[B_ADDR_LEN];//!< the address of scanner device
} gapScanReqReseiveEvent_t;


/**
 * GAP_ADV_DATA_UPDATE_DONE_EVENT message format.  This message is sent to the
 * app when Advertising Data Update is complete.
 */
typedef struct
{
    tmos_event_hdr_t hdr;    //!< GAP_MSG_EVENT and status
    uint8_t opcode;          //!< GAP_ADV_DATA_UPDATE_DONE_EVENT
    uint8_t adType;          //!< TRUE if advertising data, FALSE if SCAN_RSP
} gapAdvDataUpdateEvent_t;

/**
 * GAP_LINK_ESTABLISHED_EVENT message format.  This message is sent to the app
 * when the link request is complete.<BR>
 * <BR>
 * For an Observer, this message is sent to complete the Establish Link Request.<BR>
 * For a Peripheral, this message is sent to indicate that a link has been created.
 */
typedef struct
{
    tmos_event_hdr_t hdr;        //!< GAP_MSG_EVENT and status
    uint8_t opcode;              //!< GAP_LINK_ESTABLISHED_EVENT
    uint8_t devAddrType;         //!< Device address type: @ref GAP_ADDR_TYPE_DEFINES
    uint8_t devAddr[B_ADDR_LEN]; //!< Device address of link
    uint16_t connectionHandle;   //!< Connection Handle from controller used to ref the device
    uint8_t connRole;            //!< Connection formed as Master or Slave
    uint16_t connInterval;       //!< Connection Interval
    uint16_t connLatency;        //!< Connection Latency
    uint16_t connTimeout;        //!< Connection Timeout
    uint8_t clockAccuracy;       //!< Clock Accuracy
} gapEstLinkReqEvent_t;

/**
 * GAP_LINK_PARAM_UPDATE_EVENT message format.  This message is sent to the app
 * when the connection parameters update request is complete.
 */
typedef struct
{
    tmos_event_hdr_t hdr;       //!< GAP_MSG_EVENT and status
    uint8_t opcode;             //!< GAP_LINK_PARAM_UPDATE_EVENT
    uint8_t status;             //!< bStatus_t
    uint16_t connectionHandle;  //!< Connection handle of the update
    uint16_t connInterval;      //!< Requested connection interval
    uint16_t connLatency;       //!< Requested connection latency
    uint16_t connTimeout;       //!< Requested connection timeout
} gapLinkUpdateEvent_t;

/**
 * GAP_LINK_TERMINATED_EVENT message format.  This message is sent to the
 * app when a link to a device is terminated.
 */
typedef struct
{
    tmos_event_hdr_t hdr;      //!< GAP_MSG_EVENT and status
    uint8_t opcode;            //!< GAP_LINK_TERMINATED_EVENT
    uint16_t connectionHandle; //!< connection Handle
    uint8_t reason;            //!< termination reason from LL
    uint8_t connRole;
} gapTerminateLinkEvent_t;

/**
 * GAP_PHY_UPDATE_EVENT message format.  This message is sent to the app(GAP_MSG_EVENT)
 * when the PHY update request is complete.
 */
typedef struct
{
    tmos_event_hdr_t hdr;        //!< GAP_MSG_EVENT and status
    uint8_t opcode;              //!< GAP_PHY_UPDATE_EVENT
    uint8_t status;              //!< bStatus_t
    uint16_t connectionHandle;   //!< Connection handle of the update
    uint8_t connTxPHYS;          //!< tx phy(GAP_PHY_VAL_TYPE)
    uint8_t connRxPHYS;          //!< rx phy(GAP_PHY_VAL_TYPE)
} gapPhyUpdateEvent_t;

/**
 * GAP_PASSKEY_NEEDED_EVENT message format.  This message is sent to the
 * app when a Passkey is needed from the app's user interface.
 */
typedef struct
{
    tmos_event_hdr_t hdr;           //!< GAP_MSG_EVENT and status
    uint8_t opcode;                 //!< GAP_PASSKEY_NEEDED_EVENT
    uint8_t deviceAddr[B_ADDR_LEN]; //!< address of device to pair with, and could be either public or random.
    uint16_t connectionHandle;      //!< Connection handle
    uint8_t uiInputs;               //!< Pairing User Interface Inputs - Ask user to input passcode
    uint8_t uiOutputs;              //!< Pairing User Interface Outputs - Display passcode
} gapPasskeyNeededEvent_t;

/**
 * Passcode Callback Function
 */
typedef void (*pfnPasscodeCB_t)( uint8_t *deviceAddr, //!< address of device to pair with, and could be either public or random.
    uint16_t connectionHandle,      //!< Connection handle
    uint8_t uiInputs,              //!< Pairing User Interface Inputs - Ask user to input passcode
    uint8_t uiOutputs              //!< Pairing User Interface Outputs - Display passcode
    );

/**
 * Pairing State Callback Function
 */
typedef void (*pfnPairStateCB_t)( uint16_t connectionHandle,              //!< Connection handle
    uint8_t state,                         //!< Pairing state @ref GAPBOND_PAIRING_STATE_DEFINES
    uint8_t status                         //!< Pairing status
    );

/**
 * Callback Registration Structure
 */
typedef struct
{
    pfnPasscodeCB_t passcodeCB;     //!< Passcode callback
    pfnPairStateCB_t pairStateCB;   //!< Pairing state callback
} gapBondCBs_t;

/**
 * gapRole_States_t defined
 */
typedef unsigned long gapRole_States_t;

// gapRole_States_t @ 4b'[3-0]-advertising states
#define GAPROLE_STATE_ADV_MASK             (0xF)    //!< advertising states mask
#define GAPROLE_STATE_ADV_SHIFT            (0x0)    //!< advertising states shift
#define GAPROLE_INIT                        0       //!< Waiting to be started
#define GAPROLE_STARTED                     1       //!< Started but not advertising
#define GAPROLE_ADVERTISING                 2       //!< Currently Advertising
#define GAPROLE_WAITING                     3       //!< Device is started but not advertising, is in waiting period before advertising again
#define GAPROLE_CONNECTED                   4       //!< In a connection
#define GAPROLE_CONNECTED_ADV               5       //!< In a connection + advertising
#define GAPROLE_ERROR                       6       //!< Error occurred - invalid state

// gapRole_States_t @ 4b'[7-4]-Periodic advertising states
// Periodic advertising Enable,only effective when GAP_ADTYPE_EXT_NONCONN_NONSCAN_UNDIRECT advertising event enable
#define GAPROLE_STATE_PERIODIC_MASK         (0xF0)  //!< Periodic advertising states mask
#define GAPROLE_STATE_PERIODIC_SHIFT        (4)     //!< Periodic advertising states shift
#define GAPROLE_PERIODIC_INVALID            (0<<4)  //!< Periodic advertising Waiting to be started
#define GAPROLE_PERIODIC_ENABLE             (1<<4)  //!< Periodic advertising Enable
#define GAPROLE_PERIODIC_WAIT               (2<<4)  //!< Periodic advertising is started but disable
#define GAPROLE_PERIODIC_ERROR              (3<<4)  //!< Periodic advertising error occurred

// gapRole_States_t @ 16b'[23-8]- Reserved for future use

// gapRole_States_t @ 8b'[31-24] - indicates which fields change
#define GAPROLE_PERIODIC_STATE_VALID        (1<<24) //!< indicates periodic advertising states change

/**
 * gapRole Event Structure
 */
typedef union
{
    gapEventHdr_t gap;                     //!< GAP_MSG_EVENT and status.
    gapDeviceInitDoneEvent_t initDone;     //!< GAP initialization done.
    gapDeviceInfoEvent_t deviceInfo;       //!< Discovery device information event structure.
    gapDirectDeviceInfoEvent_t deviceDirectInfo; //!< Discovery direct device information event structure.
    gapPeriodicAdvDeviceInfoEvent_t devicePeriodicInfo; //!< Discovery periodic device information event structure.
    gapExtAdvDeviceInfoEvent_t deviceExtAdvInfo; //!< Discovery extend advertising device information event structure.
    gapDevDiscEvent_t discCmpl;            //!< Discovery complete event structure.
    gapSyncEstablishedEvent_t syncEstEvt;  //!< sync established event structure.
    gapSyncLostEvent_t syncLostEvt;        //!< sync lost event structure.
    gapScanReqReseiveEvent_t scanReqEvt;   //!< Scan_Request_Received event structure.

    gapEstLinkReqEvent_t linkCmpl;         //!< Link complete event structure.
    gapLinkUpdateEvent_t linkUpdate;       //!< Link update event structure.
    gapTerminateLinkEvent_t linkTerminate; //!< Link terminated event structure.
    gapPhyUpdateEvent_t linkPhyUpdate;     //!< Link phy update event structure.
} gapRoleEvent_t;

/**
 * Type of device.
 */
typedef struct
{
    uint8_t eventType; //!< Indicates advertising event type used by the advertiser: @ref GAP_ADVERTISEMENT_REPORT_TYPE_DEFINES
    uint8_t addrType;         //!< Scan Address Type: @ref GAP_ADDR_TYPE_DEFINES
    uint8_t addr[B_ADDR_LEN]; //!< Device's Address
    int8_t rssi;
} gapScanRec_t;

/**
 * Type of GAPRole_CreateSync command parameters.
 */
typedef struct
{
    uint8_t options;
    /*  bit0: used to determine whether the Periodic  Advertiser List is used
     0: Use the Advertising_SID, Advertisier_Address_Type, and Advertiser_Address parameters to determine which advertiser to listen to.
     1: Use the Periodic Advertiser List to determine which advertiser to listen to.
     bit1: whether GAP_PERIODIC_ADV_DEVICE_INFO_EVENT events for this periodic advertising train are initially enabled or disabled.
     0: Reporting initially enabled
     1: Reporting initially disabled  */
    uint8_t advertising_SID;  //!< if used, specifies the value that must match the Advertising SID
    uint8_t addrType;         //!< Scan Address Type: @ref GAP_ADDR_TYPE_DEFINES
    uint8_t addr[B_ADDR_LEN]; //!< Device's Address
    uint16_t skip;            //!< the maximum number of consecutive periodic advertising events that the receiver may skip after
                              //!< successfully receiving a periodic advertising packet.Range: 0x0000 to 0x01F3
    uint16_t syncTimeout;     //!< the maximum permitted time between successful receives. If this time is exceeded, synchronization is lost.
                              //!< Time = N*10 ms.Range: 0x000A to 0x4000
    uint8_t syncCTEType;      //!< specifies whether to only synchronize to periodic advertising with certain types of Constant Tone Extension
                              //!< (a value of 0 indicates that the presence or absence of a Constant Tone Extension is irrelevant).
} gapCreateSync_t;

/**
 * Type of GAPRole_SetPathLossReporting command parameters.
 */
typedef struct
{
    uint16_t connHandle;      //!< Used to identify the Connection handle
    int8_t highThreshold;     //!< High threshold for the path loss.Units: dB
    int8_t highHysteresis;    //!< Hysteresis value for the high threshold.Units: dB
    int8_t lowThreshold;      //!< High threshold for the path loss.Units: dB
    int8_t lowHysteresis;     //!< Hysteresis value for the high threshold.Units: dB
    uint16_t minTimeSpent;    //!< Minimum time in number of connection events to be observed
                              //!< once the path crosses the threshold before an event is generated.
    uint8_t enable;           //!< 0x00:Reporting disabled 0x01:Reporting enabled
} gapRoleSetPathLossReporting_t;

typedef struct
{
    uint16_t connHandle;      //!< Used to identify the Connection handle
    int8_t lowRxThreshold;    //!< High threshold for the peer power levels.Units: dB
    int8_t highRxThreshold;   //!< High threshold for the peer power levels.Units: dB
    int8_t minTxPower;        //!< Minimum transmit power level.Units: dB
    int8_t maxTxPower;        //!< Maximum transmit power level.Units: dB
} gapRolePowerlevelManagement_t;

/**
 * Callback when the device has been started.  Callback event to
 * the Notify of a state change.
 */
typedef void (*gapRolesBroadcasterStateNotify_t)( gapRole_States_t newState );

typedef void (*gapRolesScanReqRecv_t)( gapScanRec_t * pEvent );

typedef struct
{
    gapRolesBroadcasterStateNotify_t pfnStateChange; //!< Whenever the device changes state
    gapRolesScanReqRecv_t pfnScanRecv;
} gapRolesBroadcasterCBs_t;

/**
 * Observer Event Callback Function
 */
typedef void (*pfnGapObserverRoleEventCB_t)( gapRoleEvent_t *pEvent //!< Pointer to event structure.
    );

/**
 * Observer Callback Structure
 */
typedef struct
{
    pfnGapObserverRoleEventCB_t eventCB;  //!< Event callback.
} gapRoleObserverCB_t;

/**
 * Callback when the device has read an new RSSI value during a connection.
 */
typedef void (*gapRolesRssiRead_t)( uint16_t connHandle, int8_t newRSSI );

/**
 * Callback when the device has been started.  Callback event to
 * the Notify of a state change.
 */
typedef void (*gapRolesStateNotify_t)( gapRole_States_t newState, gapRoleEvent_t * pEvent );

/**
 * Callback when the connection parameteres are updated.
 */
typedef void (*gapRolesParamUpdateCB_t)( uint16_t connHandle, uint16_t connInterval,
    uint16_t connSlaveLatency, uint16_t connTimeout );

/**
 * Callback structure - must be setup by the application and used when gapRoles_StartDevice() is called.
 */
typedef struct
{
    gapRolesStateNotify_t pfnStateChange; //!< Whenever the device changes state
    gapRolesRssiRead_t pfnRssiRead; //!< When a valid RSSI is read from controller
    gapRolesParamUpdateCB_t pfnParamUpdate; //!< When the connection parameteres are updated
} gapRolesCBs_t;

/**
 * Central Event Callback Function
 */
typedef void (*pfnGapCentralRoleEventCB_t)( gapRoleEvent_t *pEvent ); //!< Pointer to event structure.
/**
 * HCI Data Length Change Event Callback Function
 */
typedef void (*pfnHciDataLenChangeEvCB_t)( uint16_t connHandle, uint16_t maxTxOctets,
    uint16_t maxRxOctets );

/**
 * Central Callback Structure
 */
typedef struct
{
    gapRolesRssiRead_t rssiCB;  //!< RSSI callback.
    pfnGapCentralRoleEventCB_t eventCB;  //!< Event callback.
    pfnHciDataLenChangeEvCB_t ChangCB;  //!< Length Change Event Callback .
} gapCentralRoleCB_t; // gapCentralRoleCB_t

/* RF-PHY define */

/*
 * RF_ROLE_STATUS_TYPE pfnRFStatusCB_t state defined
 */
// TX_MODE call RF_Tx
#define  TX_MODE_TX_FINISH            0x01  //!< basic or auto tx mode sends data successfully
                                            //!< if it is in basic mode,it will enter idle state;
                                            //!< if it is in auto mode,it will wait for receiving
#define  TX_MODE_TX_FAIL              0x11  //!< basic or auto tx mode fail to send data and enter idle state
#define  TX_MODE_TX_TIMEOUT TX_MODE_TX_FAIL //!< time of data transmission
#define  TX_MODE_RX_DATA              0x02  //!< auto tx mode receive data(ack) and enter idle state
#define  TX_MODE_RX_TIMEOUT           0x12  //!< auto tx mode receive timeout and enter idle state
#define  TX_MODE_HOP_SHUT             0x22  

// RX_MODE call RF_Rx
#define  RX_MODE_RX_DATA              0x03  //!< basic or auto rx mode receive data
                                            //!< if it is in basic mode,it will enter idle state;
                                            //!< if it is in auto mode,it will judge whether the type matches;
                                            //!< if it matches,it will send data(ack),otherwise(rsr=2), it will restart receiving
#define  RX_MODE_TX_FINISH            0x04  //!< auto rx mode sends data(ack) successfully and enters idle state
#define  RX_MODE_TX_FAIL              0x14  //!< auto rx mode fail to send data and enter idle state
#define  RX_MODE_TX_TIMEOUT RX_MODE_TX_FAIL //!< time of data transmission
#define  RX_MODE_HOP_SHUT             0x24  

// LLE_MODE_TYPE
#define  LLE_MODE_BASIC               (0)   //!< basic mode, enter idle state after sending or receive
#define  LLE_MODE_AUTO                (1)   //!< auto mode, auto swtich to the receiving status after sending and the sending status after receiving

#define  LLE_MODE_PHY_MODE_MASK       (0x30)
#define  LLE_MODE_PHY_1M              (0<<4)
#define  LLE_MODE_PHY_2M              (1<<4)
#define  LLE_MODE_PHY_CODED_S8        (2<<4)
#define  LLE_MODE_PHY_CODED_S2        (3<<4)

#define  LLE_MODE_EX_CHANNEL          (1<<6)

#define  LLE_MODE_NON_RSSI            (1<<7)

/**
 * RFRole Event Callback Function
 */
typedef void (*pfnRFStatusCB_t)( uint8_t sta, uint8_t rsr, uint8_t *rxBuf );
// sta - current status@ref RF_ROLE_STATUS_TYPE
// rsr - receive status: bit0- crc check result,bit1- type matching result
// rxBuf - receive data buffer

typedef struct tag_rf_config
{
    uint8_t LLEMode;                 //!< BIT0   0=LLE_MODE_BASIC, 1=LLE_MODE_AUTO
                                     //!< BIT4-5 00-1M  01-2M  10-coded(S8) 11-coded(S2)
                                     //!< BIT6   0=data channel(0-39)
                                     //!<        1=rf frequency (2400000kHz-2483500kHz)
                                     //!< BIT7   0=the first byte of the receive buffer is rssi
                                     //!<        1=the first byte of the receive buffer is package type
    uint8_t Channel;                 //!< rf channel(0-39)
    uint32_t Frequency;              //!< rf frequency (2400000kHz-2483500kHz)
    uint32_t accessAddress;          //!< access address,32bit PHY address
    uint32_t CRCInit;                //!< crc initial value
    pfnRFStatusCB_t rfStatusCB;      //!< status call back
    uint32_t ChannelMap;             //!< indicating  Used and Unused data channels.Every channel is represented with a
                                     //!< bit positioned as per the data channel index,The LSB represents data channel index 0
    uint8_t Resv;
    uint8_t HeartPeriod;             //!< The heart package interval shall be an integer multiple of 100ms
    uint8_t HopPeriod;               //!< hop period( T=32n*RTC clock ),default is 8
    uint8_t HopIndex;                //!< indicate the hopIncrement used in the data channel selection algorithm,default is 17
    uint8_t RxMaxlen;                //!< Maximum data length received in rf-mode(default 251)
    uint8_t TxMaxlen;                //!< Maximum data length transmit in rf-mode(default 251)
} rfConfig_t;

/* end define@RF-PHY */

/*******************************************************************************
 * UUID defined
 */
/**
 * GATT Services
 */
extern CONST uint8_t gapServiceUUID[];
extern CONST uint8_t gattServiceUUID[];

/**
 * GATT Attribute Types
 */
extern CONST uint8_t primaryServiceUUID[];
extern CONST uint8_t secondaryServiceUUID[];
extern CONST uint8_t includeUUID[];
extern CONST uint8_t characterUUID[];

/**
 * GATT Characteristic Descriptors
 */
extern CONST uint8_t charExtPropsUUID[];
extern CONST uint8_t charUserDescUUID[];
extern CONST uint8_t clientCharCfgUUID[];
extern CONST uint8_t servCharCfgUUID[];
extern CONST uint8_t charFormatUUID[];
extern CONST uint8_t charAggFormatUUID[];
extern CONST uint8_t validRangeUUID[];
extern CONST uint8_t extReportRefUUID[];
extern CONST uint8_t reportRefUUID[];

/**
 * GATT Characteristic Types
 */
extern CONST uint8_t deviceNameUUID[];
extern CONST uint8_t appearanceUUID[];
extern CONST uint8_t periPrivacyFlagUUID[];
extern CONST uint8_t reconnectAddrUUID[];
extern CONST uint8_t periConnParamUUID[];
extern CONST uint8_t serviceChangedUUID[];
extern CONST uint8_t centAddrResUUID[];

/*******************************************************************************
 * PUBLIC FUNCTIONS
 */
extern uint32_t tmos_rand( void ); // pseudo-random number
extern BOOL tmos_memcmp( const void *src1, const void *src2, uint32_t len ); // TRUE - same, FALSE - different
extern BOOL tmos_isbufset( uint8_t *buf, uint8_t val, uint32_t len ); // TRUE if all "val",FALSE otherwise
extern uint32_t tmos_strlen( char *pString );
extern void tmos_memset( void * pDst, uint8_t Value, uint32_t len );
extern void tmos_memcpy( void *dst, const void *src, uint32_t len ); // Generic memory copy.

/**
 * @brief   start a event immediately
 *
 * @param   taskID - task ID of event
 * @param   event - event value
 *
 * @return  0 - success.
 */
extern bStatus_t tmos_set_event( tmosTaskID taskID, tmosEvents event );

/**
 * @brief   clear a event already timeout, cannot be used in it own event function.
 *
 * @param   taskID - task ID of event
 * @param   event - event value
 *
 * @return  0 - success.
 */
extern bStatus_t tmos_clear_event( tmosTaskID taskID, tmosEvents event );

/**
 * @brief   start a event after period of time
 *
 * @param   taskID - task ID to set event for
 * @param   event - event to be notified with
 * @param   time - timeout value
 *
 * @return  TRUE - success.
 */
extern bStatus_t tmos_start_task( tmosTaskID taskID, tmosEvents event, tmosTimer time );

/**
 * @brief   This function is called to start a timer to expire in n system clock time.
 *          When the timer expires, the calling task will get the specified event
 *          and the timer will be reloaded with the timeout value.
 *
 * @param   taskID - task ID to set timer for
 * @param   event - event to be notified with
 * @param   time - timeout value
 *
 * @return  SUCCESS, or NO_TIMER_AVAIL.
 */
extern bStatus_t tmos_start_reload_task( tmosTaskID taskID, tmosEvents event, tmosTimer time );

/**
 * @brief   stop a event
 *
 * @param   taskID - task ID of event
 * @param   event - event value
 *
 * @param   None.
 *
 * @return  0 - success. 
 */
extern bStatus_t tmos_stop_task( tmosTaskID taskID, tmosEvents event );

/**
 * @brief   get last period of time for this event
 *
 * @param   taskID - task ID of event
 * @param   event - event value
 *
 * @return  the timer's tick count if found, zero otherwise.
 */
extern tmosTimer tmos_get_task_timer( tmosTaskID taskID, tmosEvents event );

/**
 * @brief   send msg to a task,callback events&SYS_EVENT_MSG
 *
 * @param   taskID - task ID of task need to send msg
 * @param  *msg_ptr - point of msg
 *
 * @return  SUCCESS, INVALID_TASK, INVALID_MSG_POINTER
 */
extern bStatus_t tmos_msg_send( tmosTaskID taskID, uint8_t *msg_ptr );

/**
 * @brief   delete a msg
 *
 * @param  *msg_ptr - point of msg
 *
 * @return  0 - success. 
 */
extern bStatus_t tmos_msg_deallocate( uint8_t *msg_ptr );

/**
 * @brief   receive a msg
 *
 * @param   taskID  - task ID of task need to receive msg
 *
 * @return *uint8_t - message information or NULL if no message
 */
extern uint8_t *tmos_msg_receive( tmosTaskID taskID );

/**
 * @brief   allocate buffer for msg when need to send msg
 *
 * @param   len  - length of msg
 *
 * @return  pointer to allocated buffer or NULL if allocation failed.
 */
extern uint8_t *tmos_msg_allocate( uint16_t len );

/**
 * @brief   read a data item to NV.
 *
 * @param   id   - Valid NV item Id.
 * @param   len  - Length of data to read.
 * @param  *pBuf - Data to read.
 *
 * @return  SUCCESS if successful, NV_OPER_FAILED if failed.
 */
extern uint8_t tmos_snv_read( uint8_t id, uint8_t len, void *pBuf );

/**
 * @brief   tmos system timer initialization
 *
 * @note    must initialization before call tmos task
 *
 * @param   fnGetClock - 0:system clock select RTC timer
 *                   valid:system clock select extend input
 *
 * @return  Command Status.
 */
extern bStatus_t TMOS_TimerInit( pfnGetSysClock fnGetClock );

/**
 * @brief   Process system
 *
 * @param   None.
 *
 * @return  None.
 */
extern void TMOS_SystemProcess( void );

/**
 * @brief   Get current system clock
 *
 * @param   None.
 *
 * @return  current system clock (in 0.625ms)
 */
extern uint32_t TMOS_GetSystemClock( void );

/**
 * @brief   register process event callback function
 *
 * @param   eventCb-events callback function
 *
 * @return  0xFF - error,others-task id
 */
extern tmosTaskID TMOS_ProcessEventRegister( pTaskEventHandlerFn eventCb );

/**
 * @brief   resv
 */
extern void TMOS_Set32KTuneValue( uint16_t flash_val, uint16_t ram_val );

/**
 * @brief   Add a device address into white list ( support 16 MAX )
 *
 * @param   addrType - Type of device address
 * @param   devAddr  - first address of device address
 *
 * @return  Command Status.
 */
extern bStatus_t LL_AddWhiteListDevice( uint8_t addrType, uint8_t *devAddr );

/**
 * @brief   Remove a device address from white list
 *
 * @param   addrType - Type of device address
 * @param   devAddr  - first address of device address
 *
 * @return  Command Status.
 */
extern bStatus_t LL_RemoveWhiteListDevice( uint8_t addrType, uint8_t *devAddr );

/**
 * @brief   Clear white list
 *
 * @param   None
 *
 * @return  Command Status.
 */
extern bStatus_t LL_ClearWhiteList( void );

/**
 * @brief   Encrypt data
 *
 * @param   key - key
 * @param   plaintextData - original data
 * @param   encryptData - encrypted data
 *
 * @return  Command Status.
 */
extern bStatus_t LL_Encrypt( uint8_t *key, uint8_t *plaintextData, uint8_t *encryptData );

/**
 * @brief   Decrypt data
 *
 * @param   key - key
 * @param   plaintextData - original data
 * @param   decryptData - decrypted data
 *
 * @return  Command Status.
 */
extern bStatus_t LL_Decrypt( uint8_t *key, uint8_t *plaintextData, uint8_t *decryptData );

/**
 * @brief   get number of unAck packet in current connect buffer
 *
 * @param   handle - connect handle
 *
 * @return  0xFFFFFFFF-handle error,number of packets not receiving ack 
 */
extern uint32_t LL_GetNumberOfUnAckPacket( uint16_t handle );

/**
 * @brief   Register a callback function will be called after each connect event.
 *          Only effect in single connection
 *
 * @param   connectEventCB - callback function
 *
 * @return  None.
 */
extern void LL_ConnectEventRegister( pfnConnectEventCB connectEventCB );

/**
 * @brief   set tx power level
 *
 * @param   power - tx power level
 *
 * @return  Command Status.
 */
extern bStatus_t LL_SetTxPowerLevel( uint8_t power );

/**
 * @brief   read rssi
 *
 * @param   None.
 *
 * @return  the value of rssi.
 */
extern int8_t BLE_ReadRssi( void );

/**
 * @brief   read cfo
 *
 * @param   None.
 *
 * @return  the value of cfo.
 */
extern int16_t BLE_ReadCfo( void );

/**
 * @brief   pa control init
 *          
 * @note    Can't be called until  role Init
 *
 * @param   paControl - pa control parameters(global variable)
 *
 * @return  Command Status.
 */
extern void BLE_PAControlInit( blePaControlConfig_t *paControl );

/**
 * @brief   ble register reset and rf calibration 
 *
 * @param   None 
 *
 * @return  None
 */
extern void BLE_RegInit( void );

/**
 * @brief   Init BLE lib. RTC will be occupied at the same time.
 *
 * @param   pCfg - config of BLE lib
 *
 * @return  0-success. error defined @ ERR_LIB_INIT
 */
extern bStatus_t BLE_LibInit( bleConfig_t* pCfg );

/**
 * @brief   generate a valid access address
 *
 * @param   None.
 *
 * @return  access address
 * the Access Address meets the following requirements:
 * It shall have no more than six consecutive zeros or ones. 
 * It shall not be t he advertising channel packets¡¯ Access Address.
 * It shall not be a sequence that differ s from the advertising channel packets' Access Address by only one bit.
 * It shall not have all four octets equal.
 * It shall have no more  than 24 transitions.
 * It shall have a minimum of two transitio ns in the most significant six bits.
 */
extern uint32_t BLE_AccessAddressGenerate( void );

/*
 * linkDB_Register - Register with this function to receive a callback when
 *              status changes on a connection.
 */
extern uint8_t linkDB_Register( pfnLinkDBCB_t pFunc );

/*
 * linkDB_State - Check to see if a physical link is in a specific state.
 * 
 *    returns TRUE is the link is in state. FALSE, otherwise.
 */
extern uint8_t linkDB_State( uint16_t connectionHandle, uint8_t state );

/*
 * linkDB_PerformFunc - Perform a function of each connection in the link database.
 */
extern void linkDB_PerformFunc( pfnPerformFuncCB_t cb );
/*
 * linkDB_Up - Check to see if a physical link is up (connected).
 *    Use like:  uint8_t linkDB_Up( uint16_t connectionHandle );
 *            connectionHandle - controller link connection handle.
 *            returns TRUE if the link is up. FALSE, otherwise.
 */
#define linkDB_Up( connectionHandle )  linkDB_State( (connectionHandle), LINK_CONNECTED )

/**
 * @brief   This function is used to get the MTU size of a connection.
 *
 * @param   connHandle - connection handle.
 *
 * @return  connection MTU size.<BR>
 */
extern uint16_t ATT_GetMTU( uint16_t connHandle );

/**
 * @brief   Send Handle Value Confirmation.
 *
 * @param   connHandle - connection to use
 *
 * @return  SUCCESS: Confirmation was sent successfully.<BR>
 *          INVALIDPARAMETER: Invalid confirmation field.<BR>
 *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.<BR>
 *          bleNotConnected: Connection is down.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 */
extern bStatus_t ATT_HandleValueCfm( uint16_t connHandle );

/*
 * Compare two UUIDs. The UUIDs are converted if necessary.
 */
extern uint8_t ATT_CompareUUID( const uint8_t *pUUID1, uint16_t len1, const uint8_t *pUUID2, uint16_t len2 );

/**
 * @brief   Initialize the Generic Attribute Profile Client.
 *
 * @return  SUCCESS: Client initialized successfully.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 */
extern bStatus_t GATT_InitClient( void );

/**
 * @brief   Register to receive incoming ATT Indications or Notifications
 *          of attribute values.
 *
 * @param   taskId ?task to forward indications or notifications to
 *
 * @return  void
 */
extern void GATT_RegisterForInd( uint8_t taskId );

/**
 * @brief   Find the attribute record for a given handle
 *
 * @param   handle - handle to look for
 * @param   pHandle - handle of owner of attribute (to be returned)
 * 
 * @return  Pointer to attribute record. NULL, otherwise.
 */
extern gattAttribute_t *GATT_FindHandle( uint16_t handle, uint16_t *pHandle );

/**
 * @brief   This sub-procedure is used when a server is configured to 
 *          indicate a characteristic value to a client and expects an
 *          attribute protocol layer acknowledgement that the indication
 *          was successfully received.
 *
 *          The ATT Handle Value Indication is used in this sub-procedure.
 *
 *          If the return status from this function is SUCCESS, the calling
 *          application task will receive an tmos GATT_MSG_EVENT message.
 *          The type of the message will be ATT_HANDLE_VALUE_CFM.
 *
 * @note    This sub-procedure is complete when ATT_HANDLE_VALUE_CFM
 *          (with SUCCESS or bleTimeoutstatus) is received by the calling application task.
 *
 * @param   connHandle - connection to use
 * @param   pInd - pointer to indication to be sent
 * @param   authenticated - whether an authenticated link is required
 * @param   taskId - task to be notified of response
 *
 * @return  SUCCESS: Indication was sent successfully.<BR>
 *          INVALIDPARAMETER: Invalid connection handle or request field.<BR>
 *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.<BR>
 *          bleNotConnected: Connection is down.<BR>
 *          blePending: A confirmation is pending with this client.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 *          bleTimeout: Previous transaction timed out.<BR>
 */
extern bStatus_t GATT_Indication( uint16_t connHandle, attHandleValueInd_t *pInd, uint8_t authenticated, uint8_t taskId );
/**
 * @brief   This sub-procedure is used when a server is configured to 
 *          notify a characteristic value to a client without expecting
 *          any attribute protocol layer acknowledgement that the 
 *          notification was successfully received.
 *
 *          The ATT Handle Value Notification is used in this sub-procedure.
 *
 * @note    A notification may be sent at any time and does not invoke a confirmation.
 *          No confirmation will be sent to the calling application task for
 *          this sub-procedure.
 *
 * @param   connHandle - connection to use
 * @param   pNoti - pointer to notification to be sent
 * @param   authenticated - whether an authenticated link is required
 *
 * @return  SUCCESS: Notification was sent successfully.<BR>
 *          INVALIDPARAMETER: Invalid connection handle or request field.<BR>
 *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.<BR>
 *          bleNotConnected: Connection is down.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 *          bleTimeout: Previous transaction timed out.<BR>
 */
extern bStatus_t GATT_Notification( uint16_t connHandle, attHandleValueNoti_t *pNoti, uint8_t authenticated );

/**
 * @brief   This sub-procedure is used by the client to set the ATT_MTU
 *          to the maximum possible value that can be supported by both
 *          devices when the client supports a value greater than the
 *          default ATT_MTU for the Attribute Protocol. This sub-procedure
 *          shall only be initiated once during a connection.
 *
 *          The ATT Exchange MTU Request is used by this sub-procedure.
 *
 *          If the return status from this function is SUCCESS, the calling
 *          application task will receive an tmos GATT_MSG_EVENT message.
 *          The type of the message will be either ATT_EXCHANGE_MTU_RSP or
 *          ATT_ERROR_RSP (if an error occurred on the server).
 *
 * @note    This sub-procedure is complete when either ATT_EXCHANGE_MTU_RSP
 *          (with SUCCESS or bleTimeout status) or ATT_ERROR_RSP
 *          (with SUCCESS status) is received by the calling application task.
 *
 * @param   connHandle - connection to use
 * @param   pReq - pointer to request to be sent
 * @param   taskId - task to be notified of response
 *
 * @return  SUCCESS: Request was sent successfully.<BR>
 *          INVALIDPARAMETER: Invalid connection handle or request field.<BR>
 *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.<BR>
 *          bleNotConnected: Connection is down.<BR>
 *          blePending: A response is pending with this server.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 *          bleTimeout: Previous transaction timed out.<BR>
 */
extern bStatus_t GATT_ExchangeMTU( uint16_t connHandle, attExchangeMTUReq_t *pReq, uint8_t taskId );

/**
 * @brief   This sub-procedure is used by a client to discover all 
 *          the primary services on a server.
 *
 *          The ATT Read By Group Type Request is used with the Attribute
 *          Type parameter set to the UUID for "Primary Service". The
 *          Starting Handle is set to 0x0001 and the Ending Handle is
 *          set to 0xFFFF.
 *
 *          If the return status from this function is SUCCESS, the calling
 *          application task will receive multiple tmos GATT_MSG_EVENT messages.
 *          The type of the messages will be either ATT_READ_BY_GRP_TYPE_RSP
 *          or ATT_ERROR_RSP (if an error occurred on the server).
 *
 * @note    This sub-procedure is complete when either ATT_READ_BY_GRP_TYPE_RSP
 *          (with bleProcedureComplete or bleTimeout status) or ATT_ERROR_RSP
 *          (with SUCCESS status) is received by the calling application task.
 *
 * @param   connHandle - connection to use
 * @param   taskId - task to be notified of response
 *
 * @return  SUCCESS: Request was sent successfully.<BR>
 *          INVALIDPARAMETER: Invalid connection handle or request field.<BR>
 *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.<BR>
 *          bleNotConnected: Connection is down.<BR>
 *          blePending: A response is pending with this server.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 *          bleTimeout: Previous transaction timed out.<BR>
 */
extern bStatus_t GATT_DiscAllPrimaryServices( uint16_t connHandle, uint8_t taskId );

/**
 * @brief   This sub-procedure is used by a client to discover a specific
 *          primary service on a server when only the Service UUID is
 *          known. The primary specific service may exist multiple times 
 *          on a server. The primary service being discovered is identified
 *          by the service UUID.
 *
 *          The ATT Find By Type Value Request is used with the Attribute
 *          Type parameter set to the UUID for "Primary Service" and the
 *          Attribute Value set to the 16-bit Bluetooth UUID or 128-bit
 *          UUID for the specific primary service. The Starting Handle shall
 *          be set to 0x0001 and the Ending Handle shall be set to 0xFFFF.
 *
 *          If the return status from this function is SUCCESS, the calling
 *          application task will receive multiple tmos GATT_MSG_EVENT messages.
 *          The type of the messages will be either ATT_FIND_BY_TYPE_VALUE_RSP
 *          or ATT_ERROR_RSP (if an error occurred on the server).
 *
 * @note    This sub-procedure is complete when either ATT_FIND_BY_TYPE_VALUE_RSP
 *          (with bleProcedureComplete or bleTimeout status) or ATT_ERROR_RSP
 *          (with SUCCESS status) is received by the calling application task.
 *
 * @param   connHandle - connection to use
 * @param   pUUID - pointer to service UUID to look for
 * @param   len - length of value
 * @param   taskId - task to be notified of response
 *
 * @return  SUCCESS: Request was sent successfully.<BR>
 *          INVALIDPARAMETER: Invalid connection handle or request field.<BR>
 *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.<BR>
 *          bleNotConnected: Connection is down.<BR>
 *          blePending: A response is pending with this server.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 *          bleTimeout: Previous transaction timed out.<BR>
 */
extern bStatus_t GATT_DiscPrimaryServiceByUUID( uint16_t connHandle, uint8_t *pUUID, uint8_t len, uint8_t taskId );

/**
 * @brief   This sub-procedure is used by a client to find include
 *          service declarations within a service definition on a 
 *          server. The service specified is identified by the service
 *          handle range.
 *
 *          The ATT Read By Type Request is used with the Attribute
 *          Type parameter set to the UUID for "Included Service". The
 *          Starting Handle is set to starting handle of the specified
 *          service and the Ending Handle is set to the ending handle
 *          of the specified service.
 *
 *          If the return status from this function is SUCCESS, the calling
 *          application task will receive multiple tmos GATT_MSG_EVENT messages.
 *          The type of the messages will be either ATT_READ_BY_TYPE_RSP
 *          or ATT_ERROR_RSP (if an error occurred on the server).
 *
 * @note    This sub-procedure is complete when either ATT_READ_BY_TYPE_RSP
 *          (with bleProcedureCompleteor bleTimeout status)or ATT_ERROR_RSP
 *          (with SUCCESS status) is received by the calling application task.
 *
 * @param   connHandle - connection to use
 * @param   startHandle - starting handle
 * @param   endHandle - end handle
 * @param   taskId - task to be notified of response
 *
 * @return  SUCCESS: Request was sent successfully.<BR>
 *          INVALIDPARAMETER: Invalid connection handle or request field.<BR>
 *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.<BR>
 *          bleNotConnected: Connection is down.<BR>
 *          blePending: A response is pending with this server.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 *          bleTimeout: Previous transaction timed out.<BR>
 */
extern bStatus_t GATT_FindIncludedServices( uint16_t connHandle, uint16_t startHandle, uint16_t endHandle, uint8_t taskId );

/**
 * @brief   This sub-procedure is used by a client to find all the 
 *          characteristic declarations within a service definition on
 *          a server when only the service handle range is known. The
 *          service specified is identified by the service handle range.
 *
 *          The ATT Read By Type Request is used with the Attribute Type
 *          parameter set to the UUID for "Characteristic". The Starting
 *          Handle is set to starting handle of the specified service and
 *          the Ending Handle is set to the ending handle of the specified
 *          service.
 *
 *          If the return status from this function is SUCCESS, the calling
 *          application task will receive multiple tmos GATT_MSG_EVENT messages.
 *          The type of the messages will be either ATT_READ_BY_TYPE_RSP
 *          or ATT_ERROR_RSP (if an error occurred on the server).
 *
 * @note    This sub-procedure is complete when either ATT_READ_BY_TYPE_RSP
 *          (with bleProcedureComplete or bleTimeout status)or ATT_ERROR_RSP
 *          (with SUCCESS status) is received by the calling application task.
 *
 * @param   connHandle - connection to use
 * @param   startHandle - starting handle
 * @param   endHandle - end handle
 * @param   taskId - task to be notified of response
 *
 * @return  SUCCESS: Request was sent successfully.<BR>
 *          INVALIDPARAMETER: Invalid connection handle or request field.<BR>
 *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.<BR>
 *          bleNotConnected: Connection is down.<BR>
 *          blePending: A response is pending with this server.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 *          bleTimeout: Previous transaction timed out.<BR>
 */
extern bStatus_t GATT_DiscAllChars( uint16_t connHandle, uint16_t startHandle, uint16_t endHandle, uint8_t taskId );

/**
 * @brief   This sub-procedure is used by a client to discover service 
 *          characteristics on a server when only the service handle 
 *          ranges are known and the characteristic UUID is known. 
 *          The specific service may exist multiple times on a server.
 *          The characteristic being discovered is identified by the 
 *          characteristic UUID.
 *
 *          The ATT Read By Type Request is used with the Attribute Type
 *          is set to the UUID for "Characteristic" and the Starting
 *          Handle and Ending Handle parameters is set to the service
 *          handle range.
 *
 *          If the return status from this function is SUCCESS, the calling
 *          application task will receive multiple tmos GATT_MSG_EVENT messages.
 *          The type of the messages will be either ATT_READ_BY_TYPE_RSP
 *          or ATT_ERROR_RSP (if an error occurred on the server).
 *
 * @note    This sub-procedure is complete when either ATT_READ_BY_TYPE_RSP
 *          (with bleProcedureComplete or bleTimeout status) or ATT_ERROR_RSP
 *          (with SUCCESS status) is received by the calling application task.
 *
 * @param   connHandle - connection to use
 * @param   pReq - pointer to request to be sent
 * @param   taskId - task to be notified of response
 *
 * @return  SUCCESS: Request was sent successfully.<BR>
 *          INVALIDPARAMETER: Invalid connection handle or request field.<BR>
 *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.<BR>
 *          bleNotConnected: Connection is down.<BR>
 *          blePending: A response is pending with this server.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 *          bleTimeout: Previous transaction timed out.<BR>
 */
extern bStatus_t GATT_DiscCharsByUUID( uint16_t connHandle, attReadByTypeReq_t *pReq, uint8_t taskId );

/**
 * @brief   This sub-procedure is used by a client to find all the 
 *          characteristic descriptors Attribute Handles and Attribute
 *          Types within a characteristic definition when only the 
 *          characteristic handle range is known. The characteristic 
 *          specified is identified by the characteristic handle range.
 *
 *          The ATT Find Information Request is used with the Starting
 *          Handle set to starting handle of the specified characteristic
 *          and the Ending Handle set to the ending handle of the specified
 *          characteristic. The UUID Filter parameter is NULL (zero length).
 *
 *          If the return status from this function is SUCCESS, the calling
 *          application task will receive multiple tmos GATT_MSG_EVENT messages.
 *          The type of the messages will be either ATT_FIND_INFO_RSP or
 *          ATT_ERROR_RSP (if an error occurred on the server).
 *
 * @note    This sub-procedure is complete when either ATT_FIND_INFO_RSP
 *          (with bleProcedureComplete or bleTimeout status) or ATT_ERROR_RSP
 *          (with SUCCESS status) is received by the calling application task.
 *
 * @param   connHandle - connection to use
 * @param   startHandle - starting handle
 * @param   endHandle - end handle
 * @param   taskId - task to be notified of response
 *
 * @return  SUCCESS: Request was sent successfully.<BR>
 *          INVALIDPARAMETER: Invalid connection handle or request field.<BR>
 *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.<BR>
 *          bleNotConnected: Connection is down.<BR>
 *          blePending: A response is pending with this server.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 *          bleTimeout: Previous transaction timed out.<BR>
 */
extern bStatus_t GATT_DiscAllCharDescs( uint16_t connHandle, uint16_t startHandle, uint16_t endHandle, uint8_t taskId );

/**
 * @brief   This sub-procedure is used to read a Characteristic Value
 *          from a server when the client knows the Characteristic Value
 *          Handle. The ATT Read Request is used with the Attribute Handle
 *          parameter set to the Characteristic Value Handle. The Read 
 *          Response returns the Characteristic Value in the Attribute
 *          Value parameter.
 *
 *          The Read Response only contains a Characteristic Value that
 *          is less than or equal to (ATT_MTU ?1) octets in length. If 
 *          the Characteristic Value is greater than (ATT_MTU ?1) octets
 *          in length, the Read Long Characteristic Value procedure may
 *          be used if the rest of the Characteristic Value is required.
 *
 *          If the return status from this function is SUCCESS, the calling
 *          application task will receive an tmos GATT_MSG_EVENT message.
 *          The type of the message will be either ATT_READ_RSP or
 *          ATT_ERROR_RSP (if an error occurred on the server).
 *
 * @note    This sub-procedure is complete when either ATT_READ_RSP
 *          (with SUCCESS or bleTimeout status) or ATT_ERROR_RSP
 *          (with SUCCESS status) is received by the calling application task.
 *
 * @param   connHandle - connection to use
 * @param   pReq - pointer to request to be sent
 * @param   taskId - task to be notified of response
 *
 * @return  SUCCESS: Request was sent successfully.<BR>
 *          INVALIDPARAMETER: Invalid connection handle or request field.<BR>
 *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.<BR>
 *          bleNotConnected: Connection is down.<BR>
 *          blePending: A response is pending with this server.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 *          bleTimeout: Previous transaction timed out.<BR>
 */
extern bStatus_t GATT_ReadCharValue( uint16_t connHandle, attReadReq_t *pReq, uint8_t taskId );

/**
 * @brief   This sub-procedure is used to read a Characteristic Value
 *          from a server when the client only knows the characteristic
 *          UUID and does not know the handle of the characteristic.
 *
 *          The ATT Read By Type Request is used to perform the sub-procedure.
 *          The Attribute Type is set to the known characteristic UUID and
 *          the Starting Handle and Ending Handle parameters shall be set
 *          to the range over which this read is to be performed. This is
 *          typically the handle range for the service in which the 
 *          characteristic belongs.
 *
 *          If the return status from this function is SUCCESS, the calling
 *          application task will receive an tmos GATT_MSG_EVENT messages.
 *          The type of the message will be either ATT_READ_BY_TYPE_RSP
 *          or ATT_ERROR_RSP (if an error occurred on the server).
 *
 * @note    This sub-procedure is complete when either ATT_READ_BY_TYPE_RSP
 *          (with SUCCESS or bleTimeout status) or ATT_ERROR_RSP
 *          (with SUCCESS status) is received by the calling application task.
 *
 * @param   connHandle - connection to use
 * @param   pReq - pointer to request to be sent
 * @param   taskId - task to be notified of response
 *
 * @return  SUCCESS: Request was sent successfully.<BR>
 *          INVALIDPARAMETER: Invalid connection handle or request field.<BR>
 *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.<BR>
 *          bleNotConnected: Connection is down.<BR>
 *          blePending: A response is pending with this server.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 *          bleTimeout: Previous transaction timed out.<BR>
 */
extern bStatus_t GATT_ReadUsingCharUUID( uint16_t connHandle, attReadByTypeReq_t *pReq, uint8_t taskId );

/**
 * @brief   This sub-procedure is used to read a Characteristic Value from
 *          a server when the client knows the Characteristic Value Handle
 *          and the length of the Characteristic Value is longer than can 
 *          be sent in a single Read Response Attribute Protocol message.
 *
 *          The ATT Read Blob Request is used in this sub-procedure.
 *
 *          If the return status from this function is SUCCESS, the calling
 *          application task will receive multiple tmos GATT_MSG_EVENT messages.
 *          The type of the messages will be either ATT_READ_BLOB_RSP or
 *          ATT_ERROR_RSP (if an error occurred on the server).
 *
 * @note    This sub-procedure is complete when either ATT_READ_BLOB_RSP
 *          (with bleProcedureComplete or bleTimeout status) or ATT_ERROR_RSP
 *          (with SUCCESS status) is received by the calling application task.
 *
 * @param   connHandle - connection to use
 * @param   pReq - pointer to request to be sent
 * @param   taskId - task to be notified of response
 *
 * @return  SUCCESS: Request was sent successfully.<BR>
 *          INVALIDPARAMETER: Invalid connection handle or request field.<BR>
 *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.<BR>
 *          bleNotConnected: Connection is down.<BR>
 *          blePending: A response is pending with this server.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 *          bleTimeout: Previous transaction timed out.<BR>
 */
extern bStatus_t GATT_ReadLongCharValue( uint16_t connHandle, attReadBlobReq_t *pReq, uint8_t taskId );

/**
 * @brief   This sub-procedure is used to read multiple Characteristic Values
 *          from a server when the client knows the Characteristic Value 
 *          Handles. The Attribute Protocol Read Multiple Requests is used 
 *          with the Set Of Handles parameter set to the Characteristic Value
 *          Handles. The Read Multiple Response returns the Characteristic
 *          Values in the Set Of Values parameter.
 *
 *          The ATT Read Multiple Request is used in this sub-procedure.
 *
 *          If the return status from this function is SUCCESS, the calling
 *          application task will receive an tmos GATT_MSG_EVENT message.
 *          The type of the message will be either ATT_READ_MULTI_RSP
 *          or ATT_ERROR_RSP (if an error occurred on the server).
 *
 * @note    This sub-procedure is complete when either ATT_READ_MULTI_RSP
 *          (with SUCCESS or bleTimeout status) or ATT_ERROR_RSP
 *          (with SUCCESS status) is received by the calling application task.
 *
 * @param   connHandle - connection to use
 * @param   pReq - pointer to request to be sent
 * @param   taskId - task to be notified of response
 *
 * @return  SUCCESS: Request was sent successfully.<BR>
 *          INVALIDPARAMETER: Invalid connection handle or request field.<BR>
 *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.<BR>
 *          bleNotConnected: Connection is down.<BR>
 *          blePending: A response is pending with this server.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 *          bleTimeout: Previous transaction timed out.<BR>
 */
extern bStatus_t GATT_ReadMultiCharValues( uint16_t connHandle, attReadMultiReq_t *pReq, uint8_t taskId );

/**
 * @brief   This sub-procedure is used to write a Characteristic Value
 *          to a server when the client knows the Characteristic Value
 *          Handle and the client does not need an acknowledgement that
 *          the write was successfully performed. This sub-procedure
 *          only writes the first (ATT_MTU ?3) octets of a Characteristic
 *          Value. This sub-procedure can not be used to write a long
 *          characteristic; instead the Write Long Characteristic Values
 *          sub-procedure should be used.
 * 
 *          The ATT Write Command is used for this sub-procedure. The
 *          Attribute Handle parameter shall be set to the Characteristic
 *          Value Handle. The Attribute Value parameter shall be set to
 *          the new Characteristic Value.
 *
 *          No response will be sent to the calling application task for this
 *          sub-procedure. If the Characteristic Value write request is the 
 *          wrong size, or has an invalid value as defined by the profile, 
 *          then the write will not succeed and no error will be generated 
 *          by the server.
 *
 * @param   connHandle - connection to use
 * @param   pReq - pointer to command to be sent
 *
 * @return  SUCCESS: Request was sent successfully.<BR>
 *          INVALIDPARAMETER: Invalid connection handle or request field.<BR>
 *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.<BR>
 *          bleNotConnected: Connection is down.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 *          bleTimeout: Previous transaction timed out.<BR>
 */
extern bStatus_t GATT_WriteNoRsp( uint16_t connHandle, attWriteReq_t *pReq );

/**
 * @brief   This sub-procedure is used to write a Characteristic Value
 *          to a server when the client knows the Characteristic Value 
 *          Handle and the ATT Bearer is not encrypted. This sub-procedure
 *          shall only be used if the Characteristic Properties authenticated
 *          bit is enabled and the client and server device share a bond as
 *          defined in the GAP.
 *
 *          This sub-procedure only writes the first (ATT_MTU ?15) octets
 *          of an Attribute Value. This sub-procedure cannot be used to 
 *          write a long Attribute.
 *
 *          The ATT Write Command is used for this sub-procedure. The
 *          Attribute Handle parameter shall be set to the Characteristic
 *          Value Handle. The Attribute Value parameter shall be set to 
 *          the new Characteristic Value authenticated by signing the
 *          value, as defined in the Security Manager.
 *
 *          No response will be sent to the calling application task for this
 *          sub-procedure. If the authenticated Characteristic Value that is 
 *          written is the wrong size, or has an invalid value as defined by 
 *          the profile, or the signed value does not authenticate the client,
 *          then the write will not succeed and no error will be generated by
 *          the server.
 *
 * @param   connHandle - connection to use
 * @param   pReq - pointer to command to be sent
 *
 * @return  SUCCESS: Request was sent successfully.<BR>
 *          INVALIDPARAMETER: Invalid connection handle or request field.<BR>
 *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.<BR>
 *          bleNotConnected: Connection is down.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 *          bleLinkEncrypted: Connection is already encrypted.<BR>
 *          bleTimeout: Previous transaction timed out.<BR>
 */
extern bStatus_t GATT_SignedWriteNoRsp( uint16_t connHandle, attWriteReq_t *pReq );

/**
 * @brief   This sub-procedure is used to write a characteristic value
 *          to a server when the client knows the characteristic value
 *          handle. This sub-procedure only writes the first (ATT_MTU-3)
 *          octets of a characteristic value. This sub-procedure can not
 *          be used to write a long attribute; instead the Write Long 
 *          Characteristic Values sub-procedure should be used.
 *
 *          The ATT Write Request is used in this sub-procedure. The 
 *          Attribute Handle parameter shall be set to the Characteristic
 *          Value Handle. The Attribute Value parameter shall be set to
 *          the new characteristic.
 *
 *          If the return status from this function is SUCCESS, the calling
 *          application task will receive an tmos GATT_MSG_EVENT message.
 *          The type of the message will be either ATT_WRITE_RSP
 *          or ATT_ERROR_RSP (if an error occurred on the server).
 *
 * @note    This sub-procedure is complete when either ATT_WRITE_RSP
 *          (with SUCCESS or bleTimeout status) or ATT_ERROR_RSP
 *          (with SUCCESS status) is received by the calling application task.
 *
 * @param   connHandle - connection to use
 * @param   pReq - pointer to request to be sent
 * @param   taskId - task to be notified of response
 *
 * @return  SUCCESS: Request was sent successfully.<BR>
 *          INVALIDPARAMETER: Invalid connection handle or request field.<BR>
 *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.<BR>
 *          bleNotConnected: Connection is down.<BR>
 *          blePending: A response is pending with this server.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 *          bleTimeout: Previous transaction timed out.<BR>
 */
extern bStatus_t GATT_WriteCharValue( uint16_t connHandle, attWriteReq_t *pReq, uint8_t taskId );

/**
 * @brief   This sub-procedure is used to write a Characteristic Value to
 *          a server when the client knows the Characteristic Value Handle
 *          but the length of the Characteristic Value is longer than can 
 *          be sent in a single Write Request Attribute Protocol message.
 *
 *          The ATT Prepare Write Request and Execute Write Request are
 *          used to perform this sub-procedure.
 *
 *          If the return status from this function is SUCCESS, the calling
 *          application task will receive multiple tmos GATT_MSG_EVENT messages.
 *          The type of the messages will be either ATT_PREPARE_WRITE_RSP,
 *          ATT_EXECUTE_WRITE_RSP or ATT_ERROR_RSP (if an error occurred on
 *          the server).
 *
 * @note    This sub-procedure is complete when either ATT_PREPARE_WRITE_RSP
 *          (with bleTimeout status), ATT_EXECUTE_WRITE_RSP
 *          (with SUCCESS or bleTimeout status), or ATT_ERROR_RSP
 *          (with SUCCESS status) is received by the calling application task.
 *
 * @note    The 'pReq->pValue' pointer will be freed when the sub-procedure is complete.
 *
 * @param   connHandle - connection to use
 * @param   pReq - pointer to request to be sent
 * @param   taskId - task to be notified of response
 *
 * @return  SUCCESS: Request was sent successfully.<BR>
 *          INVALIDPARAMETER: Invalid connection handle or request field.<BR>
 *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.<BR>
 *          bleNotConnected: Connection is down.<BR>
 *          blePending: A response is pending with this server.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 *          bleTimeout: Previous transaction timed out.<BR>
 */
extern bStatus_t GATT_WriteLongCharValue( uint16_t connHandle, attPrepareWriteReq_t *pReq, uint8_t taskId );

/**
 * @brief   This sub-procedure is used to write a Characteristic Value to
 *          a server when the client knows the Characteristic Value Handle,
 *          and assurance is required that the correct Characteristic Value
 *          is going to be written by transferring the Characteristic Value
 *          to be written in both directions before the write is performed.
 *          This sub-procedure can also be used when multiple values must 
 *          be written, in order, in a single operation.
 *
 *          The sub-procedure has two phases, the first phase prepares the
 *          characteristic values to be written.  Once this is complete, 
 *          the second phase performs the execution of all of the prepared
 *          characteristic value writes on the server from this client.
 *
 *          In the first phase, the ATT Prepare Write Request is used.
 *          In the second phase, the attribute protocol Execute Write
 *          Request is used.
 *
 *          If the return status from this function is SUCCESS, the calling
 *          application task will receive multiple tmos GATT_MSG_EVENT messages.
 *          The type of the messages will be either ATT_PREPARE_WRITE_RSP,
 *          ATT_EXECUTE_WRITE_RSP or ATT_ERROR_RSP (if an error occurred on
 *          the server).
 *
 * @note    This sub-procedure is complete when either ATT_PREPARE_WRITE_RSP
 *          (with bleTimeout status), ATT_EXECUTE_WRITE_RSP
 *          (with SUCCESS or bleTimeout status), or ATT_ERROR_RSP
 *          (with SUCCESS status) is received by the calling application task.
 *
 * @note    The 'pReqs' pointer will be freed when the sub-procedure is complete.
 *
 * @param   connHandle - connection to use
 * @param   pReqs - pointer to requests to be sent (must be allocated)
 * @param   numReqs - number of requests in pReq
 * @param   flags - execute write request flags
 * @param   taskId - task to be notified of response
 *
 * @return  SUCCESS: Request was sent successfully.<BR>
 *          INVALIDPARAMETER: Invalid connection handle or request field.<BR>
 *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.<BR>
 *          bleNotConnected: Connection is down.<BR>
 *          blePending: A response is pending with this server.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 *          bleTimeout: Previous transaction timed out.<BR>
 */
extern bStatus_t GATT_ReliableWrites( uint16_t connHandle, attPrepareWriteReq_t *pReqs, uint8_t numReqs,
    uint8_t flags, uint8_t taskId );
/**
 * @brief   This sub-procedure is used to read a characteristic descriptor
 *          from a server when the client knows the characteristic descriptor
 *          declaration's Attribute handle.
 *
 *          The ATT Read Request is used for this sub-procedure. The Read
 *          Request is used with the Attribute Handle parameter set to the 
 *          characteristic descriptor handle. The Read Response returns the
 *          characteristic descriptor value in the Attribute Value parameter.
 *
 *          If the return status from this function is SUCCESS, the calling
 *          application task will receive an tmos GATT_MSG_EVENT message.
 *          The type of the message will be either ATT_READ_RSP or
 *          ATT_ERROR_RSP (if an error occurred on the server).
 *
 * @note    This sub-procedure is complete when either ATT_READ_RSP
 *          (with SUCCESS or bleTimeout status) or ATT_ERROR_RSP
 *          (with SUCCESS status) is received by the calling application task.
 *
 * @param   connHandle - connection to use
 * @param   pReq - pointer to request to be sent
 * @param   taskId - task to be notified of response
 *
 * @return  SUCCESS: Request was sent successfully.<BR>
 *          INVALIDPARAMETER: Invalid connection handle or request field.<BR>
 *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.<BR>
 *          bleNotConnected: Connection is down.<BR>
 *          blePending: A response is pending with this server.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 *          bleTimeout: Previous transaction timed out.<BR>
 */
extern bStatus_t GATT_ReadCharDesc( uint16_t connHandle, attReadReq_t *pReq, uint8_t taskId );

/**
 * @brief   This sub-procedure is used to read a characteristic descriptor
 *          from a server when the client knows the characteristic descriptor
 *          declaration's Attribute handle and the length of the characteristic
 *          descriptor declaration is longer than can be sent in a single Read
 *          Response attribute protocol message.
 *
 *          The ATT Read Blob Request is used to perform this sub-procedure.
 *          The Attribute Handle parameter shall be set to the characteristic
 *          descriptor handle. The Value Offset parameter shall be the offset
 *          within the characteristic descriptor to be read.
 *
 *          If the return status from this function is SUCCESS, the calling
 *          application task will receive multiple tmos GATT_MSG_EVENT messages.
 *          The type of the messages will be either ATT_READ_BLOB_RSP or
 *          ATT_ERROR_RSP (if an error occurred on the server).
 *
 * @note    This sub-procedure is complete when either ATT_READ_BLOB_RSP
 *          (with bleProcedureComplete or bleTimeout status) or ATT_ERROR_RSP
 *          (with SUCCESS status) is received by the calling application task.
 *
 * @param   connHandle - connection to use
 * @param   pReq - pointer to request to be sent
 * @param   taskId - task to be notified of response
 *
 * @return  SUCCESS: Request was sent successfully.<BR>
 *          INVALIDPARAMETER: Invalid connection handle or request field.<BR>
 *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.<BR>
 *          bleNotConnected: Connection is down.<BR>
 *          blePending: A response is pending with this server.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 *          bleTimeout: Previous transaction timed out.<BR>
 */
extern bStatus_t GATT_ReadLongCharDesc( uint16_t connHandle, attReadBlobReq_t *pReq, uint8_t taskId );

/**
 * @brief   This sub-procedure is used to write a characteristic
 *          descriptor value to a server when the client knows the
 *          characteristic descriptor handle.
 *
 *          The ATT Write Request is used for this sub-procedure. The
 *          Attribute Handle parameter shall be set to the characteristic
 *          descriptor handle. The Attribute Value parameter shall be
 *          set to the new characteristic descriptor value.
 *
 *          If the return status from this function is SUCCESS, the calling
 *          application task will receive an tmos GATT_MSG_EVENT message.
 *          The type of the message will be either ATT_WRITE_RSP
 *          or ATT_ERROR_RSP (if an error occurred on the server).
 *
 * @note    This sub-procedure is complete when either ATT_WRITE_RSP
 *          (with SUCCESS or bleTimeout status) or ATT_ERROR_RSP
 *          (with SUCCESS status) is received by the calling application task.
 *
 * @param   connHandle - connection to use
 * @param   pReq - pointer to request to be sent
 * @param   taskId - task to be notified of response
 *
 * @return  SUCCESS: Request was sent successfully.<BR>
 *          INVALIDPARAMETER: Invalid connection handle or request field.<BR>
 *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.<BR>
 *          bleNotConnected: Connection is down.<BR>
 *          blePending: A response is pending with this server.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 *          bleTimeout: Previous transaction timed out.<BR>
 */
extern bStatus_t GATT_WriteCharDesc( uint16_t connHandle, attWriteReq_t *pReq, uint8_t taskId );

/**
 * @brief   This sub-procedure is used to write a Characteristic Value to
 *          a server when the client knows the Characteristic Value Handle
 *          but the length of the Characteristic Value is longer than can 
 *          be sent in a single Write Request Attribute Protocol message.
 *
 *          The ATT Prepare Write Request and Execute Write Request are
 *          used to perform this sub-procedure.
 *
 *          If the return status from this function is SUCCESS, the calling
 *          application task will receive multiple tmos GATT_MSG_EVENT messages.
 *          The type of the messages will be either ATT_PREPARE_WRITE_RSP,
 *          ATT_EXECUTE_WRITE_RSP or ATT_ERROR_RSP (if an error occurred on
 *          the server).
 *
 * @note    This sub-procedure is complete when either ATT_PREPARE_WRITE_RSP
 *          (with bleTimeout status), ATT_EXECUTE_WRITE_RSP
 *          (with SUCCESS or bleTimeout status), or ATT_ERROR_RSP
 *          (with SUCCESS status) is received by the calling application task.
 *
 * @note    The 'pReq->pValue' pointer will be freed when the sub-procedure is complete.
 *
 * @param   connHandle - connection to use
 * @param   pReq - pointer to request to be sent
 * @param   taskId - task to be notified of response
 *
 * @return  SUCCESS: Request was sent successfully.<BR>
 *          INVALIDPARAMETER: Invalid connection handle or request field.v
 *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.<BR>
 *          bleNotConnected: Connection is down.<BR>
 *          blePending: A response is pending with this server.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 *          bleTimeout: Previous transaction timed out.<BR>
 */
extern bStatus_t GATT_WriteLongCharDesc( uint16_t connHandle, attPrepareWriteReq_t *pReq, uint8_t taskId );

/**
 * @brief   GATT implementation of the allocator functionality.
 *
 * @note    This function should only be called by GATT and the upper layer protocol/application.
 *
 * @param   connHandle - connection that message is to be sent on.
 * @param   opcode - opcode of message that buffer to be allocated for.
 * @param   size - number of bytes to allocate from the heap.
 * @param   pSizeAlloc - number of bytes allocated for the caller from the heap.
 * @param   flag - .
 *
 * @return  pointer to the heap allocation; NULL if error or failure.
 */
extern void *GATT_bm_alloc( uint16_t connHandle, uint8_t opcode, uint16_t size, uint16_t *pSizeAlloc, uint8_t flag );

/**
 * @brief   GATT implementation of the de-allocator functionality.
 *
 * @param   pMsg - pointer to GATT message containing the memory to free.
 * @param   opcode - opcode of the message
 *
 * @return  none
 */
extern void GATT_bm_free( gattMsg_t *pMsg, uint8_t opcode );

/**
 * @brief   Register a service's attribute list and callback functions with
 *          the GATT Server Application.
 *
 * @param   pAttrs - Array of attribute records to be registered
 * @param   numAttrs - Number of attributes in array
 * @param   encKeySize - Minimum encryption key size required by service (7-16 bytes)
 * @param   pServiceCBs - Service callback function pointers
 *
 * @return  SUCCESS: Service registered successfully.<BR>
 *          INVALIDPARAMETER: Invalid service fields.<BR>
 *          FAILURE: Not enough attribute handles available.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 *          bleInvalidRange: Encryption key size's out of range.<BR>
 */
extern bStatus_t GATTServApp_RegisterService( gattAttribute_t *pAttrs, uint16_t numAttrs,
    uint8_t encKeySize, gattServiceCBs_t *pServiceCBs );

/**
 * @brief   Add function for the GATT Service.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 *
 * @return  SUCCESS: Service added successfully.<BR>
 *          INVALIDPARAMETER: Invalid service field.<BR>
 *          FAILURE: Not enough attribute handles available.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 */
extern bStatus_t GATTServApp_AddService( uint32_t services );

/**
 * @brief   Deregister a service's attribute list and callback functions from
 *          the GATT Server Application.
 *
 * @note    It's the caller's responsibility to free the service attribute
 *          list returned from this API.
 *
 * @param   handle - handle of service to be deregistered
 * @param   p2pAttrs - pointer to array of attribute records (to be returned)
 *
 * @return  SUCCESS: Service deregistered successfully.<BR>
 *          FAILURE: Service not found.<BR>
 */
extern bStatus_t GATTServApp_DeregisterService( uint16_t handle, gattAttribute_t **p2pAttrs );

/**
 * @brief   Initialize the client characteristic configuration table.
 *
 * @note    Each client has its own instantiation of the ClientCharacteristic Configuration.
 *          Reads/Writes of the Client Characteristic Configuration only only affect the
 *          configuration of that client.
 *
 * @param   connHandle - connection handle (0xFFFF for all connections).
 * @param   charCfgTbl - client characteristic configuration table.
 *
 * @return  none
 */
extern void GATTServApp_InitCharCfg( uint16_t connHandle, gattCharCfg_t *charCfgTbl );

/**
 * @brief   Send out a Service Changed Indication.
 *
 * @param   connHandle - connection to use
 * @param   taskId - task to be notified of confirmation
 *
 * @return  SUCCESS: Indication was sent successfully.<BR>
 *          FAILURE: Service Changed attribute not found.<BR>
 *          INVALIDPARAMETER: Invalid connection handle or request field.<BR>
 *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.<BR>
 *          bleNotConnected: Connection is down.<BR>
 *          blePending: A confirmation is pending with this client.<BR>
 */
extern bStatus_t GATTServApp_SendServiceChangedInd( uint16_t connHandle, uint8_t taskId );

/**
 * @brief   Read the client characteristic configuration for a given client.
 *
 * @note    Each client has its own instantiation of the Client Characteristic Configuration.
 *          Reads of the Client Characteristic Configuration only shows the configuration
 *          for that client.
 *
 * @param   connHandle - connection handle.
 * @param   charCfgTbl - client characteristic configuration table.
 *
 * @return  attribute value
 */
extern uint16_t GATTServApp_ReadCharCfg( uint16_t connHandle, gattCharCfg_t *charCfgTbl );

/**
 * @brief   Write the client characteristic configuration for a given client.
 *
 * @note    Each client has its own instantiation of the Client Characteristic Configuration.
 *          Writes of the Client Characteristic Configuration only only affect the
 *          configuration of that client.
 *
 * @param   connHandle - connection handle.
 * @param   charCfgTbl - client characteristic configuration table.
 * @param   value - attribute new value.
 *
 * @return  Success or Failure
 */
extern uint8_t GATTServApp_WriteCharCfg( uint16_t connHandle, gattCharCfg_t *charCfgTbl, uint16_t value );

/**
 * @brief   Process the client characteristic configuration
 *          write request for a given client.
 *
 * @param   connHandle - connection message was received on.
 * @param   pAttr - pointer to attribute.
 * @param   pValue - pointer to data to be written.
 * @param   len - length of data.
 * @param   offset - offset of the first octet to be written.
 * @param   validCfg - valid configuration.
 *
 * @return  Success or Failure
 */
extern bStatus_t GATTServApp_ProcessCCCWriteReq( uint16_t connHandle, gattAttribute_t *pAttr,
    uint8_t *pValue, uint16_t len, uint16_t offset, uint16_t validCfg );

/**
 * @brief   Set a GAP GATT Server parameter.
 *
 * @param   param - Profile parameter ID<BR>
 * @param   len - length of data to right
 * @param   value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16_t will be cast to
 *          uint16_t pointer).<BR>
 *
 * @return  bStatus_t
 */
extern bStatus_t GGS_SetParameter( uint8_t param, uint8_t len, void *value );

/**
 * @brief   Get a GAP GATT Server parameter.
 *
 * @param   param - Profile parameter ID<BR>
 * @param   value - pointer to data to put.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16_t will be cast to
 *          uint16_t pointer).<BR>
 *
 * @return  bStatus_t
 */
extern bStatus_t GGS_GetParameter( uint8_t param, void *value );

/**
 * @brief   Add function for the GAP GATT Service.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 *
 * @return  SUCCESS: Service added successfully.<BR>
 *          INVALIDPARAMETER: Invalid service field.<BR>
 *          FAILURE: Not enough attribute handles available.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 */
extern bStatus_t GGS_AddService( uint32_t services );

/*-------------------------------------------------------------------
 * FUNCTIONS - Initialization and Configuration
 */

/**
 * @brief   Set a GAP Parameter value.  Use this function to change  the default GAP parameter values.
 *
 * @param   paramID - parameter ID: @ref GAP_PARAMETER_ID_DEFINES
 * @param   paramValue - new param value
 *
 * @return  SUCCESS or INVALIDPARAMETER (invalid paramID)
 */
extern bStatus_t GAP_SetParamValue( uint16_t paramID, uint16_t paramValue );

/**
 * @brief   Get a GAP Parameter value.
 *
 * @note    This function is the same as GAP_PasskeyUpdate(), except that
 *          the passkey is passed in as a non-string format.
 *
 * @param   paramID - parameter ID: @ref GAP_PARAMETER_ID_DEFINES
 *
 * @return  GAP Parameter value or 0xFFFF if invalid
 */
extern uint16_t GAP_GetParamValue( uint16_t paramID );

/**
 * @brief   Setup the device's address type.  If ADDRTYPE_PRIVATE_RESOLVE is selected,
 *          the address will change periodically.
 *
 * @param   addrType - @ref GAP_ADDR_TYPE_DEFINES
 * @param   pStaticAddr - Only used with ADDRTYPE_STATIC or ADDRTYPE_PRIVATE_NONRESOLVE type
 *                   NULL to auto generate otherwise the application can specify the address value
 *
 * @return  SUCCESS: address type updated,<BR>
 *          bleNotReady: Can't be called until GAP_DeviceInit() is called
 *                   and the init process is completed
 *          bleIncorrectMode: can't change with an active connection,or INVALIDPARAMETER
 *          If return value isn't SUCCESS, the address type remains the same as before this call.
 */
extern bStatus_t GAP_ConfigDeviceAddr( uint8_t addrType, uint8_t *pStaticAddr );

/**
 * @brief   Resolves a private address against an IRK.
 *
 * @param(in)   pIRK - pointer to the IRK
 * @param(in)   pAddr - pointer to the Resolvable Private address
 *
 * @param(out)  pIRK
 * @param(out)  pAddr
 *
 * @return  SUCCESS: match,<BR>
 *          FAILURE: don't match,<BR>
 *          INVALIDPARAMETER: parameters invalid<BR>
 */
extern bStatus_t GAP_ResolvePrivateAddr( uint8_t *pIRK, uint8_t *pAddr );

/**
 * @brief   Setup or change advertising and scan response data.
 *
 * @note    if the return status from this function is SUCCESS,the task isn't complete
 *          until the GAP_ADV_DATA_UPDATE_DONE_EVENT is sent to the calling application task.
 *
 * @param   taskID - task ID of the app requesting the change
 * @param   adType - TRUE - advertisement data, FALSE  - scan response data
 * @param   dataLen - Octet length of advertData
 * @param   pAdvertData - advertising or scan response data
 *
 * @return  SUCCESS: data accepted
 *          bleIncorrectMode: invalid profile role
 */
extern bStatus_t GAP_UpdateAdvertisingData( uint8_t taskID, uint8_t adType, uint16_t dataLen, uint8_t *pAdvertData );

/*-------------------------------------------------------------------
 * FUNCTIONS - GAP Bond API
 */
/**
 * @brief       Set a GAP Bond Manager parameter.
 *
 * @note    You can call this function with a GAP Parameter ID and it will set the GAP Parameter.
 *
 * @param   param - Profile parameter ID: @ref GAPBOND_PROFILE_PARAMETERS
 * @param   len - length of data to write
 * @param   pValue - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16_t will be cast to
 *          uint16_t pointer).
 *
 * @return      SUCCESS or INVALIDPARAMETER (invalid paramID)
 */
extern bStatus_t GAPBondMgr_SetParameter( uint16_t param, uint8_t len, void *pValue );

/**
 * @brief   Get a GAP Bond Manager parameter.
 *
 * @note    You can call this function with a GAP Parameter ID and it will get a GAP Parameter.
 *
 * @param   param - Profile parameter ID: @ref GAPBOND_PROFILE_PARAMETERS
 * @param   pValue - pointer to location to get the value.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate data type.
 *          (example: data type of uint16_t will be cast to uint16_t pointer)
 *
 * @return      SUCCESS or INVALIDPARAMETER (invalid paramID)
 */
extern bStatus_t GAPBondMgr_GetParameter( uint16_t param, void *pValue );

/**
 * @brief   Respond to a passcode request.
 *
 * @param   connectionHandle - connection handle of the connected device or 0xFFFF if all devices in database.
 * @param   status - SUCCESS if passcode is available, otherwise see @ref SMP_PAIRING_FAILED_DEFINES.
 * @param   passcode - integer value containing the passcode.
 *
 * @return  SUCCESS - bond record found and changed
 *          bleIncorrectMode - Link not found.
 */
extern bStatus_t GAPBondMgr_PasscodeRsp( uint16_t connectionHandle, uint8_t status, uint32_t passcode );

/**
 * @brief   Send a security request
 *
 * @param   connHandle - connection handle
 *
 * @return  SUCCESS: will send
 *          bleNotConnected: Link not found
 *          bleIncorrectMode: wrong GAP role, must be a Peripheral Role
 */
extern bStatus_t GAPBondMgr_PeriSecurityReq( uint16_t connHandle );

/*-------------------------------------------------------------------
 * FUNCTIONS - GAPRole API
 */
/**
 * @brief   Set a GAP Role parameter.
 *
 * @note    You can call this function with a GAP Parameter ID and it will set a GAP Parameter.
 *
 * @param   param - Profile parameter ID: @ref GAPROLE_PROFILE_PARAMETERS
 * @param   len - length of data to write
 * @param   pValue - pointer to data to write.  This is dependent on the parameter ID and
 *                   WILL be cast to the appropriate data type (example: data type of uint16_t
 *                   will be cast to uint16_t pointer).
 *
 * @return  SUCCESS or INVALIDPARAMETER (invalid paramID)
 */
extern bStatus_t GAPRole_SetParameter( uint16_t param, uint16_t len, void *pValue );

/**
 * @brief   Get a GAP Role parameter.
 *
 * @note    You can call this function with a GAP Parameter ID and it will get a GAP Parameter.
 *
 * @param   param - Profile parameter ID: @ref GAPROLE_PROFILE_PARAMETERS
 * @param   pValue - pointer to location to get the value.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16_t will be cast to
 *          uint16_t pointer).
 *
 * @return      SUCCESS or INVALIDPARAMETER (invalid paramID)
 */
extern bStatus_t GAPRole_GetParameter( uint16_t param, void *pValue );

/**
 * @brief       Terminates the existing connection.
 *
 * @return      SUCCESS or bleIncorrectMode
 */
extern bStatus_t GAPRole_TerminateLink( uint16_t connHandle );

/**
 * @brief   Read Rssi Cmd.
 *
 * @param   connHandle - connection handle
 *
 * @return  bStatus_t: HCI Error Code.<BR>
 *          
 */
extern bStatus_t GAPRole_ReadRssiCmd( uint16_t connHandle );

/**
 * @brief   used to synchronize with a periodic advertising train from an advertiser and
 *          begin receiving periodic advertising packets.
 *
 * @param   pSync - sync parameters@ gapCreateSync_t
 *
 * @return  bStatus_t: HCI Error Code.<BR>
 *
 */
extern bStatus_t GAPRole_CreateSync( gapCreateSync_t *pSync );

/**
 * @brief   used to cancel the HCI_LE_Periodic_Advertising_Create_Sync command while
 *          it is pending.
 *
 * @param   None.
 *
 * @return  bStatus_t: HCI Error Code.<BR>
 *
 */
extern bStatus_t GAPRole_CancelSync( void );

/**
 * @brief   used to stop reception of the periodic advertising train identified
 *          by the Sync_Handle parameter.
 *
 * @param   syncHandle-identifying the periodic advertising train
 *
 * @return  bStatus_t: HCI Error Code.<BR>
 *
 */
extern bStatus_t GAPRole_TerminateSync( uint16_t syncHandle );

/**
 * @brief   Update the link connection parameters.
 *
 * @param   connHandle - connection handle
 * @param   connIntervalMin - minimum connection interval in 1.25ms units
 * @param   connIntervalMax - maximum connection interval in 1.25ms units
 * @param   connLatency - number of LL latency connection events
 * @param   connTimeout - connection timeout in 10ms units
 *
 * @return  SUCCESS: Connection update started started.<BR>
 *          bleIncorrectMode: No connection to update.<BR>
 */
extern bStatus_t GAPRole_UpdateLink( uint16_t connHandle, uint16_t connIntervalMin,
    uint16_t connIntervalMax, uint16_t connLatency, uint16_t connTimeout );

/**
 * @brief   Update the connection phy.
 *
 * @param   connHandle - connection handle
 * @param   all_phys - a bit field that  allows the Host to specify, for each direction
 *                     set BIT0:The Host has no preference among the transmitter PHYs supported by the Controller
 *                     set BIT1:The Host has no preference among the receiver PHYs supported by the Controller
 * @param   tx_phys - a bit field that indicates the transmitter PHYs.(GAP_PHY_BIT_TYPE)
 * @param   rx_phys - a bit field that indicates the receiver PHYs.(GAP_PHY_BIT_TYPE)
 * @param   phy_options - preferred coding when transmitting on the LE Coded PHY(0:no preferred 1:S=2 2:S=8)
 *
 * @return  SUCCESS: PHY update started started .<BR>
 *          bleIncorrectMode: No connection to update.<BR>
 */
extern bStatus_t GAPRole_UpdatePHY( uint16_t connHandle, uint8_t all_phys, uint8_t tx_phys,\
    uint8_t rx_phys, uint16_t phy_options );

/**
 * @brief   used to allow the Host to specify the privacy mode to be used  for a given entry on the resolving list.
 *
 * @note    This command shall not be used when address resolution is enabled in the Controller and:
 *          Advertising (other than periodic advertising) is enabled,
 *          Scanning is enabled, or
 *          an GAPRole_CentralEstablishLink, or GAPRole_CreateSync command is pending.
 *
 * @param   addrTypePeer - 0x00:Public Identity Address 0x01:Random (static) Identity Address
 * @param   peerAddr - Public Identity Address or Random (static) Identity Address of the advertiser
 * @param   privacyMode - 0x00:Use Network Privacy Mode for this peer device (default)
 *                        0x01:Use Device Privacy Mode for this peer device
 *
 * @return  Command Status.
 *
 */
extern bStatus_t GAPRole_SetPrivacyMode( uint8_t addrTypePeer, uint8_t *peerAddr, uint8_t privacyMode );

/**
 * @brief   used to set the path loss threshold reporting parameters.
 *
 * @param   pParm - set path loss parameters@ gapRoleSetPathLossReporting_t
 *
 * @return  Command Status.
 *
 */
extern bStatus_t GAPRole_SetPathLossReporting( gapRoleSetPathLossReporting_t *pParm );

/*-------------------------------------------------------------------
 * FUNCTIONS - BROADCASTER_PROFILE_API Broadcaster Profile API
 */
/**
 *
 * @brief   Initialization function for the GAP Role Task.
 *
 * @param   the ID assigned by tmos.This ID should be used to send messages and set timers.
 *
 * @return  SUCCESS,bleInvalidRange
 */
extern bStatus_t GAPRole_BroadcasterInit( void );

/**
 * @brief   Does the device initialization.  Only call this function once.
 *
 * @param   pAppCallbacks - pointer to application callbacks.
 *
 * @return  SUCCESS or bleAlreadyInRequestedMode
 */
extern bStatus_t GAPRole_BroadcasterStartDevice( gapRolesBroadcasterCBs_t *pAppCallbacks );

/**
 * @brief   Does the Broadcaster receive scan request call initialization.
 *
 * @param   pAppCallbacks - pointer to application callbacks.
 *
 * @return  None
 */
extern void GAPRole_BroadcasterSetCB( gapRolesBroadcasterCBs_t *pAppCallbacks );

/*-------------------------------------------------------------------
 * FUNCTIONS - OBSERVER_PROFILE_API Observer Profile API
 */
/**
 * @internal
 *
 * @brief   Observer Profile Task initialization function.
 *
 * @param   taskId - Task ID.
 *
 * @return  SUCCESS,bleInvalidRange
 */
extern bStatus_t GAPRole_ObserverInit( void );

/**
 * @brief   Start the device in Observer role.  This function is typically
 *          called once during system startup.
 *
 * @param   pAppCallbacks - pointer to application callbacks
 *
 * @return  SUCCESS: Operation successful.<BR>
 *          bleAlreadyInRequestedMode: Device already started.<BR>
 */
extern bStatus_t GAPRole_ObserverStartDevice( gapRoleObserverCB_t *pAppCallbacks );

/**
 * @brief   Start a device discovery scan.
 *
 * @param   mode - discovery mode: @ref GAP_DEVDISC_MODE_DEFINES
 * @param   activeScan - TRUE to perform active scan
 * @param   whiteList - TRUE to only scan for devices in the white list
 *
 * @return  SUCCESS: Discovery scan started.<BR>
 *          bleIncorrectMode: Invalid profile role.<BR>
 *          bleAlreadyInRequestedMode: Not available.<BR>
 */
extern bStatus_t GAPRole_ObserverStartDiscovery( uint8_t mode, uint8_t activeScan, uint8_t whiteList );

/**
 * @brief   Cancel a device discovery scan.
 *
 * @return  SUCCESS: Cancel started.<BR>
 *          bleInvalidTaskID: Not the task that started discovery.<BR>
 *          bleIncorrectMode: Not in discovery mode.<BR>
 */
extern bStatus_t GAPRole_ObserverCancelDiscovery( void );

/*-------------------------------------------------------------------
 * FUNCTIONS - PERIPHERAL_PROFILE_API Peripheral Profile API
 */
/**
 * @internal
 *
 * @brief   Initialization function for the GAP Role Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notificaiton ... ).
 *
 * @param   the ID assigned by tmos.This ID should be used to send messages and set timers.
 *
 * @return  SUCCESS,bleInvalidRange
 */
extern bStatus_t GAPRole_PeripheralInit( void );

/**
 * @brief   Does the device initialization.  Only call this function once.
 *
 * @param   pAppCallbacks - pointer to application callbacks.
 *
 * @return  SUCCESS or bleAlreadyInRequestedMode
 */
extern bStatus_t GAPRole_PeripheralStartDevice( uint8_t taskid, gapBondCBs_t *pCB, gapRolesCBs_t *pAppCallbacks );

/**
 * @brief   Update the parameters of an existing connection
 *
 * @param   connHandle - the connection Handle
 * @param   connIntervalMin - minimum connection interval in 1.25ms units
 * @param   connIntervalMax - maximum connection interval in 1.25ms units
 * @param   latency - the new slave latency
 * @param   connTimeout - the new timeout value
 * @param   taskId - taskID will recv L2CAP_SIGNAL_EVENT message
 *
 * @return  SUCCESS, bleNotConnected or bleInvalidRange
 */
extern bStatus_t GAPRole_PeripheralConnParamUpdateReq( uint16_t connHandle, uint16_t connIntervalMin,
    uint16_t connIntervalMax, uint16_t latency, uint16_t connTimeout, uint8_t taskId );

/*-------------------------------------------------------------------
 * FUNCTIONS - CENTRAL_PROFILE_API Central Profile API
 */
/**
 * @internal
 *
 * @brief   Central Profile Task initialization function.
 *
 * @param   taskId - Task ID.
 *
 * @return  SUCCESS,bleInvalidRange
 */
extern bStatus_t GAPRole_CentralInit( void );

/**
 * @brief   Start the device in Central role.  This function is typically
 *          called once during system startup.
 *
 * @param   pAppCallbacks - pointer to application callbacks
 *
 * @return  SUCCESS: Operation successful.<BR>
 *          bleAlreadyInRequestedMode: Device already started.<BR>
 */
extern bStatus_t GAPRole_CentralStartDevice( uint8_t taskid, gapBondCBs_t *pCB, gapCentralRoleCB_t *pAppCallbacks );

/**
 * @brief   Start a device discovery scan.
 *
 * @param   mode - discovery mode: @ref GAP_DEVDISC_MODE_DEFINES
 * @param   activeScan - TRUE to perform active scan
 * @param   whiteList - TRUE to only scan for devices in the white list
 *
 * @return  SUCCESS: Discovery scan started.<BR>
 *          bleIncorrectMode: Invalid profile role.<BR>
 *          bleAlreadyInRequestedMode: Not available.<BR>
 */
extern bStatus_t GAPRole_CentralStartDiscovery( uint8_t mode, uint8_t activeScan, uint8_t whiteList );

/**
 * @brief   Cancel a device discovery scan.
 *
 * @return  SUCCESS: Cancel started.<BR>
 *          bleInvalidTaskID: Not the task that started discovery.<BR>
 *          bleIncorrectMode: Not in discovery mode.<BR>
 */
extern bStatus_t GAPRole_CentralCancelDiscovery( void );

/**
 * @brief   This API is called by the Central to update the Host data channels
 *          initiating an Update Data Channel control procedure.
 *
 * @note    While it isn't specified,it is assumed that the Host expects an
 *          update channel map on all active connections and periodic advertise.
 *
 * input parameters
 *
 * @param  chanMap - A five byte array containing one bit per data channel
 *                   where a 1 means the channel is "used".
 *
 * @return  SUCCESS
 */
extern void GAPRole_SetHostChanClassification( uint8_t *chanMap );

/**
 * @brief   Establish a link to a peer device.
 *
 * @param   highDutyCycle -  TRUE to high duty cycle scan, FALSE if not
 * @param   whiteList - determines use of the white list: TRUE-enable
 * @param   addrTypePeer - address type of the peer device: @ref GAP_ADDR_TYPE_DEFINES
 * @param   peerAddr - peer device address
 *
 * @return  SUCCESS: started establish link process.<BR>
 *          bleIncorrectMode: invalid profile role.<BR>
 *          bleNotReady: a scan is in progress.<BR>
 *          bleAlreadyInRequestedMode: can't process now.<BR>
 *          bleNoResources: too many links.<BR>
 */
extern bStatus_t GAPRole_CentralEstablishLink( uint8_t highDutyCycle, uint8_t whiteList, uint8_t addrTypePeer, uint8_t *peerAddr );

/*-------------------------------------------------------------------
 * FUNCTIONS - RF_PHY Profile API
 */

/**
 * @brief   RF_PHY Profile Task initialization function.
 *
 * @param   None.
 *
 * @return  0 - success.
 */
extern bStatus_t RF_RoleInit( void );

/**
 * @brief   rf config.
 *
 * @param   pConfig - rf config parameters
 *
 * @return  0 - success.
 */
extern bStatus_t RF_Config( rfConfig_t *pConfig );

/**
 * @brief   rx mode.
 *
 * @param   txBuf - rx mode tx data
 * @param   txLen - rx mode tx length(0-251)
 * @param   pktRxType - rx mode rx package type
 *                      broadcast type(0xFF):receive all matching types,
 *                      others:receive match type or broadcast type
 * @param   pktTxType - rx mode tx package type(auto mode)
 *                      broadcast type(0xFF):received by all matching types;
 *                      others:only received by matching type
 *
 * @return  0 - success.
 */
extern bStatus_t RF_Rx( uint8_t *txBuf, uint8_t txLen, uint8_t pktRxType, uint8_t pktTxType );

/**
 * @brief   tx mode.
 *
 * @param   txBuf - tx mode tx data
 * @param   txLen - tx mode tx length(0-251)
 * @param   pktTxType - tx mode tx package type
 *                      broadcast type(0xFF):received by all matching types;
 *                      others:only received by matching type
 * @param   pktRxType - tx mode rx package type(auto mode)
 *                      broadcast type(0xFF):receive all matching types,
 *                      others:receive match type or broadcast type
 *
 * @return  0 - success.
 */
extern bStatus_t RF_Tx( uint8_t *txBuf, uint8_t txLen, uint8_t pktTxType, uint8_t pktRxType );

/**
 * @brief   shut down,stop tx/rx mode.
 *
 * @param   None.
 *
 * @return  0 - success.
 */
extern bStatus_t RF_Shut( void );

/**
 * @brief   rf mode set radio channel/frequency.
 *
 * @param   channel.
 *
 * @return  0 - success.
 */
extern void RF_SetChannel( uint32_t channel );

/**
 * @brief   shut down rf frequency hopping
 *
 * @param   None.
 *
 * @return  None.
 */
extern void RF_FrequencyHoppingShut( void );

/**
 * @brief       
 *
 * @param   resendCount - Maximum count of sending HOP_TX pdu,0 = unlimited.
 *
 * @return  0 - success.
 */
extern uint8_t RF_FrequencyHoppingTx( uint8_t resendCount );

/**
 * @brief       
 *
 * @param   timeoutMS - Maximum time to wait for receiving HOP_TX pdu(Time = n * 1mSec),0 = unlimited.
 *
 * @return  0 - success.1-fail.2-LLEMode error(shall AUTO)
 */
extern uint8_t RF_FrequencyHoppingRx( uint32_t timeoutMS );

/**
 * @brief   Erase FH bonded device
 *
 * @param   None.
 *
 * @return  None.
 */
extern void RF_BondingErase( void );

/**
 * @brief   single channel mode.
 *
 * @param   ch - rf channel,f=2402+ch*2 MHz, ch=0,...,39
 *
 * @return  0 - success.
 */
extern bStatus_t LL_SingleChannel( uint8_t ch );

/**
 * @brief   used to stop any test which is in progress.
 *
 * @param(in)   pPktNum - null
 *
 * @param(out)  the number of received packets.
 *
 * @return  0 - success.
 */
extern bStatus_t LL_TestEnd( uint8_t *pPktNum );

/**
 * @brief   used to start a test where the DUT receives test reference packets at a fixed interval
 *
 * input parameters
 *
 * @param   opcode = 0x201D
 *              pParm0 - RX_Channel
 *
 *          opcode = 0x2033
 *              pParm0 - RX_Channel
 *              pParm1 - PHY
 *              pParm2 - Modulation_Index
 *
 * @return  0 - success.
 */
extern bStatus_t API_LE_ReceiverTestCmd( uint8_t *pParm, uint16_t opcode );

/**
 * @brief   used to start a test where the DUT generates test reference packets at a fixed interval
 *
 * @param   opcode = 0x201E
 *              pParm 0 - TX_Channel
 *              pParm 1 - Test_Data_Length
 *              pParm 2 - Packet_Payload
 *
 *          opcode = 0x2034
 *              pParm 0 - TX_Channel
 *              pParm 1 - Test_Data_Length
 *              pParm 2 - Packet_Payload
 *              pParm 3 - PHY
 *
 * @return  0 - success.
 */
extern bStatus_t API_LE_TransmitterTestCmd( uint8_t *pParm, uint16_t opcode );

/**
 * @brief   used to stop any test which is in progress
 *
 * @param   None
 *
 * @return  0 - success.
 */
extern bStatus_t API_LE_TestEndCmd( void );

/**
 * @brief   used to set sensitivity level
 *
 * @param   None
 *
 * @return  None.
 */
extern void RFEND_SetSensitivity( void );

/**
 * @brief   used to set rf TxCtune value
 *
 * @param   pParm(in) - Must provide length of parameter followed by 6 bytes parameter
 *
 * @return  Command Status.
 */
extern bStatus_t RFEND_TXCtuneSet( uint8_t *pParm );

/**
 * @brief   used to get rf TxCtune value
 *
 * @param   pParm(out) - length of parameter(6) followed by 6 bytes parameter
 *
 * @return  Command Status.
 */
extern bStatus_t RFEND_TXCtuneGet( uint8_t *pParm );

/*
 * END @ Profile API
 */
/******************************************************************************/
#ifdef __cplusplus
}
#endif

#endif
