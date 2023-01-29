import random
from socket import *
from math_base import *

host = '10.0.2.7'
port = 9000

p = 65537
g = 3
a = 100
key = 0

udp_client = socket(AF_INET, SOCK_DGRAM)
udp_client.bind(('192.168.60.2', port))
p_g = 'send_p_g  ' + str(p) + ',' + str(g)
udp_client.sendto(p_g.encode('utf-8'), (host, port))


while 1:
    data, addr = udp_client.recvfrom(1224)

    head = data[:10]
    body = data[10:]

    print("sending the request...")

    if head == 'send_Yb   ':
        Ya = power_mod(g, a, p)
        print('a:', a, ',Ya:', Ya)
        send_Ya = 'send_Ya   ' + str(Ya)
        s = socket(AF_INET, SOCK_DGRAM)
        s.sendto(send_Ya.encode('utf-8'), (host, port))

        Yb = int(body)
        key = power_mod(Yb, a, p)
        print('The Key is: ', key)
