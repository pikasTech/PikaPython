#include "main.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "ff.h"
#include "sfud.h"

/* 桌面图片变量定义 */
lv_img_dsc_t bin_wallpaper;

/* user_img 描述信息结构体 */
typedef struct
{
    lv_img_dsc_t *icon_img; /* 图片变量的指针 */
    const char *icon_fname; /* 文件名 */

} user_img_item;

static const user_img_item user_img[] =
    {
        //wallpaper
        {&bin_wallpaper, "P:0:/wp1_ver.bin"},

};

FRESULT sdcard2spiflash(const char *psrc, const char *pdst) //真正搬运数据用到的函数
{
    FRESULT res;
    uint16_t br = 0;
    uint16_t bw = 0;
    FIL fsrc;
    FIL fdst;
    uint8_t tempbuf[4096];

    res = f_open(&fsrc, (const TCHAR *)psrc, FA_READ | FA_OPEN_EXISTING); //打开只读文件
    if (res == FR_OK)
        res = f_open(&fdst, (const TCHAR *)pdst, FA_WRITE | FA_CREATE_ALWAYS); //第一个打开成功,才开始打开第二个
    if (res == FR_OK)                                                          //两个都打开成功了
    {
        while (res == 0) //开始复制
        {
            res = f_read(&fsrc, tempbuf, 4096, (UINT *)&br); //源头读出512字节
            if (res || br == 0)
                break;
            res = f_write(&fdst, tempbuf, (UINT)br, (UINT *)&bw); //写入目的文件
            if (res || bw < br)
                break;
        }
        f_close(&fsrc);
        f_close(&fdst);
    }
    return res;
}

