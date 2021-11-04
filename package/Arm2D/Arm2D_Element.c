#include "Arm2D_Element.h"
#include "Arm2D_common.h"
void Arm2D_Element_update(PikaObj* self) {
    /* need to be override */
    args_setErrorCode(self, 1);
    args_setSysOut(self, "[error]: update method not be overrided !");
}

void Arm2D_Element_init(PikaObj* self) {
    pika_arm2d_element_info_t elemInfo = {0};
    elemInfo.alpha = 255;
    elemInfo.x = 0;
    elemInfo.y = 0;
    args_setStruct(self, "elemInfo", &elemInfo, sizeof(pika_arm2d_element_info_t));
}

void Arm2D_Element_setAlpha(PikaObj* self, int alpha) {
    pika_arm2d_element_info_t* elemInfo = args_getStruct(self, "elemInfo");
    elemInfo->alpha_last = elemInfo->alpha;

    elemInfo->alpha = alpha;
}

void Arm2D_Element_up(PikaObj* self, int y) {
    pika_arm2d_element_info_t* elemInfo = args_getStruct(self, "elemInfo");
    elemInfo->y_last = elemInfo->y;

    elemInfo->y = elemInfo->y - y;
}

void Arm2D_Element_down(PikaObj* self, int y) {
    pika_arm2d_element_info_t* elemInfo = args_getStruct(self, "elemInfo");
    elemInfo->y_last = elemInfo->y;

    elemInfo->y = elemInfo->y + y;
}

void Arm2D_Element_lift(PikaObj* self, int x) {
    pika_arm2d_element_info_t* elemInfo = args_getStruct(self, "elemInfo");
    elemInfo->x_last = elemInfo->x;

    elemInfo->x = elemInfo->x - x;
}

void Arm2D_Element_right(PikaObj* self, int x) {
    pika_arm2d_element_info_t* elemInfo = args_getStruct(self, "elemInfo");
    elemInfo->x_last = elemInfo->x;

    elemInfo->x = elemInfo->x + x;
}

void Arm2D_Element_move(PikaObj* self, int x, int y) {
    pika_arm2d_element_info_t* elemInfo = args_getStruct(self, "elemInfo");
    elemInfo->y_last = elemInfo->y;
    elemInfo->x_last = elemInfo->x;

    elemInfo->x = x;
    elemInfo->y = y;
}
