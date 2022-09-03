def test(a, b, e=10, *vars, **keys):
    print(keys['c'], keys['d'], a, b, vars[0], e)
test(3, 4, 5, c=1, d= 2)
test(3, 4, 5, c=1, d= 2, e = 12)
