import _thread
import threading
import time

lock = threading.Lock()


def worker():
    assert lock.acquire(True, None) == True


_thread.start_new_thread(worker, ())
time.sleep(0.05)

assert lock.locked() == True

# A regular Python Lock can be released by a thread other than its acquirer.
lock.release()
assert lock.locked() == False

handoff = threading.Lock()
assert handoff.acquire(True, None) == True


def handoff_worker():
    assert handoff.acquire(True, None) == True
    handoff.release()


_thread.start_new_thread(handoff_worker, ())
time.sleep(0.02)
handoff.release()
time.sleep(0.05)

print('PASS')
