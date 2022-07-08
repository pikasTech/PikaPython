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
        memcpy(buff, io->input_ptr + io->input_offset, nbyte);
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
    uint16_t y, width_out_rect;
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
        memcpy(direction, source, width_out_rect);

        /* Update pointers */
        source += width_out_rect;

        /* Next line */
        direction += width_rect_buf;
    }

    return 1; /* Continue to decompress */
}

void Converter_JPEGtoRGB888(PikaObj* image) {
    PikaCV_Image* img = obj_getStruct(image, "image");
    if (NULL == img) {
        return;
    }
    if (PikaCV_ImageFormat_Type_JPEG != img->format) {
        /* TODO: error handling */
        return;
    }
    uint8_t* data = obj_getBytes(image, "data");
    if (NULL == data) {
        return;
    }

    size_t sz_work = 3500;
    Arg* arg_work = arg_setBytes(NULL, "", NULL, sz_work);
    JRESULT res;
    JDEC jdec;
    void* work = arg_getBytes(arg_work);
    JpegInterface io;
    io.input_ptr = data;
    io.input_offset = 0;
    io.input_size = obj_getBytesSize(image, "data");
    io.width_pix = 0;
    res = jd_prepare(&jdec, in_func, work, sz_work, &io);

    if (res != JDR_OK) {
        printf("jd_prepare() failed (rc=%d)\n", res);
        goto exit;
    }

    /* It is ready to dcompress and image info is available here */
    printf("Image size is %u x %u.\n%u bytes of work ares is used.\n",
           jdec.width, jdec.height, (int)(sz_work - jdec.sz_pool));
    io.width_pix = jdec.width;

    res = jd_decomp(&jdec, out_func, 0);
    if (res != JDR_OK) {
        printf("jd_decomp() failed (rc=%d)\n", res);
        goto exit;
    }

exit:
    arg_deinit(arg_work);
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
        Converter_JPEGtoRGB888(image);
    }

    int size_new = img->hight * img->width;
    Arg* arg_data_new = arg_setBytes(NULL, "", NULL, size_new);
    uint8_t* data = obj_getBytes(image, "data");
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
    obj_setBytes(image, "data", data_new, size_new);
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
        Converter_JPEGtoRGB888(image);
    }

    int size_new = img->hight * img->width * 2;
    Arg* arg_data_new = arg_setBytes(NULL, "", NULL, size_new);
    uint8_t* data = obj_getBytes(image, "data");
    uint8_t* data_new = arg_getBytes(arg_data_new);
    if (img->format == PikaCV_ImageFormat_Type_RGB888) {
        for (int i = 0; i < img->size; i += 3) {
            data_new[i / 3 * 2] =
                (data[i] & 0xF8) | ((data[i + 1] >> 5) & 0x07);
            data_new[i / 3 * 2 + 1] =
                ((data[i + 1] & 0x1F) << 3) | ((data[i + 2] >> 3) & 0x07);
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
    obj_setBytes(image, "data", data_new, size_new);
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
        Converter_JPEGtoRGB888(image);
    }

    if (img->format == PikaCV_ImageFormat_Type_RGB888) {
        /* do nothing */
        return;
    }

    int size_new = img->hight * img->width * 3;
    Arg* arg_data_new = arg_setBytes(NULL, "", NULL, size_new);
    uint8_t* data = obj_getBytes(image, "data");
    uint8_t* data_new = arg_getBytes(arg_data_new);
    if (img->format == PikaCV_ImageFormat_Type_RGB565) {
        for (int i = 0; i < img->size; i += 2) {
            data_new[i / 2 * 3] = (data[i] & 0xF8) | ((data[i] << 5) & 0x0700);
            data_new[i / 2 * 3 + 1] = ((data[i] << 5) & 0xE000) |
                                      ((data[i + 1] >> 3) & 0x1F00) |
                                      ((data[i + 1] << 3) & 0x00E0);
            data_new[i / 2 * 3 + 2] =
                ((data[i + 1] << 3) & 0xF800) | ((data[i + 1] >> 1) & 0x007);
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
    obj_setBytes(image, "data", data_new, size_new);
    img->format = PikaCV_ImageFormat_Type_RGB565;
    img->size = size_new;
    arg_deinit(arg_data_new);
}
