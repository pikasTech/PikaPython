
def log(level: str, title: str, *var):
    print('---------  ', title, '  ---------')
    for i in var:
        print('[%r]' % level, i)

log("test")
