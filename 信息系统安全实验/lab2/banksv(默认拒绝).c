#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/prctl.h>
#include <seccomp.h>
#include <linux/seccomp.h>
#include <linux/filter.h>
#include "parse.h"
#include "http.h"
#include "handle.h"

#include <unistd.h>
#define DEBUG 1

static void error(int fd, int errcode, char *msg);
static void parse(int fd);

// utilities
void die(const char *msg)
{
  fprintf(stderr, "Error: %s\n", msg);
  exit(0);
}

void kprint(const char *msg)
{
  write(1, msg, sizeof(msg));
  return;
}

void write_file(int sockfd, const char *s)
{
  int size = strlen(s);

  write(sockfd, s, size);
  return;
}

void setSeccomp(){
    // init seccomp
  scmp_filter_ctx ctx;
  // 默认拒绝
  ctx = seccomp_init(SCMP_ACT_KILL);

  // 默认允许
  // ctx = seccomp_init(SCMP_ACT_ALLOW);
  if (ctx == NULL)
    exit(-1);

#if 1
  // 正常需要
  seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(read), 0);
  seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(write), 0);
  seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(openat), 0);
  seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(rt_sigaction), 0);
  seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(socketcall), 0);
  seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(clone), 0);
  seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(set_robust_list), 0);
  seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(getresuid32), 0);
  seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(getcwd), 0);
  seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(getpid), 0);
  seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(statx), 0);
  seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(close), 0);
  seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(_llseek), 0);
  seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(fcntl64), 0);
  seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(access), 0);
  seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(brk), 0);
  seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(exit_group), 0);
  seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(fstat64), 0);
  seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(stat64), 0);

 // 攻击程序需要的权限
  seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(unlink), 0);
# endif
//  seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(unlink), 0);
seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(mkdir), 0);

  seccomp_load(ctx);
  seccomp_release(ctx);
  /*------------------------------*/
}

int main(int argc, char **argv)
{
  setSeccomp();
  Http_t tree;

  if (argc < 2)
    die("server bug");

  signal(SIGCHLD, SIG_IGN);

  // get the pipe fd
  int pipefd = atoi(argv[1]);
  if (DEBUG)
    printf("pipefd = %d\n", pipefd);

  while (1)
  {
    char uri_str[1024];
    int sockfd;
    recvfd(pipefd, uri_str, sizeof(uri_str), &sockfd);
    printf("uri = %s\n", uri_str);
    // int sockfd = atoi (sockfd_str);
    if (DEBUG)
      printf("mailsv client recieves a sockfd = %d\n", sockfd);

    if (fork() == 0) // child
    {
      int ruid, euid, suid;
      getresuid(&ruid, &euid, &suid);
      printf("uid = %d %d %d \n", ruid, euid, suid);

      setReqline(REQ_KIND_POST, uri_str);
      tree = Parse_parse(sockfd, 0);

      // response
      Handle_main(sockfd, tree);

      close(sockfd);
      exit(0);
    }

    close(sockfd);
  }

  return 0;
}

////////////////////////////////////////////
// parser
void error(int fd, int errCode, char *msg)
{
  int c;

  while (read(fd, &c, 1) != -1)
    ;
  close(fd);

  fprintf(stderr, "%d\n", errCode);
  fprintf(stderr, "%s\n", msg);
  exit(0);
  return;
}
