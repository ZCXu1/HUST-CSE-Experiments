#!/usr/bin/python3


# server
from socket import *
import math
import struct


g = 3
p = 65537
b = 150
Y_server = 7688
payload = struct.pack("iii",g, p, Y_server)

udp_srv = socket(AF_INET,SOCK_DGRAM)
udp_srv.bind(('10.0.2.7', 9090))
data, addr = udp_srv.recvfrom(1024)

print(addr)
info = struct.unpack("iii", data)
print('g = %d, p = %d, Y_client = %d' %(info[0], info[1], info[2]))
print('send the reply')
udp_srv.sendto(payload, addr)

Y_client = info[2]
key = (Y_client ** b)%p
print("Our key is %s" %key)
