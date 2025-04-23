/* WALI Serve config file */
#ifndef __WL_CFG_H__
#define __WL_CFG_H__

// <<< Use Configuration Wizard in Context Menu >>>

// <h>Log Configuration
// <c1>enable kernel debug configuration
//  <i>Default: enable kernel debug configuration
//#define RT_DEBUG
// </c>
// <o>enable components initialization debug configuration<0-1>
//  <i>Default: 0
#define RT_DEBUG_INIT 0
// <c1>thread stack over flow detect
//  <i> Diable Thread stack over flow detect
//#define RT_USING_OVERFLOW_CHECK
// </c>
// </h>
// <e>Msg Map Configuration
// <i> Enables msg_map
#define USING_MSG_MAP         1
#if USING_MSG_MAP == 0
    #undef USING_MSG_MAP
#endif
// <o>The maximum length of parameters <0-128>
//  <i>Default: 128
#define MSG_ARG_LEN             128
// <o>The maximum number of parameters <0-16>
//  <i>Default: 8
#define MSG_ARG_MAX             8
// </e>

// <h>Flash BLOB Configuration
// <c1>Flash BLOB Management
//  <i>Flash BLOB Management
#define USING_FLASH_BLOB
// </c>
// </h>

// <h>Check Use Peek
// <c1>check use peek engine
//  <i>check use peek engine
#define USING_CHECK_ENGINE
// </c>
// </h>

// <e>Shell Configuration
// <i> Enables shell
#define WL_USING_SHELL         1
#if WL_USING_SHELL == 0
    #undef WL_USING_SHELL
#endif
// <q>Use echo mode
// <i> FINSH_OPTION_ECHO=0x01 is echo mode, other values are none-echo mode.
#define SHELL_OPTION_ECHO                             1
// <o>The maximum number of history lines <0-16>
//  <i>Default: 8
#define SHELL_HISTORY_LINES                           5
// </e>
// <h>IPC(Inter-process communication) Configuration
// <c1>Using Ring Queue
//  <i>Using Ring Queue
#define USING_RINGEQUEUE
// </c>
// <c1>Using Signals-Slots
//  <i>Using Signals-Slots
#define USING_SIGNALS_SLOTS
// </c>
// <c1>Using Subscribe-Publish
//  <i>Using Subscribe-Publish
#define USING_SUBSCRIBE_PUBLISH
// </c>
// <c1>Using Event
//  <i>Using Event
//#define WL_USING_EVENT
// </c>
// <c1>Using MailBox
//  <i>Using MailBox
//#define WL_USING_MAILBOX
// </c>
// </h>
// <h>Memory Management Configuration
// <c1>Memory Pool Management
//  <i>Memory Pool Management
//#define RT_USING_MEMPOOL
// </c>
// </h>
// <e>BootLoader Configuration
// <i> Enables BootLoader
#define USING_BOOT         1
#if USING_BOOT == 0
    #undef USING_BOOT
#endif
// <o>The starting address of the download
//  <i>Default: 0x8020000
//  <i>Must align with flash sector size
#define DOWNLOAD_PART_ADDR          0x80072000UL
// <o>The download size
//  <i>Default: 0X60000
//  <i>Must align with flash sector size
#define DOWNLOAD_PART_SIZE          0x80000
// <o>The starting address of the app
//  <i>Default: 0x8020000
//  <i>Must align with flash sector size
#define APP_PART_ADDR               0x80010400UL
// <o>The reset vector offset in the application
//  <i>Default: 0x4
//  <i>The offset of the reset vector from the application start
#define APP_PART_OFFSET             0x00002C00UL
// <o>The app size
//  <i>Default: 0X60000
//  <i>Must align with flash sector size
#define APP_PART_SIZE               0x60000UL
// <o>The Boot Flash Ops Addr
//  <i>Default: 0x08001000
//  <i>Must align with flash sector size
#define BOOT_FLASH_OPS_ADDR         0x80002000UL
// </e>

// <<< end of configuration section >>>
#endif
