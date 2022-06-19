//////////////////////////////////////////////////////////////
#include "RAcom.h"
//#include "RASec.h"

#include "mydebug.h"

#include "common.h"
#include "remote_attestation.h"


#define fromsize 264
#define tosize 512

#include <sys/ioctl.h>
#include <linux/if.h>
#define ETH_NAME "eth0"

//#include "debug.h"

///////////////////////////////////////////////////////////////
//
// step1: deal with the thread which has been ended
//
///////////////////////////////////////////////////////////////
static void sigchld_handler(int signo)
{
	pid_t PID;
	int status;

	do
	{
		PID = waitpid(-1,&status,WNOHANG);
	}while(PID!=-1);

	signal(SIGCHLD,sigchld_handler);
}


////////////////////////////////////////////////////////////
//
// step2: deal with error and return shell
//
////////////////////////////////////////////////////////////
static void failhandler(const char * on_what)
{
	if (errno!=0) 
	{
		fputs(strerror(errno),stderr);
		fputs(":",stderr);
	}
	fputs(on_what,stderr);
	fputc('\n',stderr);
	exit(1);
}

int main()
{
	int sockfd,client_fd,readsize=0,type,ses_length;
	short index=0,status,IPlength;
	struct sockaddr_in my_addr;					// local ip imformation //
	struct sockaddr_in remote_addr;				// client ip imformation // 
	char buf[MAXDATASIZE],* buf_p, strRemoteIP[16], addServerIP[16],addClientIP[16];
	char strMyIP[16];
	pid_t PID;
	int result;
	int reqpcr;
	int i,z;
/////////////////////////////////////////////////////////////////////////////////////
	BYTE nonce[20];
	remote_verifier_data* remote_data;
    int remote_data_size = 0;
    BYTE *receive_msg;
    int receive_msg_size = 0;

	unsigned char pcrResponse[1024];//size????
/////////////////////////////////////////////////////////////////////////////////////


   //Below Variable is for RSA encrypt and decrypt
   //RSA* clientkey;
   /*RSA* serverkey;
	RSA* serverpubkey;
	RSA* serverprivkey;
	BIO *in,*out;
	unsigned long e = RSA_3;
	BIGNUM *bne;
   int r;
   int len,rlen;
   unsigned char decryptedNonce[256];


   //Below Variable is for RSA sign and verify
   unsigned char sha1PCRNonce[20];   //sha1 of PCR and Nonce  
   unsigned char pcrsigntext[256];
   unsigned char pcrtext[20];
   unsigned char pcrResponse[1024];

	unsigned char nonce[20];
	unsigned char pcrValue[20];

	unsigned char tpmVersion[4];

	unsigned long pcrCompositeSize = 0;
	unsigned char pcrComposite[256];

	unsigned long signedPCRSize = 0;
	unsigned char signedPCR[256];

	unsigned long TPMKeyPubSize = 0;
	unsigned char TPMKeyPub[256];

	unsigned long TPMpubSigLen = 0;
	unsigned char TPMKeyPubSignedtext[256];

	unsigned char sha1TPMKeyPub[20];*/
	
	int sock;
	struct sockaddr_in sin;
	struct ifreq ifr;

	/*sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == -1)
	{
		perror("socket");
		fprintf(stdout, "===== Create socket failed. =====\n");
		return -1;
	}

	strncpy(ifr.ifr_name, ETH_NAME, IFNAMSIZ);
	ifr.ifr_name[IFNAMSIZ - 1] = 0;

	if (ioctl(sock, SIOCGIFADDR, &ifr) < 0)
	{
		perror("ioctl");
		fprintf(stdout, "===== Can't get IP of eth0. =====\n");
		return -1;
	}

	memcpy(&sin, &ifr.ifr_addr, sizeof(sin));
//	fprintf(stdout, "eth0: %s\n", inet_ntoa(sin.sin_addr));
	memcpy(strMyIP, inet_ntoa(sin.sin_addr), 16);
//	printf("The size of sin is: %d\n", sizeof(inet_ntoa(sin.sin_addr)));
	printf("The server's IP is: %s\n", strMyIP);*/



	//Read RSA Private from the PEM file
	/*OpenSSL_add_all_algorithms();
	in=BIO_new_file("serverpriv.pem","rb");
	serverprivkey=RSA_new();
	serverprivkey=PEM_read_bio_RSAPrivateKey(in,&serverprivkey,NULL,NULL);	
	if(serverprivkey->d!=NULL)
		printf("Read Private Key OK!\n");
	else
	{
		printf("Read Private Key error!\n");
		RSA_free(serverprivkey);
		RSA_free(serverkey);
		exit(-1);
	}
	BIO_flush(in);
	
	//Read RSA Pubkey from the PEM file	
	in=BIO_new_file("serverpub.pem","rb");
	serverpubkey=RSA_new();
	serverpubkey=PEM_read_bio_RSAPublicKey(in,&serverpubkey,NULL,NULL);	
	if(serverpubkey->e!=NULL)
		printf("Read Public Key OK!\n");
	else
	{
		printf("Read Private Key error!\n");
		RSA_free(serverpubkey);
		RSA_free(serverprivkey);
		RSA_free(serverkey);
		exit(-1);
	}
	BIO_flush(in);
	BIO_free(in);

*/
	//catch the SIGCHLD
	signal(SIGCHLD,sigchld_handler);

	//Print the Welcome 
	printf("\n*********************************************************\n");
	printf("***** Welcome to IBM Remote Attestation Server 111********\n");
	printf("*********************************************************\n");



	//Create the socket
	my_addr.sin_family=AF_INET;
	my_addr.sin_port=htons(RASERVERPORT);
	my_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(my_addr.sin_zero),sizeof(my_addr.sin_zero));
	if((sockfd=socket(AF_INET,SOCK_STREAM,0)) < 0)
	{
		failhandler("====== Server Create Socket error! ======\n");
	}
	
	//If the port is already used, then these two lines will make the port useable.
	struct linger sopt = {1, 0};
	setsockopt(sockfd, SOL_SOCKET, SO_LINGER, (void *)&sopt, sizeof(sopt));

	//Bind
	if(bind(sockfd,(struct sockaddr *)&my_addr,sizeof(my_addr))==-1)
	{
		failhandler("====== Server Bind error! ======\n");
	} 
	
	//Listen
	if(listen(sockfd,BACKLOG) == -1)
	{
		failhandler("====== Server Listen error! ======\n");
	}
	
	//Loop operateion
	while(1)
	{
//		sleep(2);
			
		//Accept return a new socket which communicate with client
		socklen_t length = sizeof(remote_addr);
		if ((client_fd = accept(sockfd, (struct sockaddr *)&remote_addr,&length)) < 0)
		{
			failhandler("====== Server Accept error! ======\n");
		}

		printf("\n--> --> Received a connection from: %s\n",inet_ntoa(remote_addr.sin_addr));
	
		//create a new tenor request
		if((PID=fork()) == -1)
		{

			//child process create error
			printf("====== Fork a child proess fail! ======\n");
			close(client_fd);
			continue;
		}
		else if (PID > 0)
		{
			//father tenor deal
			close(client_fd);
			continue;
		}
		/*
		*		Successfully generate a child process for a connection.
		*/
		else
		{
					
			//	son tenor to deal with request
			if(ReceiveMessage(client_fd, strRemoteIP, &type, &readsize, &buf_p) > 4)
			{
				printf("====== Receive error! ======\n");
				if (0 != SendMessage(client_fd, strMyIP,strRemoteIP, RA_RSP_ERROR_RECEIVEDATA, 0,NULL))
				{
					printf("====== Send Response messeage to RAClient Failed! ======\n");
					close(client_fd);
					continue;
				}
				close(client_fd);
				exit(0);
				continue;
			}


			if(readsize < 0)
			{
				printf("====== The type of the connection is: %d. =====\n", type);
				printf("====== Read error! ======\n");
				printf("====== Readsize < 0, readsize is :%d ======\n",readsize);

				if (buf_p != NULL)
				{
					free(buf_p);
				}
				if (0 != SendMessage(client_fd, strMyIP,strRemoteIP, RA_RSP_ERROR_READDATA, 0,NULL))
				{
					printf("====== Send Response messeage to RAClient Failed! ======\n");
					close(client_fd);
					exit(0);
					continue;
				}
				close(client_fd);
				exit(0);
				continue;
			}


			/*
			*		Normal process begins
			*/
			printf("#####type == %d \n",type);
			switch (type)
			{

				/*
				*		The Request is to: Connect to Server
				*/
				case COMM_REQ_CONNECT:
					printf("====== Received Connection request from %s ======\n",inet_ntoa(remote_addr.sin_addr));					
					if (0 != SendMessage(client_fd, strMyIP,strRemoteIP, COMM_RSP_ALLOW, 0,NULL))
					{
						printf("====== Send Response messeage to RAClient Failed! ======\n");
						close(client_fd);
						exit(0);
						continue;
					}
					break;

				/*
				*		The Request is to: Require the Server's PCR
				*/
				case RA_REQ_PCR:
					
					//nonce from the client
					memcpy(nonce, buf_p, readsize);

					printf("====== Received PCR Read request from %s ======\n",inet_ntoa(remote_addr.sin_addr));
               				printf("====== Receiced size of nonce is %d ======\n",readsize);

					remote_attestation(nonce, 20, &receive_msg, &receive_msg_size, &remote_data, &remote_data_size);
//#ifdef DAOLIDEBUG
					//print nonce
					printf("##The nonce received from the client is:##\n");
					for(i = 0; i < 20; i++)
					{
						if((i != 0) && (i%16 == 0))	printf("\n");
						printf("%02X ", *(nonce + i));
					}
					printf("\n");
					printf("##The nonce received from the client is over##\n");
//#endif

					/**
					**		Sign PCR by using TPM_Quote
					**/

/*					printf("====== TPM starts to sign PCR value internally by using TPM_Quote ======\n");
					result = TPM_Quote(	20, nonce,
												tpmVersion,
												&pcrCompositeSize, pcrComposite,
												&signedPCRSize, signedPCR,
												&TPMKeyPubSize, TPMKeyPub);
					
					if(result != 0)
					{
						printf("====== Signing the value of sha1 Error: %d======\n", result);
						if (0 != SendMessage(client_fd, strMyIP,strRemoteIP, RA_RSP_ERROR_TCSD, 0,NULL))
						{
							printf("====== Send Response messeage to RAClient Failed! ======\n");
							close(client_fd);
							exit(0);
							continue;
						}
						exit(0);
						continue;
					}
					else
						printf("\n====== TPM has signed PCR value internally and successfully ======\n");

					printf("====== The size of the data signed by TPM is: %d ======\n", signedPCRSize);
					printf("====== The size of TPM Signing Key is: %d ======\n", TPMKeyPubSize);

*/

//#ifdef DAOLIDEBUG
					/**
					**		Send all the Signed data to the client.
					**/
/*					printf("##The content of the PCR Composite is: %u##\n", pcrCompositeSize);
					for(i = 0; i < pcrCompositeSize; i++)
					{
						if((i != 0)&&(i % 16 == 0))	printf("\n");
						printf("%02X ", *(pcrComposite + i));
					}
					printf("\n");
					printf("##The content of PCR Composite is over##\n");


					//Display the Signed Value of the PCR
					printf("##The content of the Signed PCR is: %u##\n", signedPCRSize);
					for(i = 0; i < signedPCRSize; i++)
					{
						if((i != 0)&&(i % 16 == 0))	printf("\n");
						printf("%02X ", *(signedPCR + i));
					}
					printf("\n");
					printf("##The content of Signed PCR is over##\n");


					//Display the public part of the TPM Signing Key
					printf("##The public part of TPM Signing Key is: %u##\n", TPMKeyPubSize);
					for(i = 0; i < TPMKeyPubSize; i++)
					{
						if((i != 0)&&(i % 16 == 0))	printf("\n");
						printf("%02X ", *(TPMKeyPub + i));
					}
					printf("\n");
    				printf("##The public part of TPM Signing Key is over##\n");
*/    				
//#endif
/////////////////////////////////////////////////////////////////////////////////////////将remote_data结构中的信息拷贝到pcrResponse中。
/*					memcpy(pcrResponse,"11111111",8);
					memcpy(pcrResponse+8, tpmVersion, 4);
					memcpy(pcrResponse+8+4, pcrComposite, pcrCompositeSize);
					memcpy(pcrResponse+8+4+pcrCompositeSize, signedPCR, signedPCRSize);
					memcpy(pcrResponse+8+4+pcrCompositeSize+256, TPMKeyPub, TPMKeyPubSize);
					//memcpy(pcrResponse+8+4+pcrCompositeSize+256+256, TPMKeyPubSignedtext, TPMpubSigLen);
*/
					memcpy(pcrResponse,receive_msg,receive_msg_size);

					printf("====== Send Encrypted PCR to RAClient ======\n");
					if (0 != SendMessage(client_fd, strMyIP, strRemoteIP, RA_RSP_PCR, 
											receive_msg_size, pcrResponse))
					{
						printf("====== Send PCR messeage to RAClient Failed! ======\n");
						close(client_fd);
						exit(0);
						continue;
					}
					break;

				default:
					printf("====== Received Error request from %s ======\n",inet_ntoa(remote_addr.sin_addr));

			}//switch
			printf("\n");
			close(client_fd);	
			exit(0);
		}//else
	}//while
	return 0;
}//main







































