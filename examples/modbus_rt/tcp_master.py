import modbus_rt
import modbus_rt_defines as cst

ip_addr = "192.168.28.150"

tm = modbus_rt.tcp(cst.MASTER)
tm.set_net(ip_addr, 0, cst.SOCK_STREAM)
tm.set_server(ip_addr, 502)
tm.open()
# tm.excuse(1,3,0,5)
# tm.excuse(1,16,0,5,[21,23,24,25,65])
# tm.excuse(1,3,0,5)
