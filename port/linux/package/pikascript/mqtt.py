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
        print('MQTT init')
        print(port)
        print('ip:', ip)
        super().__init__(ip, port, clinetID,
                         username, password, version,
                         ca, keepalive)
