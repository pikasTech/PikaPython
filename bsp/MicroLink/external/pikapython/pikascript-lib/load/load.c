#include "pikaScript.h"
#include "ff.h"
#include "diskio.h"
#include "file_stream.h"
#include "board.h"

static shared_state_t shared_state;

ATTR_RAMFUNC_WITH_ALIGNMENT(4) 
static uint8_t bin_buffer[4096];

void load_bin(PikaObj *self, PikaTuple* val)
{
    FIL file;
    FRESULT res;
    UINT bytesRead;
    FSIZE_t fileSize, remainingSize;
    int  wFlashAddr;  
    int val_num = pikaTuple_getSize(val);
    if( val_num % 2 != 0){
        pika_platform_printf("Wrong number of parameters %d\r\n",val_num);
        return;
    }
    for(int i = 0;i<val_num;i=i+2){
        Arg* arg = pikaTuple_getArg(val, i);
        char *pchFileName = arg_getStr(arg);
        arg = pikaTuple_getArg(val, i+1);
        wFlashAddr = (int)arg_getInt(arg);
        pika_platform_printf("fileName = %s, FlashAddr = 0x%x\n",pchFileName,wFlashAddr);
        res = f_open(&file, pchFileName, FA_READ);
        if (res != FR_OK) {
            pika_platform_printf("Failed to open %s file: %d\n",pchFileName, res);
            return;
        }
        fileSize = f_size(&file);
        pika_platform_printf("open fileName: %s success,file size: %d byte\n",pchFileName,fileSize);

        if(ERROR_SUCCESS != open_bin(&shared_state)){
            pika_platform_printf("Failed to open flash load blob\n");
            f_close(&file);
            return;
        }

        remainingSize = fileSize;
        FSIZE_t offset = 0;
        set_bin_start_address(wFlashAddr);
        while (remainingSize > 0) {
            f_lseek(&file, offset);
            res = f_read(&file, bin_buffer, sizeof(bin_buffer), &bytesRead);
            if (res != FR_OK) {
                f_close(&file);
                pika_platform_printf("Failed to read file: %d\n", res);
                return;
            }
        
            if(ERROR_SUCCESS_DONE_OR_CONTINUE != write_bin(&shared_state, wFlashAddr, bin_buffer, bytesRead)){
                f_close(&file);
                close_bin(&shared_state);   
                pika_platform_printf("Failed to write file,addr: 0x%x\n", wFlashAddr);
                return;
            }
            wFlashAddr += bytesRead;
            remainingSize -= bytesRead;
            offset += bytesRead;
            int progress = (int)((int)(fileSize - remainingSize) * 100) / (int)fileSize;
            pika_platform_printf("Download: %3d%%\r\n", progress);
        } 
        f_close(&file);
        close_bin(&shared_state);   
        pika_platform_printf(" %s loaded successfully.\n", pchFileName);
    }
}

void load_hex(PikaObj *self, PikaTuple* val)
{

    Arg* arg = pikaTuple_getArg(val, 0);
    pika_platform_printf("%s\n",arg_getStr(arg));	
    

}