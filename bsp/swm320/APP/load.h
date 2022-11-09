#ifndef __LOAD_H__
#define __LOAD_H__

extern uint32_t resource_addr;

/* 桌面图片变量定义 */
extern lv_img_dsc_t bin_wallpaper;
extern lv_img_dsc_t bin_wallpaper_chip;
extern lv_img_dsc_t bin_evernote;
extern lv_img_dsc_t bin_facebook;
extern lv_img_dsc_t bin_google;
extern lv_img_dsc_t bin_message;
extern lv_img_dsc_t bin_skype;
extern lv_img_dsc_t bin_twitter;
extern lv_img_dsc_t bin_avi;
extern lv_img_dsc_t bin_gif;
extern lv_img_dsc_t bin_back;

void load_files(void);
uint8_t lv_load_user_img_from_file(void);
#endif //__LOAD_H__
