import math


print("ceil:", math.ceil(1.25))
print("floor:", math.floor(-1.25))
print("trunc:", math.trunc(-3.75))
print("fabs:", math.fabs(-2.5))
print("fmod:", math.fmod(7, 3))
print("sqrt:", math.sqrt(81))
print("sqrt-bool:", math.sqrt(True))
print("radians:", math.radians(180))
try:
    math.sqrt(-1)
    print("domain: accepted")
except ValueError:
    print("domain: ValueError")
