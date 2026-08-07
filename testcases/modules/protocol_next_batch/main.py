import ctypes
import jrpc
import modbus
import modbus_rt
import network


assert modbus.crc16(bytes([1, 3, 0, 0, 0, 1])) == 0x0A84
assert modbus_rt.validate(modbus_rt.frame(bytes([1, 3, 0, 0, 0, 1]))) == bytes([1, 3, 0, 0, 0, 1])
assert jrpc.loads(jrpc.request("sum", [1, 2], 7))["id"] == 7
assert int(ctypes.c_int(3)) == 3
assert network.WLAN(network.WLAN.STA_IF).active() is False
print("[PROTOCOL_NEXT_BATCH] modbus=jrpc=ctypes=modbus_rt=network ok")
