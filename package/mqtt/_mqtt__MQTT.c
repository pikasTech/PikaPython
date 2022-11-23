#include "_mqtt__MQTT.h"
#include "mqttclient.h"

void Subscribe_Handler(void *client, message_data_t* msg);

////////////////////////////////////////////////////////////////////
// 函 数 名：_mqtt__MQTT___init__
// 功能说明：对象初始化
// 输入参数：
// 返 回 值：无
///////////////////////////////////////////////////////////////////
void _mqtt__MQTT___init__(PikaObj* self,
                          char* ip,
                          int port,
                          char* clinetID,
                          char* username,
                          char* password,
                          char* version,
                          char* ca,
                          int keepalive) {
    obj_setInt(self, "_connected", 0);
    mqtt_client_t* _client = mqtt_lease();
    // obj_setPtr(self, "_client", _client);
    /* port to str, and cache to object */
    char port_str[10] = {0};
    int tmp = 0;

    if (strlen(ip) > 0) {
        obj_setStr(self, "host_str", ip);
        mqtt_set_host(_client, obj_getStr(self, "host_str"));
    }

    memset(port_str, 0, sizeof(port_str));
    __platform_sprintf(port_str, "%d", port);
    obj_setStr(self, "port", port_str);
    mqtt_set_port(_client, obj_getStr(self, "port"));

    if (strlen(clinetID) > 0) {
        obj_setStr(self, "id_str", clinetID);
        mqtt_set_client_id(_client, obj_getStr(self, "id_str"));
    }

    if (strlen(username) > 0) {
        obj_setStr(self, "username_str", username);
        mqtt_set_user_name(_client, obj_getStr(self, "username_str"));
    }

    if (strlen(password) > 0) {
        obj_setStr(self, "password_str", password);
        mqtt_set_password(_client, obj_getStr(self, "password_str"));
    }

    tmp = atoi(version);
    if (tmp > 0) {
        mqtt_set_version(_client, tmp);
    }

    if (strlen(ca) > 0) {
        obj_setStr(self, "ca_str", ca);
        mqtt_set_ca(_client, obj_getStr(self, "ca_str"));
    }

    if (keepalive > 0) {
        mqtt_set_keep_alive_interval(_client, keepalive);
    }

    mqtt_set_clean_session(_client, 1);

    obj_setPtr(self, "_client",
               _client);  //这里要再保存一次mqtt结构体的内容到python环境
    __platform_printf("Mqtt_Lib buildtime:%s-%s\r\n", __DATE__, __TIME__);
}

////////////////////////////////////////////////////////////////////
// 函 数 名：_mqtt__MQTT___del__
// 功能说明：释放对象资源
// 输入参数：
// 返 回 值：无
///////////////////////////////////////////////////////////////////
void _mqtt__MQTT___del__(PikaObj* self) {
    mqtt_client_t* _client = obj_getPtr(self, "_client");
    if (NULL == _client) {
        return;
    }
    /* disconnect autoly if not disconnected */
    int _connected = obj_getInt(self, "_connected");
    if (_connected) {
        _mqtt__MQTT_disconnect(self);
    }
    MQTT_LOG_E("%s:%d %s() >_<", __FILE__, __LINE__, __FUNCTION__);
    mqtt_release_free(_client);
}

////////////////////////////////////////////////////////////////////
// 函 数 名：_mqtt__MQTT_connect
// 功能说明：连接mqtt的服务端
// 输入参数：无
// 返 回 值：0=成功；非0=错误码
///////////////////////////////////////////////////////////////////
int _mqtt__MQTT_connect(PikaObj* self) {
    int ret;
    obj_setInt(self, "_connected", 1);
    mqtt_client_t* _client = obj_getPtr(self, "_client");

    ret = mqtt_connect(_client);
    if (ret != 0)
        __platform_printf("mqtt connect ERROR! :%d\r\n", ret);

    if (ret == 0)
        __platform_printf("mqtt connect OK\r\n");
    return ret;
}

