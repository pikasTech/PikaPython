import _modbus


class ModBus(_modbus._ModBus):

    def serializeWriteBits(self, addr: int, src: list) -> bytes:
        lenth = super().serializeWriteBits(addr, len(list), bytes(src))
        return self.sendBuff[0:lenth]

    def serializeWriteRegisters(self, addr: int, src: list) -> bytes:
        _src = bytes(2 * len(src))
        for i in range(len(src)):
            _src[2 * i] = src[i] % 256
            _src[2 * i + 1] = src[i] // 256
        lenth = super().serializeWriteRegisters(addr, len(src), _src)
        return self.sendBuff[0:lenth]

    def serializeReadBits(self, addr: int, nb: int) -> bytes:
        lenth = super().serializeReadBits(addr, nb)
        return self.sendBuff[0:lenth]

    def serializeReadInputBits(self, addr: int, nb: int) -> bytes:
        lenth = super().serializeReadInputBits(addr, nb)
        return self.sendBuff[0:lenth]

    def serializeReadRegisters(self, addr: int, nb: int) -> bytes:
        lenth = super().serializeReadRegisters(addr, nb)
        return self.sendBuff[0:lenth]

    def serializeReadInputRegisters(self, addr: int, nb: int) -> bytes:
        lenth = super().serializeReadInputRegisters(addr, nb)
        return self.sendBuff[0:lenth]

    def serializeWriteBit(self, addr: int, status: int) -> bytes:
        lenth = super().serializeWriteBit(addr, status)
        return self.sendBuff[0:lenth]

    def serializeWriteRegister(self, addr: int, value: int) -> bytes:
        lenth = super().serializeWriteRegister(addr, value)
        return self.sendBuff[0:lenth]

    def serializeMaskWriteRegister(self,
                                   addr: int,
                                   andMask: int,
                                   orMask: int) -> bytes:
        lenth = super().serializeMaskWriteRegister(addr, andMask, orMask)
        return self.sendBuff[0:lenth]

    def serializeReportSlaveId(self) -> int:
        lenth = super().serializeReportSlaveId()
        return self.sendBuff[0:lenth]

    def deserializeReadRegisters(self, msg: bytes) -> list:
        self.readBuff = msg
        dest = super().deserializeReadRegisters(len(msg))
        ret = []
        for i in range(0, len(dest), 2):
            ret.append(int(dest[i]) + int(dest[i + 1]) * 256)
        return ret

    def deserializeReadBits(self, msg: bytes) -> list:
        self.readBuff = msg
        length = len(msg)
        dest = super().deserializeReadBits(length)
        return list(dest)

    def deserializeReadInputBits(self, msg: bytes) -> list:
        self.readBuff = msg
        length = len(msg)
        dest = super().deserializeReadInputBits(length)
        return list(dest)

    def deserializeReadInputRegisters(self, msg: bytes) -> list:
        self.readBuff = msg
        length = len(msg)
        dest = super().deserializeReadInputRegisters(length)
        ret = []
        for i in range(0, len(dest), 2):
            ret.append(int(dest[i]) + int(dest[i + 1]) * 256)
        return ret

    def deserializeWriteAndReadRegisters(self, msg: bytes) -> list:
        self.readBuff = msg
        length = len(msg)
        dest = super().deserializeWriteAndReadRegisters(length)
        ret = []
        for i in range(0, len(dest), 2):
            ret.append(int(dest[i]) + int(dest[i + 1]) * 256)
        return ret


class ModBusRTU(ModBus):
    def __init__(self, sendBuffSize: int, readBuffSize: int):
        self.__init__rtu(sendBuffSize, readBuffSize)


class ModBusTCP(ModBus):
    def __init__(self, sendBuffSize: int, readBuffSize: int):
        self.__init__tcp(sendBuffSize, readBuffSize)
