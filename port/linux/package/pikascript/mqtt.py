import _mqtt


class MQTT(_mqtt._MQTT):
    def __init__(self,
                 ip: str,
                 port=1883,
                 clinetID='mac',
                 username='',
                 password='',
                 version='3.1.1',
                 ca='',
                 keepalive=60):
        super().__init__(ip, port, clinetID,
                         username, password, version,
                         ca, keepalive)

    def subscribe(self, topic, cb, qos=0):
        return super().subscribe(topic, cb, qos)

    def publish(self, topic, payload, qos=0):
        return super().publish(topic, payload, qos)

    def setWill(self, topic, payload, qos=0, retain=0):
        return super().setWill(topic, payload, qos, retain)

    def unsubscribe(self, topic=''):
        return super().unsubscribe(topic)
