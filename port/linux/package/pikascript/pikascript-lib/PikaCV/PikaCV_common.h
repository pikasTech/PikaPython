#include "./3rd-party/tjpgd/src/tjpgd.h"
#define M_PI 3.14159265358979323846

typedef enum PikaCV_ImageFormat_Type {
    PikaCV_ImageFormat_Type_Unknown = 0,
    PikaCV_ImageFormat_Type_JPEG,
    PikaCV_ImageFormat_Type_RGB888,
    PikaCV_ImageFormat_Type_BGR888,
    PikaCV_ImageFormat_Type_RGB565,
    PikaCV_ImageFormat_Type_GRAY,
    PikaCV_ImageFormat_Type_BMP,
} PikaCV_ImageFormat_Type;

typedef struct PikaCV_Image {
    PikaCV_ImageFormat_Type format;
    int width;
    int height;
    int size;
} PikaCV_Image;

uint8_t* _image_getData(PikaObj* self);
PIKA_RES _image_setData(PikaObj* self, uint8_t* data, int size);
int _image_getDataSize(PikaObj* self);