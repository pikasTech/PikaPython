import TemplateDevice
import time
import _thread
import json

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


def thread_test():
    print('thread_test')


def insert_task():
    # complex json string
    for i in range(100):
        json_str = '{"a": 1, "b": 2, "c": 3, "d": {"e": 4, "f": 5}, "g": [6, 7, 8], "h": null, "i": false, "j": true, "k": "string", "l": 1.234}'
        json_obj = json.loads(json_str)
        json_dump = json.dumps(json_obj)


# launch a thread to enable evnet_thread
_thread.start_new_thread(thread_test, ())
