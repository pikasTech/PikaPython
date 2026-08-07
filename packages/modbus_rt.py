"""Modbus RTU framing helpers; serial transport is supplied by the caller."""
import modbus


def frame(pdu):
    checksum = modbus.crc16(pdu)
    return pdu + bytes([checksum & 255, checksum >> 8])


def validate(frame_data):
    if len(frame_data) < 4:
        raise ValueError("Modbus RTU frame is too short")
    expected = modbus.crc16(frame_data[:-2])
    actual = frame_data[-2] | (frame_data[-1] << 8)
    if expected != actual:
        raise ValueError("Modbus RTU CRC mismatch")
    return frame_data[:-2]
