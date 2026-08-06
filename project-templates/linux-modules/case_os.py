import os

root = ".os-case"
os.mkdir(root)
path = root + "/value.txt"
descriptor = os.open(path, os.O_CREAT | os.O_RDWR | os.O_TRUNC)
os.write(descriptor, b"pika")
os.lseek(descriptor, 0, os.SEEK_SET)
print("os-read:", os.read(descriptor, 16).decode())
os.close(descriptor)
print("os-kind:", os.path.isfile(path), os.path.isdir(root))
renamed = root + "/renamed.txt"
os.rename(path, renamed)
print("os-listdir:", os.listdir(root))
os.remove(renamed)
os.rmdir(root)
