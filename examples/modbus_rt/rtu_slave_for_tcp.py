import modbus_rt
import modbus_rt_defines as cst

ip_addr = "192.168.28.150"

rst =  modbus_rt.rtu()
rst.set_over_type(cst.OVER_NET)
rst.set_net(ip_addr, 502, cst.SOCK_STREAM)
rst.add_block("A",cst.REGISTERS, 0, 10)
rst.open()
# rst.excuse(0,4,0,5)
# rst.excuse(1,4,0,5,[1,2,34,5,6])
# rst.excuse(0,4,0,5)
