from PikaObj import *

class _data_trans:
    def _reg2reg(self, val: int) -> int: ...
    def _regs2regs(self, val: list) -> list: ...
    def _regs2bytes(self, val: list, mode: int) -> bytes: ...
    def _regs2str(self, val: list, mode: int) -> str: ...
    def _regs2signed(self, val: list, mode: int) -> list: ...
    def _regs2int(self, val: list, mode: int) -> list: ...
    def _regs2uint(self, val: list, mode: int) -> list: ...
    def _regs2long(self, val: list, mode: int) -> list: ...
    # def _regs2ulong(self, val: list, mode: int) -> list: ...
    def _regs2float(self, val: list, mode: int) -> list: ...
    def _regs2double(self, val: list, mode: int) -> list: ...
    def _bytes2regs(self, val: any, mode: int) -> list: ...
    def _str2regs(self, val: str, mode: int) -> list: ...
    def _int2regs(self, val: list, mode: int) -> list: ...
    def _uint2regs(self, val: list, mode: int) -> list: ...
    def _long2regs(self, val: list, mode: int) -> list: ...
    # def _ulong2regs(self, val: list, mode: int) -> list: ...
    def _float2regs(self, val: list, mode: int) -> list: ...
    def _double2regs(self, val: list, mode: int) -> list: ...

class _rtu:
    def __del__(self): ...
    def _init(self, mode: int): ...
    def _set_serial(self, devname: str, baudrate: int, bytesize: int, parity: str, stopbits: int, xonxoff: int) -> int: ... 
    def _set_over_type(self, over_type: int) -> int: ...
    def _set_net(self, ip: str, port: int, type: int) -> int: ...
    def _set_ip(self, ip: str) -> int: ...
    def _set_port(self, port: int) -> int: ...
    def _set_type(self, type: int) -> int: ...
    def _set_p2p(self, p2p_flag: int) -> int: ...
    def _open(self) -> int: ...
    def _isopen(self) -> int: ...
    def _close(self) -> int: ...
    def _slave_set_addr(self, addr: int) -> int: ...
    def _slave_set_strict(self, strict: int) -> int: ...
    def _slave_add_block(self, name: str, type: int, addr: int, nums: int) -> int: ...
    def _slave_regs_binding(self, regs: any, type: int, addr: int, nums: int) -> int: ...
    def _slave_set_pre_ans_callback(self, cb: any) -> int: ...
    def _slave_set_done_callback(self, cb: any) -> int: ...
    def _slave_set_dev_binding(self, flag: int) -> int: ...
    def _master_set_server(self, saddr: str,sport: int) -> int: ...
    def _master_get_saddr(self) -> str: ...
    def _slave_read_regs(self, type: int, addr: int, *val) -> list: ...
    def _slave_write_regs(self, type: int, addr: int, *val) -> int: ...
    def _master_read_list(self, slave: int, fuction: int, addr: int, *val) -> list: ...
    def _master_write_int(self, slave: int, fuction: int, addr: int, *val) -> int: ...
    def _master_write_list(self, slave: int, fuction: int, addr: int, *val) -> int: ...
    def _master_download(self, slave: int, file_dev: str, file_master: str) -> int: ...
    def _master_upload(self, slave: int, file_dev: str, file_master: str) -> int: ...

class _tcp:
    def __del__(self): ...
    def _init(self, mode: int): ...
    def _set_net(self, ip: str, port: int, type: int) -> int: ...
    def _set_ip(self, ip: str) -> int: ...
    def _set_port(self, port: int) -> int: ...
    def _set_type(self, type: int) -> int: ...
    def _set_p2p(self, p2p_flag: int) -> int: ...
    def _open(self) -> int: ...
    def _isopen(self) -> int: ...
    def _close(self) -> int: ...
    def _slave_set_addr(self, addr: int) -> int: ...
    def _slave_set_strict(self, strict: int) -> int: ...
    def _slave_add_block(self, name: str, type: int, addr: int, nums: int) -> int: ...
    def _slave_regs_binding(self, regs: any, type: int, addr: int, nums: int) -> int: ...
    def _slave_set_pre_ans_callback(self, cb: any) -> int: ...
    def _slave_set_done_callback(self, cb: any) -> int: ...
    def _slave_set_dev_binding(self, flag: int) -> int: ...
    def _master_set_server(self, saddr: str,sport: int) -> int: ...
    def _master_get_saddr(self) -> str: ...
    def _slave_read_regs(self, type: int, addr: int, *val) -> list: ...
    def _slave_write_regs(self, type: int, addr: int, *val) -> int: ...
    def _master_read_list(self, slave: int, fuction: int, addr: int, *val) -> list: ...
    def _master_write_int(self, slave: int, fuction: int, addr: int, *val) -> int: ...
    def _master_write_list(self, slave: int, fuction: int, addr: int, *val) -> int: ...
    def _master_download(self, slave: int, file_dev: str, file_master: str) -> int: ...
    def _master_upload(self, slave: int, file_dev: str, file_master: str) -> int: ...