# 代码来源

本仓库代码来自于下方链接，整理了使用过程中的问题和注意事项。

https://github.com/RicheyJang/RJFireWall

# 环境配置

参考

https://github.com/GuanyunFeng/MyFireWall-netfilter（注意在内外网互相ping之前可以先用firewall ping一遍内网和外网）

新建三个虚拟机(系统版本Ubuntu16.04，内核版本：3.13.0-32-generic)，Internal，Firewall，External，分别为内网主机，防火墙，外网主机。

运行VMWARE，编辑->虚拟网络编辑器->更改设置。添加虚拟网卡VMnet2(192.168.164.0/24)和VMnet3(192.168.152.0/24)。添加后的配置如下：

![image-20220804093238564](%E9%AA%8C%E6%94%B6.assets/image-20220804093238564.png)

“右键虚拟机->设置->添加->网络适配器“可以为虚拟机添加网卡。内网主机添加虚拟网卡VMnet2，外网主机添加虚拟网卡VMnet3，防火墙添加VMnet2和VMnet3两个网卡。

## 外网配置

**先在NAT模式下完成apache安装！** “右键虚拟机->设置->网络适配器“改为NAT模式，重启。

![image-20220804103317875](%E9%AA%8C%E6%94%B6.assets/image-20220804103317875.png)

安装apache，开启web服务。并可以通过修改`/var/www/index.html`来修改网页的内容。安装完成后打开浏览器，访问127.0.0.1，如果能正常访问，说明web服务已经开启。命令如下：

```
sudo apt-get install apache2
sudo /etc/init.d/apache2 start
```

![image-20220804103009122](%E9%AA%8C%E6%94%B6.assets/image-20220804103009122.png)

**安装完了切换网卡**，“右键虚拟机->设置->添加->网络适配器“可以为虚拟机添加网卡

![image-20220804103346162](%E9%AA%8C%E6%94%B6.assets/image-20220804103346162.png)

进入外网主机对应的虚拟机系统，点击右上角设置标志，"system setting->Network->options->IPv4 Settings"设置静态ip地址为：`192.168.152.2`，并将其网关设置为`192.168.152.1`。设置如下：

![image-20220804103520548](%E9%AA%8C%E6%94%B6.assets/image-20220804103520548.png)

## 内网配置

同外网操作，只不过ip和网关不一样。

![image-20220804095843040](%E9%AA%8C%E6%94%B6.assets/image-20220804095843040.png)

## 防火墙配置

首先开启防火墙转发功能，使用命令`sudo echo 1 > /proc/sys/net/ipv4/ip_forward`开启该功能。开启后测试内网主机(192.168.164.2)是否可以ping通外网主机(192.168.152.2)如果可以，说明目前为止配置没有问题。

## 遇到的问题

## `sudo apt-get update`失败

报错`Unable to lock directory /var/lib/apt/lists/`

执行`sudo fuser -vki /var/lib/apt/lists/lock`

## 安装apache失败

使用apt安装时报错：

```bash
E: Could not get lock /var/lib/dpkg/lock-frontend - open (11: Resource temporarily unavailable)
E: Unable to acquire the dpkg frontend lock (/var/lib/dpkg/lock-frontend), is another process using it?
```

解决方案：
方案一：

`sudo killall apt apt-get`
如果提示没有apt进程：

```bash
apt: no process found
apt-get: no process found
```

往下看方案二

依次执行：

```bash
sudo rm /var/lib/apt/lists/lock
sudo rm /var/cache/apt/archives/lock
sudo rm /var/lib/dpkg/lock*
sudo dpkg --configure -a
sudo apt update
```



# 验收

## 检查环境

准备好三台主机，分别是防火墙主机firewall，外网主机external和内网主机internal

在配置之前，在外网主机（NAT模式下）装好wireshark

