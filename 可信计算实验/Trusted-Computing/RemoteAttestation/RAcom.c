//////////////////////////////////////////////////////////

// step1 : inital the connection
// step2 : send message to dest
// step3 : receive message from source
// step4 : free the memory
// step5 : disconnect the socket
// step6 : server initial the connection

//////////////////////////////////////////////////////////


#include "RAcom.h" 

/////////////////////////////////////////////////
//
// step1 : inital the connection
//
/////////////////////////////////////////////////
int initConnect(char * strSourceIP, char * strDestIP, int serverport)
{
	int sockfd;
	struct sockaddr_in source_addr,dest_addr;

	//get the sourceip
	source_addr.sin_addr.s_addr=inet_addr(strSourceIP);
	if(source_addr.sin_addr.s_addr==INADDR_NONE)
	{
		printf("sourcename is unavailable!\n");
		return -1;
	}

	//get the destip
	dest_addr.sin_addr.s_addr=inet_addr(strDestIP);
	if(dest_addr.sin_addr.s_addr==INADDR_NONE)
	{
		printf("destname is unavailable!\n");
		return -1;
	}

	//creacte a socket
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0))==-1)
	{ 
		printf("socket create error!\n");
		return -1;
	} 
	
	//For the Client is face to connection,so Bind() is not need.

/*	//set the address of source
	source_addr.sin_family=AF_INET;
	source_addr.sin_port=htons(RACLIENTPORT);
	source_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(source_addr.sin_zero),8);
		
	//bind the socket
	if(bind(sockfd,(struct sockaddr *)&source_addr,sizeof(struct sockaddr_in))!=0)
	{
		printf("Bind error!\n");
		return-1;
	} 
*/
	//set the address of dest
	dest_addr.sin_family=AF_INET;
	dest_addr.sin_port=htons(serverport);
	bzero(&(dest_addr.sin_zero),8); 


	//connect the sourceip and destip
	if(connect(sockfd, (struct sockaddr *)&dest_addr,sizeof(struct sockaddr)) != 0) 
	{
		printf("connection error!\n ");
		return -1;
	} 

	return sockfd;
}



/////////////////////////////////////////////////
//
// step2 : send message to dest
//
/////////////////////////////////////////////////
int SendMessage(int socketfd, char* strSourceIP, char* strDestIP, int dwCommTYPE, int dwTransLength, char * rgbBuffer)
{
	int dwSendbytes,dwData_type,dwData_length;
	char str_send_buf[MAXDATASIZE];

	//switch the type of the data


	//compute the whole length of the data and the head
	dwData_length = dwTransLength+40;

	//write the head to the data
	memset(str_send_buf,0,MAXDATASIZE);

	memcpy(str_send_buf,strSourceIP,16);
	memcpy(str_send_buf+16,strDestIP,16);
	memcpy(str_send_buf+32,&dwCommTYPE,4);
	memcpy(str_send_buf+36,&dwData_length,4);
	if((dwTransLength!=0)&&(rgbBuffer!=NULL))
		memcpy(str_send_buf+40,rgbBuffer,dwTransLength);

	//send the message
	if((dwSendbytes=send(socketfd, str_send_buf, dwData_length, 0)) ==-1) 
	{
		printf("Send error!\n");
		exit(1);
	}

	//send the message succesfully
	if(dwSendbytes == dwData_length)
	{
		return 0;
	}
	else 
	{
		return -1;
	}
} 




