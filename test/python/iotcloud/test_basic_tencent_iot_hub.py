from PikaStdDevice import Time
import iotcloud


productKey = "xxxx"
deviceName = "test1"
deviceSecret = "xxxxx"
topic = productKey + "/" + deviceName + "/data"

print("iotcloud tencent iot hub test")
c = iotcloud.new()
if c.tencent(productKey, deviceName, deviceSecret):
    print("tencent iot hub init ok")
else:
    print("tencent iot hub init fail")


def up_cb():
    print("sub topic:", c.recv_topic)
    print("sub msg:", c.recv_msg)


e = c.connect()
print("connect:", e)
if e == 0:
    print("subcribe status:", c.subsribe(topic, up_cb))

    for i in range(10):
        print("publish status:", c.publish(topic, '{"id":'+str(i)+'}'))
        Time.sleep_s(5)

a = c.disconnect()
print("disconnect status:", a)
