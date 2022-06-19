#include <sys/param.h>
#ifndef BSD
#include <sys/sendfile.h>
#endif
#include <sys/uio.h>
#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#pragma once

#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stddef.h>
ssize_t sendfd(int socket, const void *buffer, size_t length, int fd)
{
    struct iovec iov = {(void *)buffer, length};
    char buf[CMSG_LEN(sizeof(int))];
    struct cmsghdr *cmsg = (struct cmsghdr *)buf;
    ssize_t r;
    cmsg->cmsg_len = sizeof(buf);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    *((int *)CMSG_DATA(cmsg)) = fd;
    struct msghdr msg = {0};
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = cmsg;
    msg.msg_controllen = cmsg->cmsg_len;
    r = sendmsg(socket, &msg, 0);
    if (r < 0)
        warn("sendmsg");
    return r;
}

ssize_t recvfd(int socket, void *buffer, size_t length, int *fd)
{
    struct iovec iov = {buffer, length};
    char buf[CMSG_LEN(sizeof(int))];
    struct cmsghdr *cmsg = (struct cmsghdr *)buf;
    ssize_t r;
    cmsg->cmsg_len = sizeof(buf);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    struct msghdr msg = {0};
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = cmsg;
    msg.msg_controllen = cmsg->cmsg_len;
again:
    r = recvmsg(socket, &msg, 0);
    if (r < 0 && errno == EINTR)
        goto again;
    if (r < 0)
        warn("recvmsg");
    else
        *fd = *((int*)CMSG_DATA(cmsg));
    return r;
}

