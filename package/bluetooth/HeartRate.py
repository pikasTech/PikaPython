import bluetooth

FLAG_SRV_BSL     = 0x01  # 感器位置服务
FLAG_SRV_APPEND  = 0x02  # 其他附加服务

SENSOR_LOC_OTHER    = 0X00
SENSOR_LOC_CHEST    = 0X01
SENSOR_LOC_WRIST    = 0X02
SENSOR_LOC_FINGER   = 0X03
SENSOR_LOC_HAND     = 0X04
SENSOR_LOC_EAR_LOBE = 0X05
SENSOR_LOC_FOOT     = 0X06

class Sender():
    hr_uuid  = 0x180D
    hrm_uuid = 0x2A37
    bsl_uuid = 0x2A38 # body sensor location uuid
    hrm_handle = 20
    bsl_handle = 21
    sensor_locations = ["Other","Chest","Wrist","Finger","Hand","Ear Lobe","Foot"]

    def __init__(self):
        self._ble = bluetooth.BLE()
        self._ble.active(1)        
        self._ble.config(gap_name = "HeartRate")
        self._ble.config(gap_uuid = bluetooth.UUID(0x180d))
        self._ble.irq(self._self_irq)
        self._callback   = None
        
    # 激活
    def active(self,active_flag = None):
        return self._ble.active(active_flag)
            
    # 修改与查询参数
    def config(self,*param_name, **kv):
        try:
            self._ble._check_active()
        except:
            raise OSError

        # 查询参数
        if len(param_name) != 0:
            first_param = param_name[0]
            if first_param == "gap_name":
                return self._ble.config("gap_name")
            elif first_param == "sensor_location":
                return self.sensor_locations[int(self._ble.gatts_read(self.bsl_handle))]
            elif first_param == "hr":
                return int(self._ble.gatts_read(self.hrm_handle)[1])
            else:
                # TODO:未实现
                # return self._ble.config(first_param)
                pass
        
        # 修改参数
        if "gap_name" in kv:
            return self._ble.config(gap_name=kv["gap_name"])
        
        if "sensor_location" in kv:
            return self._ble.gatts_write(self.bsl_handle,kv["sensor_location"])
            
        if "hr" in kv:
            self.update_hr(kv["hr"])

        return 0
    
    # 注册服务
    def register_services(self,FLAG , services_append = None):
        HR_UUID = bluetooth.UUID(self.hr_uuid)
        HR_CHAR = (bluetooth.UUID(self.hrm_uuid), bluetooth.FLAG_NOTIFY,)

        if FLAG & FLAG_SRV_BSL != 0:
            BSL_CHAR = (bluetooth.UUID(self.bsl_uuid), bluetooth.FLAG_READ,)
            # BSL_CHAR = (bluetooth.UUID(self.bsl_uuid + 1), bluetooth.FLAG_READ,)
            HR_SERVICE = (HR_UUID, (HR_CHAR,BSL_CHAR),)
        else :
            HR_SERVICE = (HR_UUID, (HR_CHAR,),)

        services = [HR_SERVICE,]
        
        if FLAG & FLAG_SRV_APPEND != 0 and services_append != None:
            services_append = list(services_append)
            services += services_append

        return self._ble.gatts_register_services(tuple(services))
    
    # 广播
    def advertise(self, interval_us,adv_data=None,resp_data=None, connectable=True, adv_data_append=True):
        return self._ble.gap_advertise(interval_us,adv_data,resp_data, connectable,adv_data_append)

    # 设置回调函数
    def irq(self,func):
        self._callback = func

    # 默认回调函数
    def _self_irq(self,event_id,data):
        if event_id == 1:    # 连接
            if self._callback == None:
                print(data)
                pass
            else :
                self._callback(event_id,data)
        elif event_id == 2:  # 断开连接
            if self._callback == None:  # 默认继续扫描
                print("adv again")
                self._ble.gap_advertise(6250)
                # self._ble.gap_advertise(6250,adv_data=bytearray("adv"),resp_data=bytes([0x12,0x34,0x56]))
            else :
                self._callback(event_id,data)

        elif event_id == 4:  # 读请求
            if self._callback == None: # 默认可读
                return 0
            else :
                self._callback(event_id,data)
        elif event_id == 18: # 订阅通知
            if self._callback == None:
                pass
            else:
                self._callback(event_id,data)
        elif event_id == 27: # 连接参数更新        
            if self._callback == None:
                print("_IRQ_CONNECTION_UPDATE")
            else:
                self._callback(event_id,data)
        else :
            self._callback(event_id,data)
    
    # 更新心率
    def update_hr(self,hr):
        # TODO: 首字节默认使用0X06, 暂未弄清楚其他数字含义
        return self._ble.gatts_write(self.hrm_handle,[0x06,hr],True)
        

