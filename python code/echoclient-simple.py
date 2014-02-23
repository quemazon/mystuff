#!/usr/bin/env python

"""
A simple echo client
"""

import socket
##import androidhelper
##droid = androidhelper.Android()

host = '192.168.10.124'
port = 50000
size = 1024
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((host,port))
s.send('Hello, world, sweet!')
data = s.recv(size)
s.close()
##droid.makeToast(data)
print(data)