
import requests

a = requests.request("GET", "http://pikascript.com/pullrequest", params = {"json":'{"stepIndex":4,"packageName":"None","id":1669300904995,"version":"v1234.4321.1","releases":[],"fileList":[],"userEmail":"test@pikascript.com","userName":"pikascript","pullrequestOK":false,"pullrequestUrl":"","uploadFileNum":1}'})

print(a.headers)
print(a.content_length)
print(a.text)