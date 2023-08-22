import _modbus
import PikaStdDevice
import time
import random

CONFIG_DEBUG = False
CONFIG_FAKE_DATA = False

def debug(*info):
    if CONFIG_DEBUG:
        print("[Debug]", *info)

def error(*info):
    print("[Error]", *info)

def info(*info):
    print("[Info]", *info)

class ModBus(_modbus._ModBus):
    """
    A subclass of _modbus._ModBus that provides methods for serializing and sending modbus messages.
    """
    _option: str = None

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
        if dest is None:
            return None
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
        if dest is None:
            return None
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
        if dest is None:
            return None
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
        if dest is None:
            return None
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
        if dest is None:
            return None
        ret = []
        for i in range(0, len(dest), 2):
            ret.append(int(dest[i]) + int(dest[i + 1]) * 256)
        return ret

    def deserializeWriteBit(self, msg: bytes) -> int:
        """Deserialize a write single coil response.

        Args:
            msg (bytes): The received message as a bytes object.

        Returns:
            int: The address of the coil written to.
        """
        self.readBuff = msg
        length = len(msg)
        dest = super().deserializeWriteBit(length)
        if dest is None:
            return None
        return dest
    
    def deserializeWriteRegister(self, msg: bytes) -> int:
        """Deserialize a write single register response.

        Args:
            msg (bytes): The received message as a bytes object.

        Returns:
            int: The address of the register written to.
        """
        self.readBuff = msg
        length = len(msg)
        dest = super().deserializeWriteRegister(length)
        if dest is None:
            return None
        return dest
    
    def deserializeWriteBits(self, msg: bytes) -> int:
        """Deserialize a write multiple coils response.

        Args:
            msg (bytes): The received message as a bytes object.

        Returns:
            int: The number of coils written to.
        """
        self.readBuff = msg
        length = len(msg)
        dest = super().deserializeWriteBits(length)
        if dest is None:
            return None
        return dest
    
    def deserializeWriteRegisters(self, msg: bytes) -> int:
        """Deserialize a write multiple registers response.

        Args:
            msg (bytes): The received message as a bytes object.

        Returns:
            int: The number of registers written to.
        """
        self.readBuff = msg
        length = len(msg)
        dest = super().deserializeWriteRegisters(length)
        if dest is None:
            return None
        return dest
    
    def deserializeMaskWriteRegister(self, msg: bytes) -> int:
        """Deserialize a mask write register response.

        Args:
            msg (bytes): The received message as a bytes object.

        Returns:
            int: The address of the register modified.
        """
        self.readBuff = msg
        length = len(msg)
        dest = super().deserializeMaskWriteRegister(length)
        if dest is None:
            return None
        return dest
    
    def deserializeReportSlaveId(self, msg: bytes) -> bytes:
        """Deserialize a report slave ID response.

        Args:
            msg (bytes): The received message as a bytes object.

        Returns:
            bytes: The slave ID.
        """
        self.readBuff = msg
        length = len(msg)
        dest = super().deserializeReportSlaveId(length, 0)
        if dest is None:
            return None
        return dest
    
OP_CODE_MAP = {
    'x01': 'ReadBits',
    'x02': 'ReadInputBits',
    'x03': 'ReadRegisters',
    'x04': 'ReadInputRegisters',
    'x05': 'WriteBit',
    'x06': 'WriteRegister',
}

