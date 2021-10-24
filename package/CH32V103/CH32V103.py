import PikaStdDevice


class GPIO(PikaStdDevice.GPIO):
    # need be overrid
    def platformHigh():
        pass

    # need override
    def platformLow():
        pass

    # need override
    def platformEnable():
        pass

    # need override
    def platformDisable():
        pass

    # need override
    def platformSetMode(mode: str):
        pass
