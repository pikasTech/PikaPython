import _modbus_rt
import modbus_rt_defines as cst

LITTLE_ENDIAL_SWAP = 0
BIG_ENDIAL_SWAP = 1
LITTLE_ENDIAL = 2
BIG_ENDIAL = 3

SLAVE = 0
MASTER = 1

SOCK_STREAM = 1
SOCK_DGRAM = 2

class data_trans(_modbus_rt._data_trans):
    def reg2reg(self, val: int):
        return self._reg2reg(val) 
    def regs2regs(self, val: list):
        return self._regs2regs(val)
    def regs2bytes(self, val: list, mode = LITTLE_ENDIAL_SWAP):
        if type(val) != list:
            print("para only for type list")
            return
        return self._regs2bytes(val, mode)
    def regs2str(self, val: list, mode = LITTLE_ENDIAL_SWAP):
        return self._regs2str(val, mode)
    def regs2signed(self, val: list, mode = LITTLE_ENDIAL_SWAP):
        return self._regs2signed(val, mode)
    def regs2int(self, val: list, mode = LITTLE_ENDIAL_SWAP):
        return self._regs2uint(val, mode)
    def regs2uint(self, val: list, mode = LITTLE_ENDIAL_SWAP):
        return self._regs2int(val, mode)
    def regs2long(self, val: list, mode = LITTLE_ENDIAL_SWAP):
        return self._regs2long(val, mode)
    def regs2float(self, val: list, mode = LITTLE_ENDIAL_SWAP):
        return self._regs2float(val, mode)
    def regs2double(self, val: list, mode = LITTLE_ENDIAL_SWAP):
        return self._regs2double(val, mode)
    def bytes2regs(self, val: any, mode = LITTLE_ENDIAL_SWAP):
        return self._bytes2regs(val, mode)
    def str2regs(self, val: str, mode = LITTLE_ENDIAL_SWAP):
        return self._str2regs(val, mode)
    def signed2regs(self, val: list, mode = LITTLE_ENDIAL_SWAP):
        return self._signed2regs(val, mode)
    def int2regs(self, val: list, mode = LITTLE_ENDIAL_SWAP):
        return self._int2regs(val, mode)
    def uint2regs(self, val: list, mode = LITTLE_ENDIAL_SWAP):
        return self._uint2regs(val, mode)
    def long2regs(self, val: list, mode = LITTLE_ENDIAL_SWAP):
        return self._long2regs(val, mode)
    def float2regs(self, val: list, mode = LITTLE_ENDIAL_SWAP):
        return self._float2regs(val, mode)
    def double2regs(self, val: list, mode = LITTLE_ENDIAL_SWAP):
        return self._double2regs(val, mode)

class rtu(_modbus_rt._rtu):
    mode = SLAVE
    ascii_flag = 0
    def __init__(self, mode = SLAVE):
        return self._init(mode)
    def set_serial(self, devname: str, baudrate = 115200, bytesize = 8, parity = 'N', stopbits = 1, xonxoff = 0):
        return self._set_serial(devname, baudrate, bytesize, parity, stopbits, xonxoff)
    def set_over_type(self, over_type: int):
        return self._set_over_type(over_type)
    def set_net(self, ip = '', port = 502, type = SOCK_STREAM):
        return self._set_net(ip, port, type)
    def set_ip(self, ip: str):
        return self._set_ip(ip)
    def set_port(self, port: int):
        return self._set_port(port)
    def set_type(self, type: int):
        return self._set_type(type)
    def set_p2p(self, p2p_flag: int):
        return self._set_p2p(p2p_flag)  
    def open(self):
        return self._open()
    def isopen(self):
        return self._isopen()
    def close(self):
        return self._close()    
    # 该函数仅对从机有效
    def set_addr(self, addr: int):
        return self._slave_set_addr(addr)
    # 该函数仅对从机有效
    def set_strict(self, strict: int):
        return self._slave_set_strict(strict)
    # 该函数仅对从机有效
    def add_block(self, name: str, type: int, addr: int, nums: int):
        return self._slave_add_block(name, type, addr, nums)
    def set_pre_ans_callback(self, cb):
        return self._slave_set_pre_ans_callback(cb)
    def set_done_callback(self, cb):
        return self._slave_set_done_callback(cb)
    def set_dev_binding(self, flag: int):
        return self._slave_set_dev_binding(flag)
    # 该函数仅对主机有效
    def set_server(self, saddr: str,sport: int):
        return self._master_set_server(saddr,sport)
    def get_saddr(self):
        return self._master_get_saddr()
    def excuse(self, dir_slave: int, type_function: int, addr: int, *val):
        if self.mode == SLAVE:
            if dir_slave == 0 :
                return self._slave_read_regs(type_function, addr, *val)
            elif dir_slave == 1 :
                return self._slave_write_regs(type_function, addr, *val)
            else :
                return
        elif self.mode == MASTER:
            if type_function >= cst.READ_COILS and type_function <= cst.READ_INPUT_REGISTERS:
                return self._master_read_list(dir_slave,type_function, addr, *val)
            elif type_function >= cst.WRITE_SINGLE_COIL and type_function <= cst.WRITE_SINGLE_REGISTER:
                return self._master_write_int(dir_slave,type_function, addr, *val)
            elif type_function >= cst.WRITE_MULTIPLE_COILS and type_function <= cst.WRITE_MULTIPLE_REGISTERS:
                return self._master_write_list(dir_slave,type_function, addr, *val)
            else :
                return
        else :
            return 
    # 该函数仅对主机有效
    def download(self, slave: int, file_dev: str, file_master: str):
        return self._master_download(slave, file_dev, file_master)
    # 该函数仅对主机有效
    def upload(self, slave: int, file_dev: str, file_master: str):
        return self._master_upload(slave, file_dev, file_master)

