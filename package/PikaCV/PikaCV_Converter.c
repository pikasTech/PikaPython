#include "PikaCV_Converter.h"
#include "PikaCV_common.h"

/* Session identifier for input/output functions (name, members and usage are as
 user defined) */
typedef struct {
    uint8_t* input_ptr;  /* Pointer to input data */
    size_t input_offset; /* Offset of input data */
    size_t input_size;   /* Size of input data */

    uint8_t* ouput_ptr;     /* Pointer to output data */
    unsigned int width_pix; /* Width of the frame buffer [pix] */
} JpegInterface;

size_t
in_func(                    /* Returns number of bytes read (zero on error) */
        JDEC* jpeg_decoder, /* Decompression object */
        uint8_t* buff, /* Pointer to the read buffer (null to remove data) */
        size_t nbyte   /* Number of bytes to read/remove */
) {
    JpegInterface* io = (JpegInterface*)jpeg_decoder->device;
    if (buff) {
        if (io->input_offset + nbyte > io->input_size) {
            nbyte = io->input_size - io->input_offset;
        }
        if (nbyte > 0) {
            memcpy(buff, io->input_ptr + io->input_offset, nbyte);
            io->input_offset += nbyte;
        }
    } else {
        if (io->input_offset + nbyte > io->input_size) {
            nbyte = io->input_size - io->input_offset;
        }
        io->input_offset += nbyte;
    }
    return nbyte;
}

/*------------------------------*/
/* User defined output funciton */
/*------------------------------*/

/* Bytes per pixel of image output */
#define N_BPP (3 - JD_FORMAT)

int out_func(              /* Returns 1 to continue, 0 to abort */
             JDEC* jd,     /* Decompression object */
             void* bitmap, /* Bitmap data to be output */
             JRECT* rect   /* Rectangular region of output image */
) {
    JpegInterface* io = (JpegInterface*)jd->device; /* Session identifier
                                        (5th argument of jd_prepare function) */
    uint8_t *source, *direction;
    uint32_t y, width_out_rect;
    unsigned int width_rect_buf;

    /* Put progress indicator */
    if (rect->left == 0) {
        printf("\r%lu%%", (rect->top << jd->scale) * 100UL / jd->height);
    }

    /* Copy the output image rectanglar to the frame buffer */
    source = (uint8_t*)bitmap;

    /* Left-top of destination rectangular */
    direction =
        io->ouput_ptr + N_BPP * (rect->top * io->width_pix + rect->left);

    /* Width of output rectangular [byte] */
    width_out_rect = N_BPP * (rect->right - rect->left + 1);

    /* Width of frame buffer [byte] */
    width_rect_buf = N_BPP * io->width_pix;

    /* Output the rectangular */
    for (y = rect->top; y <= rect->bottom; y++) {
        /* Copy a line */
        __platform_memcpy(direction, source, width_out_rect);
        /* Update pointers */
        source += width_out_rect;

        /* Next line */
        direction += width_rect_buf;
    }

    return 1; /* Continue to decompress */
}

PIKA_RES Converter_JPEGtoRGB888(PikaObj* image) {
    PikaCV_Image* img = obj_getStruct(image, "image");
    PIKA_RES pika_res = PIKA_RES_OK;
    if (NULL == img) {
        return PIKA_RES_ERR_INVALID_PTR;
    }
    pika_assert(img->format == PikaCV_ImageFormat_Type_JPEG);
    uint8_t* data = _Image_getData(image);
    if (NULL == data) {
        __platform_printf("Converter_JPEGtoRGB888: data is NULL\n");
        return PIKA_RES_ERR_INVALID_PTR;
    }

    size_t sz_work = 3500;
    Arg* arg_work = arg_setBytes(NULL, "", NULL, sz_work);
    JRESULT res;
    JDEC jdec;
    void* work = arg_getBytes(arg_work);
    JpegInterface io;
    io.input_ptr = data;
    io.input_offset = 0;
    io.input_size = obj_getBytesSize(image, "_data");
    io.width_pix = 0;
    res = jd_prepare(&jdec, in_func, work, sz_work, &io);

    if (res != JDR_OK) {
        __platform_printf("Converter_JPEGtoRGB888: jd_prepare failed\n");
        pika_res = PIKA_RES_ERR_INVALID_PTR;
        goto exit;
    }

    /* It is ready to dcompress and image info is available here */
    __platform_printf(
        "Image size is %u x %u.\n%u bytes of work ares is used.\n", jdec.width,
        jdec.height, (int)(sz_work - jdec.sz_pool));
    io.width_pix = jdec.width;

    /* Allocate memory for output image */
    Arg* arg_output =
        arg_setBytes(NULL, "", NULL, jdec.width * jdec.height * N_BPP);
    if (NULL == arg_output) {
        __platform_printf("Converter_JPEGtoRGB888: arg_setBytes failed\n");
        pika_res = PIKA_RES_ERR_INVALID_PTR;
        goto exit;
    }
    io.ouput_ptr = arg_getBytes(arg_output);

    res = jd_decomp(&jdec, out_func, 0);
    if (res != JDR_OK) {
        __platform_printf("jd_decomp() failed (rc=%d)\n", res);
        pika_res = PIKA_RES_ERR_INVALID_PTR;
        goto exit;
    }

exit:
    if (pika_res == PIKA_RES_OK) {
        img->format = PikaCV_ImageFormat_Type_RGB888;
        img->width = jdec.width;
        img->height = jdec.height;
        img->size = jdec.width * jdec.height * N_BPP;
        obj_setArg(image, "_data", arg_output);
        arg_deinit(arg_output);
    }
    arg_deinit(arg_work);
    return pika_res;
}

