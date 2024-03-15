import modbus_rt
import modbus_rt_defines as cst

ip_addr = "192.168.28.150"

tsu = modbus_rt.tcp()
tsu.set_net(ip_addr, 502, cst.SOCK_DGRAM)
tsu.add_block("A",cst.REGISTERS, 0, 10)
tsu.open()
# tsu.excuse(0,4,0,5)
# tsu.excuse(1,4,0,5,[1,2,34,5,6])
# tsu.excuse(0,4,0,5)

