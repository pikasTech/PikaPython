#include "PikaCV_Image.h"
#include "PikaCV_common.h"
#include "dataStrs.h"

void PikaCV_Image___init__(PikaObj* self) {
    if (NULL != obj_getStruct(self, "image")) {
        /* already initialized */
        return;
    }
    /* init */
    PikaCV_Image image = {
        .format = PikaCV_ImageFormat_Type_Unknown,
        .width = 0,
        .height = 0,
        .size = 0,
    };
    obj_setStruct(self, "image", image);
    PikaCV_Image_setData(self, NULL, 0);
}

uint8_t* _Image_getData(PikaObj* self) {
    PikaCV_Image* image = obj_getStruct(self, "image");
    if (NULL == image) {
        return NULL;
    }
    return obj_getBytes(self, "_data");
}

PIKA_RES PikaCV_Image_setData(PikaObj* self, uint8_t* data, int size) {
    PikaCV_Image* image = obj_getStruct(self, "image");
    if (NULL == image) {
        return PIKA_RES_ERR_ARG_NO_FOUND;
    }
    obj_setBytes(self, "_data", data, size);
    image->size = size;
    return PIKA_RES_OK;
}

Arg* PikaCV_Image_data(PikaObj* self) {
    PikaCV_Image* image = obj_getStruct(self, "image");
    if (NULL == image) {
        return NULL;
    }
    return arg_copy(obj_getArg(self, "_data"));
}

int PikaCV_Image_format(PikaObj* self) {
    PikaCV_Image* image = obj_getStruct(self, "image");
    if (NULL == image) {
        return PikaCV_ImageFormat_Type_Unknown;
    }
    return image->format;
}

int PikaCV_Image_hight(PikaObj* self) {
    PikaCV_Image* image = obj_getStruct(self, "image");
    if (NULL == image) {
        return 0;
    }
    return image->height;
}

void PikaCV_Image_loadJpeg(PikaObj* self, Arg* bytes) {
    obj_setArg(self, "_data", bytes);
}

void PikaCV_Image_loadRGB565(PikaObj* self,
                             uint8_t* bytes,
                             int height,
                             int width) {
    PikaCV_Image* image = obj_getStruct(self, "image");
    if (NULL == image) {
        return;
    }
    image->format = PikaCV_ImageFormat_Type_RGB565;
    image->height = height;
    image->width = width;
    image->size = height * width * 2;
    PikaCV_Image_setData(self, bytes, image->size);
}

void PikaCV_Image_loadRGB888(PikaObj* self,
                             uint8_t* bytes,
                             int height,
                             int width) {
    PikaCV_Image* image = obj_getStruct(self, "image");
    if (NULL == image) {
        return;
    }
    image->format = PikaCV_ImageFormat_Type_RGB888;
    image->height = height;
    image->width = width;
    image->size = height * width * 3;
    PikaCV_Image_setData(self, bytes, image->size);
}

void PikaCV_Image_loadGray(PikaObj* self,
                           uint8_t* bytes,
                           int height,
                           int width) {
    PikaCV_Image* image = obj_getStruct(self, "image");
    if (NULL == image) {
        return;
    }
    image->format = PikaCV_ImageFormat_Type_GRAY;
    image->height = height;
    image->width = width;
    image->size = height * width;
    PikaCV_Image_setData(self, bytes, image->size);
}

void PikaCV_Image_read(PikaObj* self, char* path) {
    PikaCV_Image* image = obj_getStruct(self, "image");
    if (NULL == image) {
        return;
    }
    Arg* data_arg = arg_loadFile(NULL, path);
    if (NULL == data_arg) {
        return;
    }
    Args buffs = {0};
    char* suffix = strsGetLastToken(&buffs, path, '.');
    if (suffix == NULL) {
        return;
    }
    if (strEqu(suffix, "jpg")) {
        image->format = PikaCV_ImageFormat_Type_JPEG;
        PikaCV_Image_loadJpeg(self, data_arg);
    } else {
        obj_setErrorCode(self, PIKA_RES_ERR_OPERATION_FAILED);
        __platform_printf("PikaCV_Image_read: unsupported format: %s\n",
                          suffix);
    }
    arg_deinit(data_arg);
    strsDeinit(&buffs);
}

void PikaCV_Image_setPixel(PikaObj* self,
                           int channel,
                           int value,
                           int x,
                           int y) {
    PikaCV_Image* image = obj_getStruct(self, "image");
    if (NULL == image) {
        return;
    }
    uint8_t* data = _Image_getData(self);
    if (image->format == PikaCV_ImageFormat_Type_RGB565) {
        data[(y * image->width + x) * 2 + channel] = value;
    } else if (image->format == PikaCV_ImageFormat_Type_RGB888) {
        data[(y * image->width + x) * 3 + channel] = value;
    } else if (image->format == PikaCV_ImageFormat_Type_GRAY) {
        data[y * image->width + x] = value;
    } else {
        obj_setErrorCode(self, PIKA_RES_ERR_OPERATION_FAILED);
        __platform_printf("PikaCV_Image_setPixel: unsupported format: %d\n",
                          image->format);
    }
}

int PikaCV_Image_getPixel(PikaObj* self, int channel, int x, int y) {
    PikaCV_Image* image = obj_getStruct(self, "image");
    if (NULL == image) {
        return 0;
    }
    uint8_t* data = _Image_getData(self);
    if (image->format == PikaCV_ImageFormat_Type_RGB565) {
        return data[(y * image->width + x) * 2 + channel];
    } else if (image->format == PikaCV_ImageFormat_Type_RGB888) {
        return data[(y * image->width + x) * 3 + channel];
    } else if (image->format == PikaCV_ImageFormat_Type_GRAY) {
        return data[y * image->width + x];
    } else {
        obj_setErrorCode(self, PIKA_RES_ERR_OPERATION_FAILED);
        __platform_printf("PikaCV_Image_getPixel: unsupported format: %d\n",
                          image->format);
    }
    return 0;
}

int PikaCV_Image_size(PikaObj* self) {
    PikaCV_Image* image = obj_getStruct(self, "image");
    if (NULL == image) {
        return 0;
    }
    return image->size;
}

int PikaCV_Image_width(PikaObj* self) {
    PikaCV_Image* image = obj_getStruct(self, "image");
    if (NULL == image) {
        return 0;
    }
    return image->width;
}
