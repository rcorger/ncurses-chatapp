#!/bin/python3
import socket
from sys import argv
import threading

uname = argv[1]
print(uname)

def recv_messages(server_fd):
    while True:
        msg = server_fd.recv(2048)
        print("\n" + msg.decode('utf-8') + "\n")

try:
    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client.connect(('127.0.0.1', 1393))
    
    client.send(f"{uname}:".encode("utf-8"))

    thread = threading.Thread(target=recv_messages, args=(client,))
    thread.start()

    while True:
        to_send = input("> ").encode("utf-8") 
        client.send(to_send)
except KeyboardInterrupt:
    client.close()

