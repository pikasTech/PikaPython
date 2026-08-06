#!/usr/bin/env python3
import shutil
import socket
import subprocess
import sys
import tempfile
import threading
from pathlib import Path

import yaml


TIME_REFERENCE = """import time
print("sleep-zero:", time.sleep(0) is None)
print("sleep-bool:", time.sleep(False) is None)
try:
    time.sleep(-1)
    print("sleep-negative: accepted")
except ValueError:
    print("sleep-negative: rejected")
"""
TIME_EXTENSION = b"""extension-sleep: 7
extension-sleep-ms: 30
extension-wrap-positive: 5
extension-wrap-negative: -5
"""
EXPECTED = {
    "main.py": b"PikaPython module suite ready\n",
    "case_aes.py": b"""aes-128-ecb: True True
aes-192-ecb: True True
aes-256-ecb: True True
aes-128-cbc: True True
invalid-key: rejected
missing-iv: rejected
invalid-block: rejected
invalid-mode: rejected
""",
    "case_fuzzypid.py": b"""FUZZYPID_CONSTRUCTOR_RECOVERY_OK
FUZZYPID_VECTOR_OK
""",
    "case_requests.py": b"""requests-status: 200
requests-body: pika-http-ok
""",
    "case_mqtt.py": b"mqtt-client: connected published pinged\n",
    "case_os.py": b"""os-read: pika
os-kind: True True
os-listdir: ['renamed.txt']
""",
    "case_unittest.py": b"""unittest-stats: 4 1 1 True
unittest-success: False
""",
    "case_modbus.py": b"""modbus-crc: True
modbus-rtu-bytes: 8
modbus-tcp-header: 0 7 0 6
modbus-registers: [1, 4660]
""",
}


def run(command, root):
    return subprocess.run(
        command,
        cwd=root,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        timeout=30,
    )


def cpython_case(root, name):
    with tempfile.TemporaryDirectory() as directory:
        isolated = Path(directory)
        shutil.copy2(root / name, isolated / "main.py")
        return run([sys.executable, "main.py"], isolated)


def receive_mqtt_packet(connection):
    first = connection.recv(1)
    if len(first) != 1:
        raise RuntimeError("MQTT packet type is missing")
    multiplier = 1
    remaining = 0
    while True:
        encoded = connection.recv(1)
        if len(encoded) != 1:
            raise RuntimeError("MQTT remaining length is truncated")
        remaining += (encoded[0] & 127) * multiplier
        if encoded[0] & 128 == 0:
            break
        multiplier *= 128
    body = b""
    while len(body) < remaining:
        block = connection.recv(remaining - len(body))
        if len(block) == 0:
            raise RuntimeError("MQTT packet body is truncated")
        body += block
    return first[0], body


def start_http_server(root, errors):
    listener = socket.socket()
    listener.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    listener.bind(("127.0.0.1", 0))
    listener.listen(1)
    listener.settimeout(10)
    port = listener.getsockname()[1]
    (root / ".http-port").write_text(str(port), encoding="ascii")

    def serve():
        try:
            connection, _ = listener.accept()
            with connection:
                connection.settimeout(5)
                request = b""
                while b"\r\n\r\n" not in request:
                    block = connection.recv(512)
                    if len(block) == 0:
                        break
                    request += block
                if not request.startswith(b"GET /health HTTP/1."):
                    raise RuntimeError("unexpected HTTP request: %r" % request)
                body = b"pika-http-ok"
                connection.sendall(
                    b"HTTP/1.0 200 OK\r\nContent-Length: 12\r\n"
                    b"Content-Type: text/plain\r\n\r\n" + body
                )
        except Exception as error:
            errors.append(str(error))
        finally:
            listener.close()

    thread = threading.Thread(target=serve, daemon=True)
    thread.start()
    return thread


