import bluetooth

FLAG_SRV_ONLY    = 0x00  # 只有当前主要服务
FLAG_SRV_APPEND  = 0x01  # 其他附加服务

class Sender():
    BAS_uuid  = 0x180F
    BL_uuid   = 0x2A19
    BL_handle = 20

    def __init__(self):
        self._ble = bluetooth.BLE()
        self._ble.active(1)        
        self._ble.config(gap_name = "BatteryService")
        self._ble.config(gap_uuid = bluetooth.UUID(0x180F))
        self._ble.irq(self._self_irq)
        self._callback   = None
        
    # 激活
    def active(self,active_flag = None):
        return self._ble.active(active_flag)
            
    # 修改与查询参数
    def config(self,*param_name, **kv):
        # 查询参数
        if len(param_name) != 0:
            first_param = param_name[0]
            if first_param == "gap_name":
                return self._ble.config("gap_name")
            elif first_param == "bl":
                return int(self._ble.gatts_read(self.BL_handle))
            else:
                return self._ble.config(first_param)
        
        # 修改参数
        if "gap_name" in kv:
            return self._ble.config(gap_name=kv["gap_name"])
            
        if "bl" in kv:
            self.update_bl(kv["bl"])

        return 0
    
    # 注册服务
    def register_services(self,FLAG, services_append = None):
        BAS_UUID = bluetooth.UUID(self.BAS_uuid)
        BAS_CHAR = (bluetooth.UUID(self.BL_uuid), bluetooth.FLAG_NOTIFY|bluetooth.FLAG_READ,)
        BAS_SERVICE = (BAS_UUID, (BAS_CHAR,),)

        services = [BAS_SERVICE,]
        
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
            if self._callback == None:
                pass
            else:
                self._callback(event_id,data)
    
    # 更新电池
    def update_bl(self,bl):
        return self._ble.gatts_write(self.BL_handle,bl,True)
        

