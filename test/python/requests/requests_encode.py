import requests

a = requests.request("GET", 'http://www.rt-thread.com')

print(a.headers)
print(a.content)