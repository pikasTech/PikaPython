import _bluetooth
# 特性标志位
FLAG_BROADCAST = 0x0001
FLAG_READ      = 0x0002
FLAG_WRITE_NO_RESPONSE = 0x0004
FLAG_WRITE     = 0x0008
FLAG_NOTIFY    = 0x0010
FLAG_INDICATE  = 0x0020
FLAG_AUTHENTICATED_SIGNED_WRITE = 0x0040

# 描述符标志位
FLAG_DSC_READ           = 0x01
FLAG_DSC_WRITE          = 0x02
FLAG_DSC_READ_ENC       = 0x04
FLAG_DSC_READ_AUTHEN    = 0x08
FLAG_DSC_READ_AUTHOR    = 0x10
FLAG_DSC_WRITE_ENC      = 0x20
FLAG_DSC_WRITE_AUTHEN   = 0x40
FLAG_DSC_WRITE_AUTHOR   = 0x80

FLAG_AUX_WRITE           = 0x0100
FLAG_READ_ENCRYPTED      = 0x0200
FLAG_READ_AUTHENTICATED  = 0x0400
FLAG_READ_AUTHORIZED     = 0x0800
FLAG_WRITE_ENCRYPTED     = 0x1000
FLAG_WRITE_AUTHENTICATED = 0x2000
FLAG_WRITE_AUTHORIZED    = 0x4000


'''
创建具有指定值的 UUID 实例
该值可以是
- 一个 16 位整数. 例如`0x2908`
- 一个 32 位整数. 例如`0x29081234`
- 一个 128 位的 UUID 字符串。例如`'6E400001-B5A3-F393-E0A9-E50E24DCCA9E'`.
'''
class UUID():
    def __init__(self,value):
        # try:
        self.value,self._UUID_bits = self._UUID_to_bytes(value)
        # except:
            # print("ValueError: Invalid UUID")
            # raise ValueError
            

    def _UUID_to_bytes(self,value):
        value_bytes = []
        UUID_bits   = 0 
        # try:
        if isinstance(value,bytearray): 
            value_bytes = bytes(value.decode()) # 暂不支持bytes(bytearray())
            if len(value_bytes) > 16 :
                value_bytes = []
                # raise ValueError
                raise ValueError
        elif isinstance(value, bytes):
            value_bytes = value
        elif isinstance(value,str):
            # if len(value) == 36 and (value[8] == value[13] == value[18] == value[23] == '-'): #暂不支持连等
            if len(value) == 36 and (value[8] == '-') and (value[13] == '-') and (value[18] == '-') and  (value[23] == '-'):
                value_str = value.replace("-","")
                value_list = []
                if len(value_str) == 32:
                    for i in range(0, 32, 2):
                        value_list.append(int(value_str[i:i+2],16))
                    value_bytes = bytes(value_list)
                else :
                    raise ValueError
            else : 
                raise ValueError
        elif isinstance(value,int):  
            value_list = []
            if value >= 0 and value < 65536 :          #16 bit uuid
                value_list.append(int(value/256))
                value_list.append(value%256)
            elif value >= 0 and value < 65536 * 65536: #32 bit uuid
                value_list.append(int(value/(65536 * 256)))
                value_list.append(int((value%(65536 * 256))/65536))
                value_list.append(int((value%(65536))/256))
                value_list.append((value%(256)))
            else :  
                raise ValueError
            value_bytes = bytes(value_list)
        else:
            raise ValueError

        UUID_bits = len(value_bytes)  # bytes的数量
        return value_bytes,UUID_bits
            

