import TemplateDevice
import time

io1 = TemplateDevice.GPIO()
io1.setPin('PA8')
io1.setMode('in')
io1.enable()

EVENT_SIGAL_IO_RISING_EDGE = 0x01
EVENT_SIGAL_IO_FALLING_EDGE = 0x02

callback_times = 0


def callBack1(signal):
    global callback_times
    callback_times += 1
    print('callback_times: %d' % callback_times)
    if signal == EVENT_SIGAL_IO_RISING_EDGE:
        print('get rising edge!')
    elif signal == EVENT_SIGAL_IO_FALLING_EDGE:
        print('get falling edge!')
    else:
        return signal

io1.addEventCallBack(callBack1)
