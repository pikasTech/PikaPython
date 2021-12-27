import PikaStdLib
import W801Device


pb5=W801Device.GPIO()

pb5.init()
pb5.setPin('PB5')
pb5.setMode('out')
pb5.setPull('up')
pb5.enable()



print('hello PikaScript!')


while True:
    pb5.high()    
    print('h')      
    pb5.low()    
    print('l')




