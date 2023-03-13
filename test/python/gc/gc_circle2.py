class Tree:
    parent = None
    child = []


a = Tree()
b = Tree()
a.child.append(b)
b.parent = a