class BLE(_bluetooth.BLE):
    def __init__(self):
        print("BLE init")
        # a = super().__init__()
        self._last_adv_data   = ""  #广播内容
        self._last_resp_data  = ""  #回应扫描内容
        self._addr_mode       =  0  #地址类型 BLE_OWN_ADDR_PUBLIC,BLE_OWN_ADDR_RANDOM,BLE_OWN_ADDR_RPA_PUBLIC_DEFAULT,BLE_OWN_ADDR_RPA_RANDOM_DEFAULT
        self._callback_func   = None  #回调函数

        self._basic_value_handle = 20
        self._connectable = True
        self._py2c_dict    = {}
        self._c2py_dict    = {}
        self._c2value_dict = {}
        self._append_adv_data = True
        a = self.init()
        self.setCallback(self._callback)

    # 测试函数
    def test(self):
        if self.pyi_check_active() == False:
            raise OSError
    
    def test2(self):
        self.test()
        # return self.pyi_test2()s

    def test3(self,connhandle,valuehandle):
        return self.pyi_test3(connhandle,valuehandle)
    
    def test4(self,data):
        pass
    
    def test_call_some_name(self):
        super().test_call_some_name()  

    # 检查蓝牙活跃性
    def _check_active(self):
        if self.pyi_check_active() == False:
            raise OSError
        
    '''
    可选择更改 BLE 无线电的活动状态，并返回当前状态。
    Args:
        - active(bool): 为 1 则开启BLE, 否则关闭BLE; 为空则查询当前BLE状态 
    
    Returns:
        - bool:设置状态时，返回操作成功与否； 查询状态时, 返回状态, True为活跃

    '''
    def active(self,active_flag = None ):
        if active_flag == None:
            return self.pyi_check_active()
        else:
            if (active_flag > 0 or active_flag == True):
                return self.pyi_active(True)
            elif (active_flag == 0 or active_flag == False):
                return self.pyi_active(False)

    '''
    获取或设置 BLE 接口的配置值。
    
    要获得一个值，参数名称应该被引用为一个字符串，并且一次只查询一个参数。

    如 `config("gap_name")`
    
    要设置值，请使用关键字语法，并且一次可以设置一个或多个参数。

    如 `config(gap_name="nimble")`

    当前支持的值是:

    - `'mac'`:

        TODO:需要支持设置任意值嘛？有点困难
    
    - `'addr_mode'`:

    - `'gap_name'`:
    
    - `'rxbuf'`:

        TODO:暂未支持, 没找到对应函数

    - `'mtu'`:

        TODO:不明意义

    - `'bond'`:

        TODO: 目前提供的接口不支持

    - `'mitm'`:

        TODO: 目前提供的接口不支持

    - `'io'`:

        TODO:未找到对应函数

    - `'lesure'`:

        TODO:只支持设置

    '''
    def config(self, *param_name, **kv): # a.config(mac="1123",gap_name="test")
    # 获取参数属性   a.config("mac")
        try:
            self._check_active()
        except:
            raise OSError
        
        if len(param_name) != 0 :
            first_param = param_name[0]
            if first_param == "mac":
                return (self.config_addr_mode_get(),self.config_mac_get())
            elif first_param == "addr_mode":
                return self.config_addr_mode_get()
            elif first_param == "gap_name":
                return self.config_gap_name_get()
            # elif first_param == "rxbuf" :
            #     self.config_addr_rxbuf_get()
            # elif first_param == "mtu" :
            #     self.config_mtu_get()
            # elif first_param == "bond" :
            #     self.config_bond_get()
            # elif first_param == "mitm" :
            #     self.config_mitm_get()
            # elif first_param == "io":
            #     self.config_io_get()
            # elif first_param == "le_secure":
            #     self.config_le_secure_get()
            elif first_param == "gap_uuid":
                return self.config_gap_uuid_get()
            else:
                print("ValueError: unknown config param")   

    # 设置参数
        if "mac" in kv:
            self.config_mac_update(kv["mac"])

        if ("addr_mode" in kv):
            self.config_addr_mode_update(kv["_addr_mode"])

        if ("gap_name" in kv):
            self.config_gap_name_update(kv["gap_name"])

        # if ("rxbuf" in kv):
        #     return self.config_rxbuf_update(kv["rxbuf"])

        # if ("mtu" in kv):
        #     return self.config_mtu_update(kv["mtu"])

        # if ("bond" in kv):
        #     return self.config_bond_update(kv["bond"])

        # if ("mitm" in kv):
        #     return self.config_mitm_update(kv["mitm"])

        # if ("bond" in kv):
        #     return self.config_mac_update(kv["bond"])

        # if ("io" in kv):
        #     return self.config_io_update(kv["io"])

        # if ("le_secire" in kv):
        #     return self.config_le_secire_update(kv["le_secire"])
        
        if ("gap_uuid" in kv):
            uuid = kv["gap_uuid"]
            if isinstance(uuid,UUID):
                self.config_gap_uuid_update(uuid.value,uuid._UUID_bits)

    '''
    为来自 BLE 堆栈的事件注册回调。
    
    处理程序采用两个参数,event(在下面的代码中的一个) 和 data(其是值的特定事件元组)。

    '''
    # 回调事件处理函数
    def irq(self,func):
        self._callback_func = func
        return 0
    
    def _callback(self,data):
        event_id = data[0]
        # print("_callback call")
        # print("event_id",event_id)
        # print("data  ",data[1])
        if self._callback_func != None:
            if event_id > 100 :      #自定义回调事件
                if event_id == 101 : # 建立句柄映射 
                    ble_value_handles = data[1]
                    length = len(ble_value_handles)
                    for i in range(length):
                        key =  self._basic_value_handle + i
                        value = ble_value_handles[i] 
                        # py 映射 c handle
                        self._py2c_dict[str(key)] = value 
                        # c 映射 py handle
                        self._c2py_dict[str(value)] = key
                        # c handle 映射 value, 默认值为空
                        self._c2value_dict[str(value)] = bytes("")
                elif event_id == 102:       #nimble蓝牙协议栈读属性 
                    buf = self._c2value_dict[str(data[1])]
                    return len(buf),buf
            else: 
                if event_id == 3: # write
                    self._c2_change_value(data[2], data[3])
                    data = data[:3]
                elif event_id == 4: # read 请求
                    rc = self._callback_func(event_id,data[1:])
                    value    = -99
                    length   = -99
                    if rc == 0:   #允许读
                        value = self._c2value(data[2]) 
                        length = len(value)
                    return rc,length,value
                else:
                    return self._callback_func(event_id,data[1:])

    '''
    以指定的时间间隔(以微秒为单位)开始广播。

    adv_data和resp_data可以是任何实现缓冲协议的类型(例如bytes, bytearray, str)。adv_data包含在所有广播中,并且resp_data被发送以响应主动扫描。

    注意:如果adv_data(或resp_data)是None, 则传递给前一个调用的数据 gap_advertise将被重新使用。
    
    要清除广告有效负载, 请传递一个空值bytes, 即b''

    Args: 

        - interval_us(int):广播时间间隔, 单位us. 此间隔将向下舍入到最接近的 625us的倍数。要停止广告, 请将interval_us设置 为 None。

        - adv_data(bytes, bytearray, str): 自定义广播数据(添加到默认广播数据后),默认值为none
        
        TODO:1.append模式只能是0XFF数据; 归零模式全部自定义输入

        - resp_data(bytes, bytearray, str): 扫描响应数据,默认值为none

        - connectable(bool): 是否可连接, True 为可连接.

        - adv_data_append: 是否是增加数据

        TODO:存在问题
    '''
    def gap_advertise(self, interval_us,adv_data=None,resp_data=None, connectable=None, adv_data_append=None):
        try:
            self._check_active()
        except:
            raise OSError
        
        if connectable !=  None:
            self._connectable = connectable
        
        if interval_us is None: 
            return self.stop_advertise()
        else:
            # 设置广播载荷
            if adv_data is None: #参数为空，则使用上次数据广播数据
                adv_data = self._last_adv_data
            else :
                if adv_data_append != None: # 设置是否增加数据
                    self._append_adv_data = adv_data_append

                if self._append_adv_data == True: # 新增数据
                    self._last_adv_data = _to_bytes(adv_data)
                else: # 覆盖数据
                    empty_list = []
                    for i in range(len(adv_data)):
                        empty_list.append(len(adv_data[i]))
                        empty_list += adv_data[i]
                    # print(empty_list)
                    self._last_adv_data = _to_bytes(empty_list)

            # 设置响应载荷
            if resp_data is None:
                resp_data = self._last_resp_data
            else :
                self._last_resp_data = _to_bytes(resp_data)
        # self._adv_data_append = False
        # print("_addr_mode : ",self._addr_mode)
        # print("int(interval_us/625) : ",int(interval_us/625))
        # print("self._connectable: ",self._connectable)
        # print("self._last_adv_data : ",self._last_adv_data)
        # print("len(self._last_adv_data) : ",len(self._last_adv_data))
        # print("adv_data_append : ",self._append_adv_data)
        # print("self._b",self._b)
        # print("self._last_resp_data : ",self._last_resp_data)
        # print("len(self._last_resp_data) : ",len(self._last_resp_data))
        return self.advertise(self._addr_mode,int(interval_us/625),self._connectable,self._last_adv_data,
                            len(self._last_adv_data),self._append_adv_data,self._last_resp_data,len(self._last_resp_data))

        
    '''
    运行持续指定持续时间(以毫秒为单位)的扫描操作。
    
    要无限期扫描, 请将duration_ms设置为0;要停止扫描,请将duration_ms设置为 None.

    使用interval_us和window_us可选择配置占空比。扫描器将每interval_us微秒运行window_us 微秒,总共持续duration_ms毫秒。
    
    默认间隔和窗口分别为 1.28 秒和 11.25 毫秒(后台扫描).

    对于每个扫描结果, _IRQ_SCAN_RESULT将引发事件, 并带有事件数据。`(addr_type, addr, adv_type, rssi, adv_data)`

    Args:

        - duration_ms(int):扫描持续时间。要无限期扫描, 请将duration_ms设置为0;要停止扫描,请将duration_ms设置为 None.

        - interval_us(int): 扫描间间隔时间。默认值为1280000

        - window_us(int): 扫描窗口时间。默认值为11250

        - active(bool): 是否接受扫描响应。默认值False(不接受)

    Return:
        
        - 操作成功返回 0; 否则返回错误代码

    events: 

        - 单个扫描结果触发 _IRQ_SCAN_RESULT

        - 扫描结束触发    _IRQ_SCAN_DONE
    
    '''
    def gap_scan(self, duration_ms, interval_us=1280000, window_us=11250, active=False):
        try:
            self._check_active()
        except:
            raise OSError
        
        self._check_active()
        if duration_ms is None :
            return self.gap_stop_disc()
        else:
            return self.gap_disc(self._addr_mode, duration_ms,int(interval_us/625),int(window_us/625),active)

    '''
    central 设备连接 peripherals 设备
    Args:

        - peer_addr(list,bytes,bytearray): 需要连接的设备的mac地址

        - peer_addr_type(int): 连接设备的地址类型

            - 0x00 - PUBLIC - 使用控制器的公共地址。

            - 0x01 - RANDOM - 使用生成的静态地址。

            - 0x02 - RPA - 使用可解析的私有地址。

            - 0x03 - NRPA - 使用不可解析的私有地址。

    Return:

        - (int): 操作成功返回 0; 否则返回错误代码

    events:

        - 连接成功触发 CENTRAL设备:_IRQ_PERIPHERAL_CONNECT; PERIPHERAL设备:_IRQ_CENTRAL_CONNECT
    '''
    def gap_connect(self,peer_addr,peer_addr_type, scan_duration_ms=2000):
        try:
            self._check_active()
        except:
            raise OSError
        self._check_active()
        peer_addr_bytes = _to_bytes(peer_addr,6)
        return self.pyi_gap_connect(peer_addr_bytes,peer_addr_type ,scan_duration_ms)
            

    '''
    Args:

        - conn_handle(int): 连接句柄

    Return:

        - (int): 操作成功返回 0; 否则返回错误代码

    events:

        - 断连成功触发 CENTRAL设备:_IRQ_PERIPHERAL_DISCONNECT; PERIPHERAL设备:_IRQ_CENTRAL_DISCONNECT
    '''
    def gap_disconnect(self, conn_handle):
        try:
            self._check_active()
        except:
            raise OSError
        self._check_active()
        return self.pyi_gap_disconnect(conn_handle)

    
    '''
    使用指定的服务配置服务器，替换任何现有服务。

    Args:

        - services(嵌套多层tuple):

    Return:

        - (int): 操作成功返回 0; 否则返回错误代码

    '''
    def gatts_register_services(self, services):
        try:
            self._check_active()
        except:
            raise OSError
        
        self._check_active()
        convert_services = _convert_ble_service_info(services)
        offset = 0
        chr_list = _count_chrs(services) #计算每个服务的特性数量
        all_chr_count = 0
        py_handles = []

        # 计算py handle返回值
        # 计算chr总数量
        for i in range(len(chr_list)):
            py_handles_one_service = []
            all_chr_count += chr_list[i]
            for j in range(chr_list[i]):
                value_handle = self._basic_value_handle + offset
                py_handles_one_service.append(value_handle)
                offset += 1
            py_handles.append(py_handles_one_service)

        rc = self.gatts_register_svcs(convert_services,all_chr_count)
        
        if rc != 0 :
            return rc
        return tuple(py_handles) 

    
    '''
    Args:

        - value_handle(int): 蓝牙特性句柄

    Return:

        - (int): 操作成功返回 0; 否则返回错误代码

    '''
    def gatts_read(self,value_handle):
        try:
            self._check_active()
        except:
            raise OSError
        
        return self._py2value(value_handle)

    
    '''
    Args:

        - value_handle(int): 蓝牙特性句柄

        - data(str,int,bytes,list(元素为小于256大于0的整数)): 修改的数据

        - send_update(bool):是否通知客户端, False为不通知, 默认为False

    Return:

        - (int): 操作成功返回 0; 否则返回错误代码

    events:

        - send_update 为 True 时, 触发tx事件
    '''
    def gatts_write(self,value_handle, data, send_update=False):
        try:
            self._check_active()
        except:
            raise OSError
        
        print("send_update=", send_update)
        if send_update == False:
            return self._py2_change_value(value_handle,data)
        else : 
            rc = self._py2_change_value(value_handle,data)
            if rc != 0:
                return rc
            return self.gatts_chr_updated(self._py2c_dict[str(value_handle)])

        
    
    '''
    向连接的客户端发送通知请求。

    Args:

        - conn_handle(int): 连接句柄 
    
        - value_handle(int): 蓝牙特性句柄

        - data(str,int,bytes,list(元素为小于256大于0的整数)): 发送的数据, 默认值为None
        
        如果data不是None, 则该值将作为通知的一部分发送给客户端。本地值不会被修改。否则, 如果data是None, 则将发送当前本地值

    Return:

        - (int): 操作成功返回 0; 否则返回错误代码

    events:

        -
    '''
    def gatts_notify(self,conn_handle, value_handle, data=None):
        try:
            self._check_active()
        except:
            raise OSError
        
        self._check_active()
        value = ""
        if data is None:
            value = self._py2value(value_handle)
            c_value_handle = self._py2c_dict[str(value_handle)]
        else :
            if isinstance(data,bytes): #TODO:关注一下数据类型
                value = data
            elif isinstance(data,int):
                value = bytes([data])
            else:
                value = bytes(data)
        c_value_handle = self._py2c_dict[str(value_handle)]
        self.pyi_gatts_notify(conn_handle, c_value_handle,value,len(value))


    
    '''
    Args:

        -

    Return:

        - 操作是否成功

    events:

        -
    '''
    def gatts_indicate(self,conn_handle, value_handle,data=None):
        try:
            self._check_active()
        except:
            raise OSError
        
        value = ""
        if data is None:
            value = self._py2value(value_handle)
            c_value_handle = self._py2c_dict[str(value_handle)]
        else :
            if isinstance(data,bytes): 
                value = data
            elif isinstance(data,int):
                value = bytes([data])
            else:
                value = bytes(data)
        c_value_handle = self._py2c_dict[str(value_handle)]
        self.pyi_gatts_indicate(conn_handle, c_value_handle,value,len(value))

    
    '''
    Args:

        -

    Return:

        - 操作是否成功

    events:

        -
    '''
    def gatts_set_buffer(self,value_handle, len, append=False):
        try:
            self._check_active()
        except:
            raise OSError
        # TODO:暂不清楚对照哪个函数
        pass
    
    '''
    打印全部本地服务
    '''
    def gatts_show_local(self):
        try:
            self._check_active()
        except:
            raise OSError
        return self.pyi_gatts_show_local()
    
    '''
    Args:

        -

    Return:

        - 操作是否成功

    events:

        -
    '''
    def gattc_discover_services(self,conn_handle, uuid:UUID=None):
        try:
            self._check_active()
        except:
            raise OSError
        
        if uuid == None:
            return self.gattc_dis_svcs(conn_handle)
        else :
            return self.gattc_dis_svcs_by_uuid(conn_handle,uuid.value,len(uuid.value))

        '''
    Args:

        -

    Return:

        - 操作是否成功

    events:

        -
    '''
    def gattc_discover_characteristics(self,conn_handle, start_handle, end_handle, uuid:UUID=None):
        try:
            self._check_active()
        except:
            raise OSError
        
        if uuid == None:
            return self.gattc_dis_chrs(conn_handle,start_handle,end_handle)
        else :
            return self.gattc_dis_chrs_by_uuid(conn_handle, start_handle, end_handle,uuid.value,len(uuid.value))

    '''
    Args:

        -

    Return:

        - 操作是否成功

    events:

        -
    '''
    def gattc_discover_descriptors(self,conn_handle, start_handle, end_handle):
        try:
            self._check_active()
        except:
            raise OSError
        return self.gattc_dis_dscs(conn_handle,start_handle, end_handle)

    '''
    Args:

        -

    Return:

        - 操作是否成功

    events:

        -
    '''
    def gattc_read(self,conn_handle, value_handle):
        try:
            self._check_active()
        except:
            raise OSError
        return self.pyi_gattc_read(conn_handle, value_handle)

    '''
    Args:

        -

    Return:

        - 操作是否成功

    注意:单次传输最多95个byte

    events:

        -
    '''
    def gattc_write(self,conn_handle, value_handle, data, mode = 0):
        try:
            self._check_active()
        except:
            raise OSError
        
        if mode == 0:
            return self.gattc_write_with_no_rsp(conn_handle, value_handle, data, len(data))
        elif mode == 1:
            return self.gattc_write_with_rsp(conn_handle, value_handle, data,len(data))

    '''
    Args:

        -

    Return:

        - 操作是否成功

    events:

        -
    '''
    def gattc_exchange_mtu(self,conn_handle):
        try:
            self._check_active()
        except:
            raise OSError
        return self.pyi_gattc_exchange_mtu(conn_handle)

    def gattc_subscribe(self,conn_handle,value_handle,subscribe = 0):
        try:
            self._check_active()
        except:
            raise OSError
        return self.pyi_gattc_subscribe(conn_handle,value_handle,subscribe)

    # 通过C handle找值 
    def _c2value(self, handle):
        return self._c2value_dict[str(handle)]
    
    # 通过PY handle找值
    def _py2value(self,handle):
        c_handlue = self._py2c_dict[str(handle)]
        return self._c2value(c_handlue)
        # return self._c2value(25)
    
    # 通过C handle 改值, value均为bytes类型
    def _c2_change_value(self,handle,value):
        value_bytes = 12
        # self._c2value_dict[str(handle)] = value_bytes
        self._c2value_dict[str(handle)] = value
        return 0

    # 通过py handle 改值    
    def _py2_change_value(self,handle,value):
        if len(self._py2c_dict) == 0:
            return -1
        c_handlue = self._py2c_dict[str(handle)]
        value_bytes =  _to_bytes(value)
        return self._c2_change_value(c_handlue,value_bytes)
        
