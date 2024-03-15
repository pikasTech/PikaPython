import modbus_rt
import modbus_rt_defines as cst

serial_name = "uart1"

rs =  modbus_rt.rtu()
rs.set_serial(serial_name)
rs.add_block("A",cst.REGISTERS, 0, 10)
rs.open()
# rs.excuse(0,4,0,5)
# rs.excuse(1,4,0,5,[1,2,34,5,6])
# rs.excuse(0,4,0,5)
