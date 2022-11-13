#include "requests.h"
#include "requests_Response.h"
#include "webclient.h"

#define GET_HEADER_BUFSZ 1024
#define GET_RESP_BUFSZ 1024

PikaObj* requests_request(PikaObj* self,
                          char* method,
                          char* url,
                          PikaDict* kwargs) {
    PikaObj* response = newNormalObj(New_requests_Response);
    obj_setStr(response, "url", url);

    struct webclient_session* session = RT_NULL;
    unsigned char* buffer = RT_NULL;
    int ret = 0;
    int bytes_read, resp_status;
    int content_length = -1;

    buffer = (unsigned char*)web_calloc(1, GET_RESP_BUFSZ);
    if (buffer == RT_NULL) {
        rt_kprintf("no memory for receive buffer.\n");
        ret = -RT_ENOMEM;
        goto __exit;
    }

    /* create webclient session and set header response size */
    session = webclient_session_create(GET_HEADER_BUFSZ);
    if (session == RT_NULL) {
        ret = -RT_ENOMEM;
        goto __exit;
    }

    if (strEqu("GET", method)) {
        resp_status = webclient_get(session, url);
        obj_setInt(response, "status_code", resp_status);
        /* send GET request by default header */
        if (resp_status != 200) {
            goto __exit;
        }

        content_length = webclient_content_length_get(session);
        obj_setInt(response, "content_length", content_length);

        if (content_length < 0) {
            // rt_kprintf("webclient GET request type is chunked.\n");
            do {
                bytes_read =
                    webclient_read(session, (void*)buffer, GET_RESP_BUFSZ);
                if (bytes_read <= 0) {
                    break;
                }
            } while (1);
        } else {
            int content_pos = 0;
            do {
                bytes_read =
                    webclient_read(session, (void*)buffer,
                                   content_length - content_pos > GET_RESP_BUFSZ
                                       ? GET_RESP_BUFSZ
                                       : content_length - content_pos);
                if (bytes_read <= 0) {
                    break;
                }
                content_pos += bytes_read;
            } while (content_pos < content_length);
        }
        obj_setStr(response, "text", (char*)buffer);
        goto __exit;
    }

__exit:
    if (session) {
        webclient_close(session);
    }

    if (buffer) {
        web_free(buffer);
    }

    if (ret != 0) {
        if (response) {
            obj_deinit(response);
        }
        response = NULL;
    }
    return response;
}

PikaObj* requests_get(PikaObj* self, char* url, PikaDict* kwargs) {
    return requests_request(self, "GET", url, kwargs);
}

void requests___del__(PikaObj* self) {}

void requests___init__(PikaObj* self) {}