////////////////////////////////////////////////////////////////////
// 函 数 名：_mqtt__MQTT_disconnect
// 功能说明：断开 mqtt的连接
// 输入参数：无
// 返 回 值：0=成功；非0=错误码
///////////////////////////////////////////////////////////////////
int _mqtt__MQTT_disconnect(PikaObj* self) {
    int ret;
    obj_setInt(self, "_connected", 0);
    mqtt_client_t* _client = obj_getPtr(self, "_client");

    ret = mqtt_disconnect(_client);
    if (ret != 0)
        __platform_printf("mqtt disconnect ERROR! :%d\r\n", ret);

    if (ret == 0)
        __platform_printf("mqtt disconnect Done\r\n");
    return ret;
}

////////////////////////////////////////////////////////////////////
// 函 数 名：_mqtt__MQTT_listSubscribrTopic
// 功能说明：罗列出当前订阅的主题
// 输入参数：无
// 返 回 值：对象指针
///////////////////////////////////////////////////////////////////
PikaObj* _mqtt__MQTT_listSubscribrTopic(PikaObj* self) {
    int ret;
    mqtt_client_t* _client = obj_getPtr(self, "_client");

    ret = mqtt_list_subscribe_topic(_client);
    if(ret == 0)
        __platform_printf("MQTT_listSubscribrTopic Done\r\n");
    else 
        __platform_printf("MQTT_listSubscribrTopic error\r\n");

    return NULL;
}

////////////////////////////////////////////////////////////////////
// 函 数 名：_mqtt__MQTT_publish
// 功能说明：发布主题消息
// 输入参数：主题名称，有效数据
// 返 回 值：0=成功；非0=错误码
///////////////////////////////////////////////////////////////////
int _mqtt__MQTT_publish(PikaObj* self, char* topic, char* payload) {
    int ret;
    mqtt_message_t msg;

    mqtt_client_t* _client = obj_getPtr(self, "_client");
    memset(&msg,0,sizeof(msg));

    if(strlen(topic) <= 0) {
        __platform_printf("input topic error\r\n");
        return -1;
    }

    if(strlen(payload) <= 0) {
        __platform_printf("input payload error\r\n");
        return -2;
    }

    msg.payload = (void *)payload;
    msg.qos = 1;
    ret = mqtt_publish(_client,topic,&msg);
    if(ret == 0)
        __platform_printf("MQTT_publish Done\r\n");
    else 
        __platform_printf("MQTT_publish error\r\n");
    return ret;
}

////////////////////////////////////////////////////////////////////
// 函 数 名：_mqtt__MQTT_setCa
// 功能说明：设置ca值
// 输入参数：ca值
// 返 回 值：0=成功；非0=错误码
///////////////////////////////////////////////////////////////////
int _mqtt__MQTT_setCa(PikaObj* self, char* ca) {
    mqtt_client_t* _client = obj_getPtr(self, "_client");

    if (ca == NULL) {
        __platform_printf("input ca error\r\n");
        return -1;
    }
    if (strlen(ca) <= 0) {
        __platform_printf("input ca data error\r\n");
        return -2;
    }

    obj_setStr(self, "ca_str", ca);
    mqtt_set_ca(_client, obj_getStr(self, "ca_str"));

    __platform_printf("MQTT_setCa len:%d\r\n", strlen(ca));
    return 0;
}

////////////////////////////////////////////////////////////////////
// 函 数 名：_mqtt__MQTT_setClientID
// 功能说明：设置mqtt客户端的id
// 输入参数：id 字符串格式
// 返 回 值：0=成功；非0=错误码
///////////////////////////////////////////////////////////////////
int _mqtt__MQTT_setClientID(PikaObj* self, char* id) {
    mqtt_client_t* _client = obj_getPtr(self, "_client");

    if (id == NULL) {
        __platform_printf("input id error\r\n");
        return -1;
    }
    if (strlen(id) <= 0) {
        __platform_printf("input id data error\r\n");
        return -2;
    }

    obj_setStr(self, "id_str", id);
    mqtt_set_client_id(_client, obj_getStr(self, "id_str"));

    __platform_printf("MQTT_setClientID :%s\r\n", id);
    return 0;
}

