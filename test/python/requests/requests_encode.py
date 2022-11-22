import requests
requests._append_params_to_url('http://www.rt-thread.com', {
    'a': 1,
    'b': 2
})

requests._append_params_to_url('http://www.rt-thread.com', {
    'a': ' ',
    'b': '%',
})
