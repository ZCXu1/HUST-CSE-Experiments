all: RAServer RAClient init/Create_AIK

init/Create_AIK:
	$(MAKE) -C init/
RAServer: RAServer.o RAcom.o remote_attestation.o
	gcc -o RAServer RAServer.o RAcom.o remote_attestation.o ../common/common.o -L/usr/local/lib -ltspi -lssl -lcrypto 
RAClient:RAClient.o RAcom.o
	gcc -o RAClient RAClient.o RAcom.o ../common/common.o  -lssl -L/usr/local/lib -ltspi -lssl -lcrypto

RAServer.o: RAServer.c 
	gcc -c RAServer.c -I../include

RAClient.o:RAClient.c sha.h
	gcc -c RAClient.c -I../include

RAcom.o: RAcom.c RAcom.h
	gcc -c RAcom.c

remote_attestation.o: remote_attestation.c remote_attestation.h
	gcc -c remote_attestation.c -I../include

clean:
	rm -f *.o RAClient RAServer
	$(MAKE) -C init/ clean
