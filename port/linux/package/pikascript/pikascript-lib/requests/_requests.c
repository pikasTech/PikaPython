#include "_requests.h"
#include <ctype.h>
#include "_requests_Response.h"
#include "webclient.h"

/* 标准输出函数 */
#define RQ_print(fmt, ...) __platform_printf(fmt, ##__VA_ARGS__)
#define RQ_cli(fmt, ...) __platform_printf(fmt, ##__VA_ARGS__)
#define RQ_error_pointer(...) \
    RQ_print("[%s]: Checking NULL pointer of {" #__VA_ARGS__ "}.\n", __fun__)
#define RQ_error_value(str, ...)                                            \
    RQ_print("[%s]: Checking error value of {" #__VA_ARGS__ ":" str "}.\n", \
             __fun__, ##__VA_ARGS__)
#define RQ_err(fmt, ...) RQ_print("[%s]:" fmt "\n", __VA_ARGS__)

/* 补充常见宏定义 */
#ifndef unlikely
#define unlikely(x) __builtin_expect(!!(x), 0)
#endif
#ifndef likely
#define likely(x) __builtin_expect(!!(x), 1)
#endif

int _requests_Response_request(PikaObj* self,
                               char* method,
                               char* url,
                               pika_float timeout,
                               char* data) {
    const char* this_url;      /* 真实组装之后的url */
    const char* this_header;   /* 填充之后响应头信息 */
    const void* this_data;     /* POST传输的数据 */
    size_t data_len, resp_len; /* 长度信息 */
    void* resp_data;           /* 返回的负载内容 */
    int32_t ret;               /* 返回值 */
    struct webclient_session* session;

    session = (struct webclient_session*)obj_getInt(self, "session_address");
    if (session == (void*)-999999999 || session == NULL) {
        RQ_cli("Sorry, can not operate NULL session object.\n");
        return -1;
    }

    timeout = 0;      /* 默认超时时间不限 */
    this_data = NULL; /* 默认无数据 */

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

    /* 获取超时时间，默认s，最小分辨度ms */
    timeout = timeout;
    /* 获取原始字符数据 */
    this_data = data;

    /* 记录request的url */
    this_url = url;
    this_header = obj_getStr(self, "headers");

    /* FIXME */
    RQ_cli("%s", this_header);

    /* 进行post或者get操作 */
    if (strEqu(method, "GET")) {
        /* Get之后，header->buffer缓冲区内容会被清空 */
        /* FIXME: 保存一下header->buffer内容 */
        if (webclient_get(session, this_url) != 200) {
            return -1;
        }
        ret = webclient_response(session, &resp_data, &resp_len);
        if (ret <= 0) {
            return -1;
        }
        /* 正常得到了数据 */
        obj_setInt(self, "state_code", session->resp_status);
        obj_setInt(self, "content_length", resp_len);
        obj_setStr(self, "text", (char*)resp_data);
        obj_setStr(self, "headers", session->header->buffer);
        /* FIXME:暂时不保存源url */
        obj_setStr(self, "url", NULL);
    } else if (strEqu(method, "GET")) {
        if (this_data == NULL) {
            data_len = 0;
        } else {
            data_len = strlen(this_data);
        }
        /* FIXME: 默认二进制数据 */
        if (strstr(session->header->buffer, "Content-Length") == RT_NULL) {
            ret = webclient_header_fields_add(session, "Content-Length: %d\r\n",
                                              data_len);
            if (ret < 0) {
                return -1;
            }
        }
        if (strstr(session->header->buffer, "Content-Type") == RT_NULL) {
            /* 二进制数据流 */
            ret = webclient_header_fields_add(
                session, "Content-Type: application/octet-stream\r\n");
            if (ret < 0) {
                return -1;
            }
        }
        if (webclient_post(session, this_url, this_data, data_len) != 200) {
            return -1;
        }
        ret = webclient_response(session, &resp_data, &resp_len);
        if (ret <= 0) {
            return -1;
        }
        /* 正常得到了数据 */
        obj_setInt(self, "state_code", session->resp_status);
        obj_setInt(self, "content_length", resp_len);
        obj_setStr(self, "text", (char*)resp_data);
        obj_setStr(self, "headers", session->header->buffer);
        /* FIXME:暂时不保存源url */
        obj_setStr(self, "url", NULL);
    } else {
        return -1;
    }

    return 1;
}

int _requests_Response_header_write(PikaObj* self, char* header, char* value) {
    struct webclient_session* session;

    session = (struct webclient_session*)obj_getInt(self, "session_address");
    if (session == (void*)-999999999 || session == NULL) {
        RQ_cli("Sorry, can not operate NULL session object.\n");
        return -1;
    }

    /* 写入请求初始内容 */
    if (webclient_header_fields_add(session, "%s:%s\r\n", header, value) < 0) {
        RQ_cli("Sorry, request header too long.\n");
        return -1;
    }
    return 1;
}

int _requests_Response_proto_write(PikaObj* self, char* proto) {
    struct webclient_session* session;

    session = (struct webclient_session*)obj_getInt(self, "session_address");
    if (session == (void*)-999999999 || session == NULL) {
        RQ_cli("Sorry, can not operate NULL session object.\n");
        return -1;
    }

    /* 写入请求初始内容 */
    if (proto != NULL && *proto != '\0') {
        if (webclient_header_fields_add(session, " %s\r\n", proto) < 0) {
            RQ_cli("Sorry, request header too long.\n");
            return -1;
        }
    } else {
        if (webclient_header_fields_add(session, " HTTP/1.1\r\n") < 0) {
            RQ_cli("Sorry, request header too long.\n");
            return -1;
        }
    }
    return 1;
}

char to_hex(char code) {
    static char hex[] = "0123456789abcdef";
    return hex[code & 15];
}

int _requests_Response_urlencode_write(PikaObj* self,
                                       char* s1,
                                       char* s2,
                                       char* start,
                                       char* connect) {
    struct webclient_session* session;
    char *url_address, *p, *s;
    int32_t length, header_length;

    session = (struct webclient_session*)obj_getInt(self, "session_address");
    if (session == (void*)-999999999 || session == NULL) {
        RQ_cli("Sorry, can not operate NULL session object.\n");
        return -1;
    }
    header_length = session->header->length;
    url_address = session->header->buffer + header_length;
    p = url_address;

    if (start != NULL) {
        /* 写入前置符号 */
        s = (char*)start;
        while (*s) {
            *p++ = *s++;
        }
    }

    s = s1;
    while (*s) {
        if (isalnum(*s) || *s == '-' || *s == '_' || *s == '.' || *s == '~') {
            *p++ = *s;
        } else if (*s == ' ') {
            *p++ = '+';
        } else {
            *p++ = '%';
            *p++ = to_hex(*s >> 4);
            *p++ = to_hex(*s & 0xf);
        }
        s++;
    }
    if (connect != NULL) {
        /* 写入连接符号 */
        s = (char*)connect;
        while (*s) {
            *p++ = *s++;
        }
    }
    s = (char*)s2;
    if (s != NULL) {
        while (*s) {
            if (isalnum(*s) || *s == '-' || *s == '_' || *s == '.' ||
                *s == '~') {
                *p++ = *s;
            } else if (*s == ' ') {
                *p++ = '+';
            } else {
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
    if (length > session->header->size) {
        RQ_cli("Error, header buffer has overflowed: {%d}.\n", length);
        return -1;
    }
    /* 更新长度信息 */
    session->header->length = length;
    return 1;
}

int _requests_Response_request_init(PikaObj* self, char* method) {
    /* 创建会话对象，header长度固定 */
    struct webclient_session* session;
    char* temp;

    if (unlikely((strEqu(method, "GET") || strEqu(method, "POST")) == 0)) {
        /* 目前仅支持两种方法 */
        RQ_cli("Sorry, now only support method: GET and POST.\n");
        return -1;
    }

    session = webclient_session_create(WEBCLIENT_HEADER_BUFSZ);
    if (session == RT_NULL) {
        RQ_cli("Sorry, memeory is not enough.\n");
        obj_setInt(self, "session_address", 0);
        return -1;
    } else {
        /* 写入请求初始内容 */
        if (webclient_header_fields_add(session, "%s ", method) < 0) {
            _requests_Response_request_del(self);
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

PikaObj* _requests_Response_request_del(PikaObj* self) {
    struct webclient_session* session;
    session = (struct webclient_session*)obj_getInt(self, "session_address");
    if (session == (void*)-999999999) {
        session = NULL;
    }
    if (session) {
        webclient_close(session);
    }
    /* 初始化 */
    obj_setNone(self, "url");
    obj_setNone(self, "text");
    obj_setNone(self, "headers");
    obj_setInt(self, "session_address", 0);
    return NULL;
}

void _requests_Response___del__(PikaObj* self) {
    _requests_Response_request_del(self);
}

void _requests_Response___init__(PikaObj* self) {
    /* 初始化 */
    obj_setNone(self, "url");
    obj_setNone(self, "text");
    obj_setNone(self, "headers");
    obj_setInt(self, "session_address", 0);
}
