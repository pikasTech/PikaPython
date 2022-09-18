import _modbus


class ModBus(_modbus._ModBus):
    def deserializeReadBits(self, msgLength: int):
        dest = bytes(msgLength)
        super().deserializeReadBits(msgLength, dest)
        return list(dest)

    def deserializeReadInputBits(self, msgLength: int):
        dest = bytes(msgLength)
        super().deserializeReadInputBits(msgLength, dest)
        return list(dest)

    def deserializeReadRegisters(self, msgLength: int) -> list:
        dest = bytes(2 * msgLength)
        super().deserializeReadRegisters(msgLength, dest)
        ret = []
        for i in range(0, len(dest), 2):
            ret.append(dest[i] + dest[i + 1] * 256)
        return ret

    def deserializeReadInputRegisters(self, msgLength: int) -> list:
        dest = bytes(2 * msgLength)
        super().deserializeReadInputRegisters(msgLength, dest)
        ret = []
        for i in range(0, len(dest), 2):
            ret.append(dest[i] + dest[i + 1] * 256)
        return ret

    def serializeWriteBits(self, addr: int, nb: int, src: list):
        src = bytes(len(src))
        for i in range(len(src)):
            src[i] = 1 if src[i] else 0
        return super().serializeWriteBits(addr, nb, src)

    def serializeWriteRegisters(self, addr: int, nb: int, src: list):
        _src = bytes(2 * len(src))
        for i in range(len(src)):
            _src[2 * i] = src[i] % 256
            _src[2 * i + 1] = src[i] // 256
        return super().serializeWriteRegisters(addr, nb, _src)

    def deserializeWriteAndReadRegisters(self, msgLength: int) -> list:
        dest = bytes(2 * msgLength)
        super().deserializeWriteAndReadRegisters(msgLength, dest)
        ret = []
        for i in range(0, len(dest), 2):
            ret.append(dest[i] + dest[i + 1] * 256)
        return ret


class ModBusRTU(ModBus):
    def __init__(self, sendBuffSize: int, readBuffSize: int):
        self.__init__rtu(sendBuffSize, readBuffSize)


class ModBusTCP(ModBus):
    def __init__(self, sendBuffSize: int, readBuffSize: int):
        self.__init__tcp(sendBuffSize, readBuffSize)
