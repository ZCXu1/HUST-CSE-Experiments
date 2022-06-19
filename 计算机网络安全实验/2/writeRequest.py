#!/usr/bin/python
from scapy.all import *

IPpacket = IP(dst='172.17.0.3', src='172.17.0.2') 
UDPpacket = UDP(sport=33333, dport=53, chksum=0)
targetName = 'twysw.example.com'
Querysection = DNSQR(qname=targetName)
DNSpacket = DNS(rd=1, qdcount=1, qd=Querysection)
QueryPacket = IPpacket/UDPpacket/DNSpacket

with open('ip_req.bin','wb') as f:
    f.write(bytes(QueryPacket))
