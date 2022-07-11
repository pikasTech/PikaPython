#include "PikaCV_Transformer.h"
#include "PikaCV_Converter.h"
#include "PikaCV_common.h"

void PikaCV_Transformer_rotateDown(PikaObj* self, PikaObj* image) {
    PikaCV_Image* img = obj_getStruct(image, "image");
    if (NULL == img) {
        pika_assert(0);
        return;
    }
    if (img->format != PikaCV_ImageFormat_Type_BGR888 &&
        img->format != PikaCV_ImageFormat_Type_RGB888) {
        obj_setErrorCode(self, PIKA_RES_ERR_OPERATION_FAILED);
        __platform_printf("unsupported image format\n");
        return;
    }
    int width = img->width;
    int height = img->height;
    int size_new = width * height * 3;
    Arg* arg_data_new = arg_setBytes(NULL, "", NULL, size_new);
    uint8_t* data = _image_getData(image);
    uint8_t* data_new = arg_getBytes(arg_data_new);
    int i, j, k;
    /* rotate */
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            for (k = 0; k < 3; k++) {
                data_new[(height - i - 1) * width * 3 + j * 3 + k] =
                    data[i * width * 3 + j * 3 + k];
            }
        }
    }
    obj_setBytes(image, "_data", data_new, size_new);
    img->size = size_new;
    arg_deinit(arg_data_new);
}