class ModBusRTU(ModBus):
    _uart: PikaStdDevice.UART = None
    _receive_msg: bytes = None
    _request_callback_before = None
    _request_callback_after = None
    _send_callback_before = None
    _send_callback_after = None
    def __init__(self, 
                 sendBuffSize=128, 
                 readBuffSize=128, 
                 uart:PikaStdDevice.UART = None
                 ):
        """Initialize a Modbus RTU protocol instance.

        Args:
            sendBuffSize (int): The size of the send buffer in bytes.
            readBuffSize (int): The size of the read buffer in bytes.
        """
        self.__init__rtu(sendBuffSize, readBuffSize)
        if uart is not None:
            self.setUart(uart)

    def recvCallback(self, signal):
        msg = self._uart.readBytes(128)
        debug('uart origin recive', msg)
        self._receive_msg = msg
    
    def setUart(self, uart: PikaStdDevice.UART):
        self._uart = uart
        uart.setCallBack(self.recvCallback, uart.SIGNAL_RX)

    def recv(self, count: int = 10):
        """
        获取uart返回的数据
        :param count:
        :return:
        """
        if count > 255 or count < 1:
            count = 10
        i = 0

        while i < count:
            time.sleep(0.2)
            if self._receive_msg:
                res = self._receive_msg
                self._receive_msg = None
                return res
            i += 1
        error('get result timeout')
        return None
    
    def setBaudRate(self, baud: int):
        # 重新设置 波特率
        if baud and type(baud) == int and baud > 0 and baud != self._baud:
            debug('重新设置波特率', baud)
            self._uart.setBaudRate(baud)
            self._baud = baud
    
    def send(self, msg: bytes):
        """
        发送485十六进制消息
        :param msg: modbus rtu 消息
        :param band: 波特率
        :return:
        """
        # 放置 待发送消息
        self._transport_msg = msg
        if self._send_callback_before is not None:
            self._send_callback_before()
        if str(type(msg)) == "<class 'bytes'>":
            debug('sending msg by bytes', msg)
            self._uart.writeBytes(msg, len(msg))
        else:
            debug('sending msg by other', msg)
            self._uart.write(msg)
        if self._send_callback_after is not None:
            self._send_callback_after()
    
    def setOption(self, op_code:str):
        self._option = op_code
    
    def _getOperation(self, prefix:str, op_code:str):
        name = prefix + OP_CODE_MAP[op_code]
        if hasattr(self, name):
            return getattr(self, name)
        debug('not found operation', name)
        return None
    
    def getSerializer(self, op_code:str):
        return self._getOperation('serialize', op_code)
    
    def getDeserializer(self, op_code:str):
        return self._getOperation('deserialize', op_code)
    
    def serializeRequest(self, 
                         op_code:str, 
                         addr:int, 
                         arg, 
                         slave:int = None)->bytes:
        """
        序列化modbus rtu请求
        :param op_code: 操作码
        :param addr: 寄存器地址
        :param arg: 参数 读取时为数量 写入时为值
        :param slave: 从机地址
        :return: 序列化后的数据
        """
        if slave is not None:
            self.setSlave(slave)
        self.setOption(op_code)
        serializer = self.getSerializer(op_code)
        msg = serializer(addr, arg)
        return msg

    def deserializeResponse(self, op_code:str, msg:bytes):
        """
        反序列化modbus rtu响应
        :param op_code: 操作码
        :param msg: 响应消息
        :return: 解码后的数据
        """
        deserializer = self.getDeserializer(op_code)
        ret = deserializer(msg)
        return ret
        
    def _do_request(self, op_code:str, addr:int, arg, slave:int = None):
        """
        发送modbus rtu请求
        :param op_code: 操作码
        :param addr: 寄存器地址
        :param arg: 参数 读取时为数量 写入时为值
        :param slave: 从机地址
        :return: 解码后的数据
        """

        if CONFIG_FAKE_DATA:
            info("using fake data")
            ret = random.randint(0, 65535)
            return ret

        msg = self.serializeRequest(op_code, addr, arg, slave)
        self.send(msg)
        res = self.recv()
        if res is None:
            error('request timeout')
            return None
        ret = self.deserializeResponse(op_code, res)
        debug('request', 'result:', ret)
        return ret
    
    def request(self, op_code:str, addr:int, arg, slave:int = None):
        """
        发送modbus rtu请求
        :param op_code: 操作码
        :param addr: 寄存器地址
        :param arg: 参数 读取时为数量 写入时为值
        :param slave: 从机地址
        :return: 解码后的数据
        """
        if self._request_callback_before is not None:
            self._request_callback_before()
        ret = self._do_request(op_code, addr, arg, slave)
        if self._request_callback_after is not None:
            self._request_callback_after()
        return ret
    
    def set_request_callback(self, cb_before, cb_after):
        self._request_callback_before = cb_before
        self._request_callback_after = cb_after
    
    def set_send_callback(self, cb_before, cb_after):
        self._send_callback_before = cb_before
        self._send_callback_after = cb_after
        

class ModBusTCP(ModBus):
    def __init__(self, sendBuffSize: int, readBuffSize: int):
        """Initialize a Modbus TCP protocol instance.

        Args:
            sendBuffSize (int): The size of the send buffer in bytes.
            readBuffSize (int): The size of the read buffer in bytes.
        """
        self.__init__tcp(sendBuffSize, readBuffSize)
