import STM32G0
import PikaPiZero
import PikaStdLib

pin = STM32G0.GPIO()
rgb = PikaPiZero.RGB()
mem = PikaStdLib.MemChecker()

pin.setPin('PA8')
pin.setMode('out')
pin.enalbe()

rgb.init()
rgb.enable()

print('hello 2')
print('mem used max:')
mem.max()


def rgb_task():
    rgb.flow()


def led_task():
    if pin.read():
        pin.low()
    else:
        pin.high()


task = STM32G0.Task()

task.call_period_ms(rgb_task, 50)
task.call_period_ms(led_task, 500)

task.run_always()
