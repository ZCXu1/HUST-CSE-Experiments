all: 
	gcc -o svpnclient svpnclient.c -lssl -lcrypto -lpthread -w
	gcc -o svpnserver svpnserver.c -lssl -lcrypto -lpthread -lcrypt -w

clean: 
	rm svpnclient svpnserver
