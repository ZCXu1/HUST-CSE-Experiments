#include "contact.h"

// 新增过滤规则时的用户交互
struct KernelResponse cmdAddRule() {
	struct KernelResponse empty;
	char after[MAXRuleNameLen+1],name[MAXRuleNameLen+1],saddr[25],daddr[25],sport[15],dport[15],protoS[6];
	unsigned short sportMin,sportMax,dportMin,dportMax;
	unsigned int action = NF_DROP, log = 0, proto, i;
	empty.code = ERROR_CODE_EXIT;
	// 前序规则名
	printf("Please input the rule name before this rule: ");
	for(i=0;;i++) {
		if(i>MAXRuleNameLen) {
			printf("The name is too long.\n");
			return empty;
		}
		after[i] = getchar();
		if(after[i] == '\n' || after[i] == '\r') {
			after[i] = '\0';
			break;
		}
	}
	// 规则名
	printf("Please input the rule name (length of Name less than %d): ", MAXRuleNameLen);
	scanf("%s",name);
	if(strlen(name) == 0 || strlen(name)>MAXRuleNameLen) {
		printf("The name is too long or too short.\n");
		return empty;	
	}
	// 源IP
	printf("Please input the src ip and the src mask [like 192.168.0.1/24]: ");
	scanf("%s",saddr);
	// 源端口
	printf("Please input src port range [like 8000-8080 or any]: ");
	scanf("%s",sport);
	if(strcmp(sport, "any") == 0) {
		sportMin = 0,sportMax = 0xFFFFu;
	} else {
		sscanf(sport,"%hu-%hu",&sportMin,&sportMax);
	}
	if(sportMin > sportMax) {
		printf("Min > Max. Error.\n");
		return empty;
	}
	// 目的IP
	printf("Please input the dst ip and the dst mask [like 192.168.0.1/24]: ");
	scanf("%s",daddr);
	// 目的端口
	printf("Please input dst port range [like 8000-8080 or any]: ");
	scanf("%s",dport);
	if(strcmp(dport, "any") == 0) {
		dportMin = 0,dportMax = 0xFFFFu;
	} else {
		sscanf(dport,"%hu-%hu",&dportMin,&dportMax);
	}
	if(dportMin > dportMax) {
		printf("Min > Max. Error.\n");
		return empty;
	}
	// 协议
	printf("Please input the protocol name[TCP/UDP/ICMP/any]: ");
	scanf("%s",protoS);
	if(strcmp(protoS,"TCP")==0)
		proto = IPPROTO_TCP;
	else if(strcmp(protoS,"UDP")==0)
		proto = IPPROTO_UDP;
	else if(strcmp(protoS,"ICMP")==0)
		proto = IPPROTO_ICMP;
	else if(strcmp(protoS,"any")==0)
		proto = IPPROTO_IP;
	else {
		printf("Input error!\n");
		return empty;
	}
	// 动作
	printf("Please select the action[1 for accept,0 for drop]: ");
	scanf("%d",&action);
	// 是否记录日志
	printf("Use log or not[1 for yes,0 for no]: ");
	scanf("%u",&log);
	printf("Conclusion:\n");
	return addFilterRule(after,name,saddr,daddr,
		(((unsigned int)sportMin << 16) | (((unsigned int)sportMax) & 0xFFFFu)),
		(((unsigned int)dportMin << 16) | (((unsigned int)dportMax) & 0xFFFFu)),proto,log,action);
}

