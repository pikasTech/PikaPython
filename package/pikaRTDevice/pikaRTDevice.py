import PikaStdDevice


class GPIO(PikaStdDevice.GPIO):
    def platformHigh():
        pass

    def platformLow():
        pass

    def platformEnable():
        pass

    def platformDisable():
        pass

    def platformSetMode():
        pass

    def platformRead():
        pass


class PWM(PikaStdDevice.PWM):
    # override
    def platformEnable():
        pass

    # override
    def platformSetFrequency():
        pass

    # override
    def platformSetDuty():
        pass
