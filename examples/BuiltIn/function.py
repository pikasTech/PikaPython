def test(a, b, *vars, **keys):
    print(keys['c'], keys['d'], a, b, vars[0])
test(3, 4, 5, c=1, d= 2)
