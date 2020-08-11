# TCP Server
import socket
import logging
import time
import numpy as np
from threading import Thread
import struct




logging.basicConfig(format = u'[LINE:%(lineno)d]# %(levelname)-8s [%(asctime)s]  %(message)s', level = logging.NOTSET)

#sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM, proto=socket.IPPROTO_TCP)

#port = 10000
#adresa = 'localhost'
#server_address = (adresa, port)

#sock.bind(server_address)
#logging.info("Serverul a pornit pe %s si portnul portul %d", adresa, port)
#sock.listen(5)
#while True:
#    logging.info('Asteptam conexiui...')
#    conexiune, address = sock.accept()
#    logging.info("Handshake cu %s", address)
#    time.sleep(2)
#    data = conexiune.recv(1024)
#    logging.info('Content primit: "%s"', data)
#    conexiune.send(b"Server a primit mesajul: " + data)
#    conexiune.close()
#sock.close()

class Server:
    stopServer = 0
    addresses = []
    def __init__(self,ip,port):
        self.ip = ip
        self.port = port
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM, proto=socket.IPPROTO_TCP)
        self.server_address = (ip, port)
        self.sock.bind(self.server_address)
        self.sock.listen(5)

    def SendData(self,data):
        for i in range(len(self.addresses)):
            self.addresses[i][0].send(data)

    def FindAddr(self,conexiune,address,data):
        if len(data) == 18:
            (bHandShake, pUinqueID, pFrequency) = struct.unpack("!bLL", data[:9])
            payload = data[9:]
            if payload != b'handshake':
                return 0
            for i in range(len(self.addresses)):
                if address[0] == self.addresses[i][1][0]:
                    if pUinqueID != self.addresses[i][2]:
                        self.addresses.append((conexiune, address, pUinqueID, pFrequency))
                        return 1
                    else:
                        return 0
            self.addresses.append((conexiune, address, pUinqueID,pFrequency))
            return 1
        else:
            return 0
    def startServer(self):
        while True:
            try:
                conexiune, address = self.sock.accept()
                data = conexiune.recv(1024)
                if (self.FindAddr(conexiune, address, data) == 0):
                    self.SendData(data)
                    logging.info("Send Data")
                else:
                    logging.info("New Client")
            except:
                for i in range(len(self.addresses)):
                    try:
                        self.addresses[i][0].send(b"aliveevila")
                        data = self.addresses[i][0].recv(1024)
                    except:
                        for j in range(i, len(self.addresses) - 1):
                            self.addresses[j] = self.addresses[j + 1]
                        logging.info("rip")
                        self.addresses.pop()

    def CheckIfAlive(self):
        while True:
            time.sleep(10)
            for i in range(len(self.addresses)):
                try:
                    self.addresses[i][0].send(b"aliveevila")
                    #time.sleep(1)
                    data = self.addresses[i][0].recv(1024)
                except:
                    for j in range(i, len(self.addresses) - 1):
                         self.addresses[j] = self.addresses[j + 1]
                    logging.info("rip")
                    self.addresses.pop()



    def stopServer(self):
        self.sock.close()

sv = Server("localhost", 10000)
thread1 = Thread(target=sv.CheckIfAlive)
thread2 = Thread(target=sv.startServer)
thread2.start()
thread1.start()