void load_files(void)
{
    FATFS sdcard_fs;
    FATFS flash_fs;
    if (f_mount(&sdcard_fs, "1:", 1) != FR_OK) //sdcard挂载失败认为未插入sdcard
    {
        if (f_mount(&flash_fs, "0:", 1) != FR_OK) //spiflash也挂载失败，说明spiflash未格式化，需要插入sdcard来格式化spiflash
        {
            debugerror("spi flash mount error.\r\n");
            lv_obj_t *label_info = lv_label_create(lv_scr_act(), NULL);
            lv_label_set_text(label_info, LV_SYMBOL_WARNING "spi flash mount error.\nPlease insert an SD Card and try again.");
            lv_obj_align(label_info, NULL, LV_ALIGN_CENTER, 0, 0);
            lv_task_handler();
            gui2show();
            while (1)
                ;
        }
    }
    else //sdcard挂载成功则格式化spiflash，拷贝相应文件到spiflash
    {
        BYTE work[FF_MAX_SS];
        if (f_mkfs("0:", FM_ANY, 0, work, sizeof work) != FR_OK) //格式化spiflash
        {
            debugerror("spi flash formatting error.\r\n");
            lv_obj_t *label_info = lv_label_create(lv_scr_act(), NULL);
            lv_label_set_text(label_info, LV_SYMBOL_WARNING "spi flash formatting error.");
            lv_obj_align(label_info, NULL, LV_ALIGN_CENTER, 0, 0);
            lv_task_handler();
            gui2show();
            while (1)
                ;
        }
        else
        {
            if (f_mount(&flash_fs, "0:", 1) != FR_OK) //挂载spiflash
            {
                debugerror("spi flash mount error.\r\n");
                lv_obj_t *label_info = lv_label_create(lv_scr_act(), NULL);
                lv_label_set_text(label_info, LV_SYMBOL_WARNING "spi flash mount error.");
                lv_obj_align(label_info, NULL, LV_ALIGN_CENTER, 0, 0);
                lv_task_handler();
                gui2show();
                while (1)
                    ;
            }
            else
            {
                char psrc[64] = {"1:/yahei11.bin"}, pdst[64] = {"0:/yahei11.bin"};

                lv_obj_t *label_copy_head = lv_label_create(lv_scr_act(), NULL);
                lv_obj_t *label_copy_info = lv_label_create(lv_scr_act(), NULL);
                lv_label_set_text(label_copy_head, "sdcard,spiflash ok,copying files");
                lv_obj_align(label_copy_head, NULL, LV_ALIGN_CENTER, 0, -20);
                // debug("%s\r\n", psrc);
//                lv_label_set_text(label_copy_info, psrc);
//                lv_obj_align(label_copy_info, NULL, LV_ALIGN_CENTER, 0, 0);
//                lv_task_handler();
//                gui2show();

                for (uint32_t i = 0; i < sizeof(user_img) / sizeof(user_img[0]); i++)
                {
                    strcpy(psrc, "1");
                    strcpy(pdst, "0");
                    strcat(psrc, &(user_img[i].icon_fname[3]));
                    strcat(pdst, &(user_img[i].icon_fname[3]));
                    // debug("psrc %s\r\n", psrc);
                    // debug("pdst %s\r\n", pdst);

                    lv_label_set_text(label_copy_info, psrc);
                    lv_obj_align(label_copy_info, NULL, LV_ALIGN_CENTER, 0, 0);
                    lv_task_handler();
                    gui2show();

                    if (sdcard2spiflash(psrc, pdst) != FR_OK)
                    {
                        lv_label_set_text(label_copy_info, LV_SYMBOL_WARNING "sdcard to spi flash error.");
                        lv_obj_align(label_copy_info, NULL, LV_ALIGN_CENTER, 0, 0);
                        lv_task_handler();
                        gui2show();
                        while (1)
                            ;
                    }
                }
                lv_obj_del(label_copy_head);
                lv_obj_del(label_copy_info);
            }
        }
    }
}
uint32_t resource_addr = SDRAMM_BASE + 1024 * 1024; //从SDRAM内存的1M处开始存放，先存字体，再存图片
/**
* @brief 从文件系统获取bmp图片
* @param image-存储图片信息的指针
* @param file_name-文件名
* @retval	0-加载成功 非0-加载失败
*/
uint8_t lv_load_img_bin_from_file(lv_img_dsc_t *image, const char *file_name)
{
    lv_fs_file_t sd_file;
    lv_fs_res_t lv_fs_res = LV_FS_RES_OK;
    uint32_t file_size;
    uint32_t temp; //读取数据缓冲
    uint32_t br;
    uint32_t header;

    if (image == NULL)
        return 1;
    if (file_name == NULL)
        return 1;

    lv_fs_res = lv_fs_open(&sd_file, file_name, LV_FS_MODE_RD);
    if (lv_fs_res != LV_FS_RES_OK)
    {
        return 1;
    }
    lv_fs_size(&sd_file, &file_size);
    // debug("file name %s, size %d\r\n",file_name,file_size);
    if (file_size > LV_HOR_RES * LV_VER_RES * 4)
    {
        /* 文件过大了 */
        lv_fs_close(&sd_file);
        return 2;
    }

    lv_fs_res = lv_fs_read(&sd_file, &temp, 4, &br);
    if ((lv_fs_res != LV_FS_RES_OK) || (br == 0))
    {
        lv_fs_close(&sd_file);
        return 3;
    }
    /* 获取头 */
    header = temp;
    // debug("header:0X%X\r\n",header);
    // debug("width:%d height:%d\r\n", (uint16_t)(header >> 10) & 0x7ff, header >> 21);
    image->header.cf = header & 0x1F;
    image->header.always_zero = 0;
    image->header.w = (uint16_t)(header >> 10) & 0x7FF;
    image->header.h = (uint16_t)(header >> 21);
    image->data_size = file_size - 4; //去掉4字节的头,剩余的就是图片数据流长度
    image->data = (const uint8_t *)resource_addr;

    //图片bin文件为4字节图片结构体+rgb数据,保证图片长宽为偶数
    for (uint32_t i = 0; i < file_size / 4; i++)
    {
        lv_fs_res = lv_fs_read(&sd_file, &temp, 4, &br);
        if ((lv_fs_res != LV_FS_RES_OK) || (br == 0))
        {
            lv_fs_close(&sd_file);
            return 0;
        }
        *(uint32_t *)resource_addr = temp;
        resource_addr += 4;
    }

    lv_fs_close(&sd_file);

    return 0;
}

/**
* @brief 从文件系统加载所有图片到内存
* @param None
* @retval	0-加载成功 非0-加载失败
*/
uint8_t lv_load_user_img_from_file(void)
{
    lv_obj_t *label_info = lv_label_create(lv_scr_act(), NULL);
    for (uint32_t i = 0; i < sizeof(user_img) / sizeof(user_img[0]); i++)
    {
        if (lv_load_img_bin_from_file(user_img[i].icon_img, user_img[i].icon_fname) != 0)
            return 1;
        lv_label_set_text(label_info, user_img[i].icon_fname);
        lv_obj_align(label_info, NULL, LV_ALIGN_CENTER, 0, 40);
        lv_task_handler();
        gui2show();
    }

    lv_obj_del(label_info);
    return 0;
}
