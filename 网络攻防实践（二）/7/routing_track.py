import os,sys,time,subprocess
from scapy.as_resolvers import AS_resolver_radb  
from scapy.all import traceroute
domains='intel.com'
target = domains.split(' ')
dport = [80]
if len(target) >= 1 and target[0] != '':
    res, unans = traceroute(domains, minttl=1, maxttl=112, dport=dport)
    time.sleep(1)
else:
    print("IP/domain number of errors, exit")
