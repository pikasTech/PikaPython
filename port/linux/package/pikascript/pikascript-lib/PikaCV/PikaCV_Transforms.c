#include "PikaCV_Transforms.h"
#include "PikaCV_Converter.h"
#include "PikaCV_Filter.h"
#include "PikaCV_common.h"
#include "math.h"

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
    Arg* arg_data_new = arg_newBytes(NULL, size_new);
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
    else if(thresholdType == 5 ){
        PikaCV_Transforms_setOTSU(self,image);
    }else{
        pika_assert(0);
        return;
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
    Arg* arg_data_new = arg_newBytes(NULL, size_new);
    uint8_t* data = _image_getData(image);
    uint8_t* data_new = arg_getBytes(arg_data_new);

    for(int i = 0 ; i < h ; i++){
        for(int j = 0 ; j < w ; j++){
            data_new[i * 3 * w + j * 3] = data[ (i + y - 1 ) * width * 3 + (j + x - 1 ) * 3 ];
            data_new[i * 3 * w + j * 3 + 1] = data[ (i + y - 1 ) * width * 3 + ( j + x - 1 ) * 3 + 1];
            data_new[i * 3 * w + j * 3 + 2] = data[ (i + y - 1 ) * width * 3 + ( j + x - 1 ) * 3 + 2];
        }
    }
    src->height = h;
    src->width = w;
    src->size = size_new;
    
    obj_setBytes(image, "_data", data_new, size_new);
    arg_deinit(arg_data_new);

}

int PikaCV_Transforms_getOTSUthre(PikaObj *self, PikaObj* image){
    PikaCV_Image* src = obj_getStruct(image, "image");
    
    if (NULL == src) {
        pika_assert(0);
        return 0;
    }
    if (src->format != PikaCV_ImageFormat_Type_GRAY) {
        PikaCV_Converter_toGray(self,image);
    }
    const int Grayscale=256;
    int width = src->width;
    int height = src->height;
    int size = src->size;
    int threshold = 0;
    int graynum[256] = { 0 };
    uint8_t* data = _image_getData(image);
    for(int i=0 ; i < size ; i++){
        graynum[data[i]]++;
    }

    double P[256] = { 0.0 };   
    double PK[256] = { 0.0 };
    double MK[256] = { 0.0 };
    double srcpixnum = width*height, sumtmpPK = 0, sumtmpMK = 0;
    for (int i = 0; i < Grayscale; ++i){
        P[i] = graynum[i] / srcpixnum;
        PK[i] = sumtmpPK + P[i]; 
        sumtmpPK = PK[i];
        MK[i] = sumtmpMK + i*P[i];                                                                                                                                                                                                                                                                                                                                                                                                          
        sumtmpMK = MK[i];
    }

    double Var=0;
    for (int k = 0; k < Grayscale; ++k){
        if(PK[k] > 0.0)
        {
            double t = MK[Grayscale-1] * PK[k] - MK[k];
            double w = PK[k] * (1 - PK[k]);
            double variance = t * t / w ;
            if (variance > Var){
                Var = variance ;
                threshold = k;
            }
        }
    }

    return threshold;
}

void PikaCV_Transforms_setOTSU(PikaObj *self, PikaObj* image){
    PikaCV_Image* src = obj_getStruct(image, "image");

    if (NULL == src) {
        pika_assert(0);
        return;
    }
    if (src->format != PikaCV_ImageFormat_Type_GRAY) {
        PikaCV_Converter_toGray(self,image);
    }

    int thre = PikaCV_Transforms_getOTSUthre(self,image);
    uint8_t* src_data = _image_getData(image);
    
    int i;
    for (i = 0; i < (src->size) ; i++) {
        src_data[i] = src_data[i] > thre ? 255 : 0 ;
    }

    obj_setBytes(image, "_data", src_data, src->size);
}

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

void PikaCV_Transforms_resize(PikaObj *self, PikaObj* image, int resizeType, int x, int y){
    PikaCV_Image* src = obj_getStruct(image, "image");
    int width = src->width;
    int height = src->height;
    if (NULL == src) {
        pika_assert(0);
        return;
    }
    if( x <= 0 || y <= 0 ){
        pika_assert(0);
        return;
    }
    if (src->format != PikaCV_ImageFormat_Type_RGB888) {
        PikaCV_Converter_toRGB888(self,image);
    }    

    int size_new = x * y * 3;
    Arg* arg_data_new = arg_newBytes(NULL, size_new);
    uint8_t* data = _image_getData(image);
    uint8_t* data_new = arg_getBytes(arg_data_new);

    double scale_x = (double)width / (double)x;  
    double scale_y = (double)height / (double)y;   
    if(resizeType == 0){
        for (int i = 0; i < y ; ++i)  
        {  
            int sy = floor(i * scale_y);  
            sy = MIN(sy , ( height - 1 ));
            for (int j = 0; j < x ; ++j)  
            {  
                int sx = floor(j * scale_x);  
                sx = MIN(sx , ( width - 1 ));

                data_new[i * x * 3 + j * 3]=data[sy * width * 3 + sx * 3];
                data_new[i * x * 3 + j * 3 + 1]=data[sy * width * 3 + sx * 3 + 1];
                data_new[i * x * 3 + j * 3 + 2]=data[sy * width * 3 + sx * 3 + 2];
            }  
        }  
    }else{
        pika_assert(0);
        return;
    }
    src->height = y;
    src->width = x;
    src->size = size_new;
    
    obj_setBytes(image, "_data", data_new, size_new);
    arg_deinit(arg_data_new);
}

#undef MAX
#undef MIN

void PikaCV_Transforms_adaptiveThreshold(PikaObj *self, int c, PikaObj* image, int maxval, int method, int subsize){
    PikaCV_Image* src = obj_getStruct(image, "image");

    if (NULL == src) {
        pika_assert(0);
        return;
    }
    if(c<-255||c>255){
        pika_assert(0);
        return;
    }
    if (src->format != PikaCV_ImageFormat_Type_GRAY) {
        PikaCV_Converter_toGray(self,image);
    }

    int size = src->size;
    uint8_t* src_data = _image_getData(image);
    
    switch (method)
	{
	case 0:
		PikaCV_Filter_meanFilter(self,image,subsize,subsize);  //均值滤波
		break;
	case 1:
		PikaCV_Filter_medianFilter(self,image);   //中值滤波
		break;
	default:
		break;
	}

    uint8_t* smooth_data = _image_getData(image);
    for(int i=0;i<size;i++){
        smooth_data[i] -= c;
    }
    for(int i=0;i<size;i++){
        src_data[i] = src_data[i] > smooth_data[i] ? maxval : 0 ;
    }

    obj_setBytes(image, "_data", src_data, src->size);

}
