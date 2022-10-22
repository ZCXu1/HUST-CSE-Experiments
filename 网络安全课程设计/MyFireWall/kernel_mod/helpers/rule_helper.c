#include "tools.h"
#include "helper.h"

static struct IPRule *ipRuleHead = NULL;
static DEFINE_RWLOCK(ipRuleLock);

// 在名称为after的规则后新增一条规则，after为空时则在首部新增一条规则
struct IPRule * addIPRuleToChain(char after[], struct IPRule rule) {
    struct IPRule *newRule,*now;
    newRule = (struct IPRule *) kzalloc(sizeof(struct IPRule), GFP_KERNEL);
    if(newRule == NULL) {
        printk(KERN_WARNING "[fw rules] kzalloc fail.\n");
        return NULL;
    }
    memcpy(newRule, &rule, sizeof(struct IPRule));
    // 新增规则至规则链表
    write_lock(&ipRuleLock);
    if(rule.action != NF_ACCEPT) 
        eraseConnRelated(rule); // 消除新增规则的影响
    if(ipRuleHead == NULL) {
        ipRuleHead = newRule;
        ipRuleHead->nx = NULL;
        write_unlock(&ipRuleLock);
        return newRule;
    }
    if(strlen(after)==0) {
        newRule->nx = ipRuleHead;
        ipRuleHead = newRule;
        write_unlock(&ipRuleLock);
        return newRule;
    }
    for(now=ipRuleHead;now!=NULL;now=now->nx) {
        if(strcmp(now->name, after)==0) {
            newRule->nx = now->nx;
            now->nx = newRule;
            write_unlock(&ipRuleLock);
            return newRule;
        }
    }
    // 添加失败
    write_unlock(&ipRuleLock);
    kfree(newRule);
    return NULL;
}

// 删除所有名称为name的规则
int delIPRuleFromChain(char name[]) {
    struct IPRule *now,*tmp;
    int count = 0;
    write_lock(&ipRuleLock);
    while(ipRuleHead!=NULL && strcmp(ipRuleHead->name,name)==0) {
        tmp = ipRuleHead;
        ipRuleHead = ipRuleHead->nx;
        eraseConnRelated(*tmp); // 消除删除规则的影响
        kfree(tmp);
        count++;
    }
    for(now=ipRuleHead;now!=NULL && now->nx!=NULL;) {
        if(strcmp(now->nx->name,name)==0) { // 删除下条规则
            tmp = now->nx;
            now->nx = now->nx->nx;
            eraseConnRelated(*tmp); // 消除删除规则的影响
            kfree(tmp);
            count++;
        } else {
            now = now->nx;
        }
    }
    write_unlock(&ipRuleLock);
    return count;
}

// 将所有规则形成Netlink回包
void* formAllIPRules(unsigned int *len) {
    struct KernelResponseHeader *head;
    struct IPRule *now;
    void *mem,*p;
    unsigned int count;
    read_lock(&ipRuleLock);
    for(now=ipRuleHead,count=0;now!=NULL;now=now->nx,count++);
    *len = sizeof(struct KernelResponseHeader) + sizeof(struct IPRule)*count;
    mem = kzalloc(*len, GFP_ATOMIC);
    if(mem == NULL) {
        printk(KERN_WARNING "[fw rules] kzalloc fail.\n");
        read_unlock(&ipRuleLock);
        return NULL;
    }
    head = (struct KernelResponseHeader *)mem;
    head->bodyTp = RSP_IPRules;
    head->arrayLen = count;
    for(now=ipRuleHead,p=(mem + sizeof(struct KernelResponseHeader));now!=NULL;now=now->nx,p=p+sizeof(struct IPRule))
        memcpy(p, now, sizeof(struct IPRule));
    read_unlock(&ipRuleLock);
    return mem;
}

bool matchOneRule(struct IPRule *rule,
 unsigned int sip, unsigned int dip, unsigned short sport, unsigned int dport, u_int8_t proto) {
    return (isIPMatch(sip,rule->saddr,rule->smask) &&
			isIPMatch(dip,rule->daddr,rule->dmask) &&
			(sport >= ((unsigned short)(rule->sport >> 16)) && sport <= ((unsigned short)(rule->sport & 0xFFFFu))) &&
			(dport >= ((unsigned short)(rule->dport >> 16)) && dport <= ((unsigned short)(rule->dport & 0xFFFFu))) &&
			(rule->protocol == IPPROTO_IP || rule->protocol == proto));
}

// 进行过滤规则匹配，isMatch存储是否匹配到规则
struct IPRule matchIPRules(struct sk_buff *skb, int *isMatch) {
    struct IPRule *now,ret;
	unsigned short sport,dport;
	struct iphdr *header = ip_hdr(skb);
	*isMatch = 0;
	getPort(skb,header,&sport,&dport);
	read_lock(&ipRuleLock);
	for(now=ipRuleHead;now!=NULL;now=now->nx) {
		if(matchOneRule(now,ntohl(header->saddr),ntohl(header->daddr),sport,dport,header->protocol)) {
				ret = *now;
				*isMatch = 1;
				break;
		}
	}
	read_unlock(&ipRuleLock);
	return ret;
}