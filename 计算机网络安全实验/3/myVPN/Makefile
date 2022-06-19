all: 
	gcc -o mycli mycli.c -lssl -lcrypto -lpthread -w
	gcc -o myser myser.c -lssl -lcrypto -lpthread -lcrypt -w
	sudo docker cp ../myVPN HostU:/
	sudo docker cp ../myVPN HostU2:/

clean: 
	rm mycli myser
