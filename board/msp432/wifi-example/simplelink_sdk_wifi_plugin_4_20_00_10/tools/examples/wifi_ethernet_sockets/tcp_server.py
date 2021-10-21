#!/usr/bin/env python

# This script creates a continually running server in a command line.
# Clients that connect to this server send 'hello' and this script sends
# back to them 'world-n' where n is the number of clients that have connected
# and disconnected to the server while the server has been running.

import socket

from collections import deque

# Set this IP address to your computer's IP address on the network
TCP_IP = '192.168.0.102'

# This port number is arbitrary, but make sure it matches the
# port number in the wifi_ethernet_sockets.h file, also called TCPPORT
TCP_PORT = 5005
BUFFER_SIZE = 1  # Normally 1024, but we want fast response

i = 1
data = deque([])
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((TCP_IP, TCP_PORT))

try:
    while True:
        s.listen(1)
        conn, addr = s.accept()
        print('Connection address:', addr)
        while True:
            rx_data = conn.recv(BUFFER_SIZE)
            if not rx_data: 
                break
            data.append(rx_data)
            print("received data:", data[len(data)-1])
            if len(data) > 5:
                data.popleft()
            if data == deque([b"h", b"e", b"l", b"l", b"o"]):
                conn.send(str.encode('world-{}'.format(i)))
        print('Connection closed:', addr)
        conn.close()
        i += 1
except KeyboardInterrupt:
    exit()