参考 ： https://blog.csdn.net/lynnyq/article/details/79077763#:~:text=ubuntu16.04%E5%AE%89%E8%A3%85wireshark%201.%20%E5%AE%89%E8%A3%85wireshark%20%24%20sudo%20apt%20install%20wireshark,wireshark%20%24%20sudo%20dpkg-reconfigure%20wireshark%20-common%20%E9%80%89%E6%8B%A9yes%2C%E9%9D%9Eroot%E7%94%A8%E6%88%B7%E4%B9%9F%E8%83%BD%E6%8A%93%E5%8C%85%203.

并在防火墙主机安装好内核模块，不再赘述。

外网`192.168.152.2`

![image-20220923220121676](%E9%AA%8C%E6%94%B6.assets/image-20220923220121676.png)

内网`192.168.164.2`

![image-20220923220138889](%E9%AA%8C%E6%94%B6.assets/image-20220923220138889.png)

**一定要注意每个人的提示语句和变量名等一定要改**

**一定要注意每个人的提示语句和变量名等一定要改**

**一定要注意每个人的提示语句和变量名等一定要改**

本版本的验收手册没有做`目的NAT转换`，`规则修改`，`规则与文件的转换`等部分。

如果想恢复最初状态，就重新编译，指令：

```bash
sudo make clean
sudo rmmod myfw
sudo make
sudo make install
```

## 规则过滤

> 能够按照设定的五元组及相应动作的规则过滤报文，执行相应动作，并按设定记录相关访问日志

我们拿ICMP报文测试，也就是用外网主机ping内网主机，当没有添加规则的时候，如图所示，可以ping通

![image-20220923220701839](%E9%AA%8C%E6%94%B6.assets/image-20220923220701839.png)

然后我们在防火墙添加规则

`./ctrl rule add`

输入前序规则名 直接回车

输入规则名 `test`

输入源IP `192.168.152.2/24`

输入源端口 `any`

输入目的IP `192.168.164.2/24`

输入目的端口`any`

输入协议名 `ICMP`

输入动作 `0`，代表drop

输入`1`，代表记录日志

![image-20220923225413903](%E9%AA%8C%E6%94%B6.assets/image-20220923225413903.png)

此时我们再用外网ping内网可以看到是ping不通的。

![image-20220923220946529](%E9%AA%8C%E6%94%B6.assets/image-20220923220946529.png)

如果我们`./ctrl ls log`展示一下日志 可以看到这里的ping的ICMP包都被drop掉了

![image-20220923221114736](%E9%AA%8C%E6%94%B6.assets/image-20220923221114736.png)

## 规则维护

> 能够对过滤规则进行添加、删除、修改、查看，能将当前规则保存至文件，能在系统启动时从文件加载保存的规则

这里只实现了添加，删除，查看。添加的话在前面已经看到，下面看下查看和删除。

首先是查看：

`./ctrl ls rule`

可以看到我们刚才添加的规则的详细信息

![image-20220923221340022](%E9%AA%8C%E6%94%B6.assets/image-20220923221340022.png)

然后是删除

`./ctrl rule del test`

![image-20220923221443365](%E9%AA%8C%E6%94%B6.assets/image-20220923221443365.png)

此时再查看：

![image-20220923221455950](%E9%AA%8C%E6%94%B6.assets/image-20220923221455950.png)

然后看下外网ping通内网

![image-20220923221515377](%E9%AA%8C%E6%94%B6.assets/image-20220923221515377.png)

## 默认动作

> 能够设置默认动作，对未匹配规则的报文执行默认动作

初始状态下防火墙默认是放行的。比如在规则表空的时候，外网是可以ping通内网的。

现在我们默认drop，可以得到在外网内网间没有规则时，ping不通了。

`./ctrl rule default drop`

![image-20220923222119479](%E9%AA%8C%E6%94%B6.assets/image-20220923222119479.png)

ping不通

![image-20220923222133096](%E9%AA%8C%E6%94%B6.assets/image-20220923222133096.png)

所以可以更改默认允许或默认拒绝。

## 连接管理

> 能够为规则允许的通信建立连接表项，后续报文快速匹配连接表进行处理，连接超时能删除，能实时查看连接表信息

内网跑udp_server.py（自己改变量名和提示语句）

