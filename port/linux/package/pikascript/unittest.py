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
    
    def assertNotIn(self, x, y):
        msg = "Expected %r not to be in %r" % (x, y)
        assert x not in y, msg
    
    def do_run(self, suite_name, name):
        mem_before = 0.0
        mem_after = 0.0
        mem_before = mem.getNow()
        self.test_fn()
        mem_after = mem.getNow()
        print("\x1b[1m\x1b[32m[       OK ] %s.%s\x1b[0m" % (suite_name, name))
        if mem_after != mem_before:
            print("\x1b[33m[ MEM LACK ]", mem_after - mem_before,"\x1b[0m")

    def run(self, result: TestResult, suite_name, stop_on_error=False):
        for name in dir(self):
            if name.startswith("test"):
                result.testsRun += 1
                self.test_fn = getattr(self, name)
                print("[ RUN      ] %s.%s" % (suite_name, name))
                if stop_on_error:
                    self.do_run(suite_name, name)
                    continue
                try:
                    self.do_run(suite_name, name)
                except:
                    print("\x1b[1m\x1b[31m[  FAILED  ]\x1b[0m %s.%s" % (suite_name, name))
                    result.errorsNum += 1


class TestSuite:
    def __init__(self, name):
        self._tests = []
        self.name = name

    def addTest(self, case):
        self._tests.append(case)

    def run(self, result: TestResult, stop_on_error=False):
        for case in self._tests:
            case.run(result, self.name, stop_on_error)
        return result


class TextTestRunner:
    def run(self, suite: TestSuite, stop_on_error=False):
        res = TestResult()
        print("[----------] tests from %s" % suite.name)
        _ = suite.run(res, stop_on_error)
        print("[----------] %d tests from %s" % res.testsRun, suite.name)
        print('')
        print('[==========]')
        if res.failuresNum > 0 or res.errorsNum > 0:
            print("\x1b[1m\x1b[31m[  FAILED  ](%d errors, %d failures)\x1b[0m" % (res.errorsNum, res.failuresNum))
        else:
            msg = ""
            if res.skippedNum > 0:
                msg += " (skipped=%d)" % res.skippedNum
            print(msg)
            print("\x1b[1m\x1b[32m[  PASSED  ] %d tests\x1b[0m" % res.testsRun)

        return res
