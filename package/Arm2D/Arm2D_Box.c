#include "Arm2D_Box.h"
#include "Arm2D_common.h"
#include "asset_corner_box.h"
#include "dataArgs.h"

void Arm2D_Box_init(PikaObj* self) {
    /* init elem info */
    pika_arm2d_element_info_t elemInfo = {0};
    elemInfo.alpha = 255;
    elemInfo.x = 0;
    elemInfo.y = 0;
    args_setStruct(self, "elemInfo", &elemInfo, sizeof(pika_arm2d_element_info_t));
    /* init box info */
    pika_arm2d_box_info_t boxInfo = {0};
    boxInfo.wight = 50;
    boxInfo.hight = 50;
    boxInfo.color_code = getColorCode("blue");
    /* load box info to arg */
    args_setStruct(self, "boxInfo", &boxInfo, sizeof(pika_arm2d_box_info_t));
    /* add dirtyReginItem */
    args_setStruct(self, "dirtyReginItem", NULL, sizeof(arm_2d_region_list_item_t));
}

int __min(int x,int y){
	return x<y?x:y;
}

int __max(int x,int y){
	return x>y?x:y;
}

void Arm2D_Box_update(PikaObj* self) {
    pika_arm2d_box_info_t* boxInfo = args_getStruct(self, "boxInfo");

    void* target_tile = pika_arm2d_window.pfb_tile_now;
    bool bIsNewFrame = pika_arm2d_window.pfb_is_new_frame;
    if(bIsNewFrame){
        pika_arm2d_element_info_t* elemInfo = args_getStruct(self, "elemInfo");

        boxInfo->arg2d_regin.tSize.iHeight = boxInfo->hight;
        boxInfo->arg2d_regin.tSize.iWidth = boxInfo->wight;
        boxInfo->arg2d_regin.tLocation.iX = elemInfo->x;
        boxInfo->arg2d_regin.tLocation.iY = elemInfo->y;
        memcpy(&(boxInfo->elem_info), elemInfo, sizeof(pika_arm2d_element_info_t));

        arm_2d_region_list_item_t* dirtyReginItem = args_getStruct(self, "dirtyReginItem");
        dirtyReginItem->ptNext = NULL;
        int x_start = __min(boxInfo->elem_info.x, boxInfo->elem_info.x_last) - 14;
        int y_start = __min(boxInfo->elem_info.y, boxInfo->elem_info.y_last) - 7;
        int x_end = __max(boxInfo->elem_info.x + boxInfo->wight, boxInfo->elem_info.x_last + boxInfo->wight_last);
        int y_end = __max(boxInfo->elem_info.y + boxInfo->hight, boxInfo->elem_info.y_last + boxInfo->hight_last);
        int hight = y_end - y_start + 7;
        int wight = x_end - x_start + 14;
        dirtyReginItem->tRegion.tLocation.iX = x_start;
        dirtyReginItem->tRegion.tLocation.iY = y_start;
        dirtyReginItem->tRegion.tSize.iHeight = hight;
        dirtyReginItem->tRegion.tSize.iWidth = wight;
        pika_arm2d_window.dirty_region_list = dirtyReginItem;
    }
    draw_round_corner_box(target_tile, &(boxInfo->arg2d_regin), boxInfo->color_code,
    		boxInfo->elem_info.alpha, bIsNewFrame);
}

void Arm2D_Box_setColor(PikaObj* self, char* color) {
    pika_arm2d_box_info_t* boxInfo = args_getStruct(self, "boxInfo");
    boxInfo->color_code_last = boxInfo->color_code;

    boxInfo->color_code = getColorCode(color);
}

void Arm2D_Box_setSize(PikaObj* self, int x, int y) {
    pika_arm2d_box_info_t* boxInfo = args_getStruct(self, "boxInfo");
    boxInfo->wight_last = boxInfo->wight;
    boxInfo->hight_last = boxInfo->hight;

    boxInfo->wight = x;
    boxInfo->hight = y;
}
