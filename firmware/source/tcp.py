import socket
from time import sleep

HOST = "net-bmp"  # The server's hostname or IP address
PORT = 2000  # The port used by the server

cnt = 0
while True:
    cnt += 1

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))

        s.sendall(b"Hello, world")
        s.sendall(b"Is it fu")

        data = s.recv(20)
        # data2 = s.recv(1024)

        print(f"Received {data!r}")
        # print(f"Received {data2!r}")
        print(f"{cnt=}")

    sleep(1)
