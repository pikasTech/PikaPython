#include "PikaStdDevice_Time.h"
#include "BaseObj.h"
#if defined(__linux)
#include <unistd.h>
#endif
#if defined(_WIN32)
#include <windows.h>
#endif

void PikaStdDevice_Time_sleep_ms(PikaObj* self, int ms) {
#if defined(__linux)
    usleep(ms * 1000);
#elif defined(_WIN32)
    Sleep(ms);
#else
    ABSTRACT_METHOD_NEED_OVERRIDE_ERROR();
#endif
}
void PikaStdDevice_Time_sleep_s(PikaObj* self, int s) {
#if defined(__linux) 
    sleep(s);
#elif defined(_WIN32)
    Sleep(s * 1000);
#else
    ABSTRACT_METHOD_NEED_OVERRIDE_ERROR();
#endif
}

void PikaStdDevice_Time_platformGetTick(PikaObj* self) {
    obj_setErrorCode(self, 1);
    obj_setSysOut(self, "[error] platformGetTick() need to be override.");
}

/*
 * @Author: Once day
 * @LastEditTime: 2022-06-04 12:10:52
 * Encoder=utf-8,Email:once_day@qq.com
 */

#include "stdint.h"
#include "stdio.h"

//结构体时间类型定义(来源c标准库corect_wtime.h)
//无论是16位整数还是32位整数都满足需求
typedef struct _tm{
    int tm_sec;    // seconds after the minute - [0, 60] including leap second
    int tm_min;    // minutes after the hour - [0, 59]
    int tm_hour;   // hours since midnight - [0, 23]
    int tm_mday;   // day of the month - [1, 31]
    int tm_mon;    // months since January - [0, 11]
    int tm_year;   // years since 1900
    int tm_wday;   // days since Sunday - [0, 6]
    int tm_yday;   // days since January 1 - [0, 365]
    int tm_isdst;  // daylight savings time flag
} _tm;

//时间戳时间类型定义(来源c标准库time.h)
//直接支持64位秒数时间，附加时间精度为ns，根据设备决定，需要1GHz及以上时钟频率才能支持1ns级别时间精度
//内部时间比对数据类型，传递给外界的时候会使用浮点数，所以精度会降低
//但内部使用复合数据类型比对，以实现平台支持的最小时间精度比较
typedef struct {
    int64_t tv_sec;   // Seconds - >= 0
    int32_t tv_nsec;  // Nanoseconds - [0, 999999999]
} _timespec;

//错误处理
typedef int status;

#define TIME_OK 0
#define TIME_ERROR -1
#define TIME_GET_TIME_FAIL 1
#define TIME_GET_TICK_FAIL 2
#define TIME_LESS_THAN_ZERO 3
#define TIME_OVER_3200 4
#define TIME_LESS_THAN_1970 5
#define TIME_ERROR_STRUCT_TIME 6

//错误状态处理函数
void status_deal(status s) {
//输出异常信息
#define time_printf(...) __platform_printf(__VA_ARGS__)

    time_printf("\n[Error-info]Checking a exception : ");
    switch (s) {
        case TIME_ERROR:
            time_printf("Unknow error!!!\n");
            break;
        case TIME_GET_TIME_FAIL:
            time_printf("Fail to get Unix-time from hardware !\n");
            break;
        case TIME_GET_TICK_FAIL:
            time_printf("Fail to get Tick-time from hardware !\n");
            break;
        case TIME_LESS_THAN_ZERO:
            time_printf("Input a negative Unix timestamp !\n");
            break;
        case TIME_OVER_3200:
            time_printf("The time point exceeds 3200 AD !\n");
            break;
        case TIME_LESS_THAN_1970:
            time_printf("The time point less-than 1970 AD !\n");
            break;
        case TIME_ERROR_STRUCT_TIME:
            time_printf("The struct-time's range is wrong !\n");
            break;
        default:
            break;
    }
    time_printf("\n");
}

