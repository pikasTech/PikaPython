# need core > v1.4.0
import PikaStdLib

class people:
    def do_hi(self):
        print('hello i am people')
    def hi(self):
        self.do_hi(self)

class student(people):
    def do_hi(self):
        print('hello i am student')

class mymem(PikaStdLib.MemChecker):
    def mymax(self):
        print('mem used max: ' + str(self.getMax()) + ' kB')

p = people()
s = student()

p.hi()
s.hi()

mem = mymem()
mem.mymax()
