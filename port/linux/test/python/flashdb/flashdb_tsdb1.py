import flashdb
import struct
import time
import os
DB_PATH = "test/out/fdb_tsdb"


tsdb = flashdb.TSDB("env", DB_PATH, max_len=512)

for i in range(100):
    blob_i = struct.pack('i', i)
    time.sleep(0.001)
    ret = tsdb.tsl_append(blob_i)

assert ret == 0


def callback(tsl, user_data) -> int:
    # print(tsl.get_time(), tsl.to_blob())
    t = tsl.get_time()
    blob_i = tsl.to_blob()
    i = struct.unpack('i', blob_i)[0]
    print(t, i, user_data)
    return False  # False: continue, True: stop


assert tsdb.tsl_iter(callback, 'user_data') == 0

print('PASS')