//获取硬件平台的Unix时间戳,时间精度为1s级别，
status time_get_unix_time(PikaObj* self, _timespec* this_timespec) {
    this_timespec->tv_sec = (int64_t)(obj_getInt(self, "tick") / 1000);
    return TIME_OK;
}

//获取硬件平台的Tick时间,时间精度为1s级别以下
//即1s的小数部分
status time_get_tick_ns(PikaObj* self, _timespec* this_timespec) {
    this_timespec->tv_nsec = (obj_getInt(self, "tick") % 1000) * 1000000;
    return TIME_OK;
}

//标准time()方法，返回以浮点数表示的从 epoch 开始的秒数的时间值。
// epoch 是 1970 年 1 月 1 日 00:00:00 (UTC)，
double time_time(PikaObj* self) {
    status res = 0;  //状态响应
    _timespec temp_timespec = {0};
    //调用硬件平台函数，获取当前时间
    res = time_get_unix_time(self, &temp_timespec);
    if (res) {
        status_deal(res);
    }  //异常处理
    res = time_get_tick_ns(self, &temp_timespec);
    if (res) {
        status_deal(res);
    }  //异常处理
    //以浮点数返回时间，float
    return temp_timespec.tv_sec + (double)temp_timespec.tv_nsec / 1000000000;
}

//标准time_ns()方法，返回以整数表示的从 epoch 开始的纳秒数的时间值。
// epoch 是 1970 年 1 月 1 日 00:00:00 (UTC)，
int64_t time_time_ns(PikaObj* self) {
    status res = 0;  //状态响应
    _timespec temp_timespec = {0};
    //调用硬件平台函数，获取当前时间
    res = time_get_unix_time(self, &temp_timespec);
    if (res) {
        status_deal(res);
    }  //异常处理
    res = time_get_tick_ns(self, &temp_timespec);
    if (res) {
        status_deal(res);
    }  //异常处理
    //以浮点数返回时间，float
    return temp_timespec.tv_sec * 1000000000 + temp_timespec.tv_nsec;
}

//利用基姆拉尔森计算公式计算星期
int time_get_week(const _tm* this_tm) {
    //月份要+1
    int month = this_tm->tm_mon + 1;
    int year = this_tm->tm_year;
    int day = this_tm->tm_mday;
    int w;
    if (month == 1 || month == 2) {
        month += 12;
        year -= 1;
        w = day + 2 * month + 3 * (month + 1) / 5 + year + year / 4 -
            year / 100 + year / 400 + 1;  // 0~6,星期日 ~ 星期六
        w = w % 7;
    } else {
        w = day + 2 * month + 3 * (month + 1) / 5 + year + year / 4 -
            year / 100 + year / 400 + 1;  // 0~6,星期日 ~ 星期六
        w = w % 7;
    }
    return w;
}

