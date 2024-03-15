import modbus_rt
import modbus_rt_defines as cst

serial_name = "COM11"

ascii_s =  modbus_rt.ascii()
ascii_s.set_serial(serial_name)
ascii_s.add_block("A",cst.REGISTERS, 0, 10)
ascii_s.open()
# ascii_s.excuse(0,4,0,5)
# ascii_s.excuse(1,4,0,5,[1,2,34,5,6])
# ascii_s.excuse(0,4,0,5)
