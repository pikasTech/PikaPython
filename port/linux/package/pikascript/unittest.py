import _unittest


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
        _assert(x == y, msg)

    def assertNotEqual(self, x, y):
        msg = "%r not expected to be equal %r" % (x, y)
        _assert(x != y, msg)

    def assertLessEqual(self, x, y):
        msg = "%r is expected to be <= %r" % (x, y)
        _assert(x <= y, msg)

    def assertGreaterEqual(self, x, y):
        msg = "%r is expected to be >= %r" % (x, y)
        _assert(x >= y, msg)

    def assertTrue(self, x):
        msg = "Expected %r to be True" % x
        _assert(x, msg)

    def assertFalse(self, x):
        msg = "Expected %r to be False" % x
        _assert(not x, msg)

    def run(self, result, suite_name):
        _unittest._case_run(self, result, suite_name)


class TestSuite:
    def __init__(self, name):
        self._tests = []
        self.name = name

    def addTest(self, case):
        self._tests.append(case)

    def run(self, result):
        for case in self._tests:
            case.run(result, self.name)
        return result


class TextTestRunner:
    def run(self, suite: TestSuite):
        res = TestResult()
        _ = suite.run(res)
        print("----------------------------------------------------------------------")
        print("Ran %d tests\n" % res.testsRun)
        if res.failuresNum > 0 or res.errorsNum > 0:
            print("FAILED (failures=%d, errors=%d)" % (res.failuresNum, res.errorsNum))
        else:
            msg = "OK"
            if res.skippedNum > 0:
                msg += " (skipped=%d)" % res.skippedNum
            print(msg)

        return res


def _assert(res, msg):
    if not res:
        print("Assertion failed: %s" % msg)
        raise