```python
# -*- coding: utf-8 -*-

import socket  #导入socket模块
import time #导入time模块
      #server 接收端
      # 设置服务器默认端口号
PORT = 8000
      # 创建一个套接字socket对象，用于进行通讯
      # socket.AF_INET 指明使用INET地址集，进行网间通讯
      # socket.SOCK_DGRAM 指明使用数据协议，即使用传输层的udp协议
server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
address = ("192.168.164.2", PORT)  
server_socket.bind(address)  # 为服务器绑定一个固定的地址，ip和端口
server_socket.settimeout(10)  #设置一个时间提示，如果10秒钟没接到数据进行提示
    
while True:
  try:  
      now = time.time()  #获取当前时间

				      # 接收客户端传来的数据 recvfrom接收客户端的数据，默认是阻塞的，直到有客户端传来数据
				      # recvfrom 参数的意义，表示最大能接收多少数据，单位是字节
				      # recvfrom返回值说明
				      # receive_data表示接受到的传来的数据,是bytes类型
				      # client  表示传来数据的客户端的身份信息，客户端的ip和端口，元组
      receive_data, client = server_socket.recvfrom(1024)
      print(time.strftime('%Y-%m-%d %H:%M:%S',time.localtime(now))) #以指定格式显示时间
      print("来自客户端%s,发送的%s\n" % (client, receive_data))  #打印接收的内容
  except socket.timeout:  #如果10秒钟没有接收数据进行提示（打印 "time out"）
      print("time out")

```

外网跑udp_client.py（自己改变量名和提示语句）

```python
# -*- coding: utf-8 -*-
import socket
import time

#client 发送端
client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
PORT = 8000

while True:
      start = time.time()  #获取当前时间
      print(time.strftime('%Y-%m-%d %H:%M:%S',time.localtime(start)))  #以指定格式显示当前时间
      msg="This is a udp program..."
      server_address = ("192.168.164.2", PORT)  # 接收方 服务器的ip地址和端口号
      client_socket.sendto(msg, server_address) #将msg内容发送给指定接收方
      now = time.time() #获取当前时间
      run_time = now-start #计算时间差，即运行时间
      print(time.strftime('%Y-%m-%d %H:%M:%S',time.localtime(now)))
      print("run_time: %d seconds\n" %run_time)

```

内网跑tcp_server.py（自己改变量名和提示语句）

```python
#!/usr/bin/python
#coding:utf-8
 
from socket import *
HOST = '192.168.164.2'   #不指定的话就默认'localhost' 
PORT = 21568 #监<e5><80>大长度
BUFSIZ = 1024  #指定接收TCP消息的最大长度
ADDR = (HOST,PORT) #
 
#listSock是服务端的监听socket
listSock = socket(AF_INET,SOCK_STREAM) #不指定参数默认为:AF_INET,SOCK_STREAM
listSock.bind(ADDR) #将主机号、端口号绑定到套接字
listSock.listen(5)  #开启TCP监听
 
while True:
    print('waiting for connection...')
    #connSock是连接socket 其实就是一个具体的socket连接.
    connSock, addr = listSock.accept()
    print('...connnecting from:', addr)
 
    while True:
        data = connSock.recv(BUFSIZ)
        print("recv:",data)
        if not data:
            break
        #prefix = b'reply:'
        rsp = data.upper()
        connSock.send(rsp)
        '''
        注意:下面打出来的并不一定是每次真正send回去的字符串,尤其是bufsize小于收发字符串大小的时候
        '''
        #print("send back:",rsp)
    connSock.close()
listSock.close()
```

外网跑tcp_client.py（自己改变量名和提示语句）

```python
from socket import *
 
HOST = '192.168.164.2' 
PORT = 21568
BUFSIZ =1024
ADDR = (HOST,PORT)
 
conn_socket = socket(AF_INET,SOCK_STREAM)
conn_socket.connect(ADDR)
while True:
     data = 'This is a message from client'
     if not data:
         break
     conn_socket.send(data.encode())
     data = conn_socket.recv(BUFSIZ)
     if not data:
         break
     print("recv:" + data.decode('utf-8'))
conn_socket.close()
```

通过socket程序建立udp连接和tcp连接。