def start_mqtt_server(root, errors):
    listener = socket.socket()
    listener.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    listener.bind(("127.0.0.1", 0))
    listener.listen(1)
    listener.settimeout(10)
    port = listener.getsockname()[1]
    (root / ".mqtt-port").write_text(str(port), encoding="ascii")

    def serve():
        try:
            connection, _ = listener.accept()
            with connection:
                connection.settimeout(5)
                packet_type, _ = receive_mqtt_packet(connection)
                if packet_type != 16:
                    raise RuntimeError("expected MQTT CONNECT")
                connection.sendall(b"\x20\x02\x00\x00")
                packet_type, body = receive_mqtt_packet(connection)
                if packet_type != 48 or not body.endswith(b"hello"):
                    raise RuntimeError("expected MQTT PUBLISH")
                packet_type, _ = receive_mqtt_packet(connection)
                if packet_type != 192:
                    raise RuntimeError("expected MQTT PINGREQ")
                connection.sendall(b"\xd0\x00")
                packet_type, _ = receive_mqtt_packet(connection)
                if packet_type != 224:
                    raise RuntimeError("expected MQTT DISCONNECT")
        except Exception as error:
            errors.append(str(error))
        finally:
            listener.close()

    thread = threading.Thread(target=serve, daemon=True)
    thread.start()
    return thread


def expected_output(root, boot_entry):
    if boot_entry == "case_time.py":
        with tempfile.TemporaryDirectory() as directory:
            reference = run(
                [sys.executable, "-c", TIME_REFERENCE],
                Path(directory),
            )
        if reference.returncode != 0 or reference.stderr:
            raise RuntimeError(
                "CPython time reference failed: rc=%d stdout=%r stderr=%r"
                % (reference.returncode, reference.stdout, reference.stderr)
            )
        return reference.stdout + TIME_EXTENSION, "CPython common and V2 extension"
    if boot_entry in {
        "case_os.py", "case_re.py", "case_socket.py", "case_unittest.py"
    }:
        reference = cpython_case(root, boot_entry)
        if reference.returncode != 0 or reference.stderr:
            raise RuntimeError(
                "CPython reference failed: rc=%d stdout=%r stderr=%r"
                % (reference.returncode, reference.stdout, reference.stderr)
            )
        return reference.stdout, "CPython common"
    if boot_entry in EXPECTED:
        return EXPECTED[boot_entry], "V2 extension"
    raise ValueError(
        "unsupported bootEntry for this verifier: %s" % boot_entry
    )


def main():
    root = Path(__file__).resolve().parents[1]
    config = yaml.safe_load(
        (root / "pikapython.yaml").read_text(encoding="utf-8")
    )
    boot_entry = config.get("bootEntry", "main.py")
    program = root / ".pikapython" / "build" / "linux" / "linux-console"
    if not program.is_file():
        print("Linux program is missing; run pikapython-cli build", file=sys.stderr)
        return 1
    try:
        expected, verification = expected_output(root, boot_entry)
    except (RuntimeError, ValueError) as error:
        print(str(error), file=sys.stderr)
        return 1
    server_errors = []
    server_thread = None
    if boot_entry == "case_requests.py":
        server_thread = start_http_server(root, server_errors)
    elif boot_entry == "case_mqtt.py":
        server_thread = start_mqtt_server(root, server_errors)
    if boot_entry == "case_os.py":
        shutil.rmtree(root / ".os-case", ignore_errors=True)
    actual = run([str(program)], root)
    if server_thread is not None:
        server_thread.join(timeout=10)
    for temporary in (root / ".http-port", root / ".mqtt-port"):
        temporary.unlink(missing_ok=True)
    shutil.rmtree(root / ".os-case", ignore_errors=True)
    if server_errors:
        print("Server error: %s" % server_errors[0], file=sys.stderr)
        return 1
    if actual.returncode != 0 or actual.stderr or actual.stdout != expected:
        print("Expected: %r" % expected, file=sys.stderr)
        print("PikaPython: rc=%d stdout=%r stderr=%r" % (
            actual.returncode, actual.stdout, actual.stderr), file=sys.stderr)
        return 1
    print("%s behavior verified for bootEntry=%s" % (
        verification, boot_entry
    ))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
