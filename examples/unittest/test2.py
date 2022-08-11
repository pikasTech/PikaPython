import unittest


class TestUnittestAssertions(unittest.TestCase):
    def testEqual(self):
        print("in testEqual...")
        self.assertEqual(0, 0)

    def testTrue(self):
        print("in testTrue...")
        self.assertTrue(True)

    def testFalse(self):
        print("in testFalse...")
        self.assertFalse(False)

    def testFalse2(self):
        print("in testFalse2...")
        self.assertFalse(True)


suit = unittest.TestSuite("test1")
suit.addTest(TestUnittestAssertions())
runner = unittest.TextTestRunner()
res = runner.run(suit)
