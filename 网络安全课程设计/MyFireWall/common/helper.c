#include "common.h"

struct KernelResponse addFilterRule(char *after,char *name,char *sip,char *dip,unsigned int sport,unsigned int dport,u_int8_t proto,unsigned int log,unsigned int action) {
	struct APPRequest req;
    struct KernelResponse rsp;
	// form rule
	struct IPRule rule;
	if(IPstr2IPint(sip,&rule.saddr,&rule.smask)!=0) {
		rsp.code = ERROR_CODE_WRONG_IP;
		return rsp;
	}
	if(IPstr2IPint(dip,&rule.daddr,&rule.dmask)!=0) {
		rsp.code = ERROR_CODE_WRONG_IP;
		return rsp;
	}
	rule.saddr = rule.saddr;
	rule.daddr = rule.daddr;
	rule.sport = sport;
	rule.dport = dport;
	rule.log = log;
	rule.action = action;
	rule.protocol = proto;
	strncpy(rule.name, name, MAXRuleNameLen);
	// form req
	req.tp = REQ_ADDIPRule;
	req.ruleName[0]=0;
	strncpy(req.ruleName, after, MAXRuleNameLen);
	req.msg.ipRule = rule;
	// exchange
	return exchangeMsgK(&req, sizeof(req));
}

struct KernelResponse delFilterRule(char *name) {
	struct APPRequest req;
	// form request
	req.tp = REQ_DELIPRule;
	strncpy(req.ruleName, name, MAXRuleNameLen);
	// exchange
	return exchangeMsgK(&req, sizeof(req));
}

struct KernelResponse getAllFilterRules(void) {
	struct APPRequest req;
	// exchange msg
	req.tp = REQ_GETAllIPRules;
	return exchangeMsgK(&req, sizeof(req));
}

struct KernelResponse addNATRule(char *sip,char *natIP,unsigned short minport,unsigned short maxport) {
	struct APPRequest req;
	struct KernelResponse rsp;
	// form rule
	struct NATRecord rule;
	if(IPstr2IPint(natIP,&rule.daddr,&rule.smask)!=0) {
		rsp.code = ERROR_CODE_WRONG_IP;
		return rsp;
	}
	if(IPstr2IPint(sip,&rule.saddr,&rule.smask)!=0) {
		rsp.code = ERROR_CODE_WRONG_IP;
		return rsp;
	}
	rule.sport = minport;
	rule.dport = maxport;
	// form req
	req.tp = REQ_ADDNATRule;
	req.msg.natRule = rule;
	// exchange
	return exchangeMsgK(&req, sizeof(req));
}

struct KernelResponse delNATRule(int num) {
	struct APPRequest req;
	struct KernelResponse rsp;
	if(num < 0) {
		rsp.code = ERROR_CODE_NO_SUCH_RULE;
		return rsp;
	}
	req.tp = REQ_DELNATRule;
	req.msg.num = num;
	// exchange
	return exchangeMsgK(&req, sizeof(req));
}

struct KernelResponse getAllNATRules(void) {
	struct APPRequest req;
	// exchange msg
	req.tp = REQ_GETNATRules;
	return exchangeMsgK(&req, sizeof(req));
}

struct KernelResponse setDefaultAction(unsigned int action) {
	struct APPRequest req;
	// form request
	req.tp = REQ_SETAction;
	req.msg.defaultAction = action;
	// exchange
	return exchangeMsgK(&req, sizeof(req));
}

struct KernelResponse getLogs(unsigned int num) {
	struct APPRequest req;
	// exchange msg
	req.msg.num = num;
	req.tp = REQ_GETAllIPLogs;
	return exchangeMsgK(&req, sizeof(req));
}

struct KernelResponse getAllConns(void) {
	struct APPRequest req;
	// exchange msg
	req.tp = REQ_GETAllConns;
	return exchangeMsgK(&req, sizeof(req));
}
