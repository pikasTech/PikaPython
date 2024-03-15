import modbus_rt
import modbus_rt_defines as cst

ip_addr = "192.168.28.150"

tmu = modbus_rt.tcp(cst.MASTER)
tmu.set_net(ip_addr, 0, cst.SOCK_DGRAM)
tmu.open()

# tmu.set_server("255.255.255.255", 502)
# tmu.excuse(1,3,0,5)
# tmu.get_saddr()
# tmu.excuse(1,16,0,5,[21,23,24,25,65])
# tmu.excuse(1,3,0,5)
