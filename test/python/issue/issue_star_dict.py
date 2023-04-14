import time
import json

__log_level = 'debug'


def log(level: str, title: str, *var):
    """
    通用输出日志方法
    :param level:
    :param var:
    :return:
    """
    if __log_level == 'error' and level in ['debug', 'info', 'warning']:
        return
    elif __log_level == 'warning' and level in ['debug', 'info']:
        return
    elif __log_level == 'info' and level == 'debug':
        return

    print('---------  ', title, '  ---------')
    for i in var:
        print('[%r]' % level, i)


def debug(title: str, *var):
    """
    打印debug日志
    :param title:
    :param var:
    :return:
    """
    log('debug', title, *var)


testdict = json.loads(
    '{"type":"485","data":[{"addr":"5","options":[{"fun":"x03","opt_addr":0,"data":5}]}]}')

debug("testdict", testdict, type(testdict))
