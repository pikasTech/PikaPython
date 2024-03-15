import modbus_rt
import modbus_rt_defines as cst

ip_addr = "192.168.28.150"

rsu =  modbus_rt.rtu()
rsu.set_over_type(cst.OVER_NET)
rsu.set_net(ip_addr, 502, cst.SOCK_DGRAM)
rsu.add_block("A",cst.REGISTERS, 0, 10)
rsu.open()
# rsu.excuse(0,4,0,5)
# rsu.excuse(1,4,0,5,[1,2,34,5,6])
# rsu.excuse(0,4,0,5)
