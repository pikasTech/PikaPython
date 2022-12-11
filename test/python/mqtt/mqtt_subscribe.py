import mqtt
import PikaStdDevice

client = mqtt.MQTT('broker.emqx.io',port=1883,clinetID='clientid',username='name_',password='passwd_')

ret = client.connect()
print("ret:%d" % ret)

def callback0(signal):
    print("py cb: %s:%s" % (client.recv_topic, client.recv_msg))


ret = client.subscribe('topic_pikapy_qos0', 0, callback0)
print("ret:%d" % ret)
ret = client.subscribe('topic_pikapy_qos1', 1,0)
print("ret:%d" % ret)
ret = client.subscribe('topic_pikapy_qos2', 2,0)
print("ret:%d" % ret)


#sleep wait for recv data
T = PikaStdDevice.Time()
T.sleep_s(5)
    
out = client.listSubscribrTopic()
print('out',out)

client.unsubscribe('topic_pikapy_qos0');
client.unsubscribe('topic_pikapy_qos1');
client.unsubscribe('topic_pikapy_qos2');

T.sleep_s(5)
client.listSubscribrTopic()

T.sleep_s(10)

ret = client.disconnect()
print("ret:%d" % ret)