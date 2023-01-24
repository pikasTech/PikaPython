import _network

STA_IF = 0
AP_IF = 1

STAT_IDLE = 0
STAT_CONNECTING = 1
STAT_WRONG_PASSWORD = 2
STAT_NO_AP_FOUND = 3
STAT_CONNECT_FAIL = 4
STAT_GOT_IP = 5

class WLAN(_network.WLAN):
    def __init__(self, interface_id:int):
        super().__init__(interface_id)
    
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
            return super().checkIfconfig()
        else:
            return super().ifconfig(config)
