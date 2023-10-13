import bluetooth
class Sender():
    def __init__(self):
        self.ble = bluetooth.BLE()
        self._UUID_basic  = [0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80,0x05, 0x9b, 0x34, 0xfb]
        self._UUID  = self._UUID_basic
        self._Major = [0x00,0x00]
        self._Minor = [0x00,0x00]
        self._company_id = [0x4C,0x00]
        self._ibeacon_id = [0x02,0x15]
        self._rssi_level = [0xC0]

    def active(self,active_flag = None):
        if active_flag == None:
            return self.ble.pyi_check_active()
        else:
            if (active_flag > 0 or active_flag == True):
                return self.ble.pyi_active(True)
            elif (active_flag == 0 or active_flag == False):
                return self.ble.pyi_active(False)
    
    def config(self,*param_name, **kv):
        try:
            self.ble._check_active()
        except:
            raise OSError
        print(param_name)

        if len(param_name) != 0:
            first_param = param_name[0]
            if first_param == "UUID":
                return bytes(self._UUID)
            elif first_param == "Major":
                return self._Major[0] * 65536 + self._Major[1]
            elif first_param == "Minor":
                return self._Minor[0] * 65536 + self._Minor[1]
            elif first_param == "company_id":
                return self._company_id
            elif first_param == "ibeacon_id":
                return self._ibeacon_id
            elif first_param == "rssi_level":
                return self._rssi_level[0]
            else:
                raise KeyError
        
        if "UUID" in kv:
            self._input2UUID(kv["UUID"])
        
        if "Major" in kv:
            self._Major = self._uint2list(kv["Major"])

        if "Minor" in kv:
            self._Minor = self._uint2list(kv["Minor"])

        if "company_id" in kv:
            self._company_id = self._uint2list(kv["company_id"])

        if "ibeacon_id" in kv:
            self._ibeacon_id = self._uint2list(kv["ibeacon_id"])

        if "rssi_level" in kv:
            self._rssi_level = self._uint2list(kv["rssi_level"],1)

        return 0
    
    def adv(self,interval_us):
        try:
            self.ble._check_active()
        except:
            raise OSError
        
        flags = [0x01,0x02]
        ibeacon_data = [0xFF] + self._company_id + self._ibeacon_id + self._UUID + self._Major + self._Minor + self._rssi_level
        adv_data = [flags,ibeacon_data]
        # TODO:指定函数赋值问题
        return self.ble.gap_advertise(interval_us,adv_data=adv_data,connectable=False, adv_data_append=False)
        # self.ble.gap_advertise(interval_us,adv_data,connectable=False,adv_data_append=False)
        # self.ble.gap_advertise(6250,"adv_test","rsp_test",connectable=False)
        # self.ble.gap_advertise(6250,"adv_test","rsp_test")


    def _input2UUID(self,value):
        value_list = []
        if isinstance(value, bytes):
            if len(value) == 16:
                value_list = list[value]
        elif isinstance(value,int):  
            value_list = self._uint2list(value)
        elif isinstance(value,list):
            value_list = value
        elif isinstance(value,str):
            if len(value) == 36 and (value[8] == '-') and (value[13] == '-') and (value[18] == '-') and  (value[23] == '-'):
                value_str = value.replace("-","")
                if len(value_str) == 32:
                    for i in range(0, 32, 2):
                        value_list.append(int(value_str[i:i+2],16))
                else :
                    raise ValueError
            else : 
                raise ValueError
        else:
            raise ValueError
        
        UUID_bytes = len(value_list)

        if UUID_bytes == 2:
            self._UUID = self._UUID_basic
            self._UUID[2] = value_list[0]
            self._UUID[3] = value_list[1]
        elif UUID_bytes == 4:
            self._UUID = self._UUID_basic
            self._UUID[0] = value_list[0]
            self._UUID[1] = value_list[1]
            self._UUID[2] = value_list[2]
            self._UUID[3] = value_list[3]
        else:
            self._UUID = value_list

    def _uint2list(self,value,bytes_size = 2):
        if isinstance(value,int):
            value_list = []
            if value >= 0 and value < 65536 :             #16 bit 
                value_list.append(int(value/256))
                value_list.append(value%256)
            elif value > 65535 and value < 65536 * 65536 and bytes_size == 2: #32 bit 
                value_list.append(int(value/(65536 * 256)))
                value_list.append(int((value%(65536 * 256))/65536))
                value_list.append(int((value%(65536))/256))
                value_list.append((value%(256)))
            else :
                raise ValueError
            
            return value_list
        else:
            raise ValueError
    

class Receiver():
    def __init__(self) -> None:
        self._ble = bluetooth.BLE()
        self._ble.irq(self._self_irq)
        self.callback = None

    def active(self,active_flag = None):
        if active_flag == None:
            return self._ble.pyi_check_active()
        else:
            if (active_flag > 0 or active_flag == True):
                return self._ble.pyi_active(True)
            elif (active_flag == 0 or active_flag == False):
                return self._ble.pyi_active(False)

    def scan(self, duration_ms,interval_us=1280000, window_us=11250):
        return self._ble.gap_scan(duration_ms,interval_us,interval_us)

    def irq(self,func):
        self.callback = func

    def _self_irq(self,event_id,data):
        # 过滤
        if self.callback != None:
            if event_id == 5: #_IRQ_SCAN_RESULT
                # addr_type, addr, adv_type, rssi, adv_data = data
                addr = data[1]
                rssi = data[3]
                if self._is_ibeacon_packet(data[4]):
                    uuid,measured_power,Major,Minor,company_id = self._unpack_ibeacon(data[4])
                    self.callback(event_id,(addr,uuid,rssi,measured_power,Major,Minor,company_id))
        
            elif event_id == 6: #_IRQ_SCAN_DONE
                self.callback(event_id,data)

    def _unpack_ibeacon(self,value):
        company_id = bytes([value[6],value[5]])
        uuid = value[9:25]
        Major = value[25] * 256 + value[26]
        Minor = value[27] * 256 + value[28]
        measured_power = value[29] - 256
        return uuid,measured_power,Major,Minor,company_id

    # TODO:判断是不是iBeacon包的绝对正确条件未确定
    def _is_ibeacon_packet(self,data):
        if len(data) == 30 :
            if data[0] == 0x02 and data[1] == 0x01 and data[3] == 0x1A and data[4] == 0xFF \
                and data[7] == 0x02 and data[8] == 0x15 :
                return True
            else:
                return False
        else:
            return False
