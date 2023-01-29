#include <pcap.h> 
#include <stdio.h>
#include <arpa/inet.h>
#include "myheader.h"
#include <ctype.h>

/* This function will be invoked by pcap for each captured packet. 
We can process each packet inside the function. */

void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) 
{ 
	int i=0;
	int size_data=0;
	printf("\nGot a packet\n"); 
	struct ethheader *eth=(struct ethheader *)packet;
	
	if(ntohs(eth->ether_type) == 0x800)
	{
		struct ipheader *ip = (struct ipheader *)(packet + sizeof(struct ethheader));
		printf("	From: %s\n",inet_ntoa(ip->iph_sourceip));
		printf("	To: %s\n",inet_ntoa(ip->iph_destip));

		struct tcpheader *tcp = (struct tcpheader *)(packet + sizeof(struct ethheader) + sizeof(struct ipheader));

		printf("	Source Port: %d\n",ntohs(tcp->tcp_sport));
		printf("	Destination Port: %d\n",ntohs(tcp->tcp_dport));
	

		switch(ip->iph_protocol) {
			case IPPROTO_TCP:
				printf("	Protocol: TCP\n");
				break;
			case IPPROTO_UDP:
				printf("	Protocol: UDP\n");
				break;
			case IPPROTO_ICMP:
				printf("	Protocol: ICMP\n");
				break;
			default:
				printf("	Protocol: Others\n");
				break;
			}


		char *data = (u_char *)packet + sizeof(struct ethheader) + sizeof(struct ipheader) + sizeof(struct tcpheader);
		size_data = ntohs(ip->iph_len) - (sizeof(struct ipheader) + sizeof(struct tcpheader));
		if (size_data > 0) {
  			printf("   Payload (%d bytes):\n", size_data);
  			for(i = 0; i < size_data; i++) {
  			if (isprint(*data))
   				printf("%c", *data);
 			 else
   				printf(".");
  			data++;
			}
		}
 	}
	
	
return;

}

int main() 
{ 
	pcap_t *handle; 
	char errbuf[PCAP_ERRBUF_SIZE]; 
	struct bpf_program fp; 
	char filter_exp[] = "proto TCP and dst portrange 10-100"; 
	bpf_u_int32 net;

	// Step 1: Open live pcap session on NIC with interface name
	handle = pcap_open_live("enp0s3", BUFSIZ, 1, 1000, errbuf);

	// Step 2: Compile filter_exp into BPF psuedo-code 
	pcap_compile(handle, &fp, filter_exp, 0, net); 
	pcap_setfilter(handle, &fp);

	// Step 3: Capture packets 
	pcap_loop(handle, -1, got_packet, NULL);

	pcap_close(handle); //Close the handle 
	return 0;
}
