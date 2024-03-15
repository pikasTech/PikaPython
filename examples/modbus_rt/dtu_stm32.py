import modbus_rt
import modbus_rt_defines as cst

serial_name = "uart4"
ip_addr = ""

rm = modbus_rt.rtu(cst.MASTER)
rm.set_serial(serial_name)
rm.open()
ts = modbus_rt.tcp()
ts.set_net(ip_addr, 502, cst.SOCK_STREAM)
def pre_call(evt) :
    slave = evt.slave
    function = evt.function
    addr = evt.addr
    quantity = evt.quantity
    if cst.READ_DISCRETE_INPUTS == function: 
        if addr >= 0 and addr <= 16 :
            data = rm.excuse(slave, function, addr + 10000, quantity)
            ts.excuse(cst.WRITE, cst.INPUTS, addr, quantity, data)
    elif cst.READ_COILS == function: 
        if addr >= 0 and addr <= 16 :
            data = rm.excuse(slave, function, addr + 20000, quantity)
            ts.excuse(cst.WRITE, cst.CIOLS, addr, quantity, data)
def done_call(evt) :
    slave = evt.slave
    function = evt.function
    addr = evt.addr
    quantity = evt.quantity
    if cst.WRITE_SINGLE_COIL == function: 
        if addr >= 0 and addr <= 16 :
            data = ts.excuse(cst.READ, cst.CIOLS, addr, 1)  
            rm.excuse(slave, function, addr + 20000, data[0])  
    elif cst.WRITE_MULTIPLE_COILS == function: 
        if addr >= 0 and addr <= 16 :
            data = ts.excuse(cst.READ, cst.CIOLS, addr, quantity)  
            rm.excuse(slave, function, addr + 20000, quantity, data)  
ts.set_strict(0)
ts.set_pre_ans_callback(pre_call)
ts.set_done_callback(done_call)
ts.open()
