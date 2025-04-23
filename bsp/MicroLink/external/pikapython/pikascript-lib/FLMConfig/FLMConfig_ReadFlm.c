#include "pikaScript.h"
#include "flash_blob.h"
#include "board.h"
#include "flash_blob.h"
#include "ff.h"
#include "file_stream.h"

ATTR_RAMFUNC flash_blob_t tFlashBlob;

uint32_t target_flash_blob(char *pchFileName,int wFlashAddr,int wRamBase)
{
    FIL file;
    FRESULT res;
    UINT bytesRead;
    FSIZE_t fileSize, remainingSize;

    // 打开FLM文件
    res = f_open(&file, pchFileName, FA_READ);
    if (res != FR_OK) {
        pika_platform_printf("Failed to open %s file: %d\n",pchFileName, res);
        return 0;
    }
    // 获取文件大小
    fileSize = f_size(&file);
    // 读取文件内容到 tFlashBlob.target
    res = f_read(&file, &tFlashBlob.target, sizeof(tFlashBlob.target), &bytesRead);
    if (res != FR_OK || bytesRead != sizeof(tFlashBlob.target)) {
        pika_platform_printf("Failed to read file: %d\n", res);
        f_close(&file);
        return 0;
    }
    // 计算剩余数据大小
    remainingSize = fileSize - sizeof(tFlashBlob.target);

    // 确保剩余大小不超过 prog_blob 缓冲区
    if (remainingSize > sizeof(tFlashBlob.prog_blob)) {
        pika_platform_printf("Warning: Remaining file size (%llu) exceeds prog_blob buffer size (%zu), truncating...\n", 
               remainingSize, sizeof(tFlashBlob.prog_blob));
        remainingSize = sizeof(tFlashBlob.prog_blob);  // 只读取最多能存的大小
    }
    // **偏移文件指针到 algo_blob 数据位置**
    res = f_lseek(&file, sizeof(tFlashBlob.target));
    if (res != FR_OK) {
        pika_platform_printf("Failed to seek to algo_blob position, error: %d\n", res);
        f_close(&file);
        return 0;
    }
    // 读取剩余的文件数据到 tFlashBlob.prog_blob
    res = f_read(&file, tFlashBlob.prog_blob, remainingSize, &bytesRead);
    if (res != FR_OK || bytesRead != remainingSize) {
        pika_platform_printf("Failed to read algo_blob, read %u bytes\n", bytesRead);
        f_close(&file);
        return 0;
    }

    f_close(&file);

    tFlashBlob.target.init += wRamBase;
    tFlashBlob.target.uninit += wRamBase;    
    tFlashBlob.target.erase_chip += wRamBase; 
    tFlashBlob.target.erase_sector += wRamBase; 
    tFlashBlob.target.program_page += wRamBase;  
    tFlashBlob.target.sys_call_s.static_base += wRamBase;  
    tFlashBlob.target.sys_call_s.breakpoint  += wRamBase;  
    tFlashBlob.target.sys_call_s.stack_pointer  += wRamBase; 
    tFlashBlob.target.program_buffer   += wRamBase;
    tFlashBlob.target.algo_start  += wRamBase;
    tFlashBlob.target.algo_blob = (uint32_t *)tFlashBlob.prog_blob;
    tFlashBlob.sector_info_length = fileSize - sizeof(tFlashBlob.target)-tFlashBlob.target.algo_size;;
    pika_platform_printf("\nInit_addr = 0x%x\r\n",tFlashBlob.target.init);
    pika_platform_printf("uninit_addr = 0x%x\r\n",tFlashBlob.target.uninit);
    pika_platform_printf("erase_chip_addr = 0x%x\r\n",tFlashBlob.target.erase_chip);
    pika_platform_printf("erase_sector_addr = 0x%x\r\n",tFlashBlob.target.erase_sector);
    pika_platform_printf("program_page_addr = 0x%x\r\n",tFlashBlob.target.program_page);

    return (uint32_t)fileSize;
}


int FLMConfig_ReadFlm_load(PikaObj *self, PikaTuple* val)
{
    int  wFlashAddr, wRamBase;  
    if(pikaTuple_getSize(val) != 3){
        pika_platform_printf("Wrong number of parameters\r\n");
    }
    Arg* arg = pikaTuple_getArg(val, 0);
    char *pchFileName = arg_getStr(arg);
    arg = pikaTuple_getArg(val, 1);
    wFlashAddr = (int)arg_getInt(arg);
    arg = pikaTuple_getArg(val, 2);
    wRamBase = (int)arg_getInt(arg);
    if(target_flash_blob(pchFileName,wFlashAddr,wRamBase)){
        set_bin_start_address(wFlashAddr);
        pika_platform_printf("Load FLM Success,FileName = %s, FlashAddr = 0x%x, RamBase = 0x%x\n",pchFileName,wFlashAddr, wRamBase);
    }
    return 1;
}



