import PikaStdDevice
import PikaStdTask


class GPIO(PikaStdDevice.GPIO):
    # override
    def platformHigh(self):
        pass

    # override
    def platformLow(self):
        pass

    # override
    def platformEnable(self):
        pass

    # override
    def platformDisable(self):
        pass

    # override
    def platformSetMode(self):
        pass

    # override
    def platformRead(self):
        pass


class Time(PikaStdDevice.Time):
    # override
    def sleep_s(self, s: int):
        pass

    # override
    def sleep_ms(self, ms: int):
        pass


class ADC(PikaStdDevice.ADC):
    # override
    def platformEnable(self):
        pass

    # override
    def platformRead(self):
        pass

    # override
    def platformDisable(self):
        pass


class UART(PikaStdDevice.UART):
    # override
    def platformEnable(self):
        pass

    # override
    def platformWrite(self):
        pass

    # override
    def platformRead(self):
        pass

    # override
    def platformDisable(self):
        pass


class IIC(PikaStdDevice.IIC):
    # override
    def platformEnable(self):
        pass

    # override
    def platformDisable(self):
        pass

    # override
    def platformWrite(self):
        pass

    # override
    def platformRead(self):
        pass