void PikaCV_Converter_toGray(PikaObj* self, PikaObj* image) {
    PikaCV_Image* img = obj_getStruct(image, "image");
    if (NULL == img) {
        return;
    }
    if (img->format == PikaCV_ImageFormat_Type_GRAY) {
        /* do nothing */
        return;
    }
    if (img->format == PikaCV_ImageFormat_Type_JPEG) {
        if (PIKA_RES_OK != Converter_JPEGtoRGB888(image)) {
            return;
        }
    }

    int size_new = img->height * img->width;
    Arg* arg_data_new = arg_setBytes(NULL, "", NULL, size_new);
    uint8_t* data = _Image_getData(image);
    uint8_t* data_new = arg_getBytes(arg_data_new);
    if (img->format == PikaCV_ImageFormat_Type_RGB888) {
        for (int i = 0; i < size_new; i++) {
            data_new[i] = (data[i * 3] + data[i * 3 + 1] + data[i * 3 + 2]) / 3;
        }
        goto exit;
    }
    if (img->format == PikaCV_ImageFormat_Type_RGB565) {
        for (int i = 0; i < size_new; i++) {
            data_new[i] = (data[i * 2] + data[i * 2 + 1]) / 2;
        }
        goto exit;
    }
    if (img->format == PikaCV_ImageFormat_Type_GRAY) {
        memcpy(data_new, data, size_new);
        goto exit;
    }
exit:
    obj_setBytes(image, "_data", data_new, size_new);
    img->format = PikaCV_ImageFormat_Type_GRAY;
    img->size = size_new;
    arg_deinit(arg_data_new);
}

void PikaCV_Converter_toRGB565(PikaObj* self, PikaObj* image) {
    PikaCV_Image* img = obj_getStruct(image, "image");
    if (NULL == img) {
        return;
    }
    /* do nothing */
    if (img->format == PikaCV_ImageFormat_Type_RGB565) {
        return;
    }
    if (img->format == PikaCV_ImageFormat_Type_JPEG) {
        if (PIKA_RES_OK != Converter_JPEGtoRGB888(image)) {
            return;
        }
    }

    int size_new = img->height * img->width * 2;
    Arg* arg_data_new = arg_setBytes(NULL, "", NULL, size_new);
    uint8_t* data = obj_getBytes(image, "_data");
    uint8_t* data_new = arg_getBytes(arg_data_new);
    if (img->format == PikaCV_ImageFormat_Type_RGB888) {
        for (int i = 0; i < img->size; i += 3) {
            uint32_t* p888 = (uint32_t*)&data[i];
            uint16_t* p565 = (uint16_t*)&data_new[i / 3 * 2];

            *p565 = ((*p888 & 0x00F80000) >> 8) | ((*p888 & 0x0000FC00) >> 5) |
                    ((*p888 & 0x000000F8) >> 3);
        }
        goto exit;
    }
    if (img->format == PikaCV_ImageFormat_Type_GRAY) {
        for (int i = 0; i < img->size; i++) {
            data_new[i * 2] = data[i];
            data_new[i * 2 + 1] = data[i];
        }
        goto exit;
    }
exit:
    obj_setBytes(image, "_data", data_new, size_new);
    img->format = PikaCV_ImageFormat_Type_RGB565;
    img->size = size_new;
    arg_deinit(arg_data_new);
}

void PikaCV_Converter_toRGB888(PikaObj* self, PikaObj* image) {
    PikaCV_Image* img = obj_getStruct(image, "image");
    if (NULL == img) {
        return;
    }

    if (img->format == PikaCV_ImageFormat_Type_JPEG) {
        if (PIKA_RES_OK != Converter_JPEGtoRGB888(image)) {
            return;
        }
    }

    if (img->format == PikaCV_ImageFormat_Type_RGB888) {
        /* do nothing */
        return;
    }

    int size_new = img->height * img->width * 3;
    Arg* arg_data_new = arg_setBytes(NULL, "", NULL, size_new);
    uint8_t* data = obj_getBytes(image, "_data");
    uint8_t* data_new = arg_getBytes(arg_data_new);
    if (img->format == PikaCV_ImageFormat_Type_RGB565) {
        for (int i = 0; i < img->size; i += 2) {
            uint16_t* p565 = (uint16_t*)&data[i];
            uint32_t* p888 = (uint32_t*)&data_new[i / 2 * 3];
            *p888 = ((*p565 & 0xF800) << 8) | ((*p565 & 0x07E0) << 5) |
                    ((*p565 & 0x001F) << 3);
        }
        goto exit;
    }
    if (img->format == PikaCV_ImageFormat_Type_GRAY) {
        for (int i = 0; i < img->size; i++) {
            data_new[i * 3] = data[i];
            data_new[i * 3 + 1] = data[i];
            data_new[i * 3 + 2] = data[i];
        }
        goto exit;
    }
exit:
    obj_setBytes(image, "_data", data_new, size_new);
    img->format = PikaCV_ImageFormat_Type_RGB565;
    img->size = size_new;
    arg_deinit(arg_data_new);
}
