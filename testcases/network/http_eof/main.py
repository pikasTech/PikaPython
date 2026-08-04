import requests
from network_fixture import HOST, PORT


url = "http://" + HOST + ":" + str(PORT) + "/eof"
response = requests.get(url, timeout=1)
print(response.status_code)
print(response.headers["x-mode"])
print(response.content)
print(response.text)
