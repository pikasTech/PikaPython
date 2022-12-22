import PikaStdDevice as std
print('hello pikascript')

io = std.GPIO()
io.setPin('P4')
io.setMode('in')
io.enable()

def cb1(signal):
    print('cb1', signal)

io.addEventCallBack(cb1, io.SIGNAL_FALLING)

while True:
    pass
