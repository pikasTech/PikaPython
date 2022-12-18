import mqtt
import PikaStdDevice

client = mqtt.MQTT('broker.emqx.io',port=1883,clinetID='clientid',username='name_',password='passwd_')

ret = client.connect()
print("connect ret:%d" % ret)

def callback0(signal):
    recv_msg = client.getMsg(signal)
    recv_topic = client.getTopic(signal)
    print("py cb: %s-->>%s" % (recv_topic, recv_msg))

def callback1(signal):
    recv_msg = client.getMsg(signal)
    recv_topic = client.getTopic(signal)
    print("py cb: %s-->>%s" % (recv_topic, recv_msg))

def callback2(signal):
    recv_msg = client.getMsg(signal)
    recv_topic = client.getTopic(signal)
    recv_qos = client.getQos(signal)
    print("py cb: %s-qos:%d-->>%s" % (recv_topic,recv_qos, recv_msg))

# ret = client.subscribe('topic_pikapy',0,callback0)
# print("subscribe ret:%d" % ret)
# ret = client.subscribe('topic_pikapy', 1,callback1)
# print("subscribe ret:%d" % ret)
ret = client.subscribe('topic_pikapy', 2,callback2)
print("subscribe ret:%d" % ret)


#sleep wait for recv data
T = PikaStdDevice.Time()
T.sleep_s(5)
    
out = client.listSubscribrTopic()
print('out',out)

# client.unsubscribe('topic_pikapy_qos0');
# client.unsubscribe('topic_pikapy_qos1');
# client.unsubscribe('topic_pikapy_qos2');
# T.sleep_s(5)
# client.listSubscribrTopic()


# ret = client.setWill(1,'topic_will',1,'lost mqtt connect')
T.sleep_s(10)
# exit()
ret = client.disconnect()
print("ret:%d" % ret)