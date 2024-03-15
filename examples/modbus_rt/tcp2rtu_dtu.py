import modbus_rt
import modbus_rt_defines as cst

serial_name = "/dev/ttyUSB0"
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
    if cst.READ_HOLDING_REGISTERS == function: 
        data = rm.excuse(slave, function, addr, quantity)
        ts.excuse(cst.WRITE, cst.REGISTERS, addr, quantity, data)
    elif cst.READ_DISCRETE_INPUTS == function: 
        data = rm.excuse(slave, function, addr, quantity)
        ts.excuse(cst.WRITE, cst.INPUTS, addr, quantity, data)
def done_call(evt) :
    slave = evt.slave
    function = evt.function
    addr = evt.addr
    quantity = evt.quantity
    if cst.WRITE_SINGLE_COIL == function: 
        data = ts.excuse(cst.READ, cst.CIOLS, addr, 1)  
        rm.excuse(slave, function, addr, data[0])  
    elif cst.WRITE_SINGLE_REGISTER == function: 
        data = ts.excuse(cst.READ, cst.REGISTERS, addr, 1) 
        rm.excuse(slave, function, addr, data[0])  
    elif cst.WRITE_MULTIPLE_COILS == function: 
        data = ts.excuse(cst.READ, cst.CIOLS, addr, quantity)  
        rm.excuse(slave, function, addr, quantity, data)  
    elif cst.WRITE_MULTIPLE_REGISTERS == function: 
        data = ts.excuse(0, cst.REGISTERS, addr, quantity)  
        rm.excuse(slave, function, addr, quantity, data)    
ts.add_block("A", 0, 20000, 10)
ts.add_block("B", 1, 10000, 16)
ts.add_block("C", 4, 0, 10)
ts.set_strict(0)
ts.set_pre_ans_callback(pre_call)
ts.set_done_callback(done_call)
ts.open()
