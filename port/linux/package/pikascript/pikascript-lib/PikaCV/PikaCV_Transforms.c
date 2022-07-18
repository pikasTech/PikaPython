#include "PikaCV_Transforms.h"
#include "PikaCV_Converter.h"
#include "PikaCV_common.h"

void PikaCV_Transforms_rotateDown(PikaObj* self, PikaObj* image) {
    PikaCV_Image* img = obj_getStruct(image, "image");
    if (NULL == img) {
        pika_assert(0);
        return;
    }
    if ((img->format != PikaCV_ImageFormat_Type_BGR888) &&
        (img->format != PikaCV_ImageFormat_Type_RGB888)) {
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

void PikaCV_Transforms_threshold(PikaObj *self, PikaObj* image, int maxval, int thre, int thresholdType){
    PikaCV_Image* src = obj_getStruct(image, "image");
    
    if (NULL == src) {
        pika_assert(0);
        return;
    }
    if (src->format != PikaCV_ImageFormat_Type_GRAY) {
        PikaCV_Converter_toGray(self,image);
    }

    uint8_t* src_data = _image_getData(image);
    
    int i;
    if(thresholdType == 0 ){
        for (i = 0; i < (src->size) ; i++) {
            src_data[i] = src_data[i] > thre ? maxval : 0 ;
        }
    }
    else if(thresholdType == 1 ){
        for (i = 0; i < (src->size) ; i++) {
            src_data[i] = src_data[i] > thre ? 0 : maxval ;
        }
    }
    else if(thresholdType == 2 ){
        for (i = 0; i < (src->size) ; i++) {
            src_data[i] = src_data[i] > thre ? thre : src_data[i] ;
        }
    }
    else if(thresholdType == 3 ){
        for (i = 0; i < (src->size) ; i++) {
            src_data[i] = src_data[i] > thre ? src_data[i] : 0 ;
        }
    }    
    else if(thresholdType == 4 ){
        for (i = 0; i < (src->size) ; i++) {
            src_data[i] = src_data[i] > thre ? 0 : src_data[i] ;
        }
    }

    obj_setBytes(image, "_data", src_data, src->size);
}

void PikaCV_Transforms_setROI(PikaObj *self, int h, PikaObj* image, int w, int x, int y){
    PikaCV_Image* src = obj_getStruct(image, "image");
    int width = src->width;
    int height = src->height;

    if (NULL == src) {
        pika_assert(0);
        return;
    }
    if (src->format != PikaCV_ImageFormat_Type_RGB888) {
        PikaCV_Converter_toRGB888(self,image);
    }
    if( x <= 0 || y <= 0 || w <= 0 || h <= 0){
        pika_assert(0);
        return;
    }
    if( x + w > width || y + h > height ){
        pika_assert(0);
        return;
    }

    int size_new = h * w * 3;
    Arg* arg_data_new = arg_setBytes(NULL, "", NULL, size_new);
    uint8_t* data = _image_getData(image);
    uint8_t* data_new = arg_getBytes(arg_data_new);

    for(int i = 0 ; i < h ; i++){
        for(int j = 0 ; j < w ; j++){
            data_new[i * 3 * w + j * 3] = data[ (i + y-1) * width * 3 + (j + x - 1 ) * 3 ];
            data_new[i * 3 * w + j * 3 + 1] = data[ (i + y-1) * width * 3 + ( j + x - 1 ) * 3 + 1];
            data_new[i * 3 * w + j * 3 + 2] = data[ (i + y-1) * width * 3 + ( j + x - 1 ) * 3 + 2];
        }
    }
    src->height=h;
    src->width=w;
    src->size = size_new;
    
    obj_setBytes(image, "_data", data_new, size_new);
    arg_deinit(arg_data_new);

}