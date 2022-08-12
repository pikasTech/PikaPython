f = open('test/out/file3.txt', 'w')
seq = [
    'This is the first line.\n',
    'This is the second line.\n',
    'This is the third line.\n'
]
f.writelines(seq)
f.close()
