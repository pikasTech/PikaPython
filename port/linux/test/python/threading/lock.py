import threading

lock = threading.Lock()

assert lock.locked() == False
assert lock.acquire(True, None) == True
assert lock.locked() == True
assert lock.acquire(False, None) == False
assert lock.acquire(True, 0.01) == False

lock.release()
assert lock.locked() == False

caught = False
try:
    lock.release()
except RuntimeError:
    caught = True
assert caught

assert lock.acquire(True, None) == True
lock.release()
assert lock.locked() == False

print('PASS')
