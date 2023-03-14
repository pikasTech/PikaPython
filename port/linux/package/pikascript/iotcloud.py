import hmac
import base64
import random
import mqtt
import time

ONENET_API = 0
ONENET_DEVICE = 1


class IOT:
    def __init__(self):
        self._signMethodTable = ["hmacmd5", "hmacsha1", "hmacsha256"]

    def randStr(self, len):
        a = ""
        for i in range(len):
            a = a + str(random.randint(0, 9))
        return a

    def getTimeStamp(self, t):
        return str(int(time.time()) + t)

    def urlEncode(self, s):
        rs = ""
        for i in s:
            if i == "+":
                i = "%2B"
            elif i == " ":
                i = "%20"
            elif i == "/":
                i = "%2F"
            elif i == "?":
                i = "%3F"
            elif i == "%":
                i = "%25"
            elif i == "#":
                i = "%23"
            elif i == "&":
                i = "%26"
            elif i == "=":
                i = "%3D"
            rs = rs + i
        return rs

    def aliyun(self, clientId: str, productKey: str, deviceName: str, deviceSecret: str,
               signMethod="hmacmd5", regionID="cn-shanghai", ssl=False):
        if clientId == None or productKey == None or deviceName == None or deviceSecret == None:
            print("[Error]input param is None")
            return False
        if signMethod not in self._signMethodTable:
            print("[Error]not support signMethod")
            return False
        if ssl:
            securemode = "2"
            self.port = int(443)
        else:
            securemode = "3"
            self.port = int(1883)

        self.address = productKey + ".iot-as-mqtt." + regionID + ".aliyuncs.com"
        self.username = deviceName + "&" + productKey
        self.clientid = clientId + "|securemode=" + securemode + ",signmethod=" + signMethod + "|"
        hmac_payload = "clientId" + clientId + "deviceName" + deviceName + "productKey" + productKey
        self.password = hmac.new(deviceSecret.encode(),
                                 msg=hmac_payload.encode(),
                                 digestmod=signMethod.replace("hmac", "")).hexdigest()
        return True

    def tencent(self, productId, deviceName, deviceSecret, signMethod="hmacsha1", expiryTime=3600, ssl=False):
        if productId == None or deviceName == None or deviceSecret == None:
            print("[Error]input param is None")
            return False
        if signMethod not in self._signMethodTable:
            print("[Error]not support signMethod")
            return False
        connid = self.randStr(5)
        expiry = self.getTimeStamp(expiryTime)
        self.address = productId + ".iotcloud.tencentdevices.com"
        self.port = int(1883)
        self.clientid = productId + deviceName
        self.username = self.clientid + ";12010126;" + connid + ";" + expiry
        token = hmac.new(base64.b64decode(deviceSecret.encode()),
                         msg=self.username.encode(),
                         digestmod=signMethod.replace("hmac", "")).hexdigest()
        self.password = token + ";" + signMethod
        return True

    def onenet(self, productId, deviceName, accessKey, mode=ONENET_DEVICE, signMethod="hmacmd5", expiryTime=3600, ssl=False):
        if productId == None or deviceName == None or accessKey == None:
            print("[Error]input param is None")
            return False
        if signMethod not in self._signMethodTable:
            print("[Error]not support signMethod")
            return False
        if ssl:
            self.address = "mqttstls.heclouds.com"
            self.port = int(8883)
        else:
            self.address = "mqtts.heclouds.com"
            self.port = int(1883)
        self.clientid = deviceName
        self.username = productId
        method = signMethod.replace("hmac", "")
        expiry = self.getTimeStamp(expiryTime)
        if mode == ONENET_DEVICE:
            res = "products/" + productId + "/devices/" + deviceName
        elif mode == ONENET_API:
            res = "products/" + productId
        else:
            print("[Error]onenet not support mode")
            return False
        org = expiry + '\n' + method + '\n' + res + '\n' + "2018-10-31"
        k = base64.b64decode(accessKey.encode())
        h = hmac.new(k, msg=org.encode(), digestmod=method)
        sign = base64.b64encode(h.digest()).decode()
        res = self.urlEncode(res)
        sign = self.urlEncode(sign)
        self.password = "version=2018-10-31&res=%s&et=%s&method=%s&sign=%s" % (res, expiry, method, sign)
        print(self.clientid)
        print(self.username)
        print(self.password)
        return True

    def onenetMulti(self, productId, deviceId, apiKey):
        self.address = "mqtt.heclouds.com"
        self.port = int(6002)
        self.clientid = deviceId
        self.username = productId
        self.password = apiKey

    def connect(self, keepalive=600):
        self.client = mqtt.MQTT(self.address, port=self.port, clinetID=self.clientid,
                                 username=self.username, password=self.password, keepalive=keepalive)
        return self.client.connect()

    def disconnect(self):
        return self.client.disconnect()

    def subsribe(self, topic, cb, qos=0):
        return self.client.subscribe(topic, cb, qos)

    def publish(self, topic, payload, qos=0):
        return self.client.publish(topic, payload, qos)


def new():
    iot = IOT()
    return iot
