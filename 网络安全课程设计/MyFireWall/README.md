# RJ FireWall

一个基于Netfilter、Netlink的Linux传输层状态检测防火墙，核心代码不到2000行，使用红黑树实现状态检测，内核模块代码通过读写锁几乎全面实现并发安全。

仅用于学习与交流，**一定程度上**可以放心使用。

支持的功能：
- [x] 按源目的IP、端口、协议过滤报文
- [x] 掩码
- [x] 并发安全
- [x] 通过命令行应用新增、删除、查看过滤规则，更改默认动作
- [x] 记录报文过滤日志及通过命令行应用查看
- [x] 连接状态检测与记录
- [x] 通过命令行应用查看已建立的所有连接
- [x] NAT
- [x] 配置NAT规则
- [ ] ~~图形化界面 防火墙还写啥GUI~~

# 安装

### 环境

我所采用的环境为Ubuntu 16.04，Linux内核版本4.15，所有功能测试正常。

**一般而言**，所有Linux内核版本 > 4.9的系统皆可使用。

### 从源码安装

安装时需要gcc以及make包，若未安装，请预先安装：
```bash
sudo apt install gcc make
```

**编译源码**：

```bash
sudo make
```

最后，**安装**：
```bash
sudo make install
```

# 使用

在安装时，内核模块已经加载至Linux内核中，此时，只需使用上层应用ctrl来对防火墙进行控制即可。

新增一条过滤规则：
```bash
./ctrl rule add
```
随后依据命令行提示设定规则即可。

删除一条过滤规则：
```bash
./ctrl rule del 所需删除规则的名称
```

设置默认动作为Drop（防火墙初始时默认动作为Accept）：
```bash
./ctrl rule default drop
```

展示已有规则：
```bash
./ctrl ls rule
```

展示所有过滤日志：
```bash
./ctrl ls log
```

展示最后100条过滤日志：
```bash
./ctrl ls log 100
```

展示当前已有连接：
```bash
./ctrl ls connect
```

新增一条NAT规则：
```bash
./ctrl nat add
```
随后依据命令行提示设定规则即可。

删除一条NAT规则：
```bash
./ctrl nat del 所需删除NAT规则的序号
```

展示已有NAT规则：
```bash
./ctrl ls nat
```