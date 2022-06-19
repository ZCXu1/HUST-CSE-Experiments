If you run this code in vmware, need install tpm_emulator. If in physical machine start from install trousers
#1. install tpm_emulator-0.7.1
apt-get install libgmp3-dev
apt-get install cmake
tar -xvzf tpm_emulator-0.7.1.tar.gz
cd tpm_emulator-0.7.1
mkdir build
cd build
cmake ../
make
make install
modprobe tpmd_dev     (#rm /var/run/tpm/tpmd_socket:0 （出现failed：address already in use 时执行它）)
tpmd -f clear

安装完后启动软TPM的方法：

#modprobe tpmd_dev （如果出现FATAL: Module tpmd_dev not found，则先运行depmod -a）

#tpmd -f -d            （tpmd -h查看启动参数）

#2. install trousers
tar -xvzf trousers-0.3.6.tar.gz
cd trousers-0.3.6
make
make install
tcsd -f   （如果出现TCSD TDDL ERROR: tddl.c:132 Could not find a device to open!，则运行：killall tscd)

#3. make common.o
ldconfig;
cd ../pm/RA/common/;
make;

#4. do takeownership create SRK
cd ../init/;
make;
./Tspi_TPM_TakeOwnership01 -v 1.2;

#If success output is like this:
<<<test_start>>>
Testing Tspi_TPM_TakeOwnership01
	1 PASS  :  Tspi_TPM_TakeOwnership01  returned (0) TSS_SUCCESS
Cleaning up Tspi_TPM_TakeOwnership01
<<<end_test>>>

#5. do remote_attestation
cd ..;
make;
./remote_attestation -v 1.2;

#If success output is like this:
##The content of the nonce is:##
da e9 9a 21 d7 67 91 4c e3 74 8b 6b 50 74 4f 69 
b5 00 55 1b 
##The content of the nonce is over##
client  send message to 
##The content of the pcr0 Value is:##
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 
##The content of the pcr0 Value is over##
##The content of the pcr1 Value is:##
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 
##The content of the pcr1 Value is over##
##The content of the pcr2 Value is:##
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 
##The content of the pcr2 Value is over##
##The content of the pcr3 Value is:##
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 
##The content of the pcr3 Value is over##
##The content of the pcr4 Value is:##
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 
##The content of the pcr4 Value is over##
##The content of the pcr5 Value is:##
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 
##The content of the pcr5 Value is over##
##The content of the pcr6 Value is:##
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 
##The content of the pcr6 Value is over##
##The content of the pcr7 Value is:##
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 
##The content of the pcr7 Value is over##
##The content of the pcr8 Value is:##
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 
##The content of the pcr8 Value is over##
##The content of the pcr9 Value is:##
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 
##The content of the pcr9 Value is over##
##The content of the pcr12 Value is:##
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 
##The content of the pcr12 Value is over##
##The content of the pcr14 Value is:##
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 
##The content of the pcr14 Value is over##
offset = 8
##The content of the rc is:##
00 02 ff 53 00 00 00 f0 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 
##The content of the rc is over##
client  send message to 
===== credential is right! =====
##The content of the getpcrComposite is:##
01 01 00 00 51 55 4f 54 97 1e 38 15 0b 8d 95 0a 
2d dc 05 45 98 ae ed 42 f7 92 35 e3 da e9 9a 21 
d7 67 91 4c e3 74 8b 6b 50 74 4f 69 b5 00 55 1b 
##The content of the getpcrComposite is over##
##The content of the getsignedPCR is:##
23 d8 6d 4e 20 f3 3b e3 f1 5d fd 15 e0 86 18 ff 
75 06 22 39 82 b3 bf 7f 3b cd fa 01 da 1e aa 2a 
18 23 54 13 b6 59 b5 c5 2b fd 79 75 9e ea 85 c2 
c4 98 37 85 9d 83 7a 15 dc 5a 71 70 bd 21 e0 a0 
55 51 ba be cb 1b 0b 9e 62 db fd b7 84 02 92 1e 
08 34 c3 91 99 84 ba 29 c7 0a d0 1d 42 5e 75 cc 
19 5b 14 62 ab bb b9 92 b0 b7 99 e2 c4 18 32 e4 
bc 5f 16 b8 39 b4 10 d2 d3 fe f0 30 90 39 fb 83 
93 4d 33 42 41 ce 97 34 9b cf 50 dc 46 4a 97 5a 
26 e2 c8 2a 2e fe de 18 77 ef 4e 23 96 35 28 f7 
e4 d1 df 98 cd ea 63 91 79 db 01 dd 29 bd 1c 11 
4b 19 ef ca bc bc 7b d7 e3 73 91 bc 5e 12 c3 80 
23 81 4b 2d b5 86 0a dd 74 29 40 cf 29 b1 2b 6c 
6a 80 11 91 91 09 3c c8 de 75 07 01 f8 6f a4 03 
dc 88 1e 46 87 82 79 28 ff c8 a5 e2 53 c6 7d d0 
ff 18 47 03 6f ed 02 27 c7 51 9d c0 29 55 c7 5b 
##The content of the signedPCR is over##
##The content of the getkeyPub is:##
a5 a4 57 03 94 30 74 27 57 15 0c d1 58 c8 c4 57 
6a f2 4a ef b9 cc d0 79 06 22 72 d4 2f c5 b5 1a 
b1 7f 7e 5e 85 69 dc b9 55 ea bf 87 0b 15 93 90 
e8 cd bd 37 3e 74 be 22 7b bd d2 9d 63 74 8f a6 
17 32 04 16 93 ed b7 d3 11 24 fe 62 31 f6 aa 24 
5b 5e 4e 4a ec bb 8c be a5 32 97 15 bd d7 68 50 
20 00 6e de 3b 22 d4 62 6c 92 cf db ba 67 49 6d 
5a 0c dc b5 72 04 c8 17 09 fc cf c5 04 c4 aa 43 
7c d2 77 47 a0 b9 28 58 e7 07 f6 5e f2 aa 76 b5 
f6 8f 7e 8b 64 f8 b9 0c 96 c7 0a 22 03 9e ab 2d 
a0 58 21 52 0f 9d 7e c3 86 ec 53 30 78 63 91 ee 
4a 72 db 86 cc 49 b4 fb 34 3d bf 92 f9 bb 6c 70 
f6 a3 4e 52 37 9d d5 c8 fc e5 79 84 1b e9 c0 4d 
e1 e0 9d fe b7 b5 77 86 59 c1 61 0f 3a a9 13 40 
a4 26 dd 82 77 14 1a 06 7f 1d 1a 44 fd f9 8e 79 
3a 2b 01 b6 6c 67 57 45 c0 80 56 25 e7 61 8c e3 
##The content of the getkeyPub is over##
##The content of the getrc is:##
00 02 ff 53 00 00 00 f0 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 
##The content of the getrc is over##
===== PCR signature verify OK =====
##The content of the pcr_digest is:##
97 1e 38 15 0b 8d 95 0a 2d dc 05 45 98 ae ed 42 
f7 92 35 e3 
##The content of the pcr_digest is over##
===== PCR digest match! =====
===== nonce match! =====
	1 PASS  :  Remote Attestation  returned (0) TSS_SUCCESS
Cleaning up Remote Attestation
<<<end_test>>>
