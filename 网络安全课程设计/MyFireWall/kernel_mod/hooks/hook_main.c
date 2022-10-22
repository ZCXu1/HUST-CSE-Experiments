#include "tools.h"
#include "helper.h"
#include "hook.h"

unsigned int DEFAULT_ACTION = NF_ACCEPT;

unsigned int hook_main(void *priv,struct sk_buff *skb,const struct nf_hook_state *state) {
    struct IPRule rule;
    struct connNode *conn;
    unsigned short sport, dport;
    unsigned int sip, dip, action = DEFAULT_ACTION;
    int isMatch = 0, isLog = 0;
    // 初始化
	struct iphdr *header = ip_hdr(skb);
	getPort(skb,header,&sport,&dport);
    sip = ntohl(header->saddr);
    dip = ntohl(header->daddr);
    // 查询是否有已有连接
    conn = hasConn(sip, dip, sport, dport);
    if(conn != NULL) {
        if(conn->needLog) // 记录日志
            addLogBySKB(action, skb);
        return NF_ACCEPT;
    }
    // 匹配规则
    rule = matchIPRules(skb, &isMatch);
    if(isMatch) { // 匹配到了一条规则
        printk(KERN_DEBUG "[fw netfilter] patch rule %s.\n", rule.name);
        action = (rule.action==NF_ACCEPT) ? NF_ACCEPT : NF_DROP;
        if(rule.log) { // 记录日志
            isLog = 1;
            addLogBySKB(action, skb);
        }
    }
    // 更新连接池
    if(action == NF_ACCEPT) {
        addConn(sip,dip,sport,dport,header->protocol,isLog);
    }
    return action;
}