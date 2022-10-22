#ifndef _TOOLS_H
#define _TOOLS_H

#include "dependency.h"

void getPort(struct sk_buff *skb, struct iphdr *hdr, unsigned short *src_port, unsigned short *dst_port);
bool isIPMatch(unsigned int ipl, unsigned int ipr, unsigned int mask);

#endif