////////////////////////////////////////////////////////////////////
// 函 数 名：_mqtt__MQTT_setHost
// 功能说明：设置mqtt客户端，连接主机的ip或者url
// 输入参数：字符串格式
// 返 回 值：0=成功；非0=错误码
///////////////////////////////////////////////////////////////////
int _mqtt__MQTT_setHost(PikaObj* self, char* host_url) {
    mqtt_client_t* _client = obj_getPtr(self, "_client");

    if (host_url == NULL) {
        __platform_printf("input host_url error\r\n");
        return -1;
    }
    if (strlen(host_url) <= 0) {
        __platform_printf("input host_url data error\r\n");
        return -2;
    }

    obj_setStr(self, "host_str",
               host_url);  // python 环境创建一个全局变量存放 host
    mqtt_set_host(
        _client,
        obj_getStr(self,
                   "host_str"));  //从python环境中取出 host的指针 赋值给结构体
    __platform_printf("MQTT_setHost :%s\r\n", host_url);

    return 0;
}

////////////////////////////////////////////////////////////////////
// 函 数 名：_mqtt__MQTT_setKeepAlive
// 功能说明：设置mqtt客户端的 心跳包发送间隔
// 输入参数：字符串格式
// 返 回 值：0=成功；非0=错误码
///////////////////////////////////////////////////////////////////
int _mqtt__MQTT_setKeepAlive(PikaObj* self, char* time) {
    mqtt_client_t* _client = obj_getPtr(self, "_client");
    int tmp;

    tmp = atoi(time);
    if (tmp > 0) {
        mqtt_set_keep_alive_interval(_client, tmp);
    } else {
        __platform_printf("input keepalive data error \r\n");
        return -2;
    }

    __platform_printf("MQTT_setKeepAlive :%d\r\n", tmp);
    return 0;
}

////////////////////////////////////////////////////////////////////
// 函 数 名：_mqtt__MQTT_setPassword
// 功能说明：设置mqtt客户端的 密码
// 输入参数：字符串格式
// 返 回 值：0=成功；非0=错误码
///////////////////////////////////////////////////////////////////
int _mqtt__MQTT_setPassword(PikaObj* self, char* passwd) {
    mqtt_client_t* _client = obj_getPtr(self, "_client");

    if (passwd == NULL) {
        __platform_printf("input passwd error\r\n");
        return -1;
    }
    if (strlen(passwd) <= 0) {
        __platform_printf("input passwd data error\r\n");
        return -2;
    }

    obj_setStr(self, "password_str", passwd);
    mqtt_set_password(_client, obj_getStr(self, "password_str"));

    __platform_printf("MQTT_setPassword :%s\r\n", passwd);
    return 0;
}

////////////////////////////////////////////////////////////////////
// 函 数 名：_mqtt__MQTT_setPort
// 功能说明：设置mqtt客户端，连接主机的端口号
// 输入参数：字符串格式
// 返 回 值：0=成功；非0=错误码
///////////////////////////////////////////////////////////////////
int _mqtt__MQTT_setPort(PikaObj* self, int port) {
    char port_str[10] = {0};
    mqtt_client_t* _client = obj_getPtr(self, "_client");

    if (port <= 0) {
        __platform_printf("input port error\r\n");
        return -1;
    }

    __platform_sprintf(port_str, "%d", port);
    obj_setStr(self, "port", port_str);

    mqtt_set_port(_client, obj_getStr(self, "port"));
    __platform_printf("MQTT_setPort :%s\r\n", port_str);

    return 0;
}

////////////////////////////////////////////////////////////////////
// 函 数 名：_mqtt__MQTT_setUsername
// 功能说明：设置mqtt客户端的用户名
// 输入参数：字符串格式
// 返 回 值：0=成功；非0=错误码
///////////////////////////////////////////////////////////////////
int _mqtt__MQTT_setUsername(PikaObj* self, char* name) {
    mqtt_client_t* _client = obj_getPtr(self, "_client");

    if (name == NULL) {
        __platform_printf("input name error\r\n");
        return -1;
    }
    if (strlen(name) <= 0) {
        __platform_printf("input name data error\r\n");
        return -2;
    }

    obj_setStr(self, "username_str", name);
    mqtt_set_user_name(_client, obj_getStr(self, "username_str"));

    __platform_printf("MQTT_setUsername :%s\r\n", name);
    return 0;
}

