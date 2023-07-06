

while True:
    print("before try")
    try:
        print("after try")
        while True:
            raise
    except:
        break

print("after while")
