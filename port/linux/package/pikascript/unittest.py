import _unittest
class TestCase:
    def __init__(self):
        pass

    def fail(self, msg=""):
        assert False, msg

    def assertEqual(self, x, y, msg=""):
        if not msg:
            msg = "%r vs (expected) %r" % (x, y)
        assert x == y, msg

    def assertNotEqual(self, x, y, msg=""):
        if not msg:
            msg = "%r not expected to be equal %r" % (x, y)
        assert x != y, msg

    def assertLessEqual(self, x, y, msg=None):
        if msg is None:
            msg = "%r is expected to be <= %r" % (x, y)
        assert x <= y, msg

    def assertGreaterEqual(self, x, y, msg=None):
        if msg is None:
            msg = "%r is expected to be >= %r" % (x, y)
        assert x >= y, msg

    def assertTrue(self, x, msg=""):
        if not msg:
            msg = "Expected %r to be True" % x
        assert x, msg

    def assertFalse(self, x, msg=""):
        if not msg:
            msg = "Expected %r to be False" % x
        assert not x, msg

    def assertIn(self, x, y, msg=""):
        if not msg:
            msg = "Expected %r to be in %r" % (x, y)
        assert x in y, msg

    def assertIsInstance(self, x, y, msg=""):
        assert isinstance(x, y), msg

class TestSuite:
    def __init__(self, name=""):
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

    def printErrorList(self, lst):
        sep = "----------------------------------------------------------------------"
        for c, e in lst:
            detail = " ".join((str(i) for i in c))
            print("======================================================================")
            print(f"FAIL: {detail}")
            print(sep)
            print(e)

    def __repr__(self):
        # Format is compatible with CPython.
        return "<unittest.result.TestResult run=%d errors=%d failures=%d>" % (\
            self.testsRun,\
            self.errorsNum,\
            self.failuresNum,\
        )

    def __add__(self, other):
        self.errorsNum += other.errorsNum
        self.failuresNum += other.failuresNum
        self.skippedNum += other.skippedNum
        self.testsRun += other.testsRun
        self.errors.extend(other.errors)
        self.failures.extend(other.failures)
        self.skipped.extend(other.skipped)
        return self


def run_suite(c, test_result: TestResult, suite_name=""):
    if isinstance(c, TestSuite):
        c.run(test_result)
        return

    if isinstance(c, type):
        o = c()
    else:
        o = c
    set_up_class = getattr(o, "setUpClass", lambda: None)
    tear_down_class = getattr(o, "tearDownClass", lambda: None)
    set_up = getattr(o, "setUp", lambda: None)
    tear_down = getattr(o, "tearDown", lambda: None)
    exceptions = []
    try:
        suite_name += "." + c.__qualname__
    except AttributeError:
        pass

    def run_one(test_function):
        global __test_result__, __current_test__
        print("%s (%s) ..." % (name, suite_name), end="")
        set_up()
        __test_result__ = test_result
        test_container = f"({suite_name})"
        __current_test__ = (name, test_container)

        test_result._newFailures = 0
        test_result.testsRun += 1
        test_globals = dict(**globals())
        test_globals["test_function"] = test_function
        exec("test_function()", test_globals, test_globals)
        # No exception occurred, test passed
        if test_result._newFailures:
            print(" FAIL")
        else:
            print(" ok")

    set_up_class()

    if hasattr(o, "runTest"):
        name = str(o)
        run_one(o.runTest)
        return

    for name in dir(o):
        if name.startswith("test"):
            m = getattr(o, name)
            if not callable(m):
                continue
            run_one(m)

    if callable(o):
        name = o.__name__
        run_one(o)

    tear_down_class()

    return exceptions

