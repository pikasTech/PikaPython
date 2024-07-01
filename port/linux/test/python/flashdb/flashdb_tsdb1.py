import flashdb
import struct
import time
DB_PATH = "test/out/fdb_tsdb"
tsdb = flashdb.TSDB("env", DB_PATH, max_len=512)

for i in range(100):
    blob_i = struct.pack('i', i)
    time.sleep(0.001)
    ret = tsdb.append(blob_i)

assert ret == 0
print('PASS')
