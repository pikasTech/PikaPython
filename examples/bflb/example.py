import machine
import time
import PikaStdDevice as std
import bflb
import lvgl as lv
import network
import requests

def led():
    """
    led 例程
    led0、led1、led2 依次闪烁 10 次
    led0 sipeed m0p 的板载 led
    led1 是 verimake bl618 竞赛板的板载 led
    """
    led0 = machine.Led(0)
    led1 = machine.Led(1)
    for i in range(10):
        print('Blinking (%d/10)' % (i + 1))
        led0.on()
        led1.off()
        time.sleep(0.1)
        led0.off()
        led1.on()
        time.sleep(0.1)
        led0.off()
        led1.off()
    led0.off()
    led1.off()
    led0.close()
    led1.close()


def key():
    """
    key 例程
    等待 key0 被按下 3 次, 使用阻塞方式
    key0 是 verimake bl618 竞赛板的板载按键
    如果不是竞赛板，可以自行连接 P33 引脚
    """
    key0 = machine.Key(0)
    print('Waiting for key0 to be pressed')
    count_expect = 3
    count = 0
    while True:
        if key0.isPressed():
            print('Key0 is pressed (%d/%d)' % (count + 1, count_expect))
            count += 1
            time.sleep(0.3)
            if count == count_expect:
                break
        time.sleep(0.1)
    key0.close()


count = 0
count_expect = 3


def _handle_key0(siganl):
    """
    key 回调例程的回调函数
    """
    global count
    print('Key0 is pressed (%d/%d)' % (count + 1, count_expect))
    count += 1

key_cb = None
def key_callback():
    """
    key 回调例程
    等待 key0 被按下 3 次, 使用回调方式
    key0 是 verimake bl618 竞赛板的板载按键
    如果不是竞赛板，可以自行连接 P33 引脚
    """
    global count, key_cb
    count = 0
    key_cb = machine.Key(0)
    print('Waiting for key0 to be pressed')
    key_cb.setCallBack(_handle_key0, std.GPIO.SIGNAL_FALLING)
    while True:
        if count > count_expect:
            break
        time.sleep(0.1)


def adc():
    """
    adc 例程
    读取 adc0 的值 10 次
    adc 接在 P0 引脚
    """
    adc0 = machine.ADC()
    adc0.setPin('P0')
    adc0.enable()
    for i in range(10):
        print('ADC0: %f (%d/%d)' % (adc0.read(), i, 10))
        time.sleep(0.5)


def dac():
    """
    dac 例程
    输出 0.5V 的方波 10 次
    dac 接在 P3 引脚
    adc 接在 P0 引脚, 实验时可以用杜邦线将 P3 和 P0 连接起来
    """
    dac0 = machine.DAC()
    dac0.setPin('P3')
    dac0.enable()
    adc0 = machine.ADC()
    adc0.setPin('P0')
    adc0.enable()
    val_max = 1.5
    val_min = 1
    for i in range(10):
        dac0.write(val_max)
        print('DAC0: %r, ADC0: %r (%d/%d)' % (val_max, adc0.read(), i+1, 10))
        time.sleep(0.5)
        dac0.write(val_min)
        print('DAC0: %r, ADC0: %r (%d/%d)' % (val_min, adc0.read(), i+1, 10))
        time.sleep(0.5)
    dac0.disable()
    adc0.disable()

def uart():
    """
    uart 例程
    通过 uart1 进行自发自收实验，你可以将 P23 和 P24 用杜邦线连接起来
    """
    uart1 = machine.UART()
    uart1.setId(1)
    uart1.setPinTX('P23')
    uart1.setPinRX('P24')
    uart1.setBaudRate(115200)
    uart1.enable()
    print('Waiting for data from uart1')
    for i in range(10):
        uart1.write('Hello!')
        data = uart1.read(256)
        time.sleep(0.1)
        if data:
            print('Received: %r' % data)
        time.sleep(0.1)

def _uart_receive_callback(signal):
    """
    UART 回调例程的回调函数
    """
    global uart_rx_cnt
    data = uart1.read(256)
    print('Received:', data, '(%d/%d)' % (uart_rx_cnt + 1, 3))
    uart_rx_cnt += 1

def uart_callback():
    """
    uart 回调例程
    通过 uart1 与电脑通信, 使用回调方式接收数据
    电脑需要连接到 uart1 的 TX 和 RX 引脚，波特率为 115200
    例程开始后，你需要在电脑上输出三行数据
    TX引脚是 P23
    RX引脚是 P24
    """
    global uart1
    global uart_rx_cnt
    uart_rx_cnt = 0
    uart1 = machine.UART()
    uart1.setId(1)
    uart1.setPinTX('P23')
    uart1.setPinRX('P24')
    uart1.setBaudRate(115200)
    uart1.enable()
    uart1.setCallBack(_uart_receive_callback, uart1.SIGNAL_RX)
    print('Waiting for data from uart1 (0/3)')
    while True:
        if uart_rx_cnt == 3:
            break
        time.sleep(0.1)
    uart1.disable()

cam0:bflb.Camera = None
canvas:lv.canvas = None

def _cam_callback(signal):
    """
    Camera 回调例程的回调函数
    """
    global cam0, canvas
    pic_addr, pic_size = cam0.get_frame_info()
    # print('Picture address: 0x%08x' % pic_addr)
    # print('Picture size: %d' % pic_size)
    cam0.pop_one_frame()
    LV_IMG_CF_TRUE_COLOR = 4
    canvas.set_buffer(pic_addr, 320, 240, LV_IMG_CF_TRUE_COLOR)

def cam():
    global cam0, canvas

    # lvgl
    canvas = lv.canvas(lv.scr_act())
    canvas.set_size(320, 240)
    canvas.align(lv.ALIGN.TOP_MID, 0, 0)

    # camera
    cam0 = bflb.Camera()
    cam0.set_callback(_cam_callback)
    cam0.start()
    return cam0

mic0:bflb.Microphone = None
chart:lv.chart = None

def _mic_callback(signal):
    # print('Mic data received')
    chart.refresh()

def mic():
    global mic0, chart

    # init mic
    mic0 = bflb.Microphone()
    mic0.set_callback(_mic_callback)

    # init chart
    chart = lv.chart(lv.scr_act())
    chart.set_size(320, 240)
    chart.set_range(0, 0, 2000)
    LV_PART_INDICATOR = 0x020000
    chart.set_style_size(0, LV_PART_INDICATOR)
    ser = chart.add_series(lv.palette_main(lv.PALETTE.RED), 0)
    addr, size = mic0.get_frame_info()
    chart.set_point_count(size)
    chart.set_ext_y_array(ser, addr)
    print('Mic frame address: 0x%08x' % addr)
    print('Mic frame size: %d' % size)

    # start mic
    mic0.start()
    return mic0


def wifi_sta():
    """
    wifi sta 例程
    """
    wlan = network.WLAN(network.STA_IF)
    wlan.active(1)
    ssid = input('SSID: ')
    password = input('Password: ')
    wlan.connect(ssid, password)
    for i in range(10):
        print('Waiting for wifi connection...')
        if wlan.isconnected():
            break
        time.sleep(1)
    print('Wifi connected:', bool(wlan.isconnected()))
    print('GET pikapython.com header:')
    res = requests.request('GET', 'http://www.pikapython.com')
    print(res.headers)