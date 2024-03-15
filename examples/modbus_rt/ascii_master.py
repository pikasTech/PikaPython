import modbus_rt
import modbus_rt_defines as cst

serial_name = "COM11"

ascii_m =  modbus_rt.rtu(cst.MASTER)
ascii_m.set_serial(serial_name)
ascii_m.open()
# ascii_m.excuse(1,3,0,5)
# ascii_m.excuse(1,16,0,5,[21,23,24,25,65])
# ascii_m.excuse(1,3,0,5)
