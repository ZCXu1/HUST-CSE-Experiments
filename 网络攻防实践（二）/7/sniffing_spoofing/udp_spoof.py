#!/usr/bin/python3
from scapy.all import *

print("SENDING SPOOFED UDP PACKET.........")
ip  = IP(src="1.2.3.4", dst="10.0.2.7")
udp = UDP(sport=8000, dport=9090)
data = "Hello UDP!\n"
pkt = ip/udp/data
#pkt.show()
send(pkt,verbose=0)
