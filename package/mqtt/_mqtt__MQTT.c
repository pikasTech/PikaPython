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

    if(strlen(ca) > 0)
        mqtt_set_ca(_client, ca);
    if(strlen(ip) > 0)
        mqtt_set_host(_client, ip);
    if(strlen(clinetID) > 0)
        mqtt_set_client_id(_client, clinetID);
    if(strlen(username) > 0)
        mqtt_set_user_name(_client, username);
    if(strlen(password) > 0)
        mqtt_set_password(_client, password);
        
    mqtt_set_clean_session(_client, 1);
}

void _mqtt__MQTT___del__(PikaObj* self) {
    mqtt_client_t* _client = obj_getPtr(self, "_client");
    if (NULL != _client) {
        mqtt_release_free(_client);
    }
}

////////////////////////////////////////////////////////////////////
// 函 数 名：_mqtt__MQTT_set_client_id
// 功能说明：设置mqtt客户端的id
// 输入参数：id 字符串格式
// 返 回 值：0=成功；非0=错误码
///////////////////////////////////////////////////////////////////
int _mqtt__MQTT_set_client_id(PikaObj *self, char* id) {
    mqtt_client_t* _client = obj_getPtr(self, "_client");
    
    if(id == NULL) {
        __platform_printf("input id error\r\n");
        return -1;
    }
    if(strlen(id) <= 0) {
        __platform_printf("input id data error\r\n");
        return -2;
    }

    mqtt_set_client_id(_client, id);
    return 0;
}

////////////////////////////////////////////////////////////////////
// 函 数 名：_mqtt__MQTT_set_host
// 功能说明：设置mqtt客户端，连接主机的ip或者url
// 输入参数：字符串格式
// 返 回 值：0=成功；非0=错误码
///////////////////////////////////////////////////////////////////
int _mqtt__MQTT_set_host(PikaObj *self, char* host_url) {
    mqtt_client_t* _client = obj_getPtr(self, "_client");
    
    if(host_url == NULL) {
        __platform_printf("input host_url error\r\n");
        return -1;
    }
    if(strlen(host_url) <= 0) {
        __platform_printf("input host_url data error\r\n");
        return -2;
    }

    mqtt_set_host(_client, host_url);
    return 0;
}

////////////////////////////////////////////////////////////////////
// 函 数 名：_mqtt__MQTT_set_password
// 功能说明：设置mqtt客户端的 密码
// 输入参数：字符串格式
// 返 回 值：0=成功；非0=错误码
///////////////////////////////////////////////////////////////////
int _mqtt__MQTT_set_password(PikaObj *self, char* password){
    mqtt_client_t* _client = obj_getPtr(self, "_client");

    if(password == NULL) {
        __platform_printf("input password error\r\n");
        return -1;
    }
    if(strlen(password) <= 0) {
        __platform_printf("input password data error\r\n");
        return -2;
    }

    mqtt_set_password(_client, password);
    return 0;
}

////////////////////////////////////////////////////////////////////
// 函 数 名：_mqtt__MQTT_set_port
// 功能说明：设置mqtt客户端，连接主机的端口号
// 输入参数：字符串格式
// 返 回 值：0=成功；非0=错误码
///////////////////////////////////////////////////////////////////
int _mqtt__MQTT_set_port(PikaObj *self, int port){
    char port_str[10] = {0};
    mqtt_client_t* _client = obj_getPtr(self, "_client");

    __platform_sprintf(port_str, "%d", port);
    obj_setStr(self, "port", port_str);
    
    if(port_str == NULL) {
        __platform_printf("input port error\r\n");
        return -1;
    }
    if(strlen(port_str) <= 0) {
        __platform_printf("input port data error\r\n");
        return -2;
    }

    mqtt_set_port(_client, port_str);
    return 0;
}

////////////////////////////////////////////////////////////////////
// 函 数 名：_mqtt__MQTT_set_user_name
// 功能说明：设置mqtt客户端的用户名
// 输入参数：字符串格式
// 返 回 值：0=成功；非0=错误码
///////////////////////////////////////////////////////////////////
int _mqtt__MQTT_set_user_name(PikaObj *self, char* name){
    mqtt_client_t* _client = obj_getPtr(self, "_client");

    if(name == NULL) {
        __platform_printf("input name error\r\n");
        return -1;
    }
    if(strlen(name) <= 0) {
        __platform_printf("input name data error\r\n");
        return -2;
    }

    mqtt_set_user_name(_client, name);
    return 0;
}

////////////////////////////////////////////////////////////////////
// 函 数 名：_mqtt__MQTT_connect
// 功能说明：连接mqtt的服务端
// 输入参数：无
// 返 回 值：0=成功；非0=错误码
///////////////////////////////////////////////////////////////////
int _mqtt__MQTT_connect(PikaObj *self){
    int ret;

    mqtt_client_t* _client = obj_getPtr(self, "_client");

    ret = mqtt_connect(_client);
    if(ret != 0)
        __platform_printf("mqtt connect error :%d\r\n",ret);

    return ret;
}

////////////////////////////////////////////////////////////////////
// 函 数 名：_mqtt__MQTT_disconnect
// 功能说明：断开 mqtt的连接
// 输入参数：无
// 返 回 值：0=成功；非0=错误码
///////////////////////////////////////////////////////////////////
int _mqtt__MQTT_disconnect(PikaObj *self) {
    int ret;

    mqtt_client_t* _client = obj_getPtr(self, "_client");

    ret = mqtt_disconnect(_client);
    if(ret != 0)
        __platform_printf("mqtt disconnect error :%d\r\n",ret);

    return ret;
}