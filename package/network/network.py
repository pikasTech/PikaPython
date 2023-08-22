import _network

STA_IF = _network.STA_IF
AP_IF = _network.AP_IF

STAT_IDLE = _network.STAT_IDLE
STAT_CONNECTING = _network.STAT_CONNECTING
STAT_WRONG_PASSWORD = _network.STAT_WRONG_PASSWORD
STAT_NO_AP_FOUND = _network.STAT_NO_AP_FOUND
STAT_CONNECT_FAIL = _network.STAT_CONNECT_FAIL
STAT_GOT_IP = _network.STAT_GOT_IP

_instence:"WLAN" = None
class WLAN(_network.WLAN):
    def __init__(self, interface_id: int):
        global _instence
        super().__init__(interface_id)
        if _instence is None:
            _instence = self

    def active(self, is_active=None):
        if is_active is None:
            return super().checkActive()
        else:
            return super().active(is_active)

    def connect(self, ssid=None, key=None, bssid=None):
        if bssid is None:
            return super().connect(ssid, key)
        else:
            return super().connectWithBssid(ssid, key, bssid)

    def disconnect(self):
        return super().disconnect()

    def status(self, param=None):
        if param is None:
            return super().status()
        else:
            return super().statusWithParam(param)

    def isconnected(self) -> int:
        return super().isconnected()

    def config(self, *para, **kwargs):
        if len(para) == 1:
            return super().checkConfig(para[0])
        else:
            return super().config(**kwargs)

    def ifconfig(self, config=None):
        if config is None:
            t = super().checkIfconfig()
            return (t[0], t[1], t[2], t[3])
        else:
            return super().ifconfig(
                config[0], config[1], config[2], config[3])

    def scan(self):
        return super().scan()

def isconnected():
    if _instence is None:
        return False
    return _instence.isconnected()
