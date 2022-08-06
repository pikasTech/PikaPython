import unittest


class TestUnittestAssertions(unittest.TestCase):
    def testEqual(self):
        self.assertEqual(0, 0)

    def testTrue(self):
        self.assertTrue(True)

    def testFalse(self):
        self.assertFalse(False)


suit = unittest.TestSuite('test1')
suit.addTest(TestUnittestAssertions())
runner = unittest.TextTestRunner()
runner.run(suit)
