import threading

lock = threading.RLock()

assert lock.locked() == False
assert lock.acquire(True, None) == True
assert lock.acquire(True, None) == True
assert lock.locked() == True

lock.release()
assert lock.locked() == True
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
