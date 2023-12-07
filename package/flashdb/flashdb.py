import _flashdb
import struct

KVDB_CTRL = _flashdb.KVDB_CTRL()

class KVDB(_flashdb.KVDB):

    def get_blob(self, key, size):
        res = super().get_blob(key, size)
        if type(res) == list:
            return bytes(res)
        return None

    def set_by_fmt(self, key, v, fmt):
        if type(v) == list or type(v) == tuple:
            blob = struct.pack(fmt, *v)
            return super().set_blob(key, blob)
        if type(v) == type(0):
            blob = struct.pack(fmt, v)
            return super().set_blob(key, blob)

    def get_by_fmt(self, key, size, fmt):
        res = super().get_blob(key, size)

        if res is None:
            return None
        vs = struct.unpack(fmt, bytes(res))
        if len(vs) == 1:
            return vs[0]
        return vs
