class Response:
    content_length: int
    text: str
    state_code: int
    headers: dict
    url: str

    def json(self) -> dict: ...


def request(method: str, url: str, **kwargs) -> Response: ...


def get(url: str, **kwargs) -> Response: ...


def __init__(): ...
def __del__(): ...
