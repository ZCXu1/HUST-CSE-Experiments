#include "common.h"

struct KernelResponse exchangeMsgK(void *smsg, unsigned int slen) {
	struct sockaddr_nl local;
	struct sockaddr_nl kpeer;
	struct KernelResponse rsp;
	int dlen, kpeerlen = sizeof(struct sockaddr_nl);
	// init socket
	int skfd = socket(PF_NETLINK, SOCK_RAW, NETLINK_MYFW);
	if (skfd < 0) {
		//printf("[exchangeMsgK] can not create a netlink socket\n");
		rsp.code = ERROR_CODE_EXCHANGE;
		return rsp;
	}
	// bind
	memset(&local, 0, sizeof(local));
	local.nl_family = AF_NETLINK;
	local.nl_pid = getpid();
	local.nl_groups = 0;
	if (bind(skfd, (struct sockaddr *) &local, sizeof(local)) != 0) {
		//printf("[exchangeMsgK] bind() error\n");
		close(skfd);
		rsp.code = ERROR_CODE_EXCHANGE;
		return rsp;
	}
	memset(&kpeer, 0, sizeof(kpeer));
	kpeer.nl_family = AF_NETLINK;
	kpeer.nl_pid = 0;
	kpeer.nl_groups = 0;
	// set send msg
	struct nlmsghdr *message=(struct nlmsghdr *)malloc(NLMSG_SPACE(slen)*sizeof(uint8_t));
	if(!message) {
		//printf("[exchangeMsgK] malloc fail");
		close(skfd);
		rsp.code = ERROR_CODE_EXCHANGE;
		return rsp;
	}
	memset(message, '\0', sizeof(struct nlmsghdr));
	message->nlmsg_len = NLMSG_SPACE(slen);
	message->nlmsg_flags = 0;
	message->nlmsg_type = 0;
	message->nlmsg_seq = 0;
	message->nlmsg_pid = local.nl_pid;
	memcpy(NLMSG_DATA(message), smsg, slen);
	// send msg
	if (!sendto(skfd, message, message->nlmsg_len, 0, (struct sockaddr *) &kpeer, sizeof(kpeer))) {
		//printf("[exchangeMsgK] sendto fail");
		close(skfd);
		free(message);
		rsp.code = ERROR_CODE_EXCHANGE;
		return rsp;
	}
	// recv msg
	struct nlmsghdr *nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD)*sizeof(uint8_t));
	if (!nlh) {
		//printf("[exchangeMsgK] nlh malloc fail");
		close(skfd);
		free(message);
		rsp.code = ERROR_CODE_EXCHANGE;
		return rsp;
	}
	if (!recvfrom(skfd, nlh, NLMSG_SPACE(MAX_PAYLOAD), 0, (struct sockaddr *) &kpeer, (socklen_t *)&kpeerlen)) {
		//printf("[exchangeMsgK] recvfrom fail");
		close(skfd);
		free(message);
		free(nlh);
		rsp.code = ERROR_CODE_EXCHANGE;
		return rsp;
	}
	dlen = nlh->nlmsg_len - NLMSG_SPACE(0);
	rsp.data = malloc(dlen);
	if(!(rsp.data)) {
		//printf("[exchangeMsgK] dmsg malloc fail");
		close(skfd);
		free(message);
		free(nlh);
		rsp.code = ERROR_CODE_EXCHANGE;
		return rsp;
	}
	memset(rsp.data, 0, dlen);
	memcpy(rsp.data, NLMSG_DATA(nlh), dlen);
	rsp.code = dlen - sizeof(struct KernelResponseHeader);
	if(rsp.code < 0) {
		rsp.code = ERROR_CODE_EXCHANGE;
	}
	rsp.header = (struct KernelResponseHeader*)rsp.data;
	rsp.body = rsp.data + sizeof(struct KernelResponseHeader);
	// over
	close(skfd);
	free(message);
	free(nlh);
	return rsp;
}