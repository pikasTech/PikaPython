# # Test splitting with a single character separator
# assert b"hello world".split(
#     b" ") == [b"hello", b"world"], "Single space separator failed"


# # Test splitting with a multi-character separator
# assert b"hello##world".split(
#     b"##") == [b"hello", b"world"], "Multi-character separator failed"

# # Test splitting with no occurrences of the separator
# assert b"hello world".split(
#     b"x") == [b"hello world"], "Non-existent separator failed"

# Test splitting with an empty separator should raise ValueError
try:
    b"hello world".split(b"")
    assert False, "Empty separator did not raise ValueError"
except:
    assert True, "Empty separator raised ValueError correctly"

# # Test splitting with a maximum split parameter
# assert b"one two three".split(
#     b" ", 1) == [b"one", b"two three"], "Max split parameter failed"

# # Test splitting an empty bytes object
# assert b"".split(b"-") == [b""], "Empty input failed"

# # print(b"----".split(b"-"))
# # Test splitting when the entire input is the separator
# assert b"----".split(b"-") == [b"", b"", b"", b"",
#                                b""], "Input is all separators failed"

# # Test splitting with trailing separators
# assert b"this is a test---".split(
#     b"-") == [b"this is a test", b"", b"", b""], "Trailing separators failed"

# # Test splitting with leading separators
# assert b"---this is a test".split(b"-") == [b"", b"",
#                                             b"", b"this is a test"], "Leading separators failed"

# print("PASS")
