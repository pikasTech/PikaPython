import requests
form_data = '------WebKitFormBoundaryrEPACvZYkAbE4bYB\r\nContent-Disposition: form-data; name="file"; filename="test_file.txt"\r\nContent-Type: text/plain\r\n\r\nhello, pikascript!\r\n------WebKitFormBoundaryrEPACvZYkAbE4bYB\r\nContent-Disposition: form-data; name="id"\r\n\r\n1670666272201\r\n------WebKitFormBoundaryrEPACvZYkAbE4bYB\r\nContent-Disposition: form-data; name="uploadFileNum"\r\n\r\n1\r\n------WebKitFormBoundaryrEPACvZYkAbE4bYB--\r\n'

header = {"Content-Type": "multipart/form-data; boundary=----WebKitFormBoundaryrEPACvZYkAbE4bYB"}

a = requests.request("POST", "http://pikascript.com/upload", headers=header, data=form_data)

print(a.headers)
print(a.content_length)
print(a.text)