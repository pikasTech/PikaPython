#include "PikaCV_ImageFormat.h"
#include "PikaCV_common.h"

void PikaCV_ImageFormat___init__(PikaObj* self) {
    obj_setInt(self, "JPEG", PikaCV_ImageFormat_Type_JPEG);
    obj_setInt(self, "RGB888", PikaCV_ImageFormat_Type_RGB888);
    obj_setInt(self, "RGB565", PikaCV_ImageFormat_Type_RGB565);
    obj_setInt(self, "GRAY", PikaCV_ImageFormat_Type_GRAY);
}
