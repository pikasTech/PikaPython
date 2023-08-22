import network

wlan = network.WLAN(network.STA_IF)
wlan.active(True)
wlan.connect('testssid', 'testpassword')
wlan.config(ssid='configssid', channel=11)
print(wlan.config('ssid'), wlan.config('channel'))
wlan.close()
