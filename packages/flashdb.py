"""V1-compatible deterministic in-memory FlashDB API.

The V2 Linux capability has no FlashDB storage driver yet.  This module keeps
the public KVDB/TSDB contracts testable without claiming persistence.
"""
import time


class KVDB_CTRL:
    SET_SEC_SIZE = 1
    GET_SEC_SIZE = 2
    SET_LOCK = 3
    SET_UNLOCK = 4
    SET_FILE_MODE = 5
    SET_MAX_SIZE = 6
    SET_NOT_FORMAT = 7


class KVDB:
    def __init__(self, name, path, default_kv=None, user_data=None):
        self.name = name
        self.path = path
        self.user_data = user_data
        self._values = {}
        if default_kv is not None:
            for key in default_kv:
                self.set_blob(key, default_kv[key])

    def control(self, cmd, arg=None):
        return 0

    def deinit(self):
        return 0

    def set_blob(self, key, blob):
        if type(blob) == bytes:
            self._values[key] = list(blob)
        else:
            self._values[key] = list(blob)
        return 0

    def get_blob(self, key, size):
        blob = self._values.get(key)
        if blob is None:
            return None
        return blob[:size]

    def print(self):
        return len(self._values)

    def __del__(self):
        self.deinit()


class TSL:
    def __init__(self, timestamp, blob):
        self._time = timestamp
        self._blob = blob

    def get_time(self):
        return self._time

    def to_blob(self):
        return self._blob


class TSDB:
    def __init__(self, name, path, max_len=1024, user_data=None):
        self.name = name
        self.path = path
        self.max_len = max_len
        self.user_data = user_data
        self._records = []

    def tsl_append(self, blob):
        if len(self._records) >= self.max_len:
            self._records.pop(0)
        self._records.append(TSL(int(time.time()), blob))
        return 0

    def _iterate(self, records, callback, user_data):
        for record in records:
            result = callback(record, user_data)
            if result != 0:
                return result
        return 0

    def tsl_iter(self, callback, user_data=None):
        return self._iterate(self._records, callback, user_data)

    def tsl_iter_reverse(self, callback, user_data=None):
        return self._iterate(list(reversed(self._records)), callback, user_data)

    def tsl_iter_by_time(self, from_time, to_time, callback, user_data=None):
        selected = []
        for record in self._records:
            if record.get_time() >= from_time and record.get_time() <= to_time:
                selected.append(record)
        return self._iterate(selected, callback, user_data)

    def __del__(self):
        self._records = []
