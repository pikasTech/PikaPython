def tryfn(a):
    if a == 1:
        print('a:1')
        raise
        print('a:1,after raise')
        return
    if a == 0:
        print('a:0')
        return
try:
    tryfn(1)
    tryfn(0)
except:
    print('in except')
