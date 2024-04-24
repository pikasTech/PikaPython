import socket
import time


def http_download_file(url: str, file_path: str, buff_size=1024):
    # Parse the URL to extract the host and path
    if not url.startswith('http://'):
        print("Only HTTP protocol is supported")
        return -1
    host = url.split("//")[1].split('/')[0]
    path = url[url.find(host) + len(host):]
    if len(path) == 0:
        path = "/"

    print("Host:", host)
    print("Path:", path)
    # Establish a socket connection
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((host, 80))
        print("Connected to:", host)
    except Exception as e:
        print("Connection error:", e)
        return -1

    # Send HTTP GET request
    get_request = "GET " + path + " HTTP/1.1\r\nHost: " + \
        host + "\r\nConnection: close\r\n\r\n"
    sock.send(get_request.encode())

    # Open file to write
    f = open(file_path, 'wb')  # Manually open the file
    data_received = False
    head_received = False
    while True:
        try:
            data = sock.recv(buff_size)
        except:
            print('End of data')
            break
        print("[Data received:", len(data), ']')
        if head_received:
            sz = f.write(data)
            # print("Data written:", sz)
        # print(data.decode())
        if len(data) == 0:
            print("Length of data:", len(data))
            if not data_received:
                print("No data received.")
                f.close()
                return -1
            print("No more data to receive")
            break
        data_received = True

        # Handle the end of the HTTP header if it's still present
        if head_received == False:
            if b'\r\n\r\n' in data:
                # print("Header received", data)
                head_received = True
                aplited = data.split(b'\r\n\r\n', 1)
                if len(aplited) == 2:
                    sz = f.write(aplited[1])

    # Close file and socket manually
    f.close()
    sock.close()
    print("Download completed")
    return 0


assert http_download_file("http://pikapython.com", "pikapython.html") == 0
print("PASS")
