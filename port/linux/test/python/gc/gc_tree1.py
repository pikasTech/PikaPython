
class Tree:
    parent = None
    child = []

t1 = Tree()
t2 = Tree()
t1.child.append(t2)
del t2

gcdump()
