#include "board.h"
#include "ff.h"
#include "diskio.h"

ATTR_PLACE_AT_NONCACHEABLE FATFS s_sd_disk;

static const TCHAR HTML[] =        
        "<!DOCTYPE html>\n"
        "<html lang=\"zh\">\n"
        "<head>\n"
        "    <meta charset=\"UTF-8\">\n"
        "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
        "    <title>打开 MicroLink 文档</title>\n"
        "    <meta http-equiv=\"refresh\" content=\"0;url=https://microboot.readthedocs.io/zh-cn/latest/tools/microlink/microlink/\">\n"
        "</head>\n"
        "<body>\n"
        "    <p>如果没有自动跳转，请点击 <a href=\"https://microboot.readthedocs.io/zh-cn/latest/tools/microlink/microlink/\">这里</a>。</p>\n"
        "</body>\n"
        "</html>\n";


static const TCHAR VERSION[] =        
  "\
  V2.1.1 \r\n\
  1.优化RTTView,解决数据溢出问题;\r\n\
  V2.1.0 \r\n\
  1.增加RTTView;\r\n\
  V2.0.1 \r\n\
  1.修复下载时RST引脚不会拉低电平;\r\n\
  2.升级完固件自动删除;\r\n\
  3.为了方便区分串口，通过USB转串口发送数据LED灯闪烁，虚拟串口发送数据LED灯不闪烁;\r\n\
  V2.0.0 \r\n\
  1.支持SWD/JTAG接口，下载速度超越JLINK V12（时钟10Mhz）;\r\n\
  2.支持USB转串口，最大10M波特率无丢包;\r\n\
  3.支持python脚本，可以通过脚本指定下载算法;\r\n\
  4.支持Cortex-M系列U盘拖拽下载;\r\n\
  5.支持U盘离线下载，通过python脚本触发下载;\r\n\
  6.内置ymodem协议栈，通过python脚本触发;\r\n\
  7.支持系统固件升级，为后续添加更多功能;\r\n\
  8.采用winusb对window10免驱，即插即用;\r\n\
  9.支持3V3/5V大电流输出电源;\r\n\
  10.内置防倒灌和过流保护，外部电流无法反向流入USB口，防止损坏USB;\r\n\
  ";
        

static const TCHAR driver_num_buf[4] = {DEV_RAM + '0', ':', '/', '\0'};
static const char *show_error_string(FRESULT fresult);

