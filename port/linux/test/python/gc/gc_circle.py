class Tree:
    parent = None
    child = []

def test():
    a = Tree()
    b = Tree()
    a.child.append(b)
    b.parent = a

test()
c = Tree()
print('end')
