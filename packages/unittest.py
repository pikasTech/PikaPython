# SPEC: PJ2026-050114 runtime ecosystem packages v0.1; unittest core subset.


class TestResult:
    def __init__(self):
        self.testsRun = 0
        self.failures = []
        self.errors = []
        self.skipped = []

    def wasSuccessful(self):
        return len(self.failures) == 0 and len(self.errors) == 0

    def addRun(self):
        self.testsRun += 1

    def addFailure(self, message):
        self.failures.append(message)

    def addError(self, message):
        self.errors.append(message)


class TestCase:
    def __init__(self, test_method=None):
        self._test_method = test_method

    def runTest(self):
        self.fail("runTest is not configured")

    def fail(self, msg=None):
        if msg is None:
            msg = "test failed"
        raise AssertionError(msg)

    def assertEqual(self, first, second, msg=None):
        if first != second:
            if msg is None:
                msg = "values are not equal"
            raise AssertionError(msg)

    def assertNotEqual(self, first, second, msg=None):
        if first == second:
            if msg is None:
                msg = "values are equal"
            raise AssertionError(msg)

    def assertTrue(self, expression, msg=None):
        if not expression:
            if msg is None:
                msg = "expression is not true"
            raise AssertionError(msg)

    def assertFalse(self, expression, msg=None):
        if expression:
            if msg is None:
                msg = "expression is not false"
            raise AssertionError(msg)

    def assertIs(self, first, second, msg=None):
        if first is not second:
            if msg is None:
                msg = "objects are not identical"
            raise AssertionError(msg)

    def assertIsNot(self, first, second, msg=None):
        if first is second:
            if msg is None:
                msg = "objects are identical"
            raise AssertionError(msg)

    def assertIsNone(self, expression, msg=None):
        if expression is not None:
            if msg is None:
                msg = "value is not None"
            raise AssertionError(msg)

    def assertIsNotNone(self, expression, msg=None):
        if expression is None:
            if msg is None:
                msg = "value is None"
            raise AssertionError(msg)

    def assertIn(self, member, container, msg=None):
        if member not in container:
            if msg is None:
                msg = "member is not present"
            raise AssertionError(msg)

    def assertNotIn(self, member, container, msg=None):
        if member in container:
            if msg is None:
                msg = "member is present"
            raise AssertionError(msg)

    def assertLessEqual(self, first, second, msg=None):
        if first > second:
            if msg is None:
                msg = "first value is greater than second value"
            raise AssertionError(msg)

    def assertGreaterEqual(self, first, second, msg=None):
        if first < second:
            if msg is None:
                msg = "first value is less than second value"
            raise AssertionError(msg)

    def run(self, result=None):
        if result is None:
            result = TestResult()
        result.addRun()
        try:
            self.runTest()
        except AssertionError as error:
            result.addFailure(str(error))
        except Exception as error:
            result.addError(str(error))
        return result


class TestSuite:
    def __init__(self, tests=None):
        self._tests = []
        if tests is not None:
            for test in tests:
                self.addTest(test)

    def addTest(self, test):
        self._tests.append(test)

    def run(self, result=None):
        if result is None:
            result = TestResult()
        for test in self._tests:
            test.run(result)
        return result


class TextTestRunner:
    def __init__(self, verbosity=1):
        self.verbosity = verbosity

    def run(self, test):
        result = TestResult()
        test.run(result)
        if self.verbosity > 0:
            print("Ran", result.testsRun, "tests")
            if result.wasSuccessful():
                print("OK")
            else:
                print("FAILED", len(result.failures), len(result.errors))
        return result
