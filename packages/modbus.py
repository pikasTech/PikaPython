# SPEC: PJ2026-050114 runtime ecosystem packages v0.1; Modbus frame codec.


def _byte(value, name):
    if value < 0 or value > 255:
        raise ValueError(name + " is outside 0..255")
    return value


def _word(value, name):
    if value < 0 or value > 65535:
        raise ValueError(name + " is outside 0..65535")
    return bytes([value >> 8, value & 255])


def _read_function(function):
    if function not in (1, 2, 3, 4):
        raise ValueError("function is not a read function")
    return function


def crc16(data):
    """Return the Modbus CRC-16 value for a bytes-like payload."""
    crc = 65535
    for value in data:
        crc ^= value
        index = 0
        while index < 8:
            if crc & 1:
                crc = (crc >> 1) ^ 40961
            else:
                crc = crc >> 1
            index += 1
    return crc


def add_crc(data):
    """Append the little-endian Modbus CRC to a payload."""
    value = crc16(data)
    return data + bytes([value & 255, value >> 8])


def check_crc(frame):
    """Return True when an RTU frame contains a valid trailing CRC."""
    if len(frame) < 3:
        return False
    expected = crc16(frame[:-2])
    return frame[-2] == (expected & 255) and frame[-1] == (expected >> 8)


def read_pdu(function, address, quantity):
    function = _read_function(function)
    if quantity < 1 or quantity > 2000:
        raise ValueError("read quantity is outside 1..2000")
    if function == 3 or function == 4:
        if quantity > 125:
            raise ValueError("register quantity is outside 1..125")
    return bytes([function]) + _word(address, "address") + _word(
        quantity, "quantity"
    )


def write_single_pdu(function, address, value):
    if function == 5:
        if value == 1:
            value = 65280
        elif value == 0:
            value = 0
        else:
            raise ValueError("coil value must be 0 or 1")
    elif function != 6:
        raise ValueError("function is not a single-write function")
    return bytes([function]) + _word(address, "address") + _word(
        value, "value"
    )


