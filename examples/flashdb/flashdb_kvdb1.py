import flashdb
import struct

DB_PATH = "test/out/fdb_kvdb"

print('test boot_count increment 1')
boot_count = 0
boot_count_blob = struct.pack('i', boot_count)
boot_times = [0, 1, 2, 3, 0, 0, 0, 0, 0, 0]
boot_time_tuple = tuple(boot_times)
boot_time_blob = struct.pack('@10Q', *boot_time_tuple)
default_kv = {
    'username': 'armink',  # string KV
    'password': "123456",   # string KV
    'boot_count': boot_count_blob,       # int type kv
    'boot_time': boot_time_blob,   # array type kv
}
# print(default_kv)
fdb = flashdb.KVDB("env", DB_PATH, default_kv, None)

res = fdb.get_blob("boot_count", len(boot_count_blob))
assert res is not None

boot_count = struct.unpack("i", res)[0]
boot_count = boot_count+1
boot_count_blob = struct.pack('i', boot_count)

fdb.set_blob("boot_count", boot_count_blob)
res = fdb.get_blob("boot_count", len(boot_count_blob))
assert res is not None

new_boot_count = struct.unpack("i", res)[0]
assert new_boot_count == boot_count
print('PASS')
