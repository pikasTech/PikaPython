print("PikaPython V2 Windows ready")
print("中文输出: 正常")

import base64
import binascii
import hashlib
import hmac
import json
import math
import mqtt
import random
import re
import requests
import socket
import struct
import time
import zlib

print("module-math:", math.sqrt(81))
print("module-json:", json.dumps({"ready": True}))
print("module-re:", re.match("Pika.*", "PikaPython").group())
print("module-base64:", base64.b64encode(b"Pika"))
print("module-network:", socket.gethostbyname("localhost"))

total = 0
for value in range(1, 6):
    total += value

if total == 15:
    print("control-flow: ok")
else:
    print("control-flow: failed")

print("total:", total)
