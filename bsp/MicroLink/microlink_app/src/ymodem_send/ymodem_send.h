#ifndef APPLICATIONS_CHECK_AGENT_XMODEM_SEND_H_
#define APPLICATIONS_CHECK_AGENT_XMODEM_SEND_H_
#include "microboot.h"

typedef struct ymodem_send_t {
    ymodem_t      parent;
    char          chFileName[5][32];
    uint8_t       chfileNum;
    peek_byte_t   *ptReadByte;
    uint32_t      wFileSize;
    uint32_t      wOffSet;
    SIG_SLOT_OBJ;
} ymodem_send_t;

typedef struct ymodem_lib_send_t {
    check_agent_t tCheckAgent;
    ymodem_send_t tYmodemSent;
} ymodem_lib_send_t;

signals(ymodem_send_sig, ymodem_send_t *ptThis,
        args(
            uint8_t *pchByte,
            uint16_t hwLen
        ));

ymodem_lib_send_t *ymodem_lib_send_init(ymodem_lib_send_t *ptObj, peek_byte_t *ptReadByte);
#endif /* APPLICATIONS_CHECK_AGENT_XMODEM_H_ */
