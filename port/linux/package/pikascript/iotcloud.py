import hmac
import base64
import random
import mqtt
from PikaStdDevice import Time


class IOT:
    def __init__(self):
        self._signMethodTable = ["hmac-md5", "hmac-sha1", "hmac-sha256"]

    def randStr(self, len):
        a = ""
        for i in range(len):
            a = a+str(random.randint(0, 9))
        return a

    def aliyun(self, clientId: str, productKey: str, deviceName: str, deviceSecret: str,
               signMethod="hmac-md5", regionID="cn-shanghai", ssl=False):
        if clientId == None or productKey == None or deviceName == None or deviceSecret == None:
            print("[Error]input param is None")
            return False
        if signMethod not in self._signMethodTable:
            print("[Error] not support signMethod")
            return False
        if ssl:
            securemode = "2"
            self._mqttPort = int(443)
        else:
            securemode = "3"
            self._mqttPort = int(1883)

        hmac_payload = "clientId" + clientId + "deviceName" + \
            deviceName + "productKey" + productKey
        self._mqttPassword = hmac.new(deviceSecret.encode(
        ), msg=hmac_payload.encode(), digestmod=signMethod).hexdigest()
        self._mqttClientId = clientId + "|securemode=" + securemode + \
            ",signmethod="+signMethod.replace("-", "")+"|"
        self._mqttUsername = deviceName + "&" + productKey
        self._mqttUri = productKey + ".iot-as-mqtt." + regionID + ".aliyuncs.com"
        return True

    def tencent(self, productId, deiceName, deviceSecret, signMethod="hmac-sha1", ssl=False):
        if productId == None or deiceName == None or deviceSecret == None:
            print("[Error]input param is None")
            return False
        if signMethod not in self._signMethodTable:
            print("[Error] not support signMethod")
            return False
        connid = self.randStr(5)
        expiry = int(Time.time()) + 60 * 60
        self._mqttUri = productId + ".iotcloud.tencentdevices.com"
        self._mqttPort = int(1883)
        self._mqttClientId = productId + deiceName
        self._mqttUsername = self._mqttClientId+";" + \
            deiceName+";12010126;"+connid+";"+expiry
        secret = base64.b64decode(deviceSecret.encode())
        token = hmac.new(secret, msg=self._mqttUsername.encode(),
                         digestmod=signMethod).hexdigest()
        self._mqttPassword = token+";"+signMethod.replace("-", "")
        return True

    def onenet(self): ...

    def connect(self, keepalive=600):
        self._client = mqtt.MQTT(self._mqttUri, port=self._mqttPort, clinetID=self._mqttClientId,
                                 username=self._mqttUsername, password=self._mqttPassword, keepalive=keepalive)
        return self._client.connect()

    def disconnect(self):
        return self._client.disconnect()

    def subsribe(self, topic, cb, qos=0):
        return self._client.subscribe(topic, qos, cb)

    def publish(self, topic, payload, qos=0):
        return self._client.publish(qos, topic, payload)


def new():
    iot = IOT()
    return iot
