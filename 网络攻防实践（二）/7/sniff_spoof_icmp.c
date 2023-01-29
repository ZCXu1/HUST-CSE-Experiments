#include <pcap.h> 
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "myheader.h"


void reply_icmp_pkt(struct ipheader *ip, struct icmpheader *icmp);
unsigned short in_cksum(unsigned short *buf,int length);
void send_raw_ip_packet(struct ipheader* ip);


void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) 
{ 
	struct ethheader *eth=(struct ethheader *)packet;
	
	if(ntohs(eth->ether_type) == 0x800)
	{
		struct ipheader *ip = (struct ipheader *)(packet + sizeof(struct ethheader));
        if (ip->iph_protocol == IPPROTO_ICMP) {
            printf("Got an ICMP packet\n");
            printf("	From: %s\n",inet_ntoa(ip->iph_sourceip));
		    printf("	To: %s\n",inet_ntoa(ip->iph_destip));
            struct icmpheader *icmp = (struct icmpheader *)(packet + sizeof(struct ethheader) + sizeof(struct ipheader));
            if (icmp->icmp_type == 8) {
                reply_icmp_pkt(ip, icmp);
            }
        }
    }
}

int main()
{ 
	pcap_t *handle; 
	char errbuf[PCAP_ERRBUF_SIZE]; 
	struct bpf_program fp; 
	char filter_exp[] = "ip proto icmp"; 
	bpf_u_int32 net;

	// Step 1: Open live pcap session on NIC with interface name
	handle = pcap_open_live("docker0", BUFSIZ, 1, 1000, errbuf);

	// Step 2: Compile filter_exp into BPF psuedo-code 
	pcap_compile(handle, &fp, filter_exp, 0, net); 
	pcap_setfilter(handle, &fp);

	// Step 3: Capture packets 
	pcap_loop(handle, -1, got_packet, NULL);

	pcap_close(handle); //Close the handle 
	return 0;
}

void reply_icmp_pkt(struct ipheader *ip, struct icmpheader *icmp) {
    char buffer[PACKET_LEN];
    memset(buffer, 0, PACKET_LEN);

    struct icmpheader *reply_icmp;
    reply_icmp = (struct icmpheader *)(buffer + sizeof(struct ipheader));
    struct ipheader *reply_ip;
    reply_ip = (struct ipheader *) buffer;
    int icmp_len = ntohs(ip->iph_len) - sizeof(struct ipheader);
	memcpy(reply_icmp, icmp, icmp_len);

    reply_icmp->icmp_type = 0;
    reply_icmp->icmp_chksum = 0;
    void *temp = reply_icmp;
    reply_icmp->icmp_chksum = in_cksum(temp, icmp_len);
	reply_ip->iph_ver = 4;
	reply_ip->iph_ihl = 5;
	reply_ip->iph_tos = 16;
	reply_ip->iph_ident = htons(54321);
	reply_ip->iph_ttl = 64;
	reply_ip->iph_sourceip.s_addr = ip->iph_destip.s_addr;
	reply_ip->iph_destip.s_addr = ip->iph_sourceip.s_addr;
	reply_ip->iph_protocol = IPPROTO_ICMP; // The value is 1, representing ICMP.
	reply_ip->iph_len = htons(sizeof(struct ipheader) + icmp_len);

	send_raw_ip_packet(reply_ip);
	return;
}

unsigned short in_cksum(unsigned short *buf,int length)
{
        unsigned short *w = buf;
        int nleft = length;
        int sum = 0;
        unsigned short temp=0;

        /*
        * The algorithm uses a 32 bit accumulator (sum), adds
        * sequential 16 bit words to it, and at the end, folds back all the
        * carry bits from the top 16 bits into the lower 16 bits.
        */
        while (nleft > 1)  {
                sum += *w++;
                nleft -= 2;
        }

        /* treat the odd byte at the end, if any */
        if (nleft == 1) {
                *(u_char *)(&temp) = *(u_char *)w ;
                sum += temp;
        }

        /* add back carry outs from top 16 bits to low 16 bits */
        sum = (sum >> 16) + (sum & 0xffff);     // add hi 16 to low 16 
        sum += (sum >> 16);                     // add carry 
        return (unsigned short)(~sum);
}

/******************************************************************************* 
  Given an IP packet, send it out using raw socket. 
*******************************************************************************/
void send_raw_ip_packet(struct ipheader* ip)
{
    struct sockaddr_in dest_info;
    int enable = 1;

    // Create a raw network socket, and set its options.
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &enable, sizeof(enable));

    // Provide needed information about destination.
    dest_info.sin_family = AF_INET;
    dest_info.sin_addr = ip->iph_destip;

    // Send the packet out.
    printf("Sending spoofed IP packet...\n");
    if(sendto(sock,ip,ntohs(ip->iph_len),0,(struct sockaddr *)&dest_info,sizeof(dest_info)) < 0)
    {
        perror("PACKET NOT SENT\n");
        return;
    }
    else {
        printf("\n---------------------------------------------------\n");
        printf("   From: %s\n",inet_ntoa(ip->iph_sourceip));
        printf("   To: %s\n",inet_ntoa(ip->iph_destip));
        printf("\n---------------------------------------------------\n");
    }
    close(sock);
}