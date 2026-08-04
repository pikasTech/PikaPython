# SPEC: PJ2026-050109 network-ready v0.2; pure Python HTTP client.
import socket
from pika_http_config import PIKA_HTTP_BODY_LIMIT, PIKA_HTTP_HEADER_BYTES_LIMIT, PIKA_HTTP_HEADER_COUNT_LIMIT, PIKA_HTTP_PATH_LIMIT, PIKA_HTTP_RECV_LIMIT, PIKA_HTTP_URL_LIMIT


class Response:
    def __init__(self, status_code, headers, content):
        self.status_code = status_code
        self.headers = headers
        self.content = content
        self.text = content.decode()


def _parse_url(url):
    if len(url) > PIKA_HTTP_URL_LIMIT:
        raise ValueError("URL exceeds PIKA_HTTP_URL_LIMIT")
    if not url.startswith("http://"):
        raise ValueError("unsupported URL scheme")
    remainder = url[7:]
    slash = remainder.find("/")
    if slash < 0:
        authority = remainder
        path = "/"
    else:
        authority = remainder[:slash]
        path = remainder[slash:]
    if len(authority) == 0:
        raise ValueError("URL host is empty")
    if len(path) > PIKA_HTTP_PATH_LIMIT:
        raise ValueError("path exceeds PIKA_HTTP_PATH_LIMIT")
    colon = authority.find(":")
    if colon < 0:
        host = authority
        port = 80
    else:
        host = authority[:colon]
        port_text = authority[colon + 1:]
        if len(host) == 0 or len(port_text) == 0:
            raise ValueError("URL authority is invalid")
        port = int(port_text)
    if port < 0 or port > 65535:
        raise ValueError("URL port is outside 0..65535")
    return (host, port, path, authority)


def _append_user_headers(request, headers):
    if headers is None:
        return request
    for name in headers:
        value = headers[name]
        lower_name = name.lower()
        if (
            len(name) == 0
            or name.find(":") >= 0
            or name.find("\r") >= 0
            or name.find("\n") >= 0
            or value.find("\r") >= 0
            or value.find("\n") >= 0
        ):
            raise ValueError("HTTP header contains an invalid boundary")
        if lower_name == "host" or lower_name == "connection":
            raise ValueError("HTTP header is reserved")
        if (
            len(request) + len(name) + len(value) + 4
            > PIKA_HTTP_HEADER_BYTES_LIMIT
        ):
            raise ValueError(
                "request exceeds PIKA_HTTP_HEADER_BYTES_LIMIT"
            )
        request += name + ": " + value + "\r\n"
    return request


def _build_request(path, authority, headers):
    request = "GET " + path + " HTTP/1.0\r\n"
    request += "Host: " + authority + "\r\n"
    request += "Connection: close\r\n"
    request = _append_user_headers(request, headers)
    request += "\r\n"
    if len(request) > PIKA_HTTP_HEADER_BYTES_LIMIT:
        raise ValueError(
            "request exceeds PIKA_HTTP_HEADER_BYTES_LIMIT"
        )
    return request.encode()


def _read_header(connection):
    data = b""
    marker = data.find(b"\r\n\r\n")
    while marker < 0:
        block = connection.recv(PIKA_HTTP_RECV_LIMIT)
        if len(block) == 0:
            raise ValueError("HTTP response header is truncated")
        data += block
        if len(data) > PIKA_HTTP_HEADER_BYTES_LIMIT:
            raise ValueError(
                "response exceeds PIKA_HTTP_HEADER_BYTES_LIMIT"
            )
        marker = data.find(b"\r\n\r\n")
    return (data[:marker], data[marker + 4:])


def _parse_header(header_bytes):
    for value in header_bytes:
        if value > 127:
            raise ValueError("HTTP response header is not ASCII")
    header_text = header_bytes.decode()
    first_end = header_text.find("\r\n")
    if first_end < 0:
        status_line = header_text
        offset = len(header_text)
    else:
        status_line = header_text[:first_end]
        offset = first_end + 2
    parts = status_line.split(" ", 2)
    if (
        len(parts) < 2
        or (
            parts[0] != "HTTP/1.0"
            and parts[0] != "HTTP/1.1"
        )
        or len(parts[1]) != 3
    ):
        raise ValueError("HTTP status line is invalid")
    status_code = int(parts[1])
    if status_code < 100 or status_code > 999:
        raise ValueError("HTTP status code is invalid")

    headers = {}
    count = 0
    while offset < len(header_text):
        line_end = header_text.find("\r\n", offset)
        if line_end < 0:
            line_end = len(header_text)
        line = header_text[offset:line_end]
        colon = line.find(":")
        if colon <= 0:
            raise ValueError("HTTP response header line is invalid")
        name = line[:colon].lower()
        value = line[colon + 1:]
        while value.startswith(" ") or value.startswith("\t"):
            value = value[1:]
        count += 1
        if count > PIKA_HTTP_HEADER_COUNT_LIMIT:
            raise ValueError(
                "response exceeds PIKA_HTTP_HEADER_COUNT_LIMIT"
            )
        if name == "content-length" and name in headers:
            raise ValueError("duplicate Content-Length")
        headers[name] = value
        offset = line_end + 2
    if "transfer-encoding" in headers:
        raise ValueError("Transfer-Encoding is unsupported")
    return (status_code, headers)


def _read_body(connection, headers, initial):
    body = initial
    if "content-length" in headers:
        content_length = int(headers["content-length"])
        if content_length < 0:
            raise ValueError("Content-Length is negative")
        if content_length > PIKA_HTTP_BODY_LIMIT:
            raise ValueError("body exceeds PIKA_HTTP_BODY_LIMIT")
        while len(body) < content_length:
            remaining = content_length - len(body)
            receive_size = PIKA_HTTP_RECV_LIMIT
            if remaining < receive_size:
                receive_size = remaining
            block = connection.recv(receive_size)
            if len(block) == 0:
                raise ValueError("HTTP body ended before Content-Length")
            body += block
        return body[:content_length]

    if len(body) > PIKA_HTTP_BODY_LIMIT:
        raise ValueError("body exceeds PIKA_HTTP_BODY_LIMIT")
    block = connection.recv(PIKA_HTTP_RECV_LIMIT)
    while len(block) > 0:
        if len(body) + len(block) > PIKA_HTTP_BODY_LIMIT:
            raise ValueError("body exceeds PIKA_HTTP_BODY_LIMIT")
        body += block
        block = connection.recv(PIKA_HTTP_RECV_LIMIT)
    return body


def get(url, headers=None, timeout=5):
    host, port, path, authority = _parse_url(url)
    request = _build_request(path, authority, headers)
    connection = socket.socket()
    try:
        connection.settimeout(timeout)
        connection.connect((host, port))
        connection.sendall(request)
        header_bytes, initial = _read_header(connection)
        status_code, response_headers = _parse_header(header_bytes)
        content = _read_body(
            connection,
            response_headers,
            initial,
        )
        return Response(status_code, response_headers, content)
    finally:
        connection.close()
