# TCP client
import socket
import logging
import time
import sys
from struct import *
from threading import Thread
import numpy as np
import random
 
def SendMessage(args):
    port = 10000
    adresa = 'localhost'
    server_address = (adresa, port)
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM, proto=socket.IPPROTO_TCP)
    sock.connect(server_address) 
    sock.send(args.encode('utf-8'))
    sock.close()
    
    
    
def InitRecvMessages():
    frequency = 2800
    uniqueID = random.randrange(0, sys.maxsize)
    port = 10000
    adresa = 'localhost'
    server_address = (adresa, port)
    sock_recv = socket.socket(socket.AF_INET, socket.SOCK_STREAM, proto=socket.IPPROTO_TCP)
    sock_recv.connect(('localhost',10000))
    init_pack = pack("!bLL",1,uniqueID,frequency)
    init_pack += b'handshake'
    sock_recv.send(init_pack)
    data = sock_recv.recv(1024) 
    return sock_recv
    
def ReceiveMessages(socky):
    try:
        data = socky.recv(1024)
        if len(data) > 0:
            return data
        else:
            return "text"
    except:
        return ""
 