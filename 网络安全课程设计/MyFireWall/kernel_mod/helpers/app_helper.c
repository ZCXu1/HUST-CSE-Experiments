#include "helper.h"

extern unsigned int DEFAULT_ACTION;

int sendMsgToApp(unsigned int pid, const char *msg) {
    void* mem;
    unsigned int rspLen;
    struct KernelResponseHeader *rspH;
    rspLen = sizeof(struct KernelResponseHeader) + strlen(msg) + 1;
    mem = kzalloc(rspLen, GFP_ATOMIC);
    if(mem == NULL) {
        printk(KERN_WARNING "[fw k2app] sendMsgToApp kzalloc fail.\n");
        return 0;
    }
    rspH = (struct KernelResponseHeader *)mem;
    rspH->bodyTp = RSP_MSG;
    rspH->arrayLen = strlen(msg);
    memcpy(mem+sizeof(struct KernelResponseHeader), msg, strlen(msg));
    nlSend(pid, mem, rspLen);
    kfree(mem);
    return rspLen;
}

void dealWithSetAction(unsigned int action) {
    if(action != NF_ACCEPT) {
        struct IPRule rule = {
            .smask = 0,
            .dmask = 0,
            .sport = -1,
            .dport = -1
        }; // 清除全部连接
        eraseConnRelated(rule);
    }
}

int dealAppMessage(unsigned int pid, void *msg, unsigned int len) {
    struct APPRequest *req;
    struct KernelResponseHeader *rspH;
    void* mem;
    unsigned int rspLen = 0;
    req = (struct APPRequest *) msg;
    switch (req->tp)
    {
    case REQ_GETAllIPLogs:
        mem = formAllIPLogs(req->msg.num, &rspLen);
        if(mem == NULL) {
            printk(KERN_WARNING "[fw k2app] formAllIPLogs fail.\n");
            sendMsgToApp(pid, "form all logs fail.");
            break;
        }
        nlSend(pid, mem, rspLen);
        kfree(mem);
        break;
    case REQ_GETAllConns:
        mem = formAllConns(&rspLen);
        if(mem == NULL) {
            printk(KERN_WARNING "[fw k2app] formAllConns fail.\n");
            sendMsgToApp(pid, "form all conns fail.");
            break;
        }
        nlSend(pid, mem, rspLen);
        kfree(mem);
        break;
    case REQ_GETAllIPRules:
        mem = formAllIPRules(&rspLen);
        if(mem == NULL) {
            printk(KERN_WARNING "[fw k2app] formAllIPRules fail.\n");
            sendMsgToApp(pid, "form all rules fail.");
            break;
        }
        nlSend(pid, mem, rspLen);
        kfree(mem);
        break;
    case REQ_ADDIPRule:
        if(addIPRuleToChain(req->ruleName, req->msg.ipRule)==NULL) {
            rspLen = sendMsgToApp(pid, "Fail: no such rule or retry it.");
            printk("[fw k2app] add rule fail.\n");
        } else {
            rspLen = sendMsgToApp(pid, "Success.");
            printk("[fw k2app] add one rule success: %s.\n", req->msg.ipRule.name);
        }
        break;
    case REQ_DELIPRule:
        rspLen = sizeof(struct KernelResponseHeader);
        rspH = (struct KernelResponseHeader *)kzalloc(rspLen, GFP_KERNEL);
        if(rspH == NULL) {
            printk(KERN_WARNING "[fw k2app] kzalloc fail.\n");
            sendMsgToApp(pid, "form rsp fail but del maybe success.");
            break;
        }
        rspH->bodyTp = RSP_Only_Head;
        rspH->arrayLen = delIPRuleFromChain(req->ruleName);
        printk("[fw k2app] success del %d rules.\n", rspH->arrayLen);
        nlSend(pid, rspH, rspLen);
        kfree(rspH);
        break;
    case REQ_GETNATRules:
        mem = formAllNATRules(&rspLen);
        if(mem == NULL) {
            printk(KERN_WARNING "[fw k2app] formAllNATRules fail.\n");
            sendMsgToApp(pid, "form all NAT rules fail.");
            break;
        }
        nlSend(pid, mem, rspLen);
        kfree(mem);
        break;
    case REQ_ADDNATRule:
        if(addNATRuleToChain(req->msg.natRule)==NULL) {
            rspLen = sendMsgToApp(pid, "Fail: please retry it.");
            printk("[fw k2app] add NAT rule fail.\n");
        } else {
            rspLen = sendMsgToApp(pid, "Success.");
            printk("[fw k2app] add one NAT rule success.\n");
        }
        break;
    case REQ_DELNATRule:
        rspLen = sizeof(struct KernelResponseHeader);
        rspH = (struct KernelResponseHeader *)kzalloc(rspLen, GFP_KERNEL);
        if(rspH == NULL) {
            printk(KERN_WARNING "[fw k2app] kzalloc fail.\n");
            sendMsgToApp(pid, "form rsp fail but del maybe success.");
            break;
        }
        rspH->bodyTp = RSP_Only_Head;
        rspH->arrayLen = delNATRuleFromChain(req->msg.num);
        printk("[fw k2app] success del %d NAT rules.\n", rspH->arrayLen);
        nlSend(pid, rspH, rspLen);
        kfree(rspH);
        break;
    case REQ_SETAction:
        if(req->msg.defaultAction == NF_ACCEPT) {
            DEFAULT_ACTION = NF_ACCEPT;
            rspLen = sendMsgToApp(pid, "Set default action to ACCEPT.");
            printk("[fw k2app] Set default action to NF_ACCEPT.\n");
        } else {
            DEFAULT_ACTION = NF_DROP;
            rspLen = sendMsgToApp(pid, "Set default action to DROP.");
            printk("[fw k2app] Set default action to NF_DROP.\n");
        }
        dealWithSetAction(DEFAULT_ACTION);
        break;
    default:
        rspLen = sendMsgToApp(pid, "No such req.");
        break;
    }
    return rspLen;
}