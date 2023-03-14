import _modbus


class ModBus(_modbus._ModBus):
    """
    A subclass of _modbus._ModBus that provides methods for serializing and sending modbus messages.
    """

    def serializeWriteBits(self, addr: int, src: list) -> bytes:
        """Serialize a write multiple coils request.

        Args:
            addr (int): The starting address of the coils to be written.
            src (list): A list of boolean values (0 or 1) to be written to the coils.

        Returns:
            bytes: The serialized message as a bytes object.
        """
        lenth = super().serializeWriteBits(addr, len(src), bytes(src))
        return self.sendBuff[0:lenth]

    def serializeWriteRegisters(self, addr: int, src: list) -> bytes:
        """Serialize a write multiple registers request.

        Args:
            addr (int): The starting address of the registers to be written.
            src (list): A list of integer values (0-65535) to be written to the registers.

        Returns:
            bytes: The serialized message as a bytes object.
        """
        _src = bytes(2 * len(src))
        for i in range(len(src)):
            _src[2 * i] = src[i] % 256
            _src[2 * i + 1] = src[i] // 256
        lenth = super().serializeWriteRegisters(addr, len(src), _src)
        return self.sendBuff[0:lenth]

    def serializeReadBits(self, addr: int, nb: int) -> bytes:
        """Serialize a read coils request.

        Args:
            addr (int): The starting address of the coils to be read.
            nb (int): The number of coils to be read.

        Returns:
            bytes: The serialized message as a bytes object.
        """
        lenth = super().serializeReadBits(addr, nb)
        return self.sendBuff[0:lenth]

    def serializeReadInputBits(self, addr: int, nb: int) -> bytes:
        """Serialize a read discrete inputs request.

        Args:
            addr (int): The starting address of the discrete inputs to be read.
            nb (int): The number of discrete inputs to be read.

        Returns:
            bytes: The serialized message as a bytes object.
        """
        lenth = super().serializeReadInputBits(addr, nb)
        return self.sendBuff[0:lenth]

    def serializeReadRegisters(self, addr: int, nb: int) -> bytes:
        """Serialize a read holding registers request.

        Args:
            addr (int): The starting address of the holding registers to be read.
            nb (int): The number of holding registers to be read.

        Returns:
            bytes: The serialized message as a bytes object.
        """
        lenth = super().serializeReadRegisters(addr, nb)
        return self.sendBuff[0:lenth]

    def serializeReadInputRegisters(self, addr: int, nb: int) -> bytes:
        """Serialize a read input registers request.

        Args:
            addr (int): The starting address of the input registers to be read.
            nb (int): The number of input registers to be read.

        Returns:
            bytes: The serialized message as a bytes object.
        """
        lenth = super().serializeReadInputRegisters(addr, nb)
        return self.sendBuff[0:lenth]

    def serializeWriteBit(self, addr: int, status: int) -> bytes:
        """Serialize a write single coil request.

        Args:
            addr (int): The address of the coil to be written.
            status (int): The value (0 or 1) to be written to the coil.

        Returns:
            bytes: The serialized message as a bytes object.
        """
        lenth = super().serializeWriteBit(addr, status)
        return self.sendBuff[0:lenth]

    def serializeWriteRegister(self, addr: int, value: int) -> bytes:
        """Serialize a write single register request.

        Args:
            addr (int): The address of the register to be written.
            value (int): The value (0-65535) to be written to the register.

        Returns:
            bytes: The serialized message as a bytes object.
        """
        lenth = super().serializeWriteRegister(addr, value)
        return self.sendBuff[0:lenth]

    def serializeMaskWriteRegister(self,
                                   addr: int,
                                   andMask: int,
                                   orMask: int) -> bytes:
        """Serialize a mask write register request.

        Args:
            addr (int): The address of the register to be modified.
            andMask (int): The AND mask to be applied to the current value of the register.
            orMask (int): The OR mask to be applied to the result of the AND operation.

        Returns:
            bytes: The serialized message as a bytes object.
        """
        lenth = super().serializeMaskWriteRegister(addr, andMask, orMask)
        return self.sendBuff[0:lenth]

    def serializeReportSlaveId(self) -> int:
        """Serialize a report slave ID request.

        Returns:
            int: The length of the serialized message in bytes.
        """
        lenth = super().serializeReportSlaveId()
        return self.sendBuff[0:lenth]

    def deserializeReadRegisters(self, msg: bytes) -> list:
        """Deserialize a read holding registers response.

        Args:
            msg (bytes): The received message as a bytes object.

        Returns:
            list: A list of integer values (0-65535) read from the registers.
        """
        self.readBuff = msg
        dest = super().deserializeReadRegisters(len(msg))
        ret = []
        for i in range(0, len(dest), 2):
            ret.append(int(dest[i]) + int(dest[i + 1]) * 256)
        return ret

    def deserializeReadBits(self, msg: bytes) -> list:
        """Deserialize a read coils response.

        Args:
            msg (bytes): The received message as a bytes object.

        Returns:
            list: A list of boolean values (True or False) read from the coils.
        """
        self.readBuff = msg
        length = len(msg)
        dest = super().deserializeReadBits(length)
        return list(dest)

    def deserializeReadInputBits(self, msg: bytes) -> list:
        """Deserialize a read discrete inputs response.

        Args:
            msg (bytes): The received message as a bytes object.

        Returns:
            list: A list of boolean values (True or False) read from the discrete inputs.
        """
        self.readBuff = msg
        length = len(msg)
        dest = super().deserializeReadInputBits(length)
        return list(dest)

    def deserializeReadInputRegisters(self, msg: bytes) -> list:
        """Deserialize a read input registers response.

        Args:
            msg (bytes): The received message as a bytes object.

        Returns:
            list: A list of integer values (0-65535) read from the input registers.
        """
        self.readBuff = msg
        length = len(msg)
        dest = super().deserializeReadInputRegisters(length)
        ret = []
        for i in range(0, len(dest), 2):
            ret.append(int(dest[i]) + int(dest[i + 1]) * 256)
        return ret

    def deserializeWriteAndReadRegisters(self, msg: bytes) -> list:
        """Deserialize a write and read registers response.

        Args:
            msg (bytes): The received message as a bytes object.

        Returns:
            list: A list of integer values (0-65535) written to and read from the registers.
        """
        self.readBuff = msg
        length = len(msg)
        dest = super().deserializeWriteAndReadRegisters(length)
        ret = []
        for i in range(0, len(dest), 2):
            ret.append(int(dest[i]) + int(dest[i + 1]) * 256)
        return ret


class ModBusRTU(ModBus):
    def __init__(self, sendBuffSize: int, readBuffSize: int):
        """Initialize a Modbus RTU protocol instance.

        Args:
            sendBuffSize (int): The size of the send buffer in bytes.
            readBuffSize (int): The size of the read buffer in bytes.
        """
        self.__init__rtu(sendBuffSize, readBuffSize)


class ModBusTCP(ModBus):
    def __init__(self, sendBuffSize: int, readBuffSize: int):
        """Initialize a Modbus TCP protocol instance.

        Args:
            sendBuffSize (int): The size of the send buffer in bytes.
            readBuffSize (int): The size of the read buffer in bytes.
        """
        self.__init__tcp(sendBuffSize, readBuffSize)
