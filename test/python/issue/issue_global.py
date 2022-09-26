
class Test:
    def __init__(self):
        self.timer1_flag = 0
        self.timer2_flag = 0
        self.timer3_flag = 0

    def get_timer1_flag(self) -> int:
        return self.timer1_flag

    def set_timer1_flag(self, flag):
        self.timer1_flag = flag

    def fun1(self, name):
        self.timer1_flag = 1
        print("fun1", name)

    def fun2(self, name):
        #self.timer2_flag = 1
        print("fun2", name)
        print('self.timer2_flag = ', self.timer2_flag)

    def fun3(self, name):
        self.timer3_flag = 1
        print("fun3", name)


def callBack1(signal):
    print("signal", signal)
    testpara.fun1("callBack1")
    print(testpara.timer1_flag)


def callBack2(signal):
    print("signal", signal)
    #global testpara
    # global testpara.timer2_flag
    testpara.timer2_flag += 1
    testpara.fun2("callBack2")
    print('testpara.timer2_flag = ', testpara.timer2_flag)


def callBack3(signal):
    print("signal", signal)
    # global testpara
    # testpara.timer3_flag = 1
    testpara.fun3("callBack3")
    print(testpara.timer3_flag)


testpara = Test()
testpara.fun1('fun1')
testpara.fun2('fun2')
testpara.fun3('fun3')
callBack1(1)
callBack2(2)
callBack3(3)
