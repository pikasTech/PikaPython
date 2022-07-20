#include "PikaCV_Filter.h"
#include "PikaCV_Converter.h"
#include "PikaCV_common.h"

void PikaCV_Filter_meanFilter(PikaObj *self, PikaObj *image, int ksizex, int ksizey)
{
    PikaCV_Image *src = obj_getStruct(image, "image");
    int width = src->width;
    int height = src->height;

    if (NULL == src)
    {
        pika_assert(0);
        return;
    }
    if (ksizex % 2 == 0 || ksizey % 2 == 0)
    {
        pika_assert(0);
        return;
    }

    if (src->format == PikaCV_ImageFormat_Type_GRAY)
    {
        int hh = (ksizey - 1) / 2;
        int hw = (ksizex - 1) / 2;
        int sum = 0;
        int mean = 0;
        int area = ksizex * ksizey;
        int width_new = width - ksizex + 1;
        int height_new = height - ksizey + 1;
        int size_new = width_new * height_new;
        Arg *arg_data_new = arg_setBytes(NULL, "", NULL, size_new);
        uint8_t *data = _image_getData(image);
        uint8_t *data_new = arg_getBytes(arg_data_new);
        for (int i = hh; i < height_new + hh; ++i)
        {
            for (int j = hw; j < width_new + hw; ++j)
            {

                for (int r = i - hh; r <= i + hh; ++r)
                {
                    for (int c = j - hw; c <= j + hw; ++c)
                    {
                        // sum = Newsrc.at<uchar>(r, c) + sum;
                        sum += data[r * width + c];
                    }
                }
                // mean = sum / (wsize.area());
                // dst.at<uchar>(i-hh,j-hw)=mean;
                mean = sum / area;
                data_new[(i - hh) * width_new + (j - hw)] = mean;
                sum = 0;
                mean = 0;
            }
        }
        src->height = height_new;
        src->width = width_new;
        src->size = size_new;
        obj_setBytes(image, "_data", data_new, size_new);
        arg_deinit(arg_data_new);

        return;
    }
    else if (src->format != PikaCV_ImageFormat_Type_RGB888)
    {
        PikaCV_Converter_toRGB888(self, image);
    }

    int hh = (ksizey - 1) / 2;
    int hw = (ksizex - 1) / 2;
    int sumr = 0, sumg = 0, sumb = 0;
    int meanr = 0, meang = 0, meanb = 0;
    int area = ksizex * ksizey;
    int width_new = width - ksizex + 1;
    int height_new = height - ksizey + 1;
    int size_new = width_new * height_new * 3;
    Arg *arg_data_new = arg_setBytes(NULL, "", NULL, size_new);
    uint8_t *data = _image_getData(image);
    uint8_t *data_new = arg_getBytes(arg_data_new);
    for (int i = hh; i < height_new + hh; ++i)
    {
        for (int j = hw; j < width_new + hw; ++j)
        {
            for (int r = i - hh; r <= i + hh; ++r)
            {
                for (int c = j - hw; c <= j + hw; ++c)
                {
                    sumr += data[r * width * 3 + c * 3];
                    sumg += data[r * width * 3 + c * 3 + 1];
                    sumb += data[r * width * 3 + c * 3 + 2];
                }
            }
            meanr = sumr / area;
            meang = sumg / area;
            meanb = sumb / area;
            data_new[(i - hh) * width_new * 3 + (j - hw) * 3] = meanr;
            data_new[(i - hh) * width_new * 3 + (j - hw) * 3 + 1] = meang;
            data_new[(i - hh) * width_new * 3 + (j - hw) * 3 + 2] = meanb;
            sumr = 0;
            sumg = 0;
            sumb = 0;
            meanb = 0;
            meang = 0;
            meanr = 0;
        }
    }
    src->height = height_new;
    src->width = width_new;
    src->size = size_new;
    obj_setBytes(image, "_data", data_new, size_new);
    arg_deinit(arg_data_new);

    return;
}