import bluetooth

a = bluetooth.BLE()

c = bluetooth.UUID(0x291a)
c = bluetooth.UUID(0x291a123)
c = bluetooth.UUID('6E400001-B5A3-F393-E0A9-E50E24DCCA9E')
c = bluetooth.UUID(bytearray('6E400001B5A3F393E0A9E50E24DCCA9E'))

c = bluetooth.UUID('6E400001-B5A3-F393-E0A9-E50E24DCCA9')
c = bluetooth.UUID('6E400001-B5A3-F393-E0A9--E50E24DCCA9')
c = bluetooth.UUID(0x29199999)
value = '6E400001-B5A3-F393-E0A9-E50E24DCCA9E'
value = '6E400001-B5A3-F393-E0A9-E50E24DCCA9'
value = '6E400001-B5A3-F393-E0A9--50E24DCCA9'
len(value) == 36 and (value[8] == '-') and (value[13] == '-') and (value[18] == '-') and  (value[23] == '-')
c.value


# 广播
# 输入格式判断
c = a.gap_advertise(6250,"adv_test","rsp_test")
c = a.gap_advertise(6250,"adv","rsp")
c = a.gap_advertise(6250,"adv","rsp")
c = a.gap_advertise(6250,bytes("adv"),bytes("rsp"))
c = a.gap_advertise(6250,bytearray("adv"),bytearray("rsp"))
c = a.gap_advertise(6250,bytearray("adv"),bytes([0x12,0x34,0x56]))

# 暂停广播
a.gap_advertise(None)

# 测间隔
a.gap_advertise(625 * 400 ,"adv_test","rsp_test")
a.gap_advertise(625 * 10 ,"adv_test","rsp_test")
a.gap_advertise(625 * 32,"adv_test","rsp_test")
a.gap_advertise(625 * 40 ,"adv_test","rsp_test")
a.gap_advertise(0 ,"adv_test","rsp_test",False)
a.gap_advertise(0)

# 测连接
a.gap_advertise(0 ,"adv_test","rsp_test",False)

# 多次连接广播
a.gap_advertise(0)
a.gap_advertise(625 * 400 ,"adv_test","rsp_test")


#测扫描
a.gap_scan(10000,320000,active=True)
a.gap_scan(10000,320000,active=False)

# 无限扫描
a.gap_scan(0,320000,active=True)
a.gap_scan(0,320000,active=False)

# 停止扫描
a.gap_scan(None)

# 多次扫描
a.gap_scan(10000,320000,active=True)
a.gap_scan(10000,320000,active=False)

# 测connect
a.gap_scan(1000,320000,active=False)
addr = bytes([0xec,0xda,0x3b,0x67,0x7a,0x82])  # new eps32 s3
a.gap_connect(addr,0)
addr = bytes([0xec,0xda,0x3b,0x67,0x7a,0x82])  # new eps32 s3
addr = [0xec,0xda,0x3b,0x67,0x7a]  # new eps32 s3

#测disconnect
a.gap_disconnect(1)

# 测发现服务
a.gattc_discover_services(1)

a.gattc_discover_services(1,bluetooth.UUID(0x1800))
a.gattc_discover_services(1,bluetooth.UUID(b'\x6e\x40\x00\x01\xb5\xa3\xf3\x93\xe0\xa9\xe5\x0e\x24\xdc\xca\x9e'))

a.gattc_discover_services(0)
a.gattc_discover_services(1,bluetooth.UUID(0x1803))

#发现属性
a.gattc_discover_characteristics(1,0x01,0xff,bluetooth.UUID(0x2A37))
a.gattc_discover_characteristics(1,29,0xff,bluetooth.UUID(0x2A37))

a.gap_connect(addr,0)

#订阅 notify
a.gattc_write(1,32,bytes([0x01,0x00]))
a.gattc_write(1,32,b'\x00\x01')
a.gattc_write(1,32,b'\x01\x00')
# b'\x01\x00'
a.gattc_write(1,32,bytes([0x00,0x01]))
a.gattc_write(1,32,bytes([0x00,0x00]))
a.gattc_read(1,32)
a.gattc_read(3,32)

#订阅 indicate
a.gattc_write(1,38,bytes([0x02,0x00]))
a.gattc_write(1,38,bytes([0x00,0x09]))
a.gattc_write(0,38,b'\x00\x01')
a.gattc_write(0,38,b'\x00\x00')
a.gattc_write(1,38,bytes([0x00,0x00]))

a.gattc_read(1,38)
a.gattc_read(0,38)

#发现描述符
a.gattc_discover_descriptors(1,0x01,0xff)
a.gattc_discover_descriptors(1,29,0xff)

# a._c2py_dict
# a._py2c_dict
# a._c2value_dict
# notify
a.gatts_read(21)
a.gatts_write(21,bytes([0x43,0x43]))
a.gatts_notify(1,21)
a.gatts_notify(1,21,"test")

len(a.gatts_read(22))

# gattc write
a.gatts_read(22)

a.test2()
a.test3(1,1)

a.gap_connect(addr,0)

a.gattc_subscribe(1,26,1)
a.gattc_subscribe(1,26,0)
a.gattc_subscribe(1,26,2)
a.gattc_subscribe(1,26)

a.gattc_read(1,27)


a.gattc_subscribe_indicate(1,38)
a.gattc_subscribe_indicate(1,38,False)

a.gattc_subscribe_notify(1,33,False)


a.test3(1,1)

a.active(1)