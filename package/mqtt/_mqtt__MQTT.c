#include "_mqtt__MQTT.h"

void _mqtt__MQTT___init__(PikaObj* self,
                          char* ip,
                          int port,
                          char* clinetID,
                          char* username,
                          char* password,
                          char* version,
                          char* ca,
                          int keepalive) {
    __platform_printf(
        "ip: %s, port: %d, clinetID: %s, username: %s, password: %s, version: "
        "%s, ca: %s, keepalive: %d\r\n",
        ip, port, clinetID, username, password, version, ca, keepalive);
}