struct KernelResponse cmdAddNATRule() {
	struct KernelResponse empty;
	char saddr[25],daddr[25],port[15];
	unsigned short portMin,portMax;
	empty.code = ERROR_CODE_EXIT;
	printf("ONLY source NAT is supported\n");
	// 源IP
	printf("Please input the src ip and the src mask [like 192.168.64.2/24]: ");
	scanf("%s",saddr);
	// NAT IP
	printf("Please input the NAT ip[like 192.168.152.1]: ");
	scanf("%s",daddr);
	// 目的端口
	printf("Please input the NAT port range [like 10000-30000 or any]: ");
	scanf("%s",port);
	if(strcmp(port, "any") == 0) {
		portMin = 0,portMax = 0xFFFFu;
	} else {
		sscanf(port,"%hu-%hu",&portMin,&portMax);
	}
	if(portMin > portMax) {
		printf("Min > Max. Error.\n");
		return empty;
	}
	return addNATRule(saddr,daddr,portMin,portMax);
}

void wrongCommand() {
	printf("Incorrect command!\n");
	printf("./ctrl <command> <sub-command> [option]\n");
	printf("commands: rule <add | del | ls | default> [del rule's name]\n");
	printf("          nat  <add | del | ls> [del number]\n");
	printf("          ls   <rule | nat | log | connect>\n");
	exit(0);
}

int main(int argc, char *argv[]) {
	if(argc<3) {
		wrongCommand();
		return 0;
	}
	struct KernelResponse rsp;
	rsp.code = ERROR_CODE_EXIT;
	// 过滤规则相关
	if(strcmp(argv[1], "rule")==0 || argv[1][0] == 'r') {
		if(strcmp(argv[2], "ls")==0 || strcmp(argv[2], "list")==0) {
		// 列出所有过滤规则
			rsp = getAllFilterRules();
		} else if(strcmp(argv[2], "del")==0) {
		// 删除过滤规则
			if(argc < 4)
				printf("Please point rule name in option.\n");
			else if(strlen(argv[3])>MAXRuleNameLen)
				printf("rule name too long!");
			else
				rsp = delFilterRule(argv[3]);
		} else if(strcmp(argv[2], "add")==0) {
		// 添加过滤规则
			rsp = cmdAddRule();
		} else if(strcmp(argv[2], "default")==0) {
		// 设置默认规则
			if(argc < 4)
				printf("Please point default action in option.\n");
			else if(strcmp(argv[3], "accept")==0)
				rsp = setDefaultAction(NF_ACCEPT);
			else if(strcmp(argv[3], "drop")==0)
				rsp = setDefaultAction(NF_DROP);
			else
				printf("No such action. Only \"accept\" or \"drop\".\n");
		} else 
			wrongCommand();
	} else if(strcmp(argv[1], "nat")==0 || argv[1][0] == 'n') {
		if(strcmp(argv[2], "ls")==0 || strcmp(argv[2], "list")==0) {
		// 列出所有NAT规则
			rsp = getAllNATRules();
		} else if(strcmp(argv[2], "del")==0) {
		// 删除NAT规则
			if(argc < 4)
				printf("Please point rule number(seq) in option.\n");
			else {
				int num;
				sscanf(argv[3], "%d", &num);
				rsp = delNATRule(num);
			}
		} else if(strcmp(argv[2], "add")==0) {
		// 添加NAT规则
			rsp = cmdAddNATRule();
		} else {
			wrongCommand();
		}
	} else if(strcmp(argv[1], "ls")==0 || argv[1][0] == 'l') {
	// 展示相关
		if(strcmp(argv[2],"log")==0 || argv[2][0] == 'l') {
		// 过滤日志
			unsigned int num = 0;
			if(argc > 3)
				sscanf(argv[3], "%u", &num);
			rsp = getLogs(num);
		} else if(strcmp(argv[2],"con")==0 || argv[2][0] == 'c') {
		// 连接状态
			rsp = getAllConns();
		} else if(strcmp(argv[2],"rule")==0 || argv[2][0] == 'r') {
		// 已有过滤规则
			rsp = getAllFilterRules();
		} else if(strcmp(argv[2],"nat")==0 || argv[2][0] == 'n') {
		// 已有NAT规则
			rsp = getAllNATRules();
		} else
			wrongCommand();
	} else 
		wrongCommand();
	dealResponseAtCmd(rsp);
}
