#include "_time.h"
#include "_time_struct_time.h"
#include "PikaVM.h"
#if defined(__linux)
#include <unistd.h>
#endif
#if defined(_WIN32)
#include <windows.h>
#endif

void (*global_do_sleep_ms)(uint32_t);
extern volatile VMState g_PikaVMState;
volatile int g_pika_local_timezone = 8;

static void _do_sleep_ms_tick(uint32_t ms) {
    pika_sleep_ms(ms);
}

void _time_sleep_ms(PikaObj* self, int ms) {
    pika_GIL_EXIT();
    global_do_sleep_ms(ms);
    pika_GIL_ENTER();
}

void _time_sleep_s(PikaObj* self, int s) {
    pika_GIL_EXIT();
    for (int i = 0; i < s; i++) {
        global_do_sleep_ms(1000);
    }
    pika_GIL_ENTER();
}

void _time_platformGetTick(PikaObj* self) {
    obj_setInt(self, "tick", __platform_getTick());
}

/*
 * @Author: Once day
 * @LastEditTime: 2022-06-04 12:10:52
 * Encoder=utf-8,Email:once_day@qq.com
 */

#include "stdint.h"
#include "stdio.h"

typedef struct _tm {
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

typedef struct {
    int64_t tv_sec;   // Seconds - >= 0
    int32_t tv_nsec;  // Nanoseconds - [0, 999999999]
} _timespec;

typedef int status;

#define TIME_OK 0
#define TIME_ERROR -1
#define TIME_GET_TIME_FAIL 1
#define TIME_GET_TICK_FAIL 2
#define TIME_LESS_THAN_ZERO 3
#define TIME_OVER_3200 4
#define TIME_LESS_THAN_1970 5
#define TIME_ERROR_STRUCT_TIME 6

void status_deal(status s) {
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

status time_get_unix_time(PikaObj* self, _timespec* this_timespec) {
    this_timespec->tv_sec = (int64_t)(obj_getInt(self, "tick") / 1000);
    return TIME_OK;
}

status time_get_tick_ns(PikaObj* self, _timespec* this_timespec) {
    this_timespec->tv_nsec = (obj_getInt(self, "tick") % 1000) * 1000000;
    return TIME_OK;
}

pika_float time_time(PikaObj* self) {
    status res = 0;
    _timespec temp_timespec = {0};
    res = time_get_unix_time(self, &temp_timespec);
    if (res) {
        status_deal(res);
    }
    res = time_get_tick_ns(self, &temp_timespec);
    if (res) {
        status_deal(res);
    }
    return temp_timespec.tv_sec +
           (pika_float)temp_timespec.tv_nsec / 1000000000;
}

int64_t time_time_ns(PikaObj* self) {
    status res = 0;
    _timespec temp_timespec = {0};
    res = time_get_unix_time(self, &temp_timespec);
    if (res) {
        status_deal(res);
    }
    res = time_get_tick_ns(self, &temp_timespec);
    if (res) {
        status_deal(res);
    }
    return temp_timespec.tv_sec * 1000000000 + temp_timespec.tv_nsec;
}

int time_get_week(const _tm* this_tm) {
    int month = this_tm->tm_mon + 1;
    int year = this_tm->tm_year;
    int day = this_tm->tm_mday;
    int w;
    if (month == 1 || month == 2) {
        month += 12;
        year -= 1;
        w = day + 2 * month + 3 * (month + 1) / 5 + year + year / 4 -
            year / 100 + year / 400 + 1;  //
        w = w % 7;
    } else {
        w = day + 2 * month + 3 * (month + 1) / 5 + year + year / 4 -
            year / 100 + year / 400 + 1;  //
        w = w % 7;
    }
    return w;
}

status unix_time_to_utc_struct_time(_tm* this_tm, int64_t unix_time) {
    int32_t total_day;
    int32_t extra_second;
    int year_400, year_100, year_4, year_1;
    int february_offset, temp;

    if (unix_time < 0) {
        return TIME_LESS_THAN_ZERO;
    }

#define DAY_SECOND (86400)

    total_day = unix_time / DAY_SECOND;
    extra_second = unix_time - total_day * DAY_SECOND;

#define YEAR_START (1600)
#define DAY_OFFSET (135140)

    total_day += DAY_OFFSET;

#define MAX_DAY (584388)

    if (total_day > MAX_DAY) {
        return TIME_OVER_3200;
    } else {
#define DAY_OF_400Y (146097)
#define DAY_OF_100Y (36524)
#define DAY_OF_4Y (1461)
#define DAY_OF_1Y (365)
        year_400 = (total_day - 366) / DAY_OF_400Y;
        total_day -= year_400 * DAY_OF_400Y;
        year_100 = (total_day - 1) / DAY_OF_100Y;
        total_day -= year_100 * DAY_OF_100Y;
        year_4 = (total_day - 366) / DAY_OF_4Y;
        total_day -= year_4 * DAY_OF_4Y;
        if (year_100 == 4) {
            year_1 = 0;
            february_offset = 1;
        } else if (total_day <= DAY_OF_1Y * 4) {
            year_1 = (total_day - 1) / DAY_OF_1Y;
            total_day -= year_1 * DAY_OF_1Y;
            february_offset = 0;
        } else {
            year_1 = 4;
            total_day -= year_1 * DAY_OF_1Y;
            february_offset = 1;
        }

        this_tm->tm_year =
            (year_400 * 400 + year_100 * 100 + year_4 * 4 + year_1) +
            YEAR_START;
        this_tm->tm_yday = total_day;

        total_day -= february_offset;
        if (total_day <= 181) {
            if (total_day <= 90) {
                if (total_day <= 59) {
                    total_day += february_offset;
                    if (total_day <= 31) {
                        temp = 0;
                    } else {
                        total_day -= 31;
                        temp = 1;
                    }
                } else {
                    total_day -= 59;
                    temp = 2;
                }
            } else {
                total_day -= 90;
                if (total_day <= 30) {
                    temp = 3;
                } else {
                    total_day -= 30;
                    if (total_day <= 31) {
                        temp = 4;
                    } else {
                        total_day -= 31;
                        temp = 5;
                    }
                }
            }
        } else {
            total_day -= 181;
            if (total_day <= 92) {
                if (total_day <= 62) {
                    if (total_day <= 31) {
                        temp = 6;
                    } else {
                        total_day -= 31;
                        temp = 7;
                    }
                } else {
                    total_day -= 62;
                    temp = 8;
                }
            } else {
                total_day -= 92;
                if (total_day <= 61) {
                    if (total_day <= 31) {
                        temp = 9;
                    } else {
                        total_day -= 31;
                        temp = 10;
                    }
                } else {
                    total_day -= 61;
                    temp = 11;
                }
            }
        }

        this_tm->tm_mon = temp;
        this_tm->tm_mday = total_day;

        temp = extra_second / 3600;
        this_tm->tm_hour = temp;
        extra_second = extra_second - temp * 3600;

        temp = extra_second / 60;
        this_tm->tm_min = temp;
        extra_second = extra_second - temp * 60;

        this_tm->tm_sec = extra_second;

        this_tm->tm_wday = time_get_week(this_tm);

        this_tm->tm_isdst = 0;
    }
    return TIME_OK;
}

status utc_struct_time_to_unix_time(const _tm* this_tm, int64_t* unix_time) {
    int32_t total_day, total_leap_year, dyear;
    int february_offset;
    // 31, 28, 31, 30, 31, 30,    31, 31, 30, 31, 30, 31
    const int month_day[] = {0,   31,  59,  90,  120, 151,
                             181, 212, 243, 273, 304, 334};

    if (this_tm->tm_year < 1970) {
        *unix_time = 0;
        return TIME_LESS_THAN_1970;
    }
    if (this_tm->tm_year >= 3200) {
        *unix_time = 0;
        return TIME_OVER_3200;
    }

    dyear = this_tm->tm_year - YEAR_START - 1;
    total_leap_year = dyear / 4 - (dyear / 100 - dyear / 400 - 1);
    dyear += 1;
    total_day = dyear * 365 + total_leap_year;

    total_day -= DAY_OFFSET;

    if (((dyear % 4 == 0) && (dyear % 100 != 0)) || (dyear % 400 == 0)) {
        february_offset = 1;
    } else {
        february_offset = 0;
    }

    total_day += month_day[this_tm->tm_mon] + this_tm->tm_mday - 1;
    if (this_tm->tm_mon > 1) {
        total_day += february_offset;
    }

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

void time_gmtime(pika_float unix_time, _tm* this_tm) {
    status res;
    res = unix_time_to_utc_struct_time(this_tm, (int64_t)unix_time);
    if (res) {
        status_deal(res);
        unix_time_to_utc_struct_time(this_tm, (int64_t)0);
    }
}

//  struct_time
void time_localtime(pika_float unix_time, _tm* this_tm, int locale) {
    status res;
    int local_offset;

    local_offset = locale * 60 * 60;

    res = unix_time_to_utc_struct_time(this_tm,
                                       (int64_t)unix_time + local_offset);
    if (res) {
        status_deal(res);
        unix_time_to_utc_struct_time(this_tm, (int64_t)0);
    }
}

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

int64_t time_mktime(const _tm* this_tm, int locale) {
    status res;
    int local_offset;
    int64_t unix_time;

    local_offset = locale * 60 * 60;

    res = time_check_struct_time(this_tm);
    if (res) {
        status_deal(res);
        return 0;
    }

    res = utc_struct_time_to_unix_time(this_tm, &unix_time);
    if (res) {
        status_deal(res);
        return 0;
    }
    unix_time -= local_offset;

    //  time_printf("%I64d\n",unix_time);

    return unix_time;
}

pika_float _time_time(PikaObj* self) {
    /* run platformGetTick() */
    PIKA_PYTHON_BEGIN
    /* clang-format off */
        PIKA_PYTHON(
            platformGetTick()
        )
    /* clang-format on */
    const uint8_t bytes[] = {
        0x04, 0x00, 0x00, 0x00, /* instruct array size */
        0x00, 0x82, 0x01, 0x00, /* instruct array */
        0x11, 0x00, 0x00, 0x00, /* const pool size */
        0x00, 0x70, 0x6c, 0x61, 0x74, 0x66, 0x6f, 0x72, 0x6d,
        0x47, 0x65, 0x74, 0x54, 0x69, 0x63, 0x6b, 0x00, /* const pool */
    };
    PIKA_PYTHON_END
    pikaVM_runByteCode(self, (uint8_t*)bytes);
    return time_time(self);
}

int _time_time_ns(PikaObj* self) {
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

void _time_gmtime(PikaObj* self, pika_float unix_time) {
#if !PIKA_STD_DEVICE_UNIX_TIME_ENABLE
    obj_setErrorCode(self, 1);
    obj_setSysOut(
        self, "[error] PIKA_STD_DEVICE_UNIX_TIME_ENABLE need to be enable.");
#else
    _tm this_tm;
    char str[200];
    time_gmtime(unix_time, &this_tm);
    time_set_tm_value(self, &this_tm);
    time_struct_format(&this_tm, str);
    time_printf("%s\n", str);
#endif
}

PikaObj* _time_localtime(PikaObj* self, pika_float unix_time) {
#if !PIKA_STD_DEVICE_UNIX_TIME_ENABLE
    obj_setErrorCode(self, 1);
    obj_setSysOut(
        self, "[error] PIKA_STD_DEVICE_UNIX_TIME_ENABLE need to be enable.");
#else
    _tm this_tm;
    int locale = g_pika_local_timezone;
    time_localtime(unix_time, &this_tm, locale);
    time_set_tm_value(self, &this_tm);
    PikaObj* struct_time_obj = newNormalObj(New__time_struct_time);
    obj_setStruct(struct_time_obj, "_tm", this_tm);
    return struct_time_obj;
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

int _time_mktime(PikaObj* self, PikaObj* tm) {
#if !PIKA_STD_DEVICE_UNIX_TIME_ENABLE
    obj_setErrorCode(self, 1);
    obj_setSysOut(
        self, "[error] PIKA_STD_DEVICE_UNIX_TIME_ENABLE need to be enable.");
    return 0;
#else
    _tm this_tm = {0};
    int locale = g_pika_local_timezone;
    this_tm.tm_year = pikaTuple_getInt(tm, 0);
    this_tm.tm_mon = pikaTuple_getInt(tm, 1) - 1;
    this_tm.tm_mday = pikaTuple_getInt(tm, 2);
    this_tm.tm_hour = pikaTuple_getInt(tm, 3);
    this_tm.tm_min = pikaTuple_getInt(tm, 4);
    this_tm.tm_sec = pikaTuple_getInt(tm, 5);
    return time_mktime(&this_tm, locale);
#endif
}

char* _time_asctime(PikaObj* self) {
#if !PIKA_STD_DEVICE_UNIX_TIME_ENABLE
    obj_setErrorCode(self, 1);
    obj_setSysOut(
        self, "[error] PIKA_STD_DEVICE_UNIX_TIME_ENABLE need to be enable.");
    return NULL;
#else
    return _time_ctime(self, _time_time(self));
#endif
}

char* _time_ctime(PikaObj* self, pika_float unix_time) {
#if !PIKA_STD_DEVICE_UNIX_TIME_ENABLE
    obj_setErrorCode(self, 1);
    obj_setSysOut(
        self, "[error] PIKA_STD_DEVICE_UNIX_TIME_ENABLE need to be enable.");
    return NULL;
#else
    _tm this_tm;
    int locale = g_pika_local_timezone;
    time_localtime(unix_time, &this_tm, locale);
    const char* week[] = {"Sun", "Mon", "Tues", "Wed", "Thur", "Fri", "Sat"};
    const char* month[] = {"Jan", "Feb", "Mar",  "Apr", "May", "Jun",
                           "Jul", "Aug", "Sept", "Oct", "Nov", "Dec"};

    char str[100];

    sprintf(str, "%s %s %d %02d:%02d:%02d %d", week[this_tm.tm_wday],
            month[this_tm.tm_mon], this_tm.tm_mday, this_tm.tm_hour,
            this_tm.tm_min, this_tm.tm_sec, this_tm.tm_year);
    // time_printf("%s\n", str);
    return obj_cacheStr(self, str);
#endif
}

void _time___init__(PikaObj* self) {
    if (-1 == pika_platform_get_tick()) {
        global_do_sleep_ms = pika_platform_sleep_ms;
    } else {
        global_do_sleep_ms = _do_sleep_ms_tick;
    }
#if !PIKA_STD_DEVICE_UNIX_TIME_ENABLE
#else
    _tm this_tm;
    g_pika_local_timezone = 8;
    time_localtime(0.0, &this_tm, 8);
    time_set_tm_value(self, &this_tm);
#endif
}

#define _OBJ2TM(obj) obj_getStruct(obj, "_tm")

Arg* _time_struct_time___iter__(PikaObj* self) {
    obj_setInt(self, "__iter_i", 0);
    return arg_newRef(self);
}

Arg* _time_struct_time___next__(PikaObj* self) {
    int __iter_i = args_getInt(self->list, "__iter_i");
    if (__iter_i > 8) {
        return arg_newNone();
    }
    obj_setInt(self, "__iter_i", __iter_i + 1);
    return arg_newInt(_time_struct_time___getitem__(self, __iter_i));
}

int _time_struct_time___getitem__(PikaObj* self, int __key) {
    _tm* this_tm = _OBJ2TM(self);
    int tm_wday = 0;
    switch (__key) {
        case 0:
            return this_tm->tm_year;
        case 1:
            return this_tm->tm_mon + 1;
        case 2:
            return this_tm->tm_mday;
        case 3:
            return this_tm->tm_hour;
        case 4:
            return this_tm->tm_min;
        case 5:
            return this_tm->tm_sec;
        case 6:
            tm_wday = this_tm->tm_wday - 1;
            if (tm_wday < 0) {
                tm_wday = 6;
            }
            return tm_wday;
        case 7:
            return this_tm->tm_yday;
        case 8:
            return this_tm->tm_isdst;
        default:
            return 0;
    }
}

char* _time_struct_time___str__(PikaObj* self) {
    _tm* _tm = _OBJ2TM(self);
    char str[200];
    time_struct_format(_tm, str);
    return obj_cacheStr(self, str);
}

int _time_struct_time___len__(PikaObj* self) {
    return 9;
}
