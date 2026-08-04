import native_math
from native_math import ANSWER, Counter, add


print(add(right=22, left=20))
counter = Counter()
print(counter.increment(amount=Counter.STEP))
print(ANSWER)
print(native_math.ANSWER)
print(native_math.Counter.STEP)
print(0x4C + 0b10 + 0o1 + 1_000)
print(native_math.invert(False))
print(native_math.scale(1.25))
print(native_math.echo("edge"))
print(native_math.copy(b"\x00\xff"))
print(native_math.passthrough(None))
print(native_math.passthrough(True))
print(native_math.passthrough(-7))
print(native_math.passthrough(2.5))
print(native_math.passthrough("text"))
print(native_math.passthrough(b"bytes"))
print(native_math.passthrough(bytearray(b"data")))
print(native_math.nothing())