//由Unix时间戳计算标准UTC时间
status unix_time_to_utc_struct_time(_tm* this_tm, int64_t unix_time) {
    int32_t total_day;
    int32_t extra_second;
    int year_400, year_100, year_4, year_1;
    int february_offset, temp;  //二月偏移量,零时变量

    //判断是否输入小于0的时间戳
    if (unix_time < 0) {
        //暂不支持小于0的时间戳
        return TIME_LESS_THAN_ZERO;
    }

// Unix时间戳每天秒数是固定的 62*60*24
#define DAY_SECOND (86400)

    total_day = unix_time / DAY_SECOND;
    extra_second = unix_time - total_day * DAY_SECOND;

//为了减少额外闰年判断，把时间往前推到1600年，即闰年最大的一次公倍数开始计算判断
// 1970-1600 = 370 年 ，370/4 -(370/100-1)=90 个闰年
// 1600 DAY_OFFSET 365*(1970-1600)+90 = 135140,7为修正天数
#define YEAR_START (1600)    //初始年份
#define DAY_OFFSET (135140)  //时间偏移量

    total_day += DAY_OFFSET;

//从1600年到3200年有1600/4-(1600/100-1600/400)=388个闰年
//即 MAX_DAY  1600*365+388=584388 day
#define MAX_DAY (584388)  //最大可判断时间天数

    if (total_day > MAX_DAY) {
        //超过3200年的换算暂不支持
        return TIME_OVER_3200;
    } else {
//从1600年开始，天数都要多减一天，因为1600年是闰年
//但是由于日期不包含当天时间，即2月2号，实际是2月1号+时:分:秒
//所以算出来的日期要加上一天
//两者配合，无需加减

//从400年，100年，4年逐渐缩小范围
// 400个公历年天数为365*400+97=146097天
// 400年内的100个公历年天数为365*100+24=36524天
// 100年内的4年365*4+1=1461天
#define DAY_OF_400Y (146097)
#define DAY_OF_100Y (36524)
#define DAY_OF_4Y (1461)
#define DAY_OF_1Y (365)
        // 400年也要注意，要实际401年才可
        year_400 = (total_day - 366) / DAY_OF_400Y;
        total_day -= year_400 * DAY_OF_400Y;
        //计算400年内的情况
        year_100 = (total_day - 1) / DAY_OF_100Y;
        total_day -= year_100 * DAY_OF_100Y;
        //计算100年内的情况，要到第二年的第一天才算，即365+1
        year_4 = (total_day - 366) / DAY_OF_4Y;
        //计算4年，需要格外注意0-5-8年，才会计算一个闰年，因为它才包含了4这个闰年，但并不包含8
        total_day -= year_4 * DAY_OF_4Y;
        //计算4年内的情况
        //需要减去1天，因为当天是不存在的
        //需要注意闰年会多一天
        //所有闰年都放在这里来考虑，即只要当前是闰年，那么这里就会剩下第一年闰年和第四年闰年两种情况
        if (year_100 == 4) {
            //第一年是闰年,此时为400*n+1年内
            year_1 = 0;
            february_offset = 1;
        } else if (total_day <= DAY_OF_1Y * 4) {
            // 100*n+(4,8,...96)+1年，都是从第二年算起，非闰年
            //非闰年,需要减去1天，因为当天是不存在的
            year_1 = (total_day - 1) / DAY_OF_1Y;
            total_day -= year_1 * DAY_OF_1Y;
            february_offset = 0;
        } else {
            //第四年是闰年
            year_1 = 4;
            total_day -= year_1 * DAY_OF_1Y;
            february_offset = 1;
        }

        //计算出当前年份
        this_tm->tm_year =
            (year_400 * 400 + year_100 * 100 + year_4 * 4 + year_1) +
            YEAR_START;
        //保存一年的天数
        this_tm->tm_yday = total_day;

        //剩下的天数为1年内的天数,直接计算月和日
        //根据当前是否为闰年设置二月偏移量是否为1
        //能被4整除且不被100整除或者能被400整除

        //闰年需要减去一天再计算
        total_day -= february_offset;
        //使用二分法快速定位月份，使用平年计算，在月份确定到2月时，再考虑闰年
        //判断是否在1-6月里面
        // 31, 28, 31, 30, 31, 30,    31, 31, 30, 31, 30, 31
        if (total_day <= 181) {
            //判断是否在1-3月里面
            if (total_day <= 90) {
                //判断是否在1-2月里面
                if (total_day <= 59) {
                    total_day += february_offset;  //去掉二月的偏置
                    if (total_day <= 31) {
                        // 1月
                        temp = 0;
                    } else {
                        total_day -= 31;
                        // 2月
                        temp = 1;
                    }
                } else {
                    total_day -= 59;
                    // 3月
                    temp = 2;
                }
            } else {
                // 4-6月
                total_day -= 90;
                //是否在4月里面
                if (total_day <= 30) {
                    // 4月
                    temp = 3;
                } else {
                    // 5-6月
                    total_day -= 30;
                    if (total_day <= 31) {
                        // 5月
                        temp = 4;
                    } else {
                        total_day -= 31;
                        // 6月
                        temp = 5;
                    }
                }
            }
        } else {
            total_day -= 181;
            //判断是否在7-9月里面
            if (total_day <= 92) {
                //是否在7-8月
                if (total_day <= 62) {
                    if (total_day <= 31) {
                        // 7月
                        temp = 6;
                    } else {
                        total_day -= 31;
                        // 8月
                        temp = 7;
                    }
                } else {
                    // 9月
                    total_day -= 62;
                    temp = 8;
                }
            } else {
                // 10-12月
                total_day -= 92;
                //是否在10-11月
                if (total_day <= 61) {
                    if (total_day <= 31) {
                        // 10月
                        temp = 9;
                    } else {
                        // 11 月
                        total_day -= 31;
                        temp = 10;
                    }
                } else {
                    // 12月
                    total_day -= 61;
                    temp = 11;
                }
            }
        }

        //记录当前月份和天数
        this_tm->tm_mon = temp;        //月份 [0,11]
        this_tm->tm_mday = total_day;  //天数

        //利用额外秒数计算时-分-秒
        temp = extra_second / 3600;
        this_tm->tm_hour = temp;
        extra_second = extra_second - temp * 3600;

        temp = extra_second / 60;
        this_tm->tm_min = temp;
        extra_second = extra_second - temp * 60;

        this_tm->tm_sec = extra_second;

        //计算出当前日期的星期数
        this_tm->tm_wday = time_get_week(this_tm);

        //夏令时不明
        this_tm->tm_isdst = -1;
    }
    return TIME_OK;
}

