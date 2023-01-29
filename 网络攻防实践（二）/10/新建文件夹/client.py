#!/usr/bin/python3

from socket import *
import math
import struct

g = 3
p = 65537
a = 100
Y_client = 33330
payload = struct.pack("iii",g, p, Y_client)

udp_client = socket(AF_INET, SOCK_DGRAM)
udp_client.sendto(payload.encode('utf-8'),('10.0.2.7',9090))

data, addr = udp_client.recvfrom(1024)

info = struct.unpack("iii", data)
print('g : %d, p : %d, Yb : %d' %(info[0], info[1], info[2]))
print(addr)

Y_server = info[2]
key = (Y_server ** a)%p
print('Our key is %s' %key)
