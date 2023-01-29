#!/usr/bin/python3
from scapy.all import *

IP_victim    = ""
MAC_victim   = ""

IP_spoofed      = ""
MAC_spoofed     = ""

print("SENDING SPOOFED ARP REQUEST......")

ether = Ether()
ether.dst = 
ether.src = 

arp = ARP()
arp.psrc  = 
arp.hwsrc = 
arp.pdst  = 
arp.op = 1
frame = ether/arp
sendp(frame)