//由标准UTC时间生成Unix时间戳
status utc_struct_time_to_unix_time(const _tm* this_tm, int64_t* unix_time) {
    int32_t total_day, total_leap_year, dyear;
    int february_offset;  //二月偏移量,零时变量
    // 31, 28, 31, 30, 31, 30,    31, 31, 30, 31, 30, 31
    //每个月份对应前面所有月的天数
    const int month_day[] = {0,   31,  59,  90,  120, 151,
                             181, 212, 243, 273, 304, 334};

    //每天总秒数一定，将UTC时间（年月）转换成天数
    //为了减少额外闰年判断，把时间往前推到1600年，即闰年最大的一次公倍数开始计算判断
    // 1970-1600 = 370 年 ，370/4 -(370/100-1)=90 个闰年
    // 1600 DAY_OFFSET 365*(1970-1600)+90 = 135140,7为修正天数

    if (this_tm->tm_year < 1970) {
        //暂不支持1970之前的时间
        *unix_time = 0;
        return TIME_LESS_THAN_1970;
    }
    if (this_tm->tm_year >= 3200) {
        //暂不支持3200及以后的时间
        *unix_time = 0;
        return TIME_OVER_3200;
    }

    //计算总年数要去掉尾巴，如年数20年，那么实际应该4个闰年，因为20这一年没有包含在里面
    //要减去一年来算闰年次数
    //先计算到相对1600年的天数，再转换到1970年
    dyear = this_tm->tm_year - YEAR_START - 1;
    total_leap_year = dyear / 4 - (dyear / 100 - dyear / 400 - 1);
    //恢复减去的一年
    dyear += 1;
    total_day = dyear * 365 + total_leap_year;

    //减去1970到1600的总天数
    total_day -= DAY_OFFSET;

    //增加月和日的总天数
    //判断是否是闰年
    //能被4整除且不被100整除或者能被400整除
    if (((dyear % 4 == 0) && (dyear % 100 != 0)) || (dyear % 400 == 0)) {
        //闰年
        february_offset = 1;
    } else {
        february_offset = 0;
    }

    //计算含月和日的总天数,日期要减去当天
    total_day += month_day[this_tm->tm_mon] + this_tm->tm_mday - 1;
    //二月以上需要加上偏移量
    if (this_tm->tm_mon > 1) {
        total_day += february_offset;
    }

    //根据天数以及时分秒计算Unix时间戳
    *unix_time = (int64_t)total_day * DAY_SECOND + this_tm->tm_hour * 3600 +
                 this_tm->tm_min * 60 + this_tm->tm_sec;

    return TIME_OK;
}

