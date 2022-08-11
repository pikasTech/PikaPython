import _unittest
from PikaStdData import String


class TestResult:
    def __init__(self):
        self.errorsNum = 0
        self.failuresNum = 0
        self.skippedNum = 0
        self.testsRun = 0
        self.errors = []
        self.failures = []
        self.skipped = []
        self._newFailures = 0

    def wasSuccessful(self):
        return self.errorsNum == 0 and self.failuresNum == 0

    def __str__(self):
        # Format is compatible with CPython.
        s = "<unittest.result.TestResult "
        s += "run=%d, " % self.testsRun
        s += "errors=%d, " % self.errorsNum
        s += "failures=%d, " % self.failuresNum
        s += "skipped=%d>" % self.skippedNum
        return s


class TestCase:
    def assertEqual(self, x, y):
        msg = "%r vs (expected) %r" % (x, y)
        assert x == y, msg

    def assertNotEqual(self, x, y):
        msg = "%r not expected to be equal %r" % (x, y)
        assert x != y, msg

    def assertLessEqual(self, x, y):
        msg = "%r is expected to be <= %r" % (x, y)
        assert x <= y, msg

    def assertGreaterEqual(self, x, y):
        msg = "%r is expected to be >= %r" % (x, y)
        assert x >= y, msg

    def assertTrue(self, x):
        msg = "Expected %r to be True" % x
        assert x, msg

    def assertFalse(self, x):
        msg = "Expected %r to be False" % x
        assert not x, msg

    def assertIs(self, x, y):
        msg = "%r is not %r" % (x, y)
        assert x is y, msg

    def assertIsNot(self, x, y):
        msg = "%r is %r" % (x, y)
        assert x is not y, msg

    def assertIsNone(self, x):
        msg = "%r is not None" % x
        assert x is None, msg

    def assertIsNotNone(self, x):
        msg = "%r is None" % x
        assert x is not None, msg

    def assertIn(self, x, y):
        msg = "Expected %r to be in %r" % (x, y)
        assert x in y, msg

    def run(self, result: TestResult, suite_name):
        for name in dir(self):
            if String(name).startswith("test"):
                result.testsRun += 1
                self.test_fn = getattr(self, name)
                print("[ RUN      ] %s.%s" % (suite_name, name))
                self.test_fn()


class TestSuite:
    def __init__(self, name):
        self._tests = []
        self.name = name

    def addTest(self, case):
        self._tests.append(case)

    def run(self, result: TestResult):
        for case in self._tests:
            case.run(result, self.name)
        return result


class TextTestRunner:
    def run(self, suite: TestSuite):
        res = TestResult()
        _ = suite.run(res)
        print("----------------------------------------------------------------------")
        print("[----------] %d tests from %s\n" % res.testsRun, suite.name)
        if res.failuresNum > 0 or res.errorsNum > 0:
            s = "FAILED"
            s += " (%d errors, %d failures)" % (res.errorsNum, res.failuresNum)
            print(s)
        else:
            msg = "[==========]"
            if res.skippedNum > 0:
                msg += " (skipped=%d)" % res.skippedNum
            print(msg)

        return res
