import PikaStdLib
import pikaRTThread
import pikaRTDevice
import PikaDebug

thread = pikaRTThread.Thread()
mem = PikaStdLib.MemChecker()
pdb = PikaDebug.Debuger()
print('mem use max:')
mem.max()
# A2
Ain1 = pikaRTDevice.GPIO()
Ain1.setId(16)
Ain1.setMode('out')
Ain1.enable()
# A4
Ain2 = pikaRTDevice.GPIO()
Ain2.setId(20)
Ain2.setMode('out')
Ain2.enable()
# A6
Apwm = pikaRTDevice.PWM()
Apwm.setName('tim3pwm1')
Apwm.setChannel(1)
Apwm.setFreq(1000)
Apwm.setDuty(0)
Apwm.enable()
# A3
Bin1 = pikaRTDevice.GPIO()
Bin1.setId(17)
Bin1.setMode('out')
Bin1.enable()
# A5
Bin2 = pikaRTDevice.GPIO()
Bin2.setId(21)
Bin2.setMode('out')
Bin2.enable()
# A7
Bpwm = pikaRTDevice.PWM()
Bpwm.setName('tim3pwm2')
Bpwm.setChannel(2)
Bpwm.setFreq(1000)
Bpwm.setDuty(0)
Bpwm.enable()

#PC4
sensor1 = pikaRTDevice.GPIO()
sensor1.setId(24)
sensor1.setMode('in')
sensor1.setPull('up')
sensor1.enable()

#PC5
sensor2 = pikaRTDevice.GPIO()
sensor2.setId(25)
sensor2.setMode('in')
sensor2.setPull('up')
sensor2.enable()

def motor1(speed: float):
    if speed > 0:
        Ain1.high()
        Ain2.low()
        Apwm.setDuty(speed)
    else:
        Ain1.low()
        Ain2.high()
        Apwm.setDuty(-speed)


def motor2(speed: float):
    if speed > 0:
        Bin1.high()
        Bin2.low()
        Bpwm.setDuty(speed)
    else:
        Bin1.low()
        Bin2.high()
        Bpwm.setDuty(-speed)

def go():
    while True: 
        if not sensor2.read():
            print('turn right...')
            motor1(0.6)
            motor2(-0.6)
        elif not sensor1.read():
            print('turn left...')
            motor1(-0.6)
            motor2(0.6)
        else:
            print('forword...')
            motor1(0.6)
            motor2(0.6)

go()
