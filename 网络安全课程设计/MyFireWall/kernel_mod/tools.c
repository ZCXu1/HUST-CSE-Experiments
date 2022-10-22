#include "tools.h"

void getPort(struct sk_buff *skb, struct iphdr *hdr, unsigned short *src_port, unsigned short *dst_port){
	struct tcphdr *tcpHeader;
	struct udphdr *udpHeader;
	switch(hdr->protocol){
		case IPPROTO_TCP:
			//printk("TCP protocol\n");
			tcpHeader = (struct tcphdr *)(skb->data + (hdr->ihl * 4));
			*src_port = ntohs(tcpHeader->source);
			*dst_port = ntohs(tcpHeader->dest);
			break;
		case IPPROTO_UDP:
			//printk("UDP protocol\n");
			udpHeader = (struct udphdr *)(skb->data + (hdr->ihl * 4));
			*src_port = ntohs(udpHeader->source);
			*dst_port = ntohs(udpHeader->dest);
			break;
		case IPPROTO_ICMP:
		default:
			//printk("other protocol\n");
			*src_port = 0;
			*dst_port = 0;
			break;
	}
}

bool isIPMatch(unsigned int ipl, unsigned int ipr, unsigned int mask) {
	return (ipl & mask) == (ipr & mask);
}