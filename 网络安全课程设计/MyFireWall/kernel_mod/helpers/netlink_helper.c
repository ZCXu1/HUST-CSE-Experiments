#include "helper.h"

static struct sock *nlsk = NULL;

int nlSend(unsigned int pid, void *data, unsigned int len) {
	int retval;
	struct nlmsghdr *nlh;
	struct sk_buff *skb;
	// init sk_buff
	skb = nlmsg_new(len, GFP_ATOMIC);
	if (skb == NULL) {
		printk(KERN_WARNING "[fw netlink] alloc reply nlmsg skb failed!\n");
		return -1;
	}
	nlh = nlmsg_put(skb, 0, 0, 0, NLMSG_SPACE(len) - NLMSG_HDRLEN, 0);
	// send data
	memcpy(NLMSG_DATA(nlh), data, len);
    //NETLINK_CB(skb).portid = 0;
	NETLINK_CB(skb).dst_group = 0;
	retval = netlink_unicast(nlsk, skb, pid, MSG_DONTWAIT);
	printk("[fw netlink] send to user pid=%d,len=%d,ret=%d\n", pid, nlh->nlmsg_len - NLMSG_SPACE(0), retval);
	return retval;
}

void nlRecv(struct sk_buff *skb) {
	void *data;
	struct nlmsghdr *nlh = NULL;
	unsigned int pid,len;
    // check skb
    nlh = nlmsg_hdr(skb);
	if ((nlh->nlmsg_len < NLMSG_HDRLEN) || (skb->len < nlh->nlmsg_len)) {
		printk(KERN_WARNING "[fw netlink] Illegal netlink packet!\n");
		return;
	}
    // deal data
	data = NLMSG_DATA(nlh);
    pid = nlh->nlmsg_pid;
    len = nlh->nlmsg_len - NLMSG_SPACE(0);
	if(len<sizeof(struct APPRequest)) {
		printk(KERN_WARNING "[fw netlink] packet size < APPRequest!\n");
		return;
	}
	printk("[fw netlink] data receive from user: user_pid=%d, len=%d\n", pid, len);
	dealAppMessage(pid, data, len);
}

struct netlink_kernel_cfg nltest_cfg = {
	.groups = 0,
	.flags = 0,
	.input = nlRecv,
	.cb_mutex = NULL,
	.bind = NULL,
	.unbind = NULL,
	.compare = NULL,
};

struct sock *netlink_init() {
    nlsk = netlink_kernel_create(&init_net, NETLINK_MYFW, &nltest_cfg);
	if (!nlsk) {
		printk(KERN_WARNING "[fw netlink] can not create a netlink socket\n");
		return NULL;
	}
	printk("[fw netlink] netlink_kernel_create() success, nlsk = %p\n", nlsk);
    return nlsk;
}

void netlink_release() {
    netlink_kernel_release(nlsk);
}