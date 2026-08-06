import unittest

after = False


class PassingTest(unittest.TestCase):
    def runTest(self):
        self.assertEqual(2 + 2, 4)
        self.assertTrue("pika" in "pika-python")
        self.assertIsNone(None)


class FailingTest(unittest.TestCase):
    def runTest(self):
        self.assertEqual(1, 2)


class ErrorTest(unittest.TestCase):
    def runTest(self):
        raise ValueError("sample error")


class AfterTest(unittest.TestCase):
    def runTest(self):
        global after
        after = True


suite = unittest.TestSuite()
suite.addTest(PassingTest())
suite.addTest(FailingTest())
suite.addTest(ErrorTest())
suite.addTest(AfterTest())
result = unittest.TextTestRunner(verbosity=0).run(suite)
print("unittest-stats:", result.testsRun, len(result.failures), len(result.errors), after)
print("unittest-success:", result.wasSuccessful())
