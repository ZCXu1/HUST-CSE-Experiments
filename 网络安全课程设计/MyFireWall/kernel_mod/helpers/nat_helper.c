#include "tools.h"
#include "helper.h"

static struct NATRecord *natRuleHead = NULL;
static DEFINE_RWLOCK(natRuleLock);

// 首部新增一条NAT规则
struct NATRecord * addNATRuleToChain(struct NATRecord rule) {
    struct NATRecord *newRule;
    newRule = (struct NATRecord *) kzalloc(sizeof(struct NATRecord), GFP_KERNEL);
    if(newRule == NULL) {
        printk(KERN_WARNING "[fw nat] kzalloc fail.\n");
        return NULL;
    }
    memcpy(newRule, &rule, sizeof(struct NATRecord));
    // 新增规则至规则链表
    write_lock(&natRuleLock);
    if(natRuleHead == NULL) {
        natRuleHead = newRule;
        natRuleHead->nx = NULL;
        write_unlock(&natRuleLock);
        return newRule;
    }
    newRule->nx = natRuleHead;
    natRuleHead = newRule;
    write_unlock(&natRuleLock);
    return newRule;
}

// 删除序号为num的NAT规则
int delNATRuleFromChain(int num) {
    struct NATRecord *now,*tmp;
    struct IPRule iprule;
    int count = 0;
    write_lock(&natRuleLock);
    if(num == 0) {
        tmp = natRuleHead;
        natRuleHead = natRuleHead->nx;
        kfree(tmp);
        write_unlock(&natRuleLock);
        return 1;
    }
    for(now=natRuleHead,count=1;now!=NULL && now->nx!=NULL;now=now->nx,count++) {
        if(count == num) { // 删除规则
            tmp = now->nx;
            now->nx = now->nx->nx;
            iprule.saddr = tmp->saddr; // 消除连接池影响
            iprule.smask = tmp->smask;
            iprule.dmask = 0;
            iprule.sport = 0xFFFFu;
            iprule.dport = 0xFFFFu;
            eraseConnRelated(iprule);
            kfree(tmp);
            write_unlock(&natRuleLock);
            return 1;
        }
    }
    write_unlock(&natRuleLock);
    return 0;
}

// 将所有NAT规则形成Netlink回包
void* formAllNATRules(unsigned int *len) {
    struct KernelResponseHeader *head;
    struct NATRecord *now;
    void *mem,*p;
    unsigned int count;
    read_lock(&natRuleLock);
    for(now=natRuleHead,count=0;now!=NULL;now=now->nx,count++);
    *len = sizeof(struct KernelResponseHeader) + sizeof(struct NATRecord)*count;
    mem = kzalloc(*len, GFP_ATOMIC);
    if(mem == NULL) {
        printk(KERN_WARNING "[fw nat] kzalloc fail.\n");
        read_unlock(&natRuleLock);
        return NULL;
    }
    head = (struct KernelResponseHeader *)mem;
    head->bodyTp = RSP_NATRules;
    head->arrayLen = count;
    for(now=natRuleHead,p=(mem + sizeof(struct KernelResponseHeader));now!=NULL;now=now->nx,p=p+sizeof(struct NATRecord))
        memcpy(p, now, sizeof(struct NATRecord));
    read_unlock(&natRuleLock);
    return mem;
}

struct NATRecord *matchNATRule(unsigned int sip, unsigned int dip, int *isMatch) {
    struct NATRecord *now;
    *isMatch = 0;
    read_lock(&natRuleLock);
	for(now=natRuleHead;now!=NULL;now=now->nx) {
		if(isIPMatch(sip, now->saddr, now->smask) &&
           !isIPMatch(dip, now->saddr, now->smask) &&
           dip != now->daddr) {
            read_unlock(&natRuleLock);
            *isMatch = 1;
			return now;
		}
	}
	read_unlock(&natRuleLock);
    return NULL;
}

struct NATRecord genNATRecord(unsigned int preIP, unsigned int afterIP, unsigned short prePort, unsigned short afterPort) {
    struct NATRecord record;
    record.saddr = preIP;
    record.sport = prePort;
    record.daddr = afterIP;
    record.dport = afterPort;
    return record;
}
