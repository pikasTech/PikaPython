import network

wlan = network.WLAN(network.STA_IF)
wlan.active(True)
wlan.connect('testssid', 'testpassword')
wlan.isconnected()
wlan.ifconfig()
wlan.close()