def write_multiple_pdu(function, address, values):
    count = len(values)
    if function == 15:
        if count < 1 or count > 1968:
            raise ValueError("coil quantity is outside 1..1968")
        byte_count = (count + 7) // 8
        packed = bytearray(byte_count)
        index = 0
        while index < count:
            if values[index] not in (0, 1, False, True):
                raise ValueError("coil value must be boolean")
            if values[index]:
                packed[index // 8] |= 1 << (index % 8)
            index += 1
        payload = bytes(packed)
    elif function == 16:
        if count < 1 or count > 123:
            raise ValueError("register quantity is outside 1..123")
        payload = b""
        for value in values:
            payload += _word(value, "register value")
        byte_count = len(payload)
    else:
        raise ValueError("function is not a multiple-write function")
    return bytes([function]) + _word(address, "address") + _word(
        count, "quantity"
    ) + bytes([byte_count]) + payload


def rtu_frame(slave, pdu):
    slave = _byte(slave, "slave")
    if slave == 0:
        raise ValueError("slave must be non-zero")
    if len(pdu) < 1 or len(pdu) > 253:
        raise ValueError("PDU length is outside 1..253")
    return add_crc(bytes([slave]) + pdu)


def tcp_frame(transaction_id, unit_id, pdu):
    unit_id = _byte(unit_id, "unit_id")
    if len(pdu) < 1 or len(pdu) > 253:
        raise ValueError("PDU length is outside 1..253")
    header = _word(transaction_id, "transaction_id") + b"\x00\x00"
    header += _word(len(pdu) + 1, "length")
    return header + bytes([unit_id]) + pdu


def rtu_request(slave, function, address, quantity):
    return rtu_frame(slave, read_pdu(function, address, quantity))


def tcp_request(transaction_id, unit_id, function, address, quantity):
    return tcp_frame(
        transaction_id, unit_id, read_pdu(function, address, quantity)
    )


def decode_rtu_frame(frame, slave=None):
    if len(frame) < 5 or not check_crc(frame):
        raise ValueError("Modbus RTU frame CRC or length is invalid")
    if slave is not None and frame[0] != slave:
        raise ValueError("Modbus RTU slave does not match")
    return frame[1:-2]


def decode_tcp_frame(frame, transaction_id=None, unit_id=None):
    if len(frame) < 8:
        raise ValueError("Modbus TCP frame is truncated")
    actual_transaction = (frame[0] << 8) | frame[1]
    if transaction_id is not None and actual_transaction != transaction_id:
        raise ValueError("Modbus TCP transaction does not match")
    if frame[2] != 0 or frame[3] != 0:
        raise ValueError("Modbus TCP protocol identifier is not zero")
    length = (frame[4] << 8) | frame[5]
    if length != len(frame) - 6:
        raise ValueError("Modbus TCP length does not match")
    if unit_id is not None and frame[6] != unit_id:
        raise ValueError("Modbus TCP unit identifier does not match")
    return frame[7:]


def decode_registers(data):
    if len(data) % 2 != 0:
        raise ValueError("register payload length must be even")
    values = []
    index = 0
    while index < len(data):
        values.append((data[index] << 8) | data[index + 1])
        index += 2
    return values


def decode_bits(data):
    values = []
    for value in data:
        index = 0
        while index < 8:
            values.append(bool(value & (1 << index)))
            index += 1
    return values


def decode_read_response(pdu, function):
    function = _read_function(function)
    if len(pdu) < 2:
        raise ValueError("Modbus response is truncated")
    if pdu[0] == (function | 128):
        if len(pdu) != 2:
            raise ValueError("Modbus exception response is invalid")
        raise ValueError("Modbus exception code " + str(pdu[1]))
    if pdu[0] != function:
        raise ValueError("Modbus response function does not match")
    if pdu[1] != len(pdu) - 2:
        raise ValueError("Modbus response byte count does not match")
    if function == 3 or function == 4:
        return decode_registers(pdu[2:])
    return decode_bits(pdu[2:])


class ModBusRTU:
    def __init__(self, slave=1):
        self.setSlave(slave)

    def setSlave(self, slave):
        self.slave = _byte(slave, "slave")
        if self.slave == 0:
            raise ValueError("slave must be non-zero")

    def _read(self, function, address, quantity):
        return rtu_frame(self.slave, read_pdu(function, address, quantity))

    def serializeReadBits(self, address, quantity):
        return self._read(1, address, quantity)

    def serializeReadInputBits(self, address, quantity):
        return self._read(2, address, quantity)

    def serializeReadRegisters(self, address, quantity):
        return self._read(3, address, quantity)

    def serializeReadInputRegisters(self, address, quantity):
        return self._read(4, address, quantity)

    def serializeWriteBit(self, address, value):
        return rtu_frame(self.slave, write_single_pdu(5, address, value))

    def serializeWriteRegister(self, address, value):
        return rtu_frame(self.slave, write_single_pdu(6, address, value))

    def serializeWriteBits(self, address, values):
        return rtu_frame(self.slave, write_multiple_pdu(15, address, values))

    def serializeWriteRegisters(self, address, values):
        return rtu_frame(self.slave, write_multiple_pdu(16, address, values))

    def deserializeReadBits(self, frame):
        return decode_read_response(decode_rtu_frame(frame, self.slave), 1)

    def deserializeReadInputBits(self, frame):
        return decode_read_response(decode_rtu_frame(frame, self.slave), 2)

    def deserializeReadRegisters(self, frame):
        return decode_read_response(decode_rtu_frame(frame, self.slave), 3)

    def deserializeReadInputRegisters(self, frame):
        return decode_read_response(decode_rtu_frame(frame, self.slave), 4)


class ModBusTCP:
    def __init__(self, unit_id=1, transaction_id=0):
        self.unit_id = _byte(unit_id, "unit_id")
        self.transaction_id = transaction_id

    def setSlave(self, unit_id):
        self.unit_id = _byte(unit_id, "unit_id")

    def _frame(self, pdu):
        frame = tcp_frame(self.transaction_id, self.unit_id, pdu)
        self.transaction_id = (self.transaction_id + 1) & 65535
        return frame

    def _read(self, function, address, quantity):
        return self._frame(read_pdu(function, address, quantity))

    def serializeReadBits(self, address, quantity):
        return self._read(1, address, quantity)

    def serializeReadInputBits(self, address, quantity):
        return self._read(2, address, quantity)

    def serializeReadRegisters(self, address, quantity):
        return self._read(3, address, quantity)

    def serializeReadInputRegisters(self, address, quantity):
        return self._read(4, address, quantity)

    def serializeWriteBit(self, address, value):
        return self._frame(write_single_pdu(5, address, value))

    def serializeWriteRegister(self, address, value):
        return self._frame(write_single_pdu(6, address, value))

    def serializeWriteBits(self, address, values):
        return self._frame(write_multiple_pdu(15, address, values))

    def serializeWriteRegisters(self, address, values):
        return self._frame(write_multiple_pdu(16, address, values))
"""CPython-compatible Modbus PDU helpers (transport independent)."""


def crc16(data):
    value = 0xFFFF
    for byte in data:
        value ^= byte
        for _ in range(8):
            if value & 1:
                value = (value >> 1) ^ 0xA001
            else:
                value >>= 1
    return value


def read_holding_registers(unit, address, count):
    if unit < 0 or unit > 255 or address < 0 or address > 0xFFFF or count < 1 or count > 125:
        raise ValueError("invalid Modbus register request")
    return bytes([unit, 3, address >> 8, address & 255, count >> 8, count & 255])


def parse_read_holding_registers(response, expected_unit=None):
    if len(response) < 5:
        raise ValueError("Modbus response is too short")
    if expected_unit is not None and response[0] != expected_unit:
        raise ValueError("Modbus unit mismatch")
    if response[1] & 0x80:
        raise ValueError("Modbus exception %d" % response[2])
    if response[1] != 3 or response[2] != len(response) - 3:
        raise ValueError("invalid Modbus register response")
    if response[2] % 2 != 0:
        raise ValueError("Modbus register payload has odd length")
    values = []
    index = 3
    while index < len(response):
        values.append((response[index] << 8) | response[index + 1])
        index += 2
    return values
