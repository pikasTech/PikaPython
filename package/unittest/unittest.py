import _unittest


class TestCase:
    def __init__(self):
        pass

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


class TestSuite:
    def __init__(self, name):
        self._tests = []
        self.name = name

    def addTest(self, cls):
        self._tests.append(cls)

    def run(self, result):
        for c in self._tests:
            run_suite(c, result, self.name)
        return result


class TestRunner:
    def run(self, suite: TestSuite):
        res = TestResult()
        suite.run(res)

        res.printErrors()
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

    def printErrors(self):
        print()
        self.printErrorList(self.errors)
        self.printErrorList(self.failures)

    def __str__(self):
        # Format is compatible with CPython.
        return "<unittest.result.TestResult run=%d errors=%d failures=%d>" % (\
            self.testsRun,\
            self.errorsNum,\
            self.failuresNum,\
        )

def run_suite(c, test_result: TestResult, suite_name=""):
    c.run(test_result)
    return


def _assert(res, msg):
    if not res:
        print("Assertion failed: %s" % msg)
        raise
