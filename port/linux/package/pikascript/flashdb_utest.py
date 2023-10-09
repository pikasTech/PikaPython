import flashdb
import unittest
import struct


class FlashDBUnitTest(unittest.TestCase):
    def test_boot_count(self):
        print('test_boot_count')
        fdb = flashdb.FDB()
        boot_count_blob = struct.pack('i', 0)
        boot_times = [0 for i in range(10)]
        boot_time_tuple = tuple(boot_times)
        boot_time_blob  = struct.pack('@10Q', *boot_time_tuple)
        default_kv=[
            ['username', 'armink'],  # string KV
            ['password',"123456"],   # string KV
            ['boot_count', boot_count_blob, len(boot_count_blob)],       # int type kv
            ['boot_time', boot_time_blob, len(boot_time_blob)],   # array type kv
            ]
        print(default_kv)
        kvdb = fdb.kvdb_init("env", "fdb_kvdb1", default_kv, None)
        print(kvdb)
        skip="""
        
        boot_count = fdb.kv_get_blob(kvdb, "boot_count")
        boot_count= boot_count+1
        fdb.kv_set_blob(kvdb, "boot_count", boot_count)
        read_boot_count = fdb.kv_get_blob(kvdb, "boot_count")
        self.assertEqual(read_boot_count, boot_count)
        """

kvdb=None
fdb=None
def test1():
    global fdb, kvdb
    print('test_boot_count')
    if fdb is None:
        fdb = flashdb.FDB()
    print('1')
    boot_count=0
    boot_count_blob = struct.pack('i', boot_count)
    print('boot_count_blob:', boot_count_blob)
    #boot_times = [0 for i in range(10)]
    boot_times = [0, 0, 0, 0,0,0,0,0,0,0]
    print('boot_times:', boot_times)
    boot_time_tuple = tuple(boot_times)
    print('boot_time_tuple:', boot_time_tuple)
    boot_time_blob  = struct.pack('@10Q', *boot_time_tuple)
    print('boot_time_blob:', boot_time_blob)
   # _default_kv=（
   #     ('username', 'armink'),  # string KV
   #     ('password',"123456"),   # string KV
   #     ('boot_count', boot_count_blob, len(boot_count_blob)),       # int type kv
   #     ('boot_time', boot_time_blob, len(boot_time_blob)),   # array type kv
   # ）
    default_kv={
        'username': 'armink',  # string KV
        'password': "123456",   # string KV
        'boot_count': boot_count_blob,       # int type kv
        'boot_time': boot_time_blob,   # array type kv
    }
    
    print(default_kv)


    kvdb = fdb.kvdb_init("env", "fdb_kvdb1", default_kv, None)
    print('kvdb:', kvdb)
    print('kvdb1', kvdb)

    fdb.kv_print(kvdb)

    print('print again')
    print('kvdb:', kvdb)
    fdb.kv_print(kvdb)

    print('get_blob')    
    res = fdb.kv_get_blob(kvdb, "boot_count", len(boot_count_blob))
    print("get_blob res:",res)   
    fdb.kv_print(kvdb)
    skip="""  
    if res is not None:
        boot_count_blob = bytes(res)
        boot_count = struct.unpack("i", boot_count_blob)[0]
        boot_count= boot_count + 1
        print("boot_count:", boot_count)
        boot_count_blob = struct.pack("i", boot_count);
        print("boo_count_blob:", boot_count_blob)
        #res = fdb.kv_set_blob(kvdb, "boot_count", boot_count_blob)
        #print("set_blob res:", res)
    """

def utest():
    suit = unittest.TestSuite("test1")
    suit.addTest(FlashDBUnitTest())
    runner = unittest.TextTestRunner()
    res = runner.run(suit)
    
        
