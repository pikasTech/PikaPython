#include "_requests.h"
#include <ctype.h>
#include "webclient.h"
#include "_requests_Response.h"

#define GET_HEADER_BUFSZ 1024
#define GET_RESP_BUFSZ 1024

void _requests_del(PikaObj *self, char *method);

/* 标准输出函数 */
#define RQ_print(fmt, ...) __platform_printf(fmt, ##__VA_ARGS__)
#define RQ_cli(fmt, ...) __platform_printf(fmt, ##__VA_ARGS__)
#define RQ_error_pointer(...) \
    RQ_print("[%s]: Checking NULL pointer of {" #__VA_ARGS__ "}.\n", __fun__)
#define RQ_error_value(str, ...) \
    RQ_print("[%s]: Checking error value of {" #__VA_ARGS__ ":" str "}.\n", __fun__, ##__VA_ARGS__)
#define RQ_err(fmt, ...) RQ_print("[%s]:" fmt "\n", __VA_ARGS__)

/* 补充常见宏定义 */
#ifndef unlikely
#define unlikely(x) __builtin_expect(!!(x), 0)
#endif
#ifndef likely
#define likely(x) __builtin_expect(!!(x), 1)
#endif

int requests_request(PikaObj *self,
                          char *method,
                          PikaDict *kwargs)
{
    PikaObj *response;         /* 返回的Pikascript对象 */
    const char *this_url;      /* 真实组装之后的url */
    const char *this_header;   /* 填充之后响应头信息 */
    const void *this_data;     /* POST传输的数据 */
    size_t data_len, resp_len; /* 长度信息 */
    void *resp_data;           /* 返回的负载内容 */
    int32_t ret;               /* 返回值 */
    double timeout;            /* 超时时间 */
    struct webclient_session *session;

    session = obj_getInt(self, "session_address");
    if (session == -999999999 || session == 0)
    {
        RQ_cli("Sorry, can not operate NULL session object.\n");
        return -1;
    }

    timeout = 0;      /* 默认超时时间不限 */
    data_len = 0;     /* 默认无数据传输 */
    this_data = NULL; /* 默认无数据 */

    if (kwargs != NULL)
    {
        /**
         *  现在支持可选的额外参数
         *      params: 用于填充url参数，即get方法通过url传递数据
         *      headers: 用于响应头信息填充
         *      data: 负载内容
         *      json: 负载内容
         *      files: 负载内容
         *      timeout: 超时设置
         * 实际上支持有限
         */
        if (args_isArgExist(kwargs, "timeout"))
        {
            /* 获取超时时间，默认s，最小分辨度ms */
            timeout = pikaDict_getFloat(kwargs, "timeout");
        }
        if (args_isArgExist(kwargs, "files"))
        {
            /* 获取文件 */
            RQ_cli("Sorry, now don't support transport files.\n");
            return -1;
        }
        if (args_isArgExist(kwargs, "json"))
        {
            /* 获取JSON字符 */
            RQ_cli("Sorry, now don't support JSON.\n");
            return -1;
        }
        if (args_isArgExist(kwargs, "data"))
        {
            /* 获取原始字符数据 */
            this_data = pikaDict_getFloat(kwargs, "data");
        }
    }
    /* 记录request的url */
    this_url = obj_getStr(response, "url");

    /* 进行post或者get操作 */
    if (strEqu(method, "GET"))
    {
        /* Get之后，header->buffer缓冲区内容会被清空 */
        /* FIXME: 保存一下header->buffer内容 */
        if (webclient_get(session, this_url) != 200) {
            return -1;
        }
        data_len = webclient_response(session, &resp_data, &resp_len);
        if (data_len <= 0) {
            return -1;
        }
        /* 正常得到了数据 */
        obj_setInt(response, "state_code", session->resp_status);
        obj_setInt(response, "content_length", resp_len);
        obj_setStr(response, "text", (char *)resp_data);
    }
    else
    {
        ret = webclient_post(session, this_url, this_data, data_len);
    }
    else 
    {
        return -1;
    }
    obj_setInt(response, "state_code", ret);
    obj_setInt(response, "content_length", resp_len);
    obj_setStr(response, "text", (char *)resp_data);

    return response;
__exit:
    if (session)
    {
        webclient_close(session);
        session = NULL;
    }
    return NULL;
}

int _requests_header_write(PikaObj *self, char *header, char *value)
{
    struct webclient_session *session;

    session = obj_getInt(self, "session_address");
    if (session == -999999999 || session == 0)
    {
        RQ_cli("Sorry, can not operate NULL session object.\n");
        return -1;
    }

    /* 写入请求初始内容 */
    if (webclient_header_fields_add(session, "%s:%s\r\n", header, value) < 0)
    {
        RQ_cli("Sorry, request header too long.\n");
        return -1;
    }
    return 1;
}

int _requests_proto_write(PikaObj *self, char *proto)
{
    struct webclient_session *session;

    session = obj_getInt(self, "session_address");
    if (session == -999999999 || session == 0)
    {
        RQ_cli("Sorry, can not operate NULL session object.\n");
        return -1;
    }

    /* 写入请求初始内容 */
    if (proto != NULL)
    {
        if (webclient_header_fields_add(session, " %s\r\n", proto) < 0)
        {
            RQ_cli("Sorry, request header too long.\n");
            return -1;
        }
    }
    else
    {
        if (webclient_header_fields_add(session, " HTTP/1.1\r\n") < 0)
        {
            RQ_cli("Sorry, request header too long.\n");
            return -1;
        }
    }
    return 1;
}

char to_hex(char code)
{
    static char hex[] = "0123456789abcdef";
    return hex[code & 15];
}

int _requests_urlencode_write(PikaObj *self, char *s1, char *s2, char *start, char *connect)
{
    struct webclient_session *session;
    char *url_address, *p, *s;
    int32_t length, header_length;

    session = obj_getInt(self, "session_address");
    if (session == -999999999 || session == 0)
    {
        RQ_cli("Sorry, can not operate NULL session object.\n");
        return -1;
    }
    header_length = session->header->length;
    url_address = session->header->buffer + header_length;
    p = url_address;

    if (start != NULL)
    {
        /* 写入前置符号 */
        while (*start)
        {
            *p++ = *start++;
        }
    }

    s = s1;
    while (*s)
    {
        if (isalnum(*s) || *s == '-' || *s == '_' || *s == '.' || *s == '~')
        {
            *p++ = *s;
        }
        else if (*s == ' ')
        {
            *p++ = '+';
        }
        else
        {
            *p++ = '%';
            *p++ = to_hex(*s >> 4);
            *p++ = to_hex(*s & 0xf);
        }
        s++;
    }
    if (connect != NULL)
    {
        /* 写入连接符号 */
        while (*connect)
        {
            *p++ = *connect++;
        }
    }
    s = s2;
    if (s != NULL)
    {
        while (*s)
        {
            if (isalnum(*s) || *s == '-' || *s == '_' || *s == '.' || *s == '~')
            {
                *p++ = *s;
            }
            else if (*s == ' ')
            {
                *p++ = '+';
            }
            else
            {
                *p++ = '%';
                *p++ = to_hex(*s >> 4);
                *p++ = to_hex(*s & 0xf);
            }
            s++;
        }
    }
    *p = '\0';

    length = p - url_address;
    length += header_length;
    if (length > session->header->size)
    {
        RQ_cli("Error, header buffer has overflowed: {%d}.\n", length);
        return -1;
    }
    /* 更新长度信息 */
    session->header->length = length;
    return 1;
}

void _requests_request_init(PikaObj *self, char *method)
{
    /* 创建会话对象，header长度固定 */
    struct webclient_session *session;
    char *temp;

    if (unlikely((strEqu(method, "GET") || strEqu(method, "POST")) == 0))
    {
        /* 目前仅支持两种方法 */
        RQ_cli("Sorry, now only support method: GET and POST.\n");
        return -1;
    }

    session = webclient_session_create(WEBCLIENT_HEADER_BUFSZ);
    if (session == RT_NULL)
    {
        RQ_cli("Sorry, memeory is not enough.\n");
        obj_setInt(self, "session_address", 0);
        return -1;
    }
    else
    {
        /* 写入请求初始内容 */
        if (webclient_header_fields_add(session, "%s ", method) < 0)
        {
            _requests_del(self);
            RQ_cli("Sorry, request header too long.\n");
            return -1;
        }
        temp = session->header->buffer + session->header->length;
        obj_setStr(self, "url", temp);
        obj_setStr(self, "headers", session->header->buffer);
        obj_setInt(self, "session_address", (int64_t)session);
    }
    return 1;
}

void _requests_request_del(PikaObj *self)
{
    struct webclient_session *session;
    session = obj_getInt(self, "session_address");
    if (session == -999999999)
    {
        session = 0;
    }
    if (session)
    {
        webclient_close(session);
    }
    /* 初始化 */
    obj_setStr(self, "url", NULL);
    obj_setStr(self, "text", NULL);
    obj_setStr(self, "headers", NULL);
    obj_setInt(self, "session_address", 0);
}

void _requests___del__(PikaObj *self)
{
    _requests_del(self);
}

void _requests___init__(PikaObj *self)
{
    /* 初始化 */
    obj_setStr(self, "url", NULL);
    obj_setStr(self, "text", NULL);
    obj_setStr(self, "headers", NULL);
    obj_setInt(self, "session_address", 0);
}
