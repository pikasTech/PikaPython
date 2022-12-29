/*
 * @Author: jiejie, lyon
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-11 21:53:07
 * @LastEditTime: 2020-06-08 20:38:41
 * @Description: the code belongs to jiejie, please keep the author information
 * and source code according to the license.
 */
#include "test_common.h"
extern "C" {

#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../pikascript-lib/mqtt/mqttclient.h"

// #define TEST_USEING_TLS

#ifdef TEST_USEING_TLS
static const char* test_baidu_ca_crt = {
    "-----BEGIN CERTIFICATE-----\r\n"
    "MIIDXzCCAkegAwIBAgILBAAAAAABIVhTCKIwDQYJKoZIhvcNAQELBQAwTDEgMB4G\r\n"
    "A1UECxMXR2xvYmFsU2lnbiBSb290IENBIC0gUjMxEzARBgNVBAoTCkdsb2JhbFNp\r\n"
    "Z24xEzARBgNVBAMTCkdsb2JhbFNpZ24wHhcNMDkwMzE4MTAwMDAwWhcNMjkwMzE4\r\n"
    "MTAwMDAwWjBMMSAwHgYDVQQLExdHbG9iYWxTaWduIFJvb3QgQ0EgLSBSMzETMBEG\r\n"
    "A1UEChMKR2xvYmFsU2lnbjETMBEGA1UEAxMKR2xvYmFsU2lnbjCCASIwDQYJKoZI\r\n"
    "hvcNAQEBBQADggEPADCCAQoCggEBAMwldpB5BngiFvXAg7aEyiie/QV2EcWtiHL8\r\n"
    "RgJDx7KKnQRfJMsuS+FggkbhUqsMgUdwbN1k0ev1LKMPgj0MK66X17YUhhB5uzsT\r\n"
    "gHeMCOFJ0mpiLx9e+pZo34knlTifBtc+ycsmWQ1z3rDI6SYOgxXG71uL0gRgykmm\r\n"
    "KPZpO/bLyCiR5Z2KYVc3rHQU3HTgOu5yLy6c+9C7v/U9AOEGM+iCK65TpjoWc4zd\r\n"
    "QQ4gOsC0p6Hpsk+QLjJg6VfLuQSSaGjlOCZgdbKfd/+RFO+uIEn8rUAVSNECMWEZ\r\n"
    "XriX7613t2Saer9fwRPvm2L7DWzgVGkWqQPabumDk3F2xmmFghcCAwEAAaNCMEAw\r\n"
    "DgYDVR0PAQH/BAQDAgEGMA8GA1UdEwEB/wQFMAMBAf8wHQYDVR0OBBYEFI/wS3+o\r\n"
    "LkUkrk1Q+mOai97i3Ru8MA0GCSqGSIb3DQEBCwUAA4IBAQBLQNvAUKr+yAzv95ZU\r\n"
    "RUm7lgAJQayzE4aGKAczymvmdLm6AC2upArT9fHxD4q/c2dKg8dEe3jgr25sbwMp\r\n"
    "jjM5RcOO5LlXbKr8EpbsU8Yt5CRsuZRj+9xTaGdWPoO4zzUhw8lo/s7awlOqzJCK\r\n"
    "6fBdRoyV3XpYKBovHd7NADdBj+1EbddTKJd+82cEHhXXipa0095MJ6RMG3NzdvQX\r\n"
    "mcIfeg7jLQitChws/zyrVQ4PkX4268NXSb7hLi18YIvDQVETI53O9zJrlAGomecs\r\n"
    "Mx86OyXShkDOOyyGeMlhLxS67ttVb9+E7gUJTb0o2HLO02JQZR7rkpeDMdmztcpH\r\n"
    "WD9f\r\n"
    "-----END CERTIFICATE-----"};
#endif

static void topic1_handler(void* client, message_data_t* msg) {
    (void)client;
    MQTT_LOG_I(
        "----------------------------------------------------------------------"
        "-------------");
    MQTT_LOG_I("%s:%d %s()...\ntopic: %s\nmessage:%s", __FILE__, __LINE__,
               __FUNCTION__, msg->topic_name, (char*)msg->message->payload);
    MQTT_LOG_I(
        "----------------------------------------------------------------------"
        "-------------");
}

void* mqtt_publish_thread(void* arg) {
    mqtt_client_t* client = (mqtt_client_t*)arg;

    char buf[100] = {0};
    mqtt_message_t msg;
    memset(&msg, 0, sizeof(msg));
    sprintf(buf, "welcome to mqttclient, this is a publish test...");

    sleep(2);

    mqtt_list_subscribe_topic(client);

    msg.payload = (void*)buf;
    msg.qos = QOS0;
    while (1) {
        sprintf(buf,
                "welcome to mqttclient, this is a publish test, a rand number: "
                "%d ...",
                random_number());
        mqtt_publish(client, "topic1", &msg);
        sleep(4);
    }
}
}

#if !PIKA_NANO_ENABLE
TEST(mqtt, lease_free) {
    mqtt_client_t* client = mqtt_lease();
    mqtt_release_free(client);
}

TEST(mqtt, init) {
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    pikaVM_runSingleFile(pikaMain, "test/python/mqtt/mqtt_init.py");
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if 0
//! Mqtt connect will break the gichub actions, 
//! if need to test, please enable it manually.
TEST(mqtt, connect) {
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    pikaVM_runSingleFile(pikaMain, "test/python/mqtt/mqtt_connect.py");
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if 0
//! Mqtt set_para will break the gichub actions, 
//! if need to test, please enable it manually.
TEST(mqtt, set_para) {
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    pikaVM_runSingleFile(pikaMain, "test/python/mqtt/mqtt_set_para.py");
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if 0
//! Mqtt publish will break the gichub actions, 
//! if need to test, please enable it manually.
TEST(mqtt, publish) {
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    pikaVM_runSingleFile(pikaMain, "test/python/mqtt/mqtt_publish.py");
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if 0
//! Mqtt subscribe will break the gichub actions, 
//! if need to test, please enable it manually.
TEST(mqtt, subscribe) {
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    pikaVM_runSingleFile(pikaMain, "test/python/mqtt/mqtt_subscribe.py");
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#endif
