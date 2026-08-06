import os
import requests

descriptor = os.open(".http-port", os.O_RDONLY)
port = int(os.read(descriptor, 16).decode())
os.close(descriptor)
response = requests.get("http://127.0.0.1:" + str(port) + "/health")
print("requests-status:", response.status_code)
print("requests-body:", response.text)
