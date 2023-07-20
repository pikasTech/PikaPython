import requests

b = "kkk"

a = requests.request("GET", "http://pikapython.com/packages", params = {"name":"get-test"})

print(a.headers)
print(a.content_length)
print(a.text)