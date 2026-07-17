def collect(**kwargs):
    return kwargs


kwargs = collect(alpha=1, beta=2, gamma=3)
assert str(kwargs) == "{'alpha': 1, 'beta': 2, 'gamma': 3}"
assert kwargs['alpha'] == 1
assert kwargs['beta'] == 2
assert kwargs['gamma'] == 3

kwargs = collect(short=1, keyword_name_longer_than_hash=2)
assert str(kwargs) == "{'short': 1, 'keyword_name_longer_than_hash': 2}"
assert kwargs['keyword_name_longer_than_hash'] == 2

print('PASS')
