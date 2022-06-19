#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdio.h>
int int main(int argc, char const *argv[])
{
	int nRet1 = syscall(548,20,10);
	printf("%d\n",nRet1);
	int nRet2 = syscall(549,20,18,4);
	printf("%d\n",nRet2);
	return 0;
}