info = [
#   [ Age,   'Name',   'Place' ]
    [ 26,    'Jack',   'River' ],
    [ 32,    'Riven',  'England' ],
]

dict = {
#   'Name':  'Job'
    'Jack':  'Unemployed',
    'Riven': 'League of Legends'
}

#   [ Age,   'Name',   'Place' ]
info2 = [
    [ 26,    'Jack',   'River' ],
    [ 32,    'Riven',  'England' ],
]

#   'Name':  'Job'
dict2 = {
    'Jack':  'Unemployed',
    'Riven': 'League of Legends'
}

assert info[0][1] == 'Jack'
assert info[1][1] == 'Riven'
assert dict['Jack'] == 'Unemployed'
assert dict['Riven'] == 'League of Legends'
assert len(info) == 2
assert len(dict) == 2

assert info2[0][1] == 'Jack'
assert info2[1][1] == 'Riven'
assert dict2['Jack'] == 'Unemployed'
assert dict2['Riven'] == 'League of Legends'
assert len(info2) == 2
assert len(dict2) == 2
print('PASS')
