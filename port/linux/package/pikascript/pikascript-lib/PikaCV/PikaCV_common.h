#include "./3rd-party/tjpgd/src/tjpgd.h"
#include "PikaObj.h"
#define M_PI 3.14159265358979323846

typedef enum PikaCV_ImageFormat_Type {
    PikaCV_ImageFormat_Type_Empty = 0,
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

typedef enum {
    PikaCV_Check_ReturnError = 0,
    PikaCV_Check_Converter,
} PikaCV_Check_ReturnMode;

typedef enum {
    PikaCV_Check_Size = 0,
    PikaCV_Check_SHW,
    PikaCV_Check_HW,
} PikaCV_Check_SizeMode;

uint8_t* _image_getData(PikaObj* self);
PIKA_RES _image_setData(PikaObj* self, uint8_t* data, int size);
int _image_getDataSize(PikaObj* self);

int PikaCV_Format_Check(PikaObj* image,
                        PikaCV_ImageFormat_Type type,
                        PikaCV_Check_ReturnMode returnMode);
int PikaCV_Format_CheckTwo(PikaObj* image1,
                           PikaObj* image2,
                           PikaCV_Check_ReturnMode returnMode);
int PikaCV_Size_Check(PikaObj* image1,
                      PikaObj* image2,
                      PikaCV_Check_SizeMode sizeMode);
