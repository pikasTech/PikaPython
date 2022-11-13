
class _MQTT:
    def __init__(self, ip: str, port: int, clinetID: str,
                 username: str, password: str, version: str,
                 ca: str, keepalive: int): ...

    def __del__(self): ...
