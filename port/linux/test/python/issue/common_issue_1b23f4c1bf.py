
class Test1:
    """Test1 docstring"""

class Test2(Test1):
    """Test2 docstring"""
    def print(self):
        print("Test2")

a = Test2()
a.print()
