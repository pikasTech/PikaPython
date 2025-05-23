import _requests


class Response(_requests.Response):
    def __init__(self):
        super().__init__()


def _append_params_to_url(rqst: Response, url: str, params: dict) -> int:
    if params is None:
        ret = rqst.urlencode_write(url, '', '', '')
        return 1
    if '?' in url:
        first_connect = '&'

    else:
        first_connect = '?'
    # 初始化连接url
    ret = rqst.urlencode_write(url, '', '', '')
    if ret != 1:
        return ret
    count = 0
    connect = '='
    start = '&'
    for k, v in params.items():
        if count == 0:
            ret = rqst.urlencode_write(str(k), str(v), first_connect, connect)
            if ret != 1:
                return ret
            count += 1
        else:
            ret = rqst.urlencode_write(str(k), str(v), start, connect)
            if ret != 1:
                return ret
    return 1


def _append_headers(rqst: Response, headers: dict) -> int:
    if headers is None:
        return 1
    for k, v in headers.items():
        # print("\nheaders:", str(k), "=", str(v),"\n")
        ret = rqst.header_write(str(k), str(v))
        if ret != 1:
            return ret

    return 1


def request(
        method: str,
        url: str,
        params=None,
        headers=None,
        timeout=0.0,
        files=None,
        json=None,
        data=None) -> Response:
    if files is not None:
        print("files is not supported")
        return None
    if json is not None:
        print("json is not supported")
        return None
    """ 
    初始化请求对象，分配内存和固定请求头 
    """
    rqst = Response()
    rqst.url = url
    # 初始化，分配内存, 写入方法POST/GET
    ret = rqst.request_init(method)
    # print("Request init ret: " + str(ret))
    if ret != 1:
        print("Failed to initialize the request object.")
        return None

    # 写入URL
    ret = _append_params_to_url(rqst, url, params)
    # print("Append params to URL ret: " + str(ret))
    if ret != 1:
        # 出现错误，需要释放对象
        print("Error appending params to the URL.")
        return None

    # 写入默认HTTP版本号
    ret = rqst.proto_write('')
    # print("Write HTTP version ret: " + str(ret))
    if ret != 1:
        print("Error writing HTTP version.")
        return None

    # 写入响应头数据
    ret = _append_headers(rqst, headers)
    # print("Append headers ret: " + str(ret))
    if ret != 1:
        print("Error appending headers to the request.")
        return None

    ret = rqst.request(method, rqst.url, timeout, data)
    # print("Request ret: " + str(ret))
    if ret != 1:
        print("Request failed with response code: " + str(ret))
        return None

    return rqst


def get(url: str, params=None) -> Response:
    return request('GET', url, params)