class ascii(rtu):
    mode = SLAVE
    ascii_flag = 0


class tcp(_modbus_rt._tcp):
    mode = SLAVE
    def __init__(self, mode = SLAVE):
        return self._init(mode)
    def set_net(self, ip = '', port = 502, type = SOCK_STREAM):
        return self._set_net(ip, port, type)
    def set_ip(self, ip: str):
        return self._set_ip(ip)
    def set_port(self, port: int):
        return self._set_port(port)
    def set_type(self, type: int):
        return self._set_type(type)
    def set_p2p(self, p2p_flag: int):
        return self._set_p2p(p2p_flag)  
    def open(self):
        return self._open()
    def isopen(self):
        return self._isopen()
    def close(self):
        return self._close()     
    # 该函数仅对从机有效
    def set_addr(self, addr: int):
        return self._slave_set_addr(addr)
    # 该函数仅对从机有效
    def set_strict(self, strict: int):
        return self._slave_set_strict(strict)
    # 该函数仅对从机有效
    def add_block(self, name: str, type: int, addr: int, nums: int):
        return self._slave_add_block(name, type, addr, nums)
    def set_pre_ans_callback(self, cb):
        return self._slave_set_pre_ans_callback(cb)
    def set_done_callback(self, cb):
        return self._slave_set_done_callback(cb)
    def set_dev_binding(self, flag: int):
        return self._slave_set_dev_binding(flag)
    # 该函数仅对主机有效
    def set_server(self, saddr: str,sport: int):
        return self._master_set_server(saddr,sport)
    def get_saddr(self):
        return self._master_get_saddr()
    def excuse(self, dir_slave: int, type_function: int, addr: int, *val):
        if self.mode == SLAVE:
            if dir_slave == 0 :
                return self._slave_read_regs(type_function, addr, *val)
            elif dir_slave == 1 :
                return self._slave_write_regs(type_function, addr, *val)
            else :
                return
        elif self.mode == MASTER:
            if type_function >= cst.READ_COILS and type_function <= cst.READ_INPUT_REGISTERS:
                return self._master_read_list(dir_slave,type_function, addr, *val)
            elif type_function >= cst.WRITE_SINGLE_COIL and type_function <= cst.WRITE_SINGLE_REGISTER:
                return self._master_write_int(dir_slave,type_function, addr, *val)
            elif type_function >= cst.WRITE_MULTIPLE_COILS and type_function <= cst.WRITE_MULTIPLE_REGISTERS:
                return self._master_write_list(dir_slave,type_function, addr, *val)
            else :
                return
        else :
            return
    # 该函数仅对主机有效
    def download(self, slave: int, file_dev: str, file_master: str):
        return self._master_download(slave, file_dev, file_master) 
    # 该函数仅对主机有效
    def upload(self, slave: int, file_dev: str, file_master: str):
        return self._master_upload(slave, file_dev, file_master)
