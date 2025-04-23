#include "pikaScript.h"
#include "ymodem_send.h"
#include "ff.h"
#include "diskio.h"
#include "board.h"

static FIL file;

ATTR_RAMFUNC_WITH_ALIGNMENT(4) 
static uint8_t s_chQueueBuffer[1024] ;
static uint16_t ymodem_send_file_name(ymodem_t *ptObj, uint8_t *pchBuffer, uint16_t hwSize)
{
   static uint8_t file_count = 0;  
    FRESULT res;
    ymodem_send_t *(ptThis) = (ymodem_send_t *)ptObj;

    if(0 == this.chfileNum) {
       file_count = 0;
       this.wFileSize = 0;
       this.wOffSet = 0;
       return 0;
    }

    res = f_open(&file, this.chFileName[file_count], FA_READ);
    if (res != FR_OK) {
        pika_platform_printf("Failed to open %s file: %d\n",this.chFileName[file_count], res);
        return 0;
    }
    this.wFileSize = (uint32_t)f_size(&file);
    sprintf((char *)pchBuffer, "%s%c%d", this.chFileName[file_count], '\0', this.wFileSize);
    file_count++;
    this.chfileNum--;
    return hwSize;
}

static uint16_t ymodem_send_file_data(ymodem_t *ptObj, uint8_t *pchBuffer, uint16_t hwSize)
{
    FRESULT res;
    ymodem_send_t *(ptThis) = (ymodem_send_t *)ptObj;
    UINT hwReadSize;
    res = f_read(&file, pchBuffer, hwSize, &hwReadSize);
    if (res != FR_OK) {
        f_close(&file);
        pika_platform_printf("Failed to read file: %d\n", res);
        return 0;
    }
    return hwReadSize;
}

static uint16_t ymodem_read_data(ymodem_t *ptObj, uint8_t* pchByte, uint16_t hwSize)
{
    ymodem_send_t *(ptThis) = (ymodem_send_t *)ptObj;
    uint16_t hwReceiveSize = this.ptReadByte->fnGetByte(this.ptReadByte, pchByte, hwSize);
    return hwReceiveSize;
}

static uint16_t ymodem_write_data(ymodem_t *ptObj, uint8_t* pchByte, uint16_t hwSize)
{
    static uint8_t old_per = 0;
    ymodem_send_t *(ptThis) = (ymodem_send_t *)ptObj;
    assert(NULL != ptObj);
    emit(ymodem_send_sig, ptThis,
         args(
             pchByte,
             hwSize
         ));
    this.wOffSet += hwSize;
    if(this.wFileSize > 0){
        uint8_t progress = (this.wOffSet) * 100 / this.wFileSize;
        if(progress >= 100){
            progress = 100;
        }
        if(old_per != progress){
            old_per = progress;
            pika_platform_printf("send: %3d%%\r\n", progress);
        }
    }
    return hwSize;
}

fsm_rt_t ymodem_lib_send(ymodem_t *ptObj)
{
    ymodem_state_t tState = ymodem_send(ptObj);

    if(tState == STATE_ON_GOING) {
        return fsm_rt_on_going;
    }else if(tState == STATE_INCORRECT_NBlk || tState == STATE_INCORRECT_CHAR) {
        return fsm_rt_user_req_drop;
    }else if(tState == STATE_TIMEOUT) {
        return fsm_rt_user_req_timeout;
    }else if(tState == STATE_FINSH ){
        return fsm_rt_cpl;
    }else {
        return fsm_rt_cpl;
    }
}

ymodem_lib_send_t *ymodem_lib_send_init(ymodem_lib_send_t *ptObj, peek_byte_t *ptReadByte)
{
    ymodem_lib_send_t *(ptThis) = ptObj;
    assert(NULL != ptObj);
    this.tCheckAgent.pAgent = &this.tYmodemSent.parent;
    this.tCheckAgent.fnCheck = (check_hanlder_t *)ymodem_lib_send;
    this.tCheckAgent.ptNext = NULL;
    this.tCheckAgent.hwPeekStatus = 0;
    this.tCheckAgent.bIsKeepingContext = true;
    this.tYmodemSent.ptReadByte = ptReadByte;
    ymodem_ops_t s_tOps = {
        .pchBuffer = s_chQueueBuffer,
        .hwSize = sizeof(s_chQueueBuffer),
        .fnOnFileData = ymodem_send_file_data,
        .fnOnFilePath = ymodem_send_file_name,
        .fnReadData = ymodem_read_data,
        .fnWriteData = ymodem_write_data
    };
    ymodem_init(&this.tYmodemSent.parent, &s_tOps);

    return ptObj;
}

