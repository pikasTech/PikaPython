import flashdb
import unittest
import struct

kvdb=None
fdb=flashdb.FDB()

class FlashDBUnitTest(unittest.TestCase):
    def test_boot_count1(self):
        print('test boot_count increment 1')
        if fdb is None:
            fdb = flashdb.FDB()
        boot_count=0
        boot_count_blob = struct.pack('i', boot_count)
        boot_times = [0,1,2,3,0,0,0,0,0,0]
        boot_time_tuple = tuple(boot_times)
        boot_time_blob  = struct.pack('@10Q', *boot_time_tuple)
        default_kv={
            'username': 'armink',  # string KV
            'password': "123456",   # string KV
            'boot_count': boot_count_blob,       # int type kv
            'boot_time': boot_time_blob,   # array type kv
        }
        #print(default_kv)
        kvdb = fdb.kvdb_init("env", "fdb_kvdb1", default_kv, None)

        res = fdb.kv_get_blob(kvdb, "boot_count", len(boot_count_blob))
        self.assertIsNotNone(res)
        
        boot_count = struct.unpack("i", res)[0]      
        boot_count= boot_count+1
        boot_count_blob = struct.pack('i', boot_count)
        
        fdb.kv_set_blob(kvdb, "boot_count", boot_count_blob)
        res = fdb.kv_get_blob(kvdb, "boot_count", len(boot_count_blob))
        self.assertIsNotNone(res)

        new_boot_count = struct.unpack("i", res)[0]      
        
        self.assertEqual(new_boot_count, boot_count)
        
    def test_boot_count2(self):
        print('test boot_count increment 2')
        if fdb is None:
            fdb = flashdb.FDB()
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
        #print(default_kv)
        kvdb = fdb.kvdb_init("env", "fdb_kvdb1", default_kv, None)

        boot_count = fdb.kv_get_by_fmt(kvdb, "boot_count", boot_count_size, boot_count_fmt)
        self.assertIsNotNone(boot_count)
        print("==================== kvdb_basic_sample ====================")
        print( "get the 'boot_count' value is %d" % boot_count)
        
        boot_count = boot_count +1
        res =fdb.kv_set_by_fmt(kvdb, "boot_count", boot_count, boot_count_fmt)

        new_boot_count = fdb.kv_get_by_fmt(kvdb, "boot_count", boot_count_size, boot_count_fmt)
        self.assertIsNotNone(new_boot_count)
        print( "get the 'boot_count' value is %d" % new_boot_count)
        print("===========================================================")

        self.assertEqual(new_boot_count, boot_count)
        


def utest():
    suit = unittest.TestSuite("test1")
    suit.addTest(FlashDBUnitTest())
    runner = unittest.TextTestRunner()
    res = runner.run(suit)
    
        
