# TCP client
import socket
import logging
import time
import sys
from struct import *
from threading import Thread
import numpy as np
import random

uniqueID = random.randrange(0, sys.maxsize)
frequency = 2800

logging.basicConfig(format=u'[LINE:%(lineno)d]# %(levelname)-8s [%(asctime)s]  %(message)s', level=logging.NOTSET)

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM, proto=socket.IPPROTO_TCP)

port = 10000
adresa = 'localhost'
server_address = (adresa, port)
mesaj = "Uite mesajul"


def SendMessage(content):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM, proto=socket.IPPROTO_TCP)
    sock.connect(server_address)
    sock.send(content.encode('utf-8'))
    sock.close()


sock_recv = socket.socket(socket.AF_INET, socket.SOCK_STREAM, proto=socket.IPPROTO_TCP)


def InitRecvMessages():
    sock_recv.connect(('localhost', 10000))
    init_pack = pack("!bLL", 1, uniqueID, frequency)
    init_pack += b'handshake'
    sock_recv.send(init_pack)
    data = sock_recv.recv(1024)
    return 1


def ReceiveMessages():
    try:
        data = sock_recv.recv(1024)
        if len(data) > 0 and data != b'aliveevila':
            print(data)
            return data
        else:
            return ""
    except:
        return ""


InitRecvMessages()
while True:
    print(ReceiveMessages())