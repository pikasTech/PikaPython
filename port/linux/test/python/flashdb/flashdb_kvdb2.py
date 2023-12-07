import flashdb
import struct

DB_PATH = "test/out/fdb_kvdb"

print('test boot_count increment 2')
boot_count=0
boot_count_fmt='i'
boot_count_blob = struct.pack(boot_count_fmt, boot_count)
boot_count_size = len(boot_count_blob)

boot_time = [0,1,2,3,0,0,0,0,0,0]
boot_time_fmt='@10Q'
boot_time_tuple = tuple(boot_time)
boot_time_blob  = struct.pack(boot_time_fmt, *boot_time_tuple)

default_kv={
    'username': 'armink',  # string KV
    'password': "123456",   # string KV
    'boot_count': boot_count_blob,       # int type kv
    'boot_time': boot_time_blob,   # array type kv
}

fdb = flashdb.KVDB("env", DB_PATH, default_kv, None)
#print(default_kv)
kvdb = fdb

boot_count = fdb.get_by_fmt("boot_count", boot_count_size, boot_count_fmt)
assert boot_count is not None
print("==================== kvdb_basic_sample ====================")
print( "get the 'boot_count' value is %d" % boot_count)
        
boot_count = boot_count +1
res =fdb.set_by_fmt("boot_count", boot_count, boot_count_fmt)

new_boot_count = fdb.get_by_fmt("boot_count", boot_count_size, boot_count_fmt)
assert new_boot_count is not None
print( "get the 'boot_count' value is %d" % new_boot_count)
print("===========================================================")
assert new_boot_count == boot_count
print('PASS')
