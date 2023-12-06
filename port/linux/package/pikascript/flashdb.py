import _flashdb
import struct


class FDB(_flashdb.FlashDB):

    def kv_get_blob(self, kvdb, key, size):
        res = super().kv_get_blob(kvdb, key, size)
        if type(res) == list:
            return bytes(res)
        return None

    def kv_set_by_fmt(self, kvdb, key, v, fmt):
        if type(v) == type([]) or type(v) == type(()):
            blob = struct.pack(fmt, *v)
            return super().kv_set_blob(kvdb, key, blob)
        if type(v) == type(0):
            blob = struct.pack(fmt, v)
            return super().kv_set_blob(kvdb, key, blob)

    def kv_get_by_fmt(self, kvdb, key, size, fmt):
        res = super().kv_get_blob(kvdb, key, size)

        if res is None:
            return None
        vs = struct.unpack(fmt, bytes(res))
        if len(vs) == 1:
            return vs[0]
        return vs
