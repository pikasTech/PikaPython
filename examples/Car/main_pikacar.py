import machine

time = machine.Time()

# init adc pin
pa2 = machine.ADC()
pa2.setPin('PA2')
pa2.enable()

# init motor pin
def pin_init(pin, pin_name):
    pin.setPin(pin_name)
    pin.setMode('out')
    pin.enable()
    pin.low()

pa6 = machine.GPIO()
pa7 = machine.GPIO()
pb0 = machine.GPIO()
pb1 = machine.GPIO()

pin_init(pa6,'PA6')
pin_init(pa7,'PA7')
pin_init(pb0,'PB0')
pin_init(pb1,'PB1')

# init sensor
pa2 = machine.ADC()
pa2.setPin('PA2')
pa2.enable()

# car functions
def read_sensor():
    if pa2.read() > 2.5:
        return 0
    else:
        return 1

def motor(cmd):
    if cmd == 'f' :
        pa6.low()
        pa7.high()
        pb0.high()
        pb1.low()
    elif cmd == 'b':
        pa6.high()
        pa7.low()
        pb0.low()
        pb1.high()
    elif cmd == 'r':
        pa6.high()
        pa7.low()
        pb0.high()
        pb1.low()
    elif cmd == 'l':
        pa6.low()
        pa7.high()
        pb0.low()
        pb1.high()

print('hardware init ok ')

while True:
    if read_sensor():
        motor('r')
        time.sleep_s(1)
    else:
        motor('f')

