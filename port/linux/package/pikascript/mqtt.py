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
    def subscribe(self,topic='',qos=0,cb=''):
        return super().subscribe(topic,qos,cb)

    def publish(self,topic='',qos=0,payload=''):
        return super().publish(topic,qos,payload)

    def setWill(self,topic='',payload='',qos=1,retain):
        return super().setWill(topic,payload,qos,retain)

    def unsubscribe(self,topic=''):
        return super().unsubscribe(topic)
