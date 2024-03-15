import modbus_rt
import modbus_rt_defines as cst

serial_name = "COM11"

rm =  modbus_rt.rtu(cst.MASTER)
rm.set_serial(serial_name)
rm.open()
# rm.excuse(1,3,0,5)
# rm.excuse(1,16,0,5,[21,23,24,25,65])
# rm.excuse(1,3,0,5)
