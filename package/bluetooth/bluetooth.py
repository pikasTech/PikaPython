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

