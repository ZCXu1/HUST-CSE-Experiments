#include "common.h"

int IPstr2IPint(const char *ipStr, unsigned int *ip, unsigned int *mask){
	// init
	int p = -1, count = 0;
	unsigned int len = 0, tmp = 0, r_mask = 0, r_ip = 0,i;
	for(i = 0; i < strlen(ipStr); i++){
		if(!(ipStr[i]>='0' && ipStr[i]<='9') && ipStr[i]!='.' && ipStr[i]!='/') {
			return -1;
		}
	}
	// 获取掩码
	for(i = 0; i < strlen(ipStr); i++){
        if(p != -1){
            len *= 10;
            len += ipStr[i] - '0';
        }
        else if(ipStr[i] == '/')
            p = i;
    }
	if(len > 32 || (p>=0 && p<7)) {
		return -1;
	}
    if(p != -1){
        if(len)
            r_mask = 0xFFFFFFFF << (32 - len);
    }
    else r_mask = 0xFFFFFFFF;
	// 获取IP
    for(i = 0; i < (p>=0 ? p : strlen(ipStr)); i++){
        if(ipStr[i] == '.'){
            r_ip = r_ip | (tmp << (8 * (3 - count)));
            tmp = 0;
            count++;
            continue;
        }
        tmp *= 10;
        tmp += ipStr[i] - '0';
		if(tmp>256 || count>3)
			return -2;
    }
    r_ip = r_ip | tmp;
	*ip = r_ip;
	*mask = r_mask;
    return 0;
}

int IPint2IPstr(unsigned int ip, unsigned int mask, char *ipStr) {
    unsigned int i,ips[4],maskNum = 32;
    if(ipStr == NULL) {
        return -1;
    }
	if(mask == 0)
		maskNum = 0;
	else {
		while((mask & 1u) == 0) {
                	maskNum--;
                	mask >>= 1;
        	}
	}
    for(i=0;i<4;i++) {
        ips[i] = ((ip >> ((3-i)*8)) & 0xFFU);
    }
	sprintf(ipStr, "%u.%u.%u.%u/%u", ips[0], ips[1], ips[2], ips[3], maskNum);
	return 0;
}

int IPint2IPstrNoMask(unsigned int ip, char *ipStr) {
    unsigned int i,ips[4];
    if(ipStr == NULL) {
        return -1;
    }
    for(i=0;i<4;i++) {
        ips[i] = ((ip >> ((3-i)*8)) & 0xFFU);
    }
	sprintf(ipStr, "%u.%u.%u.%u", ips[0], ips[1], ips[2], ips[3]);
	return 0;
}

int IPint2IPstrWithPort(unsigned int ip, unsigned short port, char *ipStr) {
    if(port == 0) {
        return IPint2IPstrNoMask(ip, ipStr);
    }
    unsigned int i,ips[4];
    if(ipStr == NULL) {
        return -1;
    }
    for(i=0;i<4;i++) {
        ips[i] = ((ip >> ((3-i)*8)) & 0xFFU);
    }
	sprintf(ipStr, "%u.%u.%u.%u:%u", ips[0], ips[1], ips[2], ips[3], port);
	return 0;
}