# 将UUID类型转换为字符串
def _convert_ble_service_info(data):
    new_list = []
    for i in data :
        if isinstance(i,UUID) :
            new_list.append(i.value)
            new_list.append(i._UUID_bits)
            # print(i.value)
        elif isinstance(i, tuple):
            new_list.append(_convert_ble_service_info(i))
        else:
            new_list.append(i)
            # print(i)
    return tuple(new_list)

def _count_chrs(srvs_tuple):
    srv_count = len(srvs_tuple)
    chr_count = [] # 每个服务的特性数量
    for i in range(srv_count):
        if len(srvs_tuple[i]) > 1 :
            chr_count.append(len(srvs_tuple[i][1]))
        else :
            chr_count.append(0)
    return chr_count


def _to_bytes(data,size=None):
    data_bytes = []
    if isinstance(data,bytes):
        data_bytes = data
    elif isinstance(data,bytearray):
        data_bytes = bytes(data.decode())
    elif isinstance(data,str):
        data_bytes = bytes(data)
    elif isinstance(data,list):
        data_bytes = bytes(data)
    elif isinstance(data,int):
        data_list = []
        if data == 0:
            data_list.append(0)
        else :
            while data > 0:
                remainder = data % 256
                data_list.insert(0, remainder)
                data //= 256
        data_bytes = bytes(data_list)
    else:
        raise ValueError
    
    if size != None and len(data_bytes) != size:
        raise ValueError
    
    return data_bytes


    