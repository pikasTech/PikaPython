import requests
from network_fixture import HOST, PORT


BASE_URL = "http://" + HOST + ":" + str(PORT)


response = requests.get(
    BASE_URL + "/sensor",
    {"X-Token": "alpha"},
    1,
)
print(response.status_code)
print(response.headers["content-type"])
print(response.headers["x-test"])
print(response.content)
print(response.text)


class ApiClient:
    def __init__(self, base_url):
        self.base_url = base_url
        self.headers = {"X-Client": "device"}

    def get(self, path):
        return requests.get(
            self.base_url + path,
            self.headers,
            1,
        )


client = ApiClient(BASE_URL)
print(client.get("/status").status_code)
