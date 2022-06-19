#include "RAcom.h"
//#include "RASec.h"
#include "sha.h"

//#include "debug.h"

#include "mydebug.h"
#include "common.h"
#include "remote_attestation.h"

#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/x509.h>

int currlevel=DINFO;
//#define LOAD32(buffer,offset)         ( ntohl(*(uint32_t *)&buffer[offset]) )
//#define LOAD16(buffer,offset)         ( ntohs(*(uint16_t *)&buffer[offset]) )

/*
*	transfer ASCII string to number
*/
/*void ASCII2bin(unsigned char *strASCII, unsigned char *strbin)
{
	int i;
	int	data_temp,
			data_high=0,
			data_low=0;
			
	int index=0;
	int pre_inData=0;
	int high_flag=1;  //true

	for(i=0;i<40;i++)
	{
		data_temp=strASCII[i];
		if(data_temp<='9'&&data_temp>='0')
			data_temp-=48;	//change char '0'~'9' to int 0~9
		if(data_temp<='F'&&data_temp>='A')					
			data_temp-=55;	//change char 'A'~'F' to int 10~15
		if(data_temp<='f'&&data_temp>='a')
			data_temp-=87; //change char 'a'~'f' to int 10~15										
			
			if(high_flag)			//the char change to high 4 bite
			{
				data_high=data_temp*16;
				high_flag=0;
			}
			else								//the char change to low 4 bite
			{
				data_low=(unsigned char)data_temp;	
				data_temp=data_high+data_low;
						
				strbin[index]=(unsigned char) data_temp;
						
				index++;
				high_flag=1;
			}
	}
}
*/
/*
*	write a string to a certain file
*	char *file points to the file name
*/
void str2file(char *str, char *file)
{
	FILE *fp;
	int i=0,j=0;
	char buf[20];
	memset(buf,0,20);
	fp=fopen(file,"w+");					
	if(fp==NULL)
	{
		printf("Error: in func str2file, IPSetFile create error!\n");
		exit(ERROR_NO_IPSETFILE);
	}

	while(j<strlen(str))
	{
		while(str[j]!=':'&&j<strlen(str))
			j++;
		memcpy(buf,str+i,j-i);
		fprintf(fp,"%s\n",buf);
		j++;		
		i=j;
	}
	fclose(fp);	
	
	
}


