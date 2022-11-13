#include "_mqtt__MQTT.h"
#include "mqttclient.h"

void _mqtt__MQTT___init__(PikaObj* self,
                          char* ip,
                          int port,
                          char* clinetID,
                          char* username,
                          char* password,
                          char* version,
                          char* ca,
                          int keepalive) {
    mqtt_client_t* _client = mqtt_lease();
    obj_setPtr(self, "_client", _client);
    /* port to str, and cache to object */
    char port_str[10] = {0};
    __platform_sprintf(port_str, "%d", port);
    obj_setStr(self, "port", port_str);
    mqtt_set_port(_client, port_str);

    mqtt_set_ca(_client, ca);
    mqtt_set_host(_client, ip);
    mqtt_set_client_id(_client, clinetID);
    mqtt_set_user_name(_client, username);
    mqtt_set_password(_client, password);
    mqtt_set_clean_session(_client, 1);
}

void _mqtt__MQTT___del__(PikaObj* self) {
    mqtt_client_t* _client = obj_getPtr(self, "_client");
    if (NULL != _client) {
        mqtt_release_free(_client);
    }
}
