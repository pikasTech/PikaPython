def try_while_return():
    try:
        i = 0
        while i < 5:
            i += 1
            if i == 3:
                return i
    except Exception as e:
        return "Error occurred: %s" % str(e)


def try_finally_while_return():
    try:
        i = 0
        while i < 5:
            i += 1
            if i == 3:
                return i
    finally:
        pass


def while_try_return():
    i = 0
    while i < 5:
        try:
            if i == 3:
                return i
            i += 1
        except Exception as e:
            return "Error occurred: %s" % str(e)


def while_try_finally_return():
    i = 0
    while i < 5:
        try:
            if i == 3:
                return i
            i += 1
        finally:
            pass


def try_except_return():
    try:
        raise ValueError("An error occurred")
    except ValueError as e:
        return "Caught an error: %s" % str(e)

def try_except_return_sinple():
    try:
        raise
    except:
        return "Caught an error"


def while_try_except_return():
    i = 0
    while i < 5:
        try:
            if i == 3:
                raise ValueError("An error occurred at 3")
            i += 1
        except ValueError as e:
            return "Caught an error: %s" % str(e)

def while_try_except_return_simple():
    i = 0
    while i < 5:
        try:
            if i == 3:
                raise
            i += 1
        except:
            return "Caught an error"

assert try_while_return() == 3
assert while_try_return() == 3
assert try_except_return_sinple() == "Caught an error"
assert while_try_except_return_simple() == "Caught an error"
# assert while_try_except_return() == "Caught an error: An error occurred at 3"
# assert try_except_return() == "Caught an error: An error occurred"
# assert try_finally_while_return() == 3
# assert while_try_finally_return() == 3
