import PikaStdLib
mem = PikaStdLib.MemChecker()

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
        return "<unittest.result.TestResult run=%d, errors=%d, failures=%d,skipped=%d>" % (
            self.testsRun, self.errorsNum, self.failuresNum, self.skippedNum)


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
            if name.startswith("test"):
                result.testsRun += 1
                self.test_fn = getattr(self, name)
                print("[ RUN      ] %s.%s" % (suite_name, name))
                try:
                    mem_before = 0.0
                    mem_after = 0.0
                    mem_before = mem.getNow()
                    self.test_fn()
                    mem_after = mem.getNow()
                    print("[       OK ] %s.%s" % (suite_name, name))
                    if mem_after != mem_before:
                        print("[ MEM LACK ]", mem_after - mem_before)
                except:
                    print("[  FAILED  ] %s.%s" % (suite_name, name))
                    result.errorsNum += 1


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
        print("[----------] tests from %s" % suite.name)
        _ = suite.run(res)
        print("[----------] %d tests from %s" % res.testsRun, suite.name)
        print('')
        print('[==========]')
        if res.failuresNum > 0 or res.errorsNum > 0:
            print("[  FAILED  ] (%d errors, %d failures)" % (
                res.errorsNum, res.failuresNum))
        else:
            msg = ""
            if res.skippedNum > 0:
                msg += " (skipped=%d)" % res.skippedNum
            print(msg)
            print("[  PASSED  ] %d tests" % res.testsRun)

        return res
