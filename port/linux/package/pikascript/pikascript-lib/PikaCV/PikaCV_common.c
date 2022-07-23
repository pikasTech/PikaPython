#include "PikaCV_common.h"
#include "PikaCV_Converter.h"

int PikaCV_Format_Check(PikaObj* image,PikaCV_ImageFormat_Type type,PikaCV_Check_ReturnMode returnMode){
    // return mode: 0 PikaCV_Check_ReturnError Only return inequality or equality
    //              1 PikaCV_Check_Converter converter format to type when inequality

    PikaCV_Image* src = obj_getStruct(image, "image");
    switch (returnMode)
    {
    case PikaCV_Check_ReturnError:
        if(src->format != type){
            return 0;
        }else{
            return 1;
        }
        break;
    case PikaCV_Check_Converter:
        /* code */
        if(src->format != type){
            PikaCV_Converter_converter((PikaObj*)NULL,type,image);
            return 0;
        }else{
            return 1;
        }
        break;    
    default:
        return 0;
        break;
    }

    
}

int PikaCV_Format_CheckTwo(PikaObj* image1,PikaObj* image2,PikaCV_Check_ReturnMode returnMode){
    // return mode: 0 PikaCV_Check_ReturnError Only return inequality or equality
    //              1 PikaCV_Check_Converter converter format to type when inequality
    PikaCV_Image* src1 = obj_getStruct(image1, "image");
    PikaCV_Image* src2 = obj_getStruct(image2, "image");

    switch (returnMode)
    {
    case PikaCV_Check_ReturnError:
        if(src1->format != src2->format){
            return 0;
        }else{
            return 1;
        }
        break;
    case PikaCV_Check_Converter:
        /* code */
        if(src1->format != src2->format){
            PikaCV_Converter_converter((PikaObj*)NULL,src1->format,image2);
            return 0;
        }else{
            return 1;
        }
        break;    
    default:
        break;
    }

    return 0;
}

int PikaCV_Size_Check(PikaObj* image1,PikaObj* image2,PikaCV_Check_SizeMode sizeMode){
    //sizeMode : 0 PikaCV_Check_Size Check size
    //           1 PikaCV_Check_SHW  Check size,height and width
    //           2 PikaCV_Check_HW   Check height and width
    PikaCV_Image* src1 = obj_getStruct(image1, "image");
    PikaCV_Image* src2 = obj_getStruct(image2, "image");

    switch (sizeMode)
    {
    case PikaCV_Check_Size:
        /* code */
        if(src1->size != src2->size){
            return 0;
        }else{
            return 1;
        }
        break;
    case PikaCV_Check_SHW:
        /* code */
        if(src1->size != src2->size || src1->height!= src2->height || src1->width != src2->width){
            return 0;
        }else{
            return 1;
        }
        break;
    case PikaCV_Check_HW:
        /* code */
        if(src1->height!= src2->height || src1->width != src2->width){
            return 0;
        }else{
            return 1;
        }
        break;
    default:
        break;
    }

    return 0;
}