/////////////////////////////////////////////////
//
// step3 : receive message from source
//
/////////////////////////////////////////////////
int ReceiveMessage(int socketfd, char* strSourceIP, int* dwCommTYPE, int * dwTransLength,char * * prgbBuffer)
{
	char * str_rcv_buf;
	int dw_rec_length,dwdata_length,type;


	//malloc some memory to receive the data
	str_rcv_buf=(char *)malloc(MAXDATASIZE);

	//initializtion the receive buffer: str_rcv_buf
	memset(str_rcv_buf,0,MAXDATASIZE);

	if(str_rcv_buf==NULL)
	{
		printf("malloc memory error!\n");
		return -1;
	}

	//receive the data from sourceIP
	if((dw_rec_length=recv(socketfd, str_rcv_buf, MAXDATASIZE, 0)) ==-1) 
	{
		printf("Receive error!\n");
		return -2;
	}
	
	//copy the sourceIP & translength & data from the buf
	memcpy(strSourceIP,str_rcv_buf,16);
	memcpy(&type,str_rcv_buf+32,4);
	memcpy(&dwdata_length,str_rcv_buf+36,4);

	if(dwdata_length!=dw_rec_length)
	{
		printf("Recieved Data length error!\n");
		return -3;		
	}

	(*dwCommTYPE)=type;
	(*dwTransLength)=dwdata_length-40;
	if((*dwTransLength)>0)
		(*prgbBuffer) = str_rcv_buf+40;
	else
		(*prgbBuffer) = NULL;

	return 0;	
}



/////////////////////////////////////////////////
//
// step4 : free the memory
//
/////////////////////////////////////////////////
void releaseMemory(char * rgbBuffer)
{
	free(rgbBuffer);
}



/////////////////////////////////////////////////
//
// step5 : disconnect the socket
//
/////////////////////////////////////////////////
int disconnect(int socketfd)
{
	return	close(socketfd);
}

/////////////////////////////////////////////////
//
// ADD:Client inital the connection
//
/////////////////////////////////////////////////
int ServerInitConnect(char * strSourceIP, char * strDestIP,int clientPort)
{
	int sockfd;
	struct sockaddr_in source_addr,dest_addr;

	//get the sourceip
	source_addr.sin_addr.s_addr=inet_addr(strSourceIP);
	if(source_addr.sin_addr.s_addr==INADDR_NONE)
	{
		printf("sourcename is unavailable!\n");
		return -1;
	}

	//get the destip
	dest_addr.sin_addr.s_addr=inet_addr(strDestIP);
	if(dest_addr.sin_addr.s_addr==INADDR_NONE)
	{
		printf("destname is unavailable!\n");
		return -1;
	}

	//creacte a socket
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0))==-1)
	{ 
		printf("socket create error!\n");
		return -1;
	} 

	//set the address of dest
	dest_addr.sin_family=AF_INET;
	dest_addr.sin_port=htons(clientPort);
	bzero(&(dest_addr.sin_zero),8); 

	//connect the sourceip and destip
	if(connect(sockfd, (struct sockaddr *)&dest_addr,sizeof(struct sockaddr)) == -1) 
	{
		printf("connection error!\n ");
		return -1;
	} 

	return sockfd;
}

void printReceivedMessage(int socketfd, char* strSourceIP, int dwCommTYPE, int  dwTransLength,char *prgbBuffer)
{
	printf("\nThis is the recieved Message\n");
	printf("Socket ID is %d\n",socketfd);
	printf("Source IP is %s\n",strSourceIP);
	printf("Comm Type is %d\n",dwCommTYPE);
	printf("TraLength is %d\n",dwTransLength);
	if(prgbBuffer!=NULL)printf("Buffer 	is %s\n",prgbBuffer);
}
void printSendMessage(int socketfd, char* strSourceIP, char* strDestIP, int dwCommTYPE, int dwTransLength, char * rgbBuffer)
{
	printf("\nThis is the send Message\n");
	printf("Socket ID is %d\n",socketfd);
	printf("Source IP is %s\n",strSourceIP);
	printf("Dest	IP is %s\n",strDestIP);
	printf("Comm Type is %d\n",dwCommTYPE);
	printf("TraLength is %d\n",dwTransLength);
	if(rgbBuffer!=NULL)printf("Buffer 	is %s\n",rgbBuffer);
}


