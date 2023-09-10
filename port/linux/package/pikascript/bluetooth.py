import _bluetooth
# 需要额外注意类型转换

class BLE(_bluetooth.BLE):
    # last_adv_data  = ""  #广播内容
    # last_resp_data = ""  #回应扫描内容
    # addr_mode = ""       #地址类型
    # def __init__(self):
        # super().__init__()
        # pass

    def test(self):
        pass
    
    def active(self):
        # _bluetooth.BLE.active(self)
        # a = _bluetooth.BLE()
        # a.active()
        _ = super().active()
        # self.active()
        return print("bluetooth active")
    
    # def config(self,param_name, /):
    #     if(param_name=="mac"):
    #         pass
    #     elif(param_name=="addr_mode"):
    #         pass
    #     elif(param_name=="gap_name"):
    #         pass
    #     elif(param_name=="rxbuf"):
    #         pass
    #     elif(param_name=="mtu"):
    #         pass
    #     elif(param_name=="bond"):
    #         pass
    #     elif(param_name=="mitm"):
    #         pass
    #     elif(param_name=="io"):
    #         pass
    #     elif(param_name=="le_secire"):
    #         pass
    #     else:
    #         print("there is not params")

    # def config(self,*, mac,addr_mode,gap_name,rxbuf,mtu,bond,mitm,io,le_secire=False):
    #     if gap_name != None:
    #         super.config_name_update(gap_name)
    
    # # TODO:如何进行事件处理函数
    # def irq():
    #     pass

    # """
    # interval_us:广告间隔时间, 为none则停止广播
    # adv_data: 包含在所有广播中, 是任何实现缓冲协议的类型(例如bytes, bytearray, str)
    # resp_data: 被发送以响应主动扫描, 是任何实现缓冲协议的类型(例如bytes, bytearray, str)

    # """
    # def gap_advertise(self, interval_us, adv_data=None, *, resp_data=None, connectable=True):
    #     # 停止广播
    #     if interval_us is None: 
    #         return super.stop_advertise()

    #     if adv_data is None:
    #         adv_data = self.last_adv_data
    #     else :
    #         self.last_adv_data = adv_data

    #     if resp_data is None:
    #         resp_data = self.last_resp_data
    #     else :
    #         self.last_resp_data = resp_data

        
    # #TODO:active的作用是什么意思
    # """
    # 使用interval_us和window_us可选择配置占空比。扫描器将每interval_us微秒运行window_us 微秒，
    # 总共持续duration_ms毫秒。默认间隔和窗口分别为 1.28 秒和 11.25 毫秒（后台扫描）。
    # TODO:无法完全实现和micropython相同API
    # """
    # # def gap_scan(self, duration_ms, interval_us=1280000, window_us=11250, active=False, /):
    # #     if duration_ms is None :
    # #         super.gap_stop_scan()
    # #     elif (duration_ms == 0):
    # #         super.gap_scan_forever()
    # #     else:
    # #         super.gap_scan_forever()

    # """
    # micropython:直接输入时间

    # nimble: 按照单位算时间
    # duration:  

    # """
    # def gap_scan(self, duration_ms, interval_us=1280000, window_us=11250, active=False, /):
    #     if duration_ms is None :
    #         super.gap_stop_scan()
    #     else:
    #         duration = duration_ms / 10
    #         super.gap_scan(duration_ms, interval_us, window_us, active)

    # def gap_connect(self, addr_type, addr, scan_duration_ms=2000, /):
    #     super.gap_connect(addr_type, addr,scan_duration_ms)
    #     pass

    # def gap_disconnect(self, conn_handle, /):
    #     super.gap_disconnect()
    #     pass

    # def gatts_register_services(self, services_definition, /):
    #     for service in services_definition:
    #         service_uuid, characteristics = service
    
    # print(f"Service UUID: {service_uuid}")
    
    # # 遍历特征
    # for characteristic in characteristics:
    #     char_uuid, flags = characteristic
    #     print(f"Characteristic UUID: {char_uuid}")
    #     print(f"Flags: {flags}")
    #     super.register_a_service()


    # def gatts_read(value_handle, /):
    #     pass

    # def gatts_write(value_handle, data, send_update=False, /):
    #     pass

    # def gatts_notify(conn_handle, value_handle, data=None, /):
    #     pass

    # def gatts_indicate(conn_handle, value_handle, /):
    #     pass

    # def gatts_set_buffer(value_handle, len, append=False, /):
    #     pass

    # def gattc_discover_services(conn_handle, uuid=None, /):
    #     pass

    # def gattc_discover_characteristics(conn_handle, start_handle, end_handle, uuid=None, /):
    #     pass

    # def gattc_discover_descriptors(conn_handle, start_handle, end_handle, /):
    #     pass

    # def gattc_read(conn_handle, value_handle, /):
    #     pass

    # def gattc_write(conn_handle, value_handle, data, mode=0, /):
    #     pass

    # def gattc_exchange_mtu(conn_handle, /):
    #     pass

