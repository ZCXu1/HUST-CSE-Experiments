import random
from socket import *
from math_base import *


host = gethostname()
port = 9000

udp_srv = socket(AF_INET, SOCK_DGRAM)
udp_srv.bind((host, port))

p = 65537
g = 3
b = 150
key = 0

while 1:
    data, addr = udp_srv.recvfrom(1224)

    head = data[:10]
    body = data[10:]

    print("sending the reply...")

    if head == 'send_p_g  ':
        print('get p:', p, ',g:', g)
        Yb = power_mod(g, b, p)
        print('b:', b, ',Yb:', Yb, )
        send_Yb = 'send_Yb   ' + str(Yb)
        udp_srv.sendto(send_Yb.encode('utf-8'), addr)
    elif head == 'send_Ya   ':
        Ya = int(body)
        print("Ya,p,b", Ya, p, b)
        key = power_mod(Ya, b, p)
        print('The Key is: ', key)
    elif head == 'message   ':
        c = int(body)
        print('get message', c)
