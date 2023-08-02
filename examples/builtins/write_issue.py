from PikaStdLib import MemChecker as mem
a = open(".test.py", "wb")
m0 = mem().getNow()
for i in range(1000):
    n = a.write(b'# print(r_data[0])\n# print(r_data[1])\n\n# import _thread\n# import')

assert n == 64
m1 = mem().getNow()
for i in range(1000):
    n = a.write(b'import osTimer33332 cnt=0332 cnt1=033332 def test_cb(arg):332global cnt332cnt+=1332123123123123231231')

assert n == 101
m2 = mem().getNow()
a.close()
print('PASS')
print('mem: ', m2- m0)
exit()
