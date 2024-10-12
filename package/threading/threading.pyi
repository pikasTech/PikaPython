import _thread
from PikaObj import *

def get_ident() ->int64: ...

class Lock():

    def __init__(self): # real signature unknown
        self._mutex_ = None

    def acquire(self, block:bool, timeout:any) -> bool: ... # real signature unknown; restored from __doc__
        
    def locked(self) -> bool: ...# real signature unknown; restored from __doc__

    def release(self): ... # real signature unknown; restored from __doc__

    def __del__(self): ... 


class RLock():

    def __init__(self): # real signature unknown
        self._rmutex_ = None

    def acquire(self, block:bool, timeout:any) -> bool: ...# real signature unknown; restored from __doc__

    def locked(self) -> bool: ... # real signature unknown; restored from __doc__

    def release(self): ...# real signature unknown; restored from __doc__

    def __del__(self): ...


class Condition:

    def __init__(self): # real signature unknown
        self._cond_ = None
        self._rtmetux_ = None

    def wait(self, timeout:any) -> bool: ...
    
    def notify(self, n:int): ...

    def notify_all(self): ...
    
    def _is_owned(self) -> bool: ... 

    def __del__(self): ... 

    def acquire(self, block:bool, timeout:any) -> bool: ...

    def release(self): ... 
