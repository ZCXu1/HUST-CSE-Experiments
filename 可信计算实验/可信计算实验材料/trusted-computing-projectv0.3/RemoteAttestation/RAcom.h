#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h> 
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <time.h>

/*********************************************************************/
/*struct cpu_state {
    unsigned long eax, ebx, ecx, edx, edi, esi;
    unsigned short cs, ds, es, fs, gs, ss;
    unsigned long efer, eflags, esp, ebp;
    unsigned long cr0, cr2, cr3, cr4;

    /* idt, gdt actually 48 bits each *
    unsigned short gdt_pad;
    unsigned short gdt_limit;
    unsigned long gdt_base;
    unsigned short idt_pad;
    unsigned short idt_limit;
    unsigned long idt_base;
    unsigned short ldt;
    unsigned short tss;
    unsigned long tr;
    unsigned long safety;
    unsigned long return_address;
    /* used for storing timing information across SLB invocation *
    unsigned long startl, starth, endl, endh;
} __attribute__((packed));

typedef struct cpu_state cpu_t;


#define rdtsc(low,high) \
      __asm__ __volatile__("rdtsc" : "=a" (low), "=d" (high))

#define start_timer() \
    rdtsc(startlow, starthigh);

#define stop_timer() \
    rdtsc(endlow, endhigh); 

#define CPU_MHZ 1829

unsigned long startlow, starthigh, endlow, endhigh;
 static void tpm_print_timer(const char *functionname);

static void tpm_print_timer(const char * functionname) {
    /* 64-bit subtract - gcc just messes up with long longs
     * Stolen from arch/i386/kernel/timers/common.c
     *
    __asm__("subl %2,%0\n\t"
            "sbbl %3,%1"
            :"=a" (endlow), "=d" (endhigh)
            :"g" (startlow), "g" (starthigh),
            "0" (endlow), "1" (endhigh));

     printf("%s Elapsed time in TSC ticks: %08lu %08lu \n", 
            functionname,endhigh, endlow / CPU_MHZ ); 


}
*/

/*********************************************************************/

//Const
#define RASERVERPORT	3333	// the num of RAServer port //
#define RACLIENTPORT	4444	// the num of client port //
//#define IMRSERVERPORT	5555	// the num of IMRServer port//               ???
#define BACKLOG 10				// the max num of connection //
#define MAXDATASIZE 65536		// the max num of transferred bytes//
#define MaxIPNum 100  // the max num of IPs that need to compare the PCR

//Client Exit code

#define ERROR_BAD_PARAMFORMAT			1
#define ERROR_NO_IPSETFILE				2

#define ERROR_READ_SERVERPUB			3
#define ERROR_TOO_MANY_SERVER			100



//Communication Type
#define COMM_REQ_CONNECT		0x00000001
#define COMM_RSP_ALLOW			0x00000002
#define COMM_RSP_DENY			0x00000003
#define RA_REQ_PCR				0x00000004
#define RA_RSP_PCR				0x00000005
#define RA_RSP_ERROR			0x00000006
#define RA_REQ_EVENTLOG			0x00000007
#define RA_RSP_EVENTLOG			0x00000008
#define RA_RSP_RESULT			0x00000009
#define COMM_RSP_FINAL			0x0000000A
#define IMR_REQ_EVENTLOG		0x0000000B
#define IMR_RSP_EVENTLOG		0x0000000C
#define IMR_RSP_NORECORD		0x0000000D
#define IMR_RSP_ERROR			0x0000000E
#define COMM_RSP_EVENTLOG     0x0000000F
#define RA_RSP_SESSIONKEY1		0x00000011  //SESSIONKEY BETWEEN C AND S
#define RA_RSP_SESSIONKEY2		0x00000012  //SESSIONKEY BETWEEN C AND IMRS
#define RA_REQ_PLATFORMSTATE	0x00000013
#define RA_RSP_PCRandEVENTLOG	0x00000014
#define RA_REQ_INTEGRITYREF		0x00000015  //C ASK TO IMRS FOR THE INTEGRITY REFERENCE VALUE 
#define RA_RSP_INTEGRITYREF		0x00000016
#define RA_RSP_SESSIONKEY1RESULT		0x00000017
#define RA_RSP_SESSIONKEY2RESULT		0x00000018


#define RA_RSP_ERROR_TCSD			0x00000019
#define RA_RSP_ERROR_SSLSIGN		0x0000001A
#define RA_RSP_ERROR_DECRYTION 	0x0000001B

#define RA_RSP_ERROR_RECEIVEDATA	0x0000001C
#define RA_RSP_ERROR_READDATA		0x0000001D

//Functions
int initConnect(char * strSourceIP, char * strDestIP, int serverport);
int SendMessage(int socketfd, char* strSourceIP, char* strDestIP, int dwCommTYPE, int dwTransLength, char * rgbBuffer);
int ReceiveMessage(int socketfd, char* strSourceIP, int* dwCommTYPE, int * dwTransLength,char * * prgbBuffer);
void releaseMemory(char * rgbBuffer);
int disconnect(int socketfd);
void printReceivedMessage(int socketfd, char* strSourceIP, int dwCommTYPE, int dwTransLength,char *prgbBuffer);
void printSendMessage(int socketfd, char* strSourceIP, char* strDestIP, int dwCommTYPE, int dwTransLength, char * rgbBuffer);

//Structs
typedef	struct 
{
	char index[30];
	char filename[30];
}	eventlogIndex;

