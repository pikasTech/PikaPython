import time
import PikaStdDevice

uart_g = None
recv_g = ""
wait_g = False


def ClientCB(signal):
    global recv_g, wait_g
    recv_g = uart_g.read(128)
    wait_g = False


class Client:
    def __init__(self, uart: PikaStdDevice.UART):
        global uart_g
        uart_g = uart
        uart_g.setCallBack(ClientCB, uart_g.SIGNAL_RX)

    def configMode(self):
        uart_g.write("+++")
        time.sleep(0.3)

    def cmd(self, cmd):
        global wait_g
        wait_g = True
        uart_g.write("AT+" + cmd + "\r\n")
        retry = 0
        while wait_g and retry < 10:
            retry += 1
            time.sleep(0.1)

    def get(self, arg):
        self.cmd(arg + "?")
        raw = self.res()
        for line in raw.split("\r\n"):
            if "+" + arg + ":" in line:
                return line.split(":")[1]

    def set(self, arg, val):
        self.cmd(arg + "=" + str(val))

    def res(self):
        return recv_g
    
    def __getattr__(self, name):
        return self.get(name)
    
    def __setattr__(self, name, value):
        self.set(name, value)
