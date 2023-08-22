import requests

a = requests.request("GET", "http://pikascript.com/")

print(a.headers)
print(a.content_length)
print(a.text)