int main(int argc, char* argv[])
{
	//Below Variable is for Socket communication
	struct sockaddr_in my_addr;
	struct sockaddr_in remote_addr;		
	char 	srcIP[16] = {0};
	char 	rcIP[16] = {0};
	char	IPsetFile[32]="ipset.txt";
	char	bkupSrcIP[16];
	char	bkuprcIP[16];
	int 	clientfd;
	int	dwTransLength = 0;
	char *buffer = NULL;
	int dwStatus = 0; 
	int type;
	FILE *fpIPset;
	int z,i;

///////////////////////////////////////////////////////////////////////////////////////
BYTE nonce[21] ="\x0\x1\x2\x3\x4\x5\x6\x7\x8\x9\xA\xB\xC\xD\xE\xF\x10\x11\x12\x13"; 	//input interface for QT?
//BYTE nonce[21] ="\xda\xe9\x9a\x21\xd7\x67\x91\x4c\xe3\x74\x8b\x6b\x50\x74\x4f\x69\xb5\x00\x55\x1b";
unsigned long pcrCompositeSize=0;
unsigned long signedPCRSize=0;
unsigned long keyPubSize=0;
unsigned long sizeof_rc=0;

unsigned char * pcrComposite;
unsigned char * signedPCR;
unsigned char * keyPub;
unsigned char * rc;

BYTE pcr_digest[20];

int PCRMatch[MaxIPNum];
int IPCnt=0;


///////////////////////////////////////////////////////////////////////////////////////

   //Below Variable is for RSA encrypt and decrypt
   /*RSA* serverpubkey;
   int r;
   BIO *in,*out;
   int result,i;
   int len,rlen;
	unsigned char decryptedNonce[256];

	
   // Below Variable is for RSA sign and verify
   unsigned char pcrText[20];
   unsigned char pcrplainplus[148+8];
   unsigned char pcrplain[148];
	unsigned char sha1PCRNonce[20];	

	unsigned char salt[16];			// Time salt
	unsigned char nonce[20];		// message digest of passwd+salt

	unsigned char pcrdecrypt[148+128+8];  
	
	
	FILE *fp;
	unsigned char  rc[29]="\x00\x03\x00\x00\x01\x0\x0\x0\x14\x8F\xB2\xCB\xE2\x37\x09\x2C\x0BA\xA8\xF8\x16\xB4\x7F\x06\xD6\x50\x2B\x65\x83\x76";
	unsigned char pcr_digest[20],pcr_data[40],pcr_tpm[20];
	unsigned char pcr_value[21]="12345678901234567890";
	int first;
	char * nameOfFunction="DRAClient";

	// Pcr record parameters
	const int  pcrNumber=24;
	const int  pcrValueLength=20;
	char pcrIndexPrefix[9]="pcr[15]:";
	struct pcrRecord
	{
		char pcrIndexStr[sizeof(pcrIndexPrefix)];
		unsigned char pcrValues[pcrValueLength];
	} pcrRecordNeed;
	unsigned char RequiredPCRASCII[pcrValueLength*2];
	unsigned char RequiredPCR[pcrValueLength];
	int reqpcr=15;
	FILE *fpRec;
	int PCRMatch[MaxIPNum];
	int IPCnt=0;
	int j;

	// Signing PCR related
	unsigned char tpmVersion[4];

	unsigned char pcrComposite[48];

	unsigned char signedPCR[256];

	int siglen = sizeof(signedPCR);

	unsigned char TPMKeyPub[256];

	unsigned char TPMKeyPubSignedText[256];

	unsigned char sha1TPMKeyPub[20];

	unsigned char sha1PCRComposite[20];

	unsigned char sha1QuoteInfo[20];
*/
	if(argc != 3)
	{
		printf("===== Please Input as: ./DRAClient [Local IP] [IP Set Strings] =====\n");
		printf("[IP Set Strings] example: 192.168.0.33:192.168.0.44\n");
		exit(ERROR_BAD_PARAMFORMAT);
	}
	else
	{
		strcpy( srcIP, argv[1]);
		str2file(argv[2],IPsetFile);
		//strcpy( RequiredPCRASCII, argv[3]);
		//ASCII2bin(RequiredPCRASCII,RequiredPCR);
		strcpy( bkupSrcIP, argv[1]);
	}

	
	//start_timer();
	/*
	*	Read the server's public key from the file severpub.pem
	*/
/*	in = BIO_new_file("serverpub.pem","rb");
	serverpubkey = RSA_new();
	serverpubkey = PEM_read_bio_RSAPublicKey(in, &serverpubkey, NULL, NULL);	
	if(serverpubkey->e != NULL)
		printf("===== Read Server Public Key OK! =====\n");
	else
	{
		printf("Read Server Public Key error!\n");
		RSA_free(serverpubkey);
		exit(ERROR_READ_SERVERPUB);
	}
	BIO_flush(in);
	BIO_free(in);
*/
	/*
	*	Read the IP set from the file [IPsetFile]
	*/
	fpIPset=fopen(IPsetFile,"r");
	if(fpIPset==NULL)
	{
		printf("Error in func main: open IPsetFile error!\n");
		exit(ERROR_NO_IPSETFILE);
	}

	/*
	*	Process the IPs in the file [IPsetFile],
	*	In file [IPsetFile], each line represents a IP.
	*	Each line is going to be processed in the below loop.
	*	The loop won't be over until all of the IPs have been processed.
	*/

	while(fgets(rcIP,16,fpIPset)!=NULL)
	{
		strcpy( bkuprcIP,  rcIP);
		printf("source IP: %s###\n",rcIP);
		if(strcmp(rcIP,"\n") == 0)  
			goto OUT;		//end of rcIP
		IPCnt++;
		if(IPCnt >= MaxIPNum)
		{
			printf("Error in func main: too many IPs for the program to handle!\n");
		}

		/**
		**		Read one IP from file, and now begin to conmmunicate with it.
		**/
		printf("\n--> -->  Dealing with No.%d: %s", IPCnt, rcIP);

		/**
		**		Init the port, send the connection request message,
		**		and receive the returned message.
		**/
		printf("===== Start to Communicate with RAServer =====\n");
		if (0 > (clientfd = initConnect(bkupSrcIP, bkuprcIP, RASERVERPORT)))
		{
			printf("===== Connect to server Failed =====\n");
			printf("ERROR ");
			printf("%s", bkuprcIP);
			PCRMatch[IPCnt] = -1;
			continue;
		}
	
		if (0 != (dwStatus = SendMessage(clientfd, srcIP, rcIP, COMM_REQ_CONNECT, 0, NULL)))
		{
			printf("===== Send messeage to RA SERVER Failed! =====\n");
			printf("ERROR ");
			printf("%s", bkuprcIP);
			PCRMatch[IPCnt] = -1;
			continue;
		}

		if (0 != (dwStatus = ReceiveMessage(clientfd, rcIP, &type, &dwTransLength,&buffer)))
		{
			printf("===== Received message from RA SERVER Failed! =====\n");
			printf("ERROR ");
			printf("%s", bkuprcIP);
			PCRMatch[IPCnt] = -1;
			continue;
		}
		close(clientfd);

		/**
		**		Switch the [type] of the connection:
		**		If the server is not ready to be connected, the [type] is COMM_RSP_DENY;
		**		If the server is ready to be connected, the [type] is COMM_RSP_ALLOW;
		**		If unknown stituation happens, the [type] is the other value.
		**/
		printf("#####type: %d\n",type);
		switch (type)
		{

			case COMM_RSP_DENY:						
				printf("====== Check Status DONE: CONNECTION Forbidden! ======\n");
				printf("ERROR ");
				printf("%s", bkuprcIP);
				PCRMatch[IPCnt] = -1;
				continue;	

			case COMM_RSP_ALLOW:
				printf("===== Check Status DONE: CONNECTION Permited! =====\n");
				if (0 > (clientfd = initConnect(bkupSrcIP, bkuprcIP, RASERVERPORT)))
				{
					printf("===== Connect to RA SERVER Failed! =====\n");
					printf("ERROR ");
					printf("%s", bkuprcIP);
					PCRMatch[IPCnt] = -1;
					continue;
				}

				/***
				***		Generate a random number [nonce].
				***		The random number [nonce] to the Server when requiring the PCR value,
				***		in order to make sure that the session is fresh.
				***/
/*				*((time_t*)salt) = time(0);
				SHA_CTX c;
				SHA1_Init(&c);
				SHA1_Update(&c, salt, sizeof(time_t));
				SHA1_Final(nonce, &c);

#ifdef DAOLIDEBUG
				int i;
				printf("##The content of the nonce is:##\n");
				for(i = 0; i < 20; i++)
				{
					if((i != 0)&&(i % 16 == 0))	printf("\n");
					//nonce[i]=0X11;
					printf("%02X ", *(nonce + i));
				}
				printf("\n");
				printf("##The content of the nonce is over##\n");
#endif

				OpenSSL_add_all_algorithms();
				memset(salt, 0, strlen(salt));
*/	
				/***
				***		Init the conmmunication of the socket,
				***		and send the decrypted [nonce] to the server.
				***/

				if (0 != (dwStatus = SendMessage(clientfd, srcIP, rcIP, RA_REQ_PCR, 20, nonce)))
				{						
					printf("===== Send messeage to RC SERVER Failed! =====\n");
					close(clientfd);
					printf("ERROR ");
					printf("%s", bkuprcIP);
					PCRMatch[IPCnt] = -1;
					continue;
				}
				break;

			case RA_RSP_ERROR_RECEIVEDATA:
				printf("===== Some error happened when server received the data! =====\n");
				close(clientfd);
				printf("ERROR ");
				printf("%s", bkuprcIP);
				PCRMatch[IPCnt] = -1;
				continue;

			case RA_RSP_ERROR_READDATA:
				printf("===== Some error happened when server read the data! =====\n");
				close(clientfd);
				printf("ERROR ");
				printf("%s", bkuprcIP);
				PCRMatch[IPCnt] = -1;
				continue;

			default:
				printf("===== Check Status DONE: Unknown Status! =====\n");			
				break;				
		}//switch


		/**
		**		Deal With the received information of PCR.
		**		Verify the signature of the PCR, and the signature of the TPM's Signing Key.
		**		The PCR's value is signed by the TPM's Signing Key.	
		**/
		if (0 != (dwStatus = ReceiveMessage(clientfd, rcIP, &type, &dwTransLength,&buffer)))
		{
			printf("===== ReceiveMessage Failed! =====\n");
			close(clientfd);
			printf("ERROR ");
			printf("%s", bkuprcIP);
			PCRMatch[IPCnt] = -1;
			continue;	
		}
		close(clientfd);


		switch (type)
		{
			case RA_RSP_PCR:

				//printf("\n===== The length of the PCR response is: %d =====\n", dwTransLength);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
//				printf("TPM_PCRVALUE======%d",sizeof(TPM_PCRVALUE));
				memcpy(&pcrCompositeSize,buffer,4);
				memcpy(&signedPCRSize,buffer + sizeof(pcrCompositeSize),4);
				memcpy(&keyPubSize,buffer + sizeof(pcrCompositeSize) + sizeof(keyPubSize),4);
				memcpy(&sizeof_rc,buffer + sizeof(pcrCompositeSize) + sizeof(keyPubSize) + sizeof(signedPCRSize),4);
				printf("sizeof_rc=====%d\n\n\n\n\n\n\n\n",sizeof_rc);
				printf("keyPubSize=====%d\n\n\n\n\n\n\n\n",keyPubSize);
				
				pcrComposite=malloc(pcrCompositeSize);
				signedPCR=malloc(signedPCRSize);
				keyPub=malloc(keyPubSize);
				rc=malloc(sizeof_rc);
				
				memcpy(pcrComposite,buffer+4*sizeof(pcrCompositeSize),pcrCompositeSize);
				memcpy(signedPCR,buffer+4*sizeof(pcrCompositeSize)+pcrCompositeSize,signedPCRSize);
				memcpy(keyPub,buffer+4*sizeof(pcrCompositeSize)+pcrCompositeSize+signedPCRSize,keyPubSize);
				memcpy(rc,buffer+4*sizeof(pcrCompositeSize)+pcrCompositeSize+signedPCRSize+keyPubSize,sizeof_rc);
				
				printf("\n===== The length of the PCR response(buffer) is: %d =====\n", dwTransLength);
				MYDBGLF(DINFO,"##The content of the PCR response buffer is:##\n");
				print_hex(buffer,dwTransLength);
				MYDBGLF(DINFO,"##The content of the PCR response buffer is over##\n\n");

				printf("signedPCRSize: %d\n\n",signedPCRSize);
				MYDBGLF(DINFO,"##The content of the signedPCR is:##\n");
				print_hex(buffer + 4*sizeof(pcrCompositeSize) + pcrCompositeSize,signedPCRSize);
				MYDBGLF(DINFO,"##The content of the signedPCR is over##\n\n");

				printf("keyPubSize: %d\n\n",keyPubSize);
				MYDBGLF(DINFO,"##The content of the keyPub is:##\n");
				print_hex(buffer + 4*sizeof(pcrCompositeSize) + pcrCompositeSize + signedPCRSize,signedPCRSize);
				MYDBGLF(DINFO,"##The content of the keyPub is over##\n\n");
				
				printf("sizeof_rc: %d\n\n",sizeof_rc);				
				MYDBGLF(DINFO,"##The content of the rc is:##\n");
				print_hex(buffer + 4*sizeof(pcrCompositeSize) + pcrCompositeSize + signedPCRSize + keyPubSize,sizeof_rc);
				MYDBGLF(DINFO,"##The content of the rc is over##\n\n");

//**************************************************************************************************************************************************//
//****************************************************   verify PCR signature  *********************************************************************//
//**************************************************************************************************************************************************//

//	unsigned char *getkeyPub;
//	unsigned long getkeyPubSize;
	unsigned char sha1PCRComposite[20];
	int result1 = 0;	

	SHA_CTX b;
	SHA1_Init(&b);
	SHA1_Update(&b, pcrComposite, pcrCompositeSize);
	SHA1_Final(sha1PCRComposite, &b);
	
	//	Prepare to verify PCR signature
	unsigned char exp[] = {0x01, 0x00, 0x01};			//	65537	hex
	
	RSA *rsa = RSA_new();
	if(rsa == NULL)
	{
		printf("===== Generate The RSA key error =====\n");
		printf("ERROR ");
		//return;	
	}
	
	//	Set the public key value in the OpenSSL object
	rsa->n = BN_bin2bn(keyPub, keyPubSize, rsa->n);
	//	Set the public exponent
	rsa->e = BN_bin2bn(exp, sizeof(exp), rsa->e);
	
	if((rsa->n == NULL) || (rsa->e == NULL))
	{
		printf("===== Assign RSA key error =====\n");
		printf("ERROR ");
		//return;	
	}
		
	// Verify SHA1(TCPA_QUOTE_INFO) signature
	printf("Verify SHA1(TCPA_QUOTE_INFO) signature...\n");
	sleep(3);
	result1 = RSA_verify(NID_sha1, sha1PCRComposite, 20, signedPCR, signedPCRSize, rsa);
	if(result1 == 0)
	{
		printf("===== Error in PCR verify =====\n");
		printf("ERROR ");
		//return;	
	}
	else
		printf("===== PCR signature verify OK =====\n");
//**************************************************************************************************************************************************//
//****************************************************   verify PCR signature over  ****************************************************************//
//**************************************************************************************************************************************************//





//************************************************************************//
//* 		Challenger code: verify PCRValue with Composite		 *//
//************************************************************************//
		printf("Verify nonce...\n");
		sleep(3);
				/*SHA_CTX a;
				SHA1_Init(&a);
				SHA1_Update(&a, rc, sizeof_rc);
				SHA1_Final(pcr_digest, &a);
				
				
				MYDBGLF(DINFO,"##The content of the pcr_digest is:##\n");
				print_hex(pcr_digest, 20);
				MYDBGLF(DINFO,"##The content of the pcr_digest is over##\n\n");*/
				
				printf("pcrCompositeSize: %d\n\n",pcrCompositeSize);
				
				MYDBGLF(DINFO,"##The content of the pcrComposite is:##\n");
				print_hex(pcrComposite,pcrCompositeSize);
				MYDBGLF(DINFO,"##The content of the pcrComposite is over##\n\n");				

				/*MYDBGLF(DINFO,"##The content of the pcrComposite(pcr_digest) is:##\n");
				print_hex(pcrComposite+8,20);
				MYDBGLF(DINFO,"##The content of the pcrComposite(pcr_digest) is over##\n\n");

				if(!memcmp(pcr_digest, pcrComposite + 8, 20))
					printf("===== PCR digest match! =====\n");	
				else 	MYDBGLF(DERROR,"===== PCR digest not match! =====\n");

				//print_hex(sha1PCRComposite, 20);*/

				MYDBGLF(DINFO,"##The content of the pcrComposite(nonce) is:##\n");
				print_hex(pcrComposite+28,20);
				MYDBGLF(DINFO,"##The content of the pcrComposite(nonce) is over##\n\n");

				if(!memcmp(nonce, pcrComposite + 28, 20))
				printf("===== nonce match! =====\n");
				else 	MYDBGLF(DIMPORTANT,"===== nonce not match! =====\n");

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


				//int pcrCompositeSize = dwTransLength-4-8-(256*2);
				
				/*
				*pcrCompositeSize consists of 4 parts
				*	1.tpm version information ,4B
				*	2.magic info:"QUTE":4B
				*	3.pcr digest,20B
				*	4.antireplay nonce,20B
				*	totally:48B
				*/
/*				memcpy(tpmVersion, buffer+8, 4);
				memcpy(pcrComposite, buffer+8+4, pcrCompositeSize);
				memcpy(signedPCR, buffer+8+4+pcrCompositeSize, 256);
				memcpy(TPMKeyPub, buffer+8+4+pcrCompositeSize+256, 256);
*/				//memcpy(TPMKeyPubSignedText, buffer+8+4+pcrCompositeSize+256+256, 256);

//#ifdef DAOLIDEBUG			
				/**
				**		Display what have been received for the PCR request.
				**/
/*				printf("##The content of pcrComposite is:##\n");for(i=0;i<20;i++)
				for(i = 0; i < pcrCompositeSize; i++)
				{
					if((i != 0)&&(i % 16 == 0))	printf("\n");
					printf("%02X ", *(pcrComposite + i));
				}
				printf("\n");
				printf("##The content of pcrComposite is over##\n");

				printf("##The content of the SignedPCR SHA1(TCPA_QUOTE_INFO) is:##\n");
				for(i = 0; i < 256; i++)
				{
					if((i != 0)&&(i % 16 == 0))	printf("\n");
					printf("%02X ", *(signedPCR + i));
				}
				printf("\n");
				printf("##The content of the SignedPCR SHA1(TCPA_QUOTE_INFO) is over##\n");
*/

/*				printf("##The content of the public part of TPM Signing Key is:##\n");
				for(i = 0; i < 256; i++)
				{
					if((i != 0)&&(i % 16 == 0))	printf("\n");
					printf("%02X ", *(TPMKeyPub + i));
				}
				printf("\n");
				printf("##The content of public part of TPM Signing Key is over##\n");

#endif
*/

/*				SHA_CTX b;
				SHA1_Init(&b);
				SHA1_Update(&b, pcrComposite, pcrCompositeSize);
				SHA1_Final(sha1PCRComposite, &b);

				//	Prepare to verify PCR signature
				unsigned char exp[] = {0x01, 0x00, 0x01};			//	65537	hex

				RSA *rsa = RSA_new();
				if(rsa == NULL)
				{
					SHA_CTX a;
					SHA1_Init(&a);
					SHA1_Update(&a, rc, 28);
					SHA1_Final(pcr_digest, &a);
					printf("===== Generate The RSA key error =====\n");
					printf("ERROR ");
					printf("%s", bkuprcIP);
					continue;	
				}
	
				//	Set the public key value in the OpenSSL object
				rsa->n = BN_bin2bn(TPMKeyPub, 256, rsa->n);
				//	Set the public exponent
				rsa->e = BN_bin2bn(exp, sizeof(exp), rsa->e);

				if((rsa->n == NULL) || (rsa->e == NULL))
				{
					printf("===== Assign RSA key error =====\n");
					printf("ERROR ");
					printf("%s", bkuprcIP);
					continue;	
				}
				
				
				// Verify SHA1(TCPA_QUOTE_INFO) signature
				result = RSA_verify(NID_sha1, sha1PCRComposite, 20, signedPCR, 256, rsa);
				if(result == 0)
				{
					printf("===== Error in PCR verify =====\n");
					printf("ERROR ");
					printf("%s", bkuprcIP);
					continue;	
				}
				else
					printf("===== PCR signature verify OK =====\n");

				//get pcr value from the log file and perpare to match pcr digest
				/*fp=fopen("/root/IBMRA/pcr.txt","r");
				if(fpIPset==NULL)
				{
					printf("Error in func main: open pcr logfile error!\n");
					goto OUT;
				}
				if(NULL==fgets(pcr_value,21,fp))
				{
					printf("Error in func main: read log file error!\n");
					goto OUT;
				}
				
				pcr_value[20]='\0';*/
/*				memset(pcr_data,0xFF,20);
				memcpy(pcr_data+20,pcr_value,20);
*/				
				//SHA1 the pcr value from the log file in order to get the value of pcr in the tpm 
/*				SHA_CTX c;
				SHA1_Init(&c);
				SHA1_Update(&c, pcr_data, 40);
				SHA1_Final(pcr_tpm, &c);
*/				
				//do the paddding what Tspi_TPM_Quote do to get the final pcr digest 
/*				for(i=0;i<20;i++){ 
					rc[9+i]=pcr_tpm[i];
					printf("%02x,%02x##",rc[i+9],pcr_tpm[i]);
*///				}
				//fclose(fp);
				
/*				SHA_CTX a;
				SHA1_Init(&a);
				SHA1_Update(&a, rc, 29);
				SHA1_Final(pcr_digest, &a);
				
				//match the pcr digest,if not match,exit
				printf("===== PCR Match Results =====\n");
				for(i=0;i<20;i++){
					printf("%02X,%02X,%02X\n",rc[i+9],pcr_digest[i],pcrComposite[8+i]);
					if(pcr_digest[i]!=pcrComposite[8+i]){
						printf("ERROR!PCR digest not match!\n");
						//goto OUT;
					}
				}
				printf("PCR digest match!\n");
*/				
				break;
	
			case	RA_RSP_ERROR_DECRYTION:
				printf("===== Some error happened to decrypt the nonce =====\n");
				close(clientfd);
				printf("ERROR ");
				printf("%s", bkuprcIP);
				PCRMatch[IPCnt] = -1;
				break;

			case RA_RSP_ERROR_TCSD:
				printf("===== Some error happened to TCSD! =====\n");
				close(clientfd);
				printf("ERROR ");
				printf("%s", bkuprcIP);
				PCRMatch[IPCnt] = -1;
				break;

			case RA_RSP_ERROR_SSLSIGN:
				printf("===== Some error happened when OPENSSL Signing! =====\n");
				close(clientfd);
				printf("ERROR ");
				printf("%s", bkuprcIP);
				PCRMatch[IPCnt] = -1;
				break;
	
			case RA_RSP_ERROR_RECEIVEDATA:
				printf("===== Some error happened when server received the data! =====\n");
				close(clientfd);
				printf("ERROR ");
				printf("%s", bkuprcIP);
				PCRMatch[IPCnt] = -1;
				break;

			case RA_RSP_ERROR_READDATA:
				printf("===== Some error happened when server read the data! =====\n");
				close(clientfd);
				printf("ERROR ");
				printf("%s", bkuprcIP);
				PCRMatch[IPCnt] = -1;
				break;

			default:
				printf("===== Unknown error happened to the server! =====\n");
				close(clientfd);
				printf("ERROR ");
				printf("%s", bkuprcIP);	
				PCRMatch[IPCnt] = -1;		
				break;						

		}//switch

	}//while


OUT:
//	stop_timer();
//	tpm_print_timer(nameOfFunction);
	exit(0);
	return 0;
}












































