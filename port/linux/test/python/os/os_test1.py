import os
origin = os.getcwd()
os.chdir("test/out")
os.getcwd()

try:
    # cleanup
    os.remove("_testdir/testfile")
    os.rmdir("_testdir")
except:
    pass

os.mkdir("_testdir")
os.chdir("_testdir")


f = os.open("testfile", os.O_CREAT | os.O_RDWR)
assert os.write(f, b"Hello World!") == 12
assert os.lseek(f, 0, 0) == 0
print(os.read(f, 100))
os.close(f)
os.chdir("..")
os.chdir(origin)
print("PASS")