////////////////////////////////////////////////////////////////////
// 函 数 名：_mqtt__MQTT_setVersion
// 功能说明：设置mqtt 协议版本
// 输入参数：字符串格式
// 返 回 值：0=成功；非0=错误码
///////////////////////////////////////////////////////////////////
int _mqtt__MQTT_setVersion(PikaObj* self, char* version) {
    mqtt_client_t* _client = obj_getPtr(self, "_client");
    int tmp;

    tmp = atoi(version);
    if (tmp > 0) {
        mqtt_set_version(_client, tmp);
    } else {
        __platform_printf("input version data error \r\n");
        return -2;
    }

    __platform_printf("MQTT_setVersion :%d\r\n", tmp);
    return 0;
}

////////////////////////////////////////////////////////////////////
// 函 数 名：_mqtt__MQTT_setWill
// 功能说明：
// 输入参数：
// 返 回 值：0=成功；非0=错误码
///////////////////////////////////////////////////////////////////
int _mqtt__MQTT_setWill(PikaObj* self,
                        int qos,
                        char* topic,
                        int retain,
                        char* payload) {

    mqtt_client_t* _client = obj_getPtr(self, "_client");
    int ret;

    if (strlen(topic) <= 0) {
        __platform_printf("input topic error\r\n");
        return -1;
    }

    if ((qos < 0) || (qos > 2)) {
        __platform_printf("input qos error\r\n");
        return -1;
    }

    if (strlen(payload) <= 0) {
        __platform_printf("input payload error\r\n");
        return -1;
    }

    __platform_printf("input retain :%d\r\n",(uint8_t )retain);

    ret = mqtt_set_will_options(_client,topic,qos,(uint8_t )retain,payload);

    if (ret == 0) {
        __platform_printf("MQTT_setWill success\r\n", topic);
    } else
        __platform_printf("MQTT_setWill error\r\n");

    return 0;
}

////////////////////////////////////////////////////////////////////
// 函 数 名：_mqtt__MQTT_subscribe
// 功能说明：设置mqtt 订阅主题
// 输入参数：
// 返 回 值：0=成功；非0=错误码
///////////////////////////////////////////////////////////////////
int _mqtt__MQTT_subscribe(PikaObj* self, char* topic, int qos, Arg* cb) {
    mqtt_client_t* _client = obj_getPtr(self, "_client");
    int ret;

    if (strlen(topic) <= 0) {
        __platform_printf("input topic error\r\n");
        return -1;
    }

    if ((qos < 0) || (qos > 2)) {
        __platform_printf("input qos error\r\n");
        return -1;
    }

    ret = mqtt_subscribe(_client,topic,qos,Subscribe_Handler);

    if (ret == 0) {
        __platform_printf("MQTT_subscribe Topic :%s success\r\n", topic);
    } else
        __platform_printf("MQTT_subscribe Topic error\r\n");

    return 0;
}

////////////////////////////////////////////////////////////////////
// 函 数 名：_mqtt__MQTT_unsubscribe
// 功能说明：取消mqtt 订阅主题
// 输入参数：
// 返 回 值：0=成功；非0=错误码
///////////////////////////////////////////////////////////////////
int _mqtt__MQTT_unsubscribe(PikaObj* self, char* topic) {
    mqtt_client_t* _client = obj_getPtr(self, "_client");
    int ret;

    if (strlen(topic) <= 0) {
        __platform_printf("input topic error\r\n");
        return -1;
    }

    ret = mqtt_unsubscribe(_client,topic);
    if (ret == 0) {
        __platform_printf("MQTT_unsubscribe :%s success\r\n", topic);
    } else
        __platform_printf("MQTT_unsubscribe :%s error\r\n", topic);

    return 0;
}


////////////////////////////////////////////////////////////////////
// 函 数 名：_mqtt__MQTT_unsubscribe
// 功能说明：取消mqtt 订阅主题
// 输入参数：
// 返 回 值：0=成功；非0=错误码
///////////////////////////////////////////////////////////////////
void Subscribe_Handler(void *client, message_data_t* msg) {
    MQTT_LOG_I("\n>>>------------------");
    MQTT_LOG_I("Topic:%s \nlen:%d,message: %s",msg->topic_name,(int)msg->message->payloadlen,(char *)msg->message->payload);
    MQTT_LOG_I("------------------<<<");

}