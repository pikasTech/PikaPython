import _requests


class Response(_requests.Response):
    ...


def _append_params_to_url(url: str, params: dict) -> str:
    if params is None:
        return url
    if '?' in url:
        url += '&'
    else:
        url += '?'
    for k, v in params.items():
        k = _requests.urlencode(str(k))
        v = _requests.urlencode(str(v))
        url += '%s=%s&' % (k, v)
    return url[:-1]


def request(method: str, url: str, params=None) -> Response:
    url = _append_params_to_url(url, params)
    return _requests.request(method, url)


def get(url: str, params=None) -> Response:
    return request('GET', url, params)


def __init__():
    _requests.__init__()


def __del__():
    _requests.__del__()
