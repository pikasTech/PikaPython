# api
from PikaObj import *
import PikaStdDevice
import PikaStdTask


class GPIO(PikaStdDevice.GPIO):
    ...


class ADC(PikaStdDevice.ADC):
    ...


class UART(PikaStdDevice.UART):
    ...


class PWM(PikaStdDevice.PWM):
    ...


class Task(PikaStdTask.Task):
    ...
