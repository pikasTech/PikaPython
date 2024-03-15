import modbus_rt
import modbus_rt_defines as cst

ip_addr = "192.168.28.150"

ts = modbus_rt.tcp()
ts.set_net(ip_addr, 502, cst.SOCK_STREAM)
ts.add_block("A",cst.REGISTERS, 0, 10)
ts.open()
# ts.excuse(0,4,0,5)
# ts.excuse(1,4,0,5,[1,2,34,5,6])
# ts.excuse(0,4,0,5)