void time_struct_format(const _tm* this_tm, char* str) {
    sprintf(str,
            "time.struct_time(tm_year=%d, tm_mon=%d,tm_mday=%d, tm_hour=%d, "
            "tm_min=%d, tm_sec=%d, tm_wday=%d,tm_yday=%d, tm_isdst=%d)",
            this_tm->tm_year, this_tm->tm_mon + 1, this_tm->tm_mday,
            this_tm->tm_hour, this_tm->tm_min, this_tm->tm_sec,
            this_tm->tm_wday, this_tm->tm_yday, this_tm->tm_isdst);
}

//标准库函数gmtime,将以自 epoch 开始的秒数表示的时间转换为 UTC 的 struct_time
void time_gmtime(double unix_time, _tm* this_tm) {
    status res;
    //转化时间
    res = unix_time_to_utc_struct_time(this_tm, (int64_t)unix_time);
    if (res) {
        status_deal(res);  //异常情况处理
        //返回默认值
        // note: 异常情况返回默认时间起始点
        unix_time_to_utc_struct_time(this_tm, (int64_t)0);
    }
}

//标准库函数localtime,将以自 epoch 开始的秒数表示的时间转换为当地时间的
// struct_time
void time_localtime(double unix_time, _tm* this_tm, int locale) {
    status res;
    int local_offset;

    //获取本地时间偏移量(小时)
    local_offset = locale * 60 * 60;

    //转化时间
    res = unix_time_to_utc_struct_time(this_tm,
                                       (int64_t)unix_time + local_offset);
    if (res) {
        status_deal(res);  //异常情况处理
        //这里处理的策略和标准库不同，标准库最初始的时间是1970-1-1,00:00:00，对于不同时区来说，其值是不一样的
        //但本函数是要求各时区的起始时间不超过1970-1-1,00:00:00，实际上UTC时间可以更前，可靠的最早时间可到1600年
        //对于西时区来说，时间会缺失
        unix_time_to_utc_struct_time(this_tm, (int64_t)0);
    }
}

//检测结构体时间是否在合适的范围内，但不检查它的正确性
status time_check_struct_time(const _tm* this_tm) {
    if (this_tm->tm_sec < 0 || this_tm->tm_sec > 60) {
        return TIME_ERROR_STRUCT_TIME;
    }
    if (this_tm->tm_min < 0 || this_tm->tm_min > 59) {
        return TIME_ERROR_STRUCT_TIME;
    }
    if (this_tm->tm_hour < 0 || this_tm->tm_hour > 23) {
        return TIME_ERROR_STRUCT_TIME;
    }
    if (this_tm->tm_mday < 1 || this_tm->tm_mday > 31) {
        return TIME_ERROR_STRUCT_TIME;
    }
    if (this_tm->tm_mon < 0 || this_tm->tm_mon > 11) {
        return TIME_ERROR_STRUCT_TIME;
    }
    if (this_tm->tm_wday < 0 || this_tm->tm_wday > 6) {
        return TIME_ERROR_STRUCT_TIME;
    }
    if (this_tm->tm_yday < 0 || this_tm->tm_yday > 366) {
        return TIME_ERROR_STRUCT_TIME;
    }
    return TIME_OK;
}

