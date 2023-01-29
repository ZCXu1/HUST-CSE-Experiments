#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "myheader.h"

#define SRC_IP   "10.0.2.6"
#define DEST_IP  "8.8.8.8"


unsigned short in_cksum(unsigned short *buf,int length);
void send_raw_ip_packet(struct ipheader* ip);



/******************************************************************
  Spoof an ICMP echo request using an arbitrary source IP Address
*******************************************************************/
int main() {		
   char buffer[PACKET_LEN];
	
   memset(buffer, 0, PACKET_LEN);


   // Construct the ICMP Header
   struct icmpheader *icmp;
   icmp = (struct icmpheader *)(buffer + sizeof(struct ipheader));

   icmp->icmp_type = 8; //ICMP Type: 8 is request, 0 is reply.

   // Calculate the checksum for integrity
   icmp->icmp_chksum = 0; 
   icmp->icmp_chksum = in_cksum((unsigned short *)icmp, 
                                sizeof(struct icmpheader));


   //  Construct the IP header.
   struct ipheader *ip = (struct ipheader *) buffer;
   ip->iph_ver = 4;
   ip->iph_ihl = 5;
   ip->iph_tos = 16;
   ip->iph_ident = htons(54321);
   ip->iph_ttl = 64; 
   ip->iph_sourceip.s_addr = inet_addr(SRC_IP);
   ip->iph_destip.s_addr = inet_addr(DEST_IP);
   ip->iph_protocol = IPPROTO_ICMP; // The value is 1, representing ICMP.
   ip->iph_len = htons(sizeof(struct ipheader) + sizeof(struct icmpheader));
	
   // No need to set the following fileds, as they will be set by the system.
   // ip->iph_chksum = ...;





   //  Finally, send the spoofed packet
   send_raw_ip_packet (ip);

   return 0;
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

