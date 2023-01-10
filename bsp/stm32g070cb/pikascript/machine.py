import STM32G0
import PikaPiZero
import time

def Time():
    return time

class GPIO(STM32G0.GPIO):
    pass


class ADC(STM32G0.ADC):
    pass


class UART(STM32G0.UART):
    pass


class PWM(STM32G0.PWM):
    pass


class Task(STM32G0.Task):
    pass

class IIC(STM32G0.IIC):
    pass

class RGB(PikaPiZero.RGB):
    pass


class KEY(PikaPiZero.KEY):
    pass


class LCD(PikaPiZero.LCD):
    pass


class Point(PikaPiZero.Point):
    pass