我们添加一个TCP接收和一个UDP接收规则。

![image-20220923230611378](%E9%AA%8C%E6%94%B6.assets/image-20220923230611378.png)

注意添加udp的时候前序规则名写我们刚添加的tcp

![image-20220923230646467](%E9%AA%8C%E6%94%B6.assets/image-20220923230646467.png)

注意tcp连接双向，再加一条反向的规则

![image-20220923235224045](%E9%AA%8C%E6%94%B6.assets/image-20220923235224045.png)

![image-20220923235330896](%E9%AA%8C%E6%94%B6.assets/image-20220923235330896.png)

然后启动udp socket

在内网`sudo python udp_server.py`

在外网`sudo python udp_client.py`

可以看到外网通过udp socket开始和内网通信

外网截图如下

![image-20220923235506636](%E9%AA%8C%E6%94%B6.assets/image-20220923235506636.png)

内网截图如下

![image-20220924000615365](%E9%AA%8C%E6%94%B6.assets/image-20220924000615365.png)

此时我们 `./ctrl ls connect` 发现udp连接已经建立

![image-20220924000554864](%E9%AA%8C%E6%94%B6.assets/image-20220924000554864.png)

再开一个终端

在内网`sudo python3 tcp_server.py`

在外网`sudo python3 tcp_client.py`

建立tcp连接

外网主机截图如下

![image-20220924000652339](%E9%AA%8C%E6%94%B6.assets/image-20220924000652339.png)

内网主机截图如下

![image-20220924000704594](%E9%AA%8C%E6%94%B6.assets/image-20220924000704594.png)

此时我们 `./ctrl ls connect` 发现tcp连接已经建立

![image-20220924000715730](%E9%AA%8C%E6%94%B6.assets/image-20220924000715730.png)

然后我们在内外主机通过`ctrl+c`先断开udp程序，一开始连接表还是有udp，等待一段时间，发现连接表没有udp了

![image-20220924000814387](%E9%AA%8C%E6%94%B6.assets/image-20220924000814387.png)

然后我们在内网主机通过`ctrl+c`断开tcp程序（TCP双向连接，断开内网即都断开），一开始连接表还是有tcp，等待一段时间，发现连接表没有tcp了

![image-20220924000939570](%E9%AA%8C%E6%94%B6.assets/image-20220924000939570.png)

## 状态检测

> 为TCP、UDP、ICMP分别设计状态机，在连接表项中记录通信状态，连接表匹配时能检查合法状态放行并变迁或非法状态丢弃

在前面的部分已体现。

## 日志审计

> 能够保存日志，对日志进行一定条件的查询，除访问日志外能记录查询一定管理日志

展示所有过滤日志：

```bash
./ctrl ls log
```

展示最后100条过滤日志：

```bash
./ctrl ls log 100
```

![image-20220924001147319](%E9%AA%8C%E6%94%B6.assets/image-20220924001147319.png)

## NAT转换

> 能够设置NAT策略，根据策略对通信进行源NAT或目的NAT转换，保障通信正常进行

这里我们实现了源NAT转换。

输入`./ctrl nat add`开始添加nat规则，我们把内网（192.168.164.x）的`192.168.164.2`转换到外网（192.168.152.x）的`192.168.152.1`，注意这里只能写防火墙主机有的ip，必须是`192.168.152.1`

![image-20220924163705632](%E9%AA%8C%E6%94%B6.assets/image-20220924163705632.png)

然后内网主机(192.168.164.2)ping外网主机(192.168.152.2)，同时外网开启wireshark抓包

![image-20220924163743078](%E9%AA%8C%E6%94%B6.assets/image-20220924163743078.png)

可以看到这里的NAT转换。

## 基本界面

> 界面友好，能够完成上述设置、查询等管理操作

命令行界面

**一定要注意每个人的提示语句和变量名等一定要改**

**一定要注意每个人的提示语句和变量名等一定要改**

**一定要注意每个人的提示语句和变量名等一定要改**

## 拓展任务

> 若实现图形界面、规则支持时间控制，额外加分。加分后作品得分不超过100分

大四就别卷了。