FRESULT flash_mount_fs(void)
{
    FRESULT fresult;
    FIL file;
    UINT bw;
    FILINFO fno;
    DIR dir;
    // 尝试挂载文件系统
    fresult = f_mount(&s_sd_disk, driver_num_buf, 1);
    
    if (fresult == FR_OK) {
        printf("SD card has been mounted successfully\n");
        fresult = f_chdrive(driver_num_buf);
        if (fresult != FR_OK) {
            printf("Failed to change drive, cause: %s\n", show_error_string(fresult));
            return fresult;
        }
        // 遍历并删除 MicroLink*.rbl 文件
        if (f_opendir(&dir, "") == FR_OK) {
            while (f_readdir(&dir, &fno) == FR_OK && fno.fname[0]) {
                if (strncmp(fno.fname, "MicroLink", 9) == 0 && strstr(fno.fname, ".rbl")) {
                    printf("Deleting file: %s\n", fno.fname);
                    f_unlink(fno.fname);
                }
            }
            f_closedir(&dir);
        }
        // 检查 version.txt 是否存在
        fresult = f_open(&file, "version.txt", FA_READ | FA_OPEN_EXISTING);
        if (fresult == FR_OK) {
            char buffer[8] = {0};  // 存储读取的前 8 个字节
            UINT br;
    
            f_read(&file, buffer, sizeof(buffer), &br);
            f_close(&file);
    
            // 检查是否成功读取 8 字节，且内容不同
            if (br != 8 || memcmp(buffer, VERSION, 8) != 0) {
                // 重新写入 version.txt
                fresult = f_open(&file, "version.txt", FA_WRITE | FA_CREATE_ALWAYS);
                if (fresult == FR_OK) {
                    f_write(&file, VERSION, strlen(VERSION), &bw);
                    f_close(&file);
                    printf("version.txt updated successfully\n");
                } else {
                    printf("Failed to update version.txt, cause: %s\n", show_error_string(fresult));
                }
            }
        } else if (fresult == FR_NO_FILE) {
            // 文件不存在，创建 version.txt
            fresult = f_open(&file, "version.txt", FA_WRITE | FA_CREATE_ALWAYS);
            if (fresult == FR_OK) {
                f_write(&file, VERSION, strlen(VERSION), &bw);
                f_close(&file);
                printf("version.txt created successfully\n");
            } else {
                printf("Failed to create version.txt, cause: %s\n", show_error_string(fresult));
            }
        } else {
            printf("Failed to open version.txt, cause: %s\n", show_error_string(fresult));
        }

        // 检查 HTM.txt 是否存在
        fresult = f_open(&file, "MBED.HTM", FA_READ);
        if (fresult == FR_NO_FILE) {
          // 创建 HTM.txt 文件
          fresult = f_open(&file, "MBED.HTM", FA_WRITE | FA_CREATE_ALWAYS);
          if (fresult == FR_OK) {
              f_write(&file, HTML, strlen(HTML), &bw);
              f_close(&file);
              printf("MBED.HTM created successfully\n");
          } else {
              printf("Failed to create MBED.HTM, cause: %s\n", show_error_string(fresult));
          }   
        }            
    } else {

          printf("Failed to format SD card, cause: %s\n", show_error_string(fresult));     
    }
    
    return fresult;
}

static const char *show_error_string(FRESULT fresult)
{
    const char *result_str;

    switch (fresult) {
    case FR_OK:
        result_str = "succeeded";
        break;
    case FR_DISK_ERR:
        result_str = "A hard error occurred in the low level disk I/O level";
        break;
    case FR_INT_ERR:
        result_str = "Assertion failed";
        break;
    case FR_NOT_READY:
        result_str = "The physical drive cannot work";
        break;
    case FR_NO_FILE:
        result_str = "Could not find the file";
        break;
    case FR_NO_PATH:
        result_str = "Could not find the path";
        break;
    case FR_INVALID_NAME:
        result_str = "Tha path name format is invalid";
        break;
    case FR_DENIED:
        result_str = "Access denied due to prohibited access or directory full";
        break;
    case FR_EXIST:
        result_str = "Access denied due to prohibited access";
        break;
    case FR_INVALID_OBJECT:
        result_str = "The file/directory object is invalid";
        break;
    case FR_WRITE_PROTECTED:
        result_str = "The physical drive is write protected";
        break;
    case FR_INVALID_DRIVE:
        result_str = "The logical driver number is invalid";
        break;
    case FR_NOT_ENABLED:
        result_str = "The volume has no work area";
        break;
    case FR_NO_FILESYSTEM:
        result_str = "There is no valid FAT volume";
        break;
    case FR_MKFS_ABORTED:
        result_str = "THe f_mkfs() aborted due to any problem";
        break;
    case FR_TIMEOUT:
        result_str = "Could not get a grant to access the volume within defined period";
        break;
    case FR_LOCKED:
        result_str = "The operation is rejected according to the file sharing policy";
        break;
    case FR_NOT_ENOUGH_CORE:
        result_str = "LFN working buffer could not be allocated";
        break;
    case FR_TOO_MANY_OPEN_FILES:
        result_str = "Number of open files > FF_FS_LOCK";
        break;
    case FR_INVALID_PARAMETER:
        result_str = "Given parameter is invalid";
        break;
    default:
        result_str = "Unknown error";
        break;
    }
    return result_str;
}