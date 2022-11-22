class Response:
    content_length: int
    text: str
    state_code: int
    headers: dict
    url: str

    def json(self) -> dict: ...


def request(method: str, url: str) -> Response: ...

def urlencode(s: str) -> str: ...


def __init__(): ...
def __del__(): ...
