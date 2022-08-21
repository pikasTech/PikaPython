#include "PikaCV_Filter.h"
#include "PikaCV_Converter.h"
#include "PikaCV_common.h"
#include "dataQueue.h"

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

typedef struct _Range
{
    uint8_t start, end;
} Range;
Range new_Range(uint8_t s, uint8_t e)
{
    Range r;
    r.start = s;
    r.end = e;
    return r;
}
void swap(uint8_t *x, uint8_t *y)
{
    uint8_t t = *x;
    *x = *y;
    *y = t;
}
void quick_sort(uint8_t arr[], const int len)
{
	Range *r;
	int p = 0;

    if (len <= 0){
        return;
    }

	r = (Range*)pikaMalloc(sizeof(Range)*len);
	if (r == NULL) {
		return;
	}
    
    r[p++] = new_Range(0, len - 1);
    while (p)
    {
        Range range = r[--p];
        if (range.start >= range.end)
            continue;
        int mid = arr[(range.start + range.end) / 2];
        int left = range.start, right = range.end;
        do
        {
            while (arr[left] < mid)
                ++left;
            while (arr[right] > mid)
                --right;

            if (left <= right)
            {
                swap(&arr[left], &arr[right]);
                left++;
                right--;
            }
        } while (left <= right);

        if (range.start < right)
            r[p++] = new_Range(range.start, right);
        if (range.end > left)
            r[p++] = new_Range(left, range.end);
    }

	pikaFree((void*)r, sizeof(Range)*len);
}

void PikaCV_Filter_medianFilter(PikaObj *self, PikaObj *image)
{
    PikaCV_Image *src = obj_getStruct(image, "image");
    int width = src->width;
    int height = src->height;
    int ksizex = 3;
    int ksizey = 3;

    if (NULL == src)
    {
        pika_assert(0);
        return;
    }

    if (src->format == PikaCV_ImageFormat_Type_GRAY)
    {
        int hh = (ksizey - 1) / 2;
        int hw = (ksizex - 1) / 2;
        int width_new = width - ksizex + 1;
        int height_new = height - ksizey + 1;
        const int size_new = width_new * height_new;

        uint8_t data_tmp[9] = {0};

        Arg *arg_data_new = arg_setBytes(NULL, "", NULL, size_new);
        uint8_t *data = _image_getData(image);
        uint8_t *data_new = arg_getBytes(arg_data_new);
        for (int i = hh; i < height_new + hh; ++i)
        {
            for (int j = hw; j < width_new + hw; ++j)
            {
                int index = 0;
                for (int r = i - hh; r <= i + hh; ++r)
                {
                    for (int c = j - hw; c <= j + hw; ++c)
                    {
                        data_tmp[index++] = data[r * width + c];
                    }
                }
                quick_sort(data_tmp, 9);
                data_new[(i - hh) * width_new + (j - hw)] = data_tmp[4];
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
    int width_new = width - ksizex + 1;
    int height_new = height - ksizey + 1;
    const int size_new = width_new * height_new * 3;

    uint8_t data_tmpr[9] = {0};
    uint8_t data_tmpg[9] = {0};
    uint8_t data_tmpb[9] = {0};

    Arg *arg_data_new = arg_setBytes(NULL, "", NULL, size_new);
    uint8_t *data = _image_getData(image);
    uint8_t *data_new = arg_getBytes(arg_data_new);
    for (int i = hh; i < height_new + hh; ++i)
    {
        for (int j = hw; j < width_new + hw; ++j)
        {
            int index = 0;
            for (int r = i - hh; r <= i + hh; ++r)
            {
                for (int c = j - hw; c <= j + hw; ++c)
                {
                    data_tmpr[index] = data[r * width * 3 + c * 3];
                    data_tmpg[index] = data[r * width * 3 + c * 3 + 1];
                    data_tmpb[index] = data[r * width * 3 + c * 3 + 2];
                    index++;
                }
            }
            quick_sort(data_tmpr, 9);
            quick_sort(data_tmpg, 9);
            quick_sort(data_tmpb, 9);

            data_new[(i - hh) * width_new * 3 + (j - hw) * 3] = data_tmpr[4];
            data_new[(i - hh) * width_new * 3 + (j - hw) * 3 + 1] = data_tmpg[4];
            data_new[(i - hh) * width_new * 3 + (j - hw) * 3 + 2] = data_tmpb[4];
        }
    }
    src->height = height_new;
    src->width = width_new;
    src->size = size_new;
    obj_setBytes(image, "_data", data_new, size_new);
    arg_deinit(arg_data_new);

    return;
}