//标准库函数mktime(t),将当地时间的
// struct_time转换为以自epoch开始的秒数表示的时间
int64_t time_mktime(const _tm* this_tm, int locale) {
    status res;
    int local_offset;
    int64_t unix_time;

    //获取本地时间偏移量(小时)
    local_offset = locale * 60 * 60;

    //检测时间结构体范围正确性
    res = time_check_struct_time(this_tm);
    if (res) {
        status_deal(res);
        return 0;
    }  //异常情况返回时间零点

    //转化时间
    res = utc_struct_time_to_unix_time(this_tm, &unix_time);
    if (res) {
        status_deal(res);
        return 0;
    }  //异常情况返回时间零点
    //减去本地偏移时间
    //可能出现负数，严格来说，这不影响什么！
    unix_time -= local_offset;

    //显示出来
    // time_printf("%I64d\n",unix_time);

    //返回数据
    return unix_time;
}

//标准库函数asctime()
//把结构化时间struct_time元组表示为以下形式的字符串: `'Sun Jun 20 23:21:05
// 1993'`。
void time_asctime(const _tm* this_tm) {
    //星期缩写，python标准库是三个字母，这里并不相同
    const char* week[] = {"Sun", "Mon", "Tues", "Wed", "Thur", "Fri", "Sat"};
    //月份缩写
    const char* month[] = {"Jan", "Feb", "Mar",  "Apr", "May", "Jun",
                           "Jul", "Aug", "Sept", "Oct", "Nov", "Dec"};

    char str[100];

    sprintf(str, "%s %s %d %02d:%02d:%02d %d", week[this_tm->tm_wday],
            month[this_tm->tm_mon], this_tm->tm_mday, this_tm->tm_hour,
            this_tm->tm_min, this_tm->tm_sec, this_tm->tm_year);
    time_printf("%s\n", str);
}

double PikaStdDevice_Time_time(PikaObj* self) {
    /* run platformGetTick() */
    PIKA_PYTHON_BEGIN
    /* clang-format off */
        PIKA_PYTHON(
            platformGetTick()
        )
    /* clang-format on */
    const uint8_t bytes[] = {
        0x04, 0x00,             /* instruct array size */
        0x00, 0x82, 0x01, 0x00, /* instruct array */
        0x11, 0x00,             /* const pool size */
        0x00, 0x70, 0x6c, 0x61, 0x74, 0x66, 0x6f, 0x72, 0x6d,
        0x47, 0x65, 0x74, 0x54, 0x69, 0x63, 0x6b, 0x00, /* const pool */
    };
    PIKA_PYTHON_END
    pikaVM_runByteCode(self, (uint8_t*)bytes);
    return time_time(self);
}

int PikaStdDevice_Time_time_ns(PikaObj* self) {
    return time_time_ns(self);
}

void time_set_tm_value(PikaObj* self, const _tm* this_tm) {
#if !PIKA_STD_DEVICE_UNIX_TIME_ENABLE
    obj_setErrorCode(self, 1);
    obj_setSysOut(
        self, "[error] PIKA_STD_DEVICE_UNIX_TIME_ENABLE need to be enable.");
#else

    obj_setInt(self, "tm_sec", this_tm->tm_sec);
    obj_setInt(self, "tm_min", this_tm->tm_min);
    obj_setInt(self, "tm_hour", this_tm->tm_hour);
    obj_setInt(self, "tm_mday", this_tm->tm_mday);
    obj_setInt(self, "tm_mon", this_tm->tm_mon);
    obj_setInt(self, "tm_year", this_tm->tm_year);
    obj_setInt(self, "tm_wday", this_tm->tm_wday);
    obj_setInt(self, "tm_yday", this_tm->tm_yday);
    obj_setInt(self, "tm_isdst", this_tm->tm_isdst);

#endif
}

void PikaStdDevice_Time_gmtime(PikaObj* self, double unix_time) {
#if !PIKA_STD_DEVICE_UNIX_TIME_ENABLE
    obj_setErrorCode(self, 1);
    obj_setSysOut(
        self, "[error] PIKA_STD_DEVICE_UNIX_TIME_ENABLE need to be enable.");
#else
    _tm this_tm;
    char str[200];
    time_gmtime(unix_time, &this_tm);
    time_set_tm_value(self, &this_tm);
    //格式化字符
    time_struct_format(&this_tm, str);
    //显示出来
    time_printf("%s\n", str);
#endif
}

void PikaStdDevice_Time_localtime(PikaObj* self, double unix_time) {
#if !PIKA_STD_DEVICE_UNIX_TIME_ENABLE
    obj_setErrorCode(self, 1);
    obj_setSysOut(
        self, "[error] PIKA_STD_DEVICE_UNIX_TIME_ENABLE need to be enable.");
#else
    _tm this_tm;
    char str[200];
    int locale = obj_getInt(self, "locale");
    time_localtime(unix_time, &this_tm, locale);
    time_set_tm_value(self, &this_tm);
    //格式化字符
    time_struct_format(&this_tm, str);
    //显示出来
    time_printf("%s\n", str);
#endif
}

void time_get_tm_value(PikaObj* self, _tm* this_tm) {
#if !PIKA_STD_DEVICE_UNIX_TIME_ENABLE
    obj_setErrorCode(self, 1);
    obj_setSysOut(
        self, "[error] PIKA_STD_DEVICE_UNIX_TIME_ENABLE need to be enable.");
#else
    this_tm->tm_sec = obj_getInt(self, "tm_sec");
    this_tm->tm_min = obj_getInt(self, "tm_min");
    this_tm->tm_hour = obj_getInt(self, "tm_hour");
    this_tm->tm_mday = obj_getInt(self, "tm_mday");
    this_tm->tm_mon = obj_getInt(self, "tm_mon");
    this_tm->tm_year = obj_getInt(self, "tm_year");
    this_tm->tm_wday = obj_getInt(self, "tm_wday");
    this_tm->tm_yday = obj_getInt(self, "tm_yday");
    this_tm->tm_isdst = obj_getInt(self, "tm_isdst");
#endif
}

int PikaStdDevice_Time_mktime(PikaObj* self) {
#if !PIKA_STD_DEVICE_UNIX_TIME_ENABLE
    obj_setErrorCode(self, 1);
    obj_setSysOut(
        self, "[error] PIKA_STD_DEVICE_UNIX_TIME_ENABLE need to be enable.");
    return 0;
#else
    _tm this_tm;
    int locale = obj_getInt(self, "locale");
    time_get_tm_value(self, &this_tm);
    return time_mktime(&this_tm, locale);
#endif
}

void PikaStdDevice_Time_asctime(PikaObj* self) {
#if !PIKA_STD_DEVICE_UNIX_TIME_ENABLE
    obj_setErrorCode(self, 1);
    obj_setSysOut(
        self, "[error] PIKA_STD_DEVICE_UNIX_TIME_ENABLE need to be enable.");
#else
    _tm this_tm;
    time_get_tm_value(self, &this_tm);
    time_asctime(&this_tm);
#endif
}
void PikaStdDevice_Time_ctime(PikaObj* self, double unix_time) {
#if !PIKA_STD_DEVICE_UNIX_TIME_ENABLE
    obj_setErrorCode(self, 1);
    obj_setSysOut(
        self, "[error] PIKA_STD_DEVICE_UNIX_TIME_ENABLE need to be enable.");
#else
    _tm this_tm;
    int locale = obj_getInt(self, "locale");
    time_localtime(unix_time, &this_tm, locale);
    time_asctime(&this_tm);
#endif
}

void PikaStdDevice_Time___init__(PikaObj* self) {
#if !PIKA_STD_DEVICE_UNIX_TIME_ENABLE
#else
    _tm this_tm;
    obj_setInt(self, "locale", 8);
    time_localtime(0.0, &this_tm, 8);
    time_set_tm_value(self, &this_tm);
#endif
}

void PikaStdDevice_Time_sleep(PikaObj* self, double s) {
    Args* args = New_args(NULL);
    args_setInt(args, "ms", s * 1000);
    obj_runNativeMethod(self, "sleep_ms", args);
    args_deinit(args);
}
