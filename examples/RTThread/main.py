import PikaStdLib
import pikaRTThread
import pikaRTDevice
import PikaDebug

thread = pikaRTThread.Thread()
mem = PikaStdLib.MemChecker()
pdb = PikaDebug.Debuger()

print('mem use max:')
mem.max()

led = pikaRTDevice.GPIO()
led.setId(57)
led.setMode('out')
led.enable()

pwm = pikaRTDevice.PWM()
pwm.setName('tim3pwm1')
pwm.setChannel(1)
pwm.setFrequency(1000)
pwm.setDuty(0.5)
pwm.enable()

