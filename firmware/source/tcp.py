import socket

HOST = "net-bmp"  # The server's hostname or IP address
PORT = 2000  # The port used by the server

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    s.sendall(b"Hello, world")

    data = s.recv(1)

print(f"Received {data!r}")
