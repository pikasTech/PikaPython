#include "pikaScript.h"
#include "ff.h"
#include "diskio.h"
#include "board.h"
#include "ymodem_send.h"

extern void ymodme_agent_register(void);
extern void ymodme_unagent_register(void);

ymodem_lib_send_t tYmodemLibSend;


void ymodem_state_handler(ymodem_state_t state)
{
    ymodme_unagent_register();
}

void ym_send(PikaObj *self, PikaTuple* val)
{
    
    tYmodemLibSend.tYmodemSent.chfileNum = pikaTuple_getSize(val);
    pika_platform_printf("file_count = %d\r\n",tYmodemLibSend.tYmodemSent.chfileNum);
    if(tYmodemLibSend.tYmodemSent.chfileNum > 0){
        for(uint8_t i = 0 ; i < tYmodemLibSend.tYmodemSent.chfileNum && i < 5 ; i++){
          Arg* arg = pikaTuple_getArg(val, i);
          char *pchFileName = arg_getStr(arg);
          strncpy(tYmodemLibSend.tYmodemSent.chFileName[i], pchFileName, strlen(pchFileName));
          pika_platform_printf("file%d name %s\r\n",i,tYmodemLibSend.tYmodemSent.chFileName[i]);
        }
        ymodme_agent_register();
    }else{
        pika_platform_printf("Wrong number of parameters\r\n");
    }
}



void ym_receive(PikaObj *self, PikaTuple* val)
{



    pika_platform_printf("ymodem_receive\r\n");

}



