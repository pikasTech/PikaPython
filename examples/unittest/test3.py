import socket
import random
import unittest


class TestUnittestAssertions(unittest.TestCase):
    def testSocket(self):
        server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        host = "127.0.0.1"
        port = 9999 + random.randint(0, 1000) + 1
        print("port:", port)
        server.bind((host, port))
        server.listen(5)
        
        client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client.connect((host, port))
        
        accept, addr = server.accept()
        self.assertEqual(addr, "127.0.0.1")
        
        print("recv from client: %s" % str(addr))
        
        client.send("send test from client".encode())
        print("server recv:", accept.recv(1024).decode())
        
        accept.send("send test from server".encode())
        print("client recv:", client.recv(1024).decode())
        
        accept.close()
        client.close()
        server.close()


suit = unittest.TestSuite("test1")
suit.addTest(TestUnittestAssertions())
runner = unittest.TextTestRunner()
res = runner.run(suit)
