#include <sys/mman.h>
#include <err.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

int main(void) {
  void *map = mmap((void*)0x10000, 0x1000, PROT_READ|PROT_WRITE,
                   MAP_PRIVATE|MAP_ANONYMOUS|MAP_GROWSDOWN|MAP_FIXED, -1, 0);
  if (map == MAP_FAILED) err(1, "mmap");
  int fd = open("/proc/self/mem", O_RDWR);
  if (fd == -1) err(1, "open");
  unsigned long addr = (unsigned long)map;
  while (addr != 0) {
    addr -= 0x1000;
    if (lseek(fd, addr, SEEK_SET) == -1) err(1, "lseek");
    char cmd[1000];
    sprintf(cmd, "LD_DEBUG=help su 1>&%d", fd);
    system(cmd);
  }
  system("head -n1 /proc/$PPID/maps");
  printf("data at NULL: 0x%lx\n", *(unsigned long *)0);
}