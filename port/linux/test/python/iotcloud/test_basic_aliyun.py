
from PikaStdDevice import Time
import iotcloud

clientId = "pikascript"
productKey = "xxx"
deviceName = "test1"
deviceSecret = "xxxxx"

topic = "/" + productKey + "/" + deviceName + "/user/update"

print("iotcloud aliyun test")
c = iotcloud.new()
c.aliyun(clientId, productKey, deviceName, deviceSecret)


def up_cb(signal):
    recv_msg = c.client.getMsg(signal)
    recv_topic = c.client.getTopic(signal)
    recv_qos = c.client.getQos(signal)
    print("cb: %s-qos:%d-->>%s" % (recv_topic, recv_qos, recv_msg))

e = c.connect()
print("connect:", e)
if e == 0:
    print("subcribe status:", c.subsribe(topic, up_cb))

    for i in range(10):
        print("publish status:", c.publish(topic, '{"id":'+str(i)+'}'))
        Time.sleep_s(3)

a = c.disconnect()
print("disconnect status:", a)

