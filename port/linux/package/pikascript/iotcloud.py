import hmac
import mqtt


class IOT:
    def __init__(self):
        self._signMethodTable = ["hmac-md5", "hmac-sha1", "hmac-sha256"]

    def aliyun(self, clientId: str, productKey: str, deviceName: str, deviceSecret: str,
               signMethod="hmac-md5", regionID="cn-shanghai", ssl=False):
        if clientId == None or productKey == None or deviceName == None or deviceSecret == None:
            print("[Error]input param is None")
            return False
        if not signMethod in self._signMethodTable:
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

    def tencent(self): ...
    def onenet(self): ...

    def connect(self):
        self._client = mqtt.MQTT(self._mqttUri, port=self._mqttPort, clinetID=self._mqttClientId,
                                 username=self._mqttUsername, password=self._mqttPassword, keepalive=600)
        return self._client.connect()

    def disconnect(self):
        return self._client.disconnect()

    def subsribe(self, topic, qos, cb):
        return self._client.subscribe(topic, qos, cb)

    def publish(self, topic, qos, payload):
        return self._client.publish(qos, topic, payload)


def new():
    iot = IOT()
    return iot
