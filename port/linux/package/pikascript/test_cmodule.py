import PikaStdLib

def test_mem():
    mem = PikaStdLib.MemChecker()
    print('mem used max:')
    mem.max()