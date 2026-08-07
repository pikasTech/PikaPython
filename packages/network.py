"""Network API contract. Platform adapters are provided by the target."""


class AbstractNIC:
    def __init__(self, interface=None):
        self.interface = interface
        self._active = False

    def active(self, value=None):
        if value is None:
            return self._active
        self._active = bool(value)
        return None

    def connect(self, *args, **kwargs):
        raise OSError("network adapter is not available on this target")

    def disconnect(self):
        return None

    def isconnected(self):
        return False

    def ifconfig(self, config=None):
        if config is not None:
            raise OSError("network adapter is not available on this target")
        return ("0.0.0.0", "0.0.0.0", "0.0.0.0", "0.0.0.0")


class WLAN(AbstractNIC):
    STA_IF = 0
    AP_IF = 1


class LAN(AbstractNIC):
    pass
