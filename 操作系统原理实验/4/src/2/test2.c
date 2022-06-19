#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#define CHAR_DEV_NAME "/dev/xzcDrive"

char calc[32];
char res[32];
int main()
{
	int ret;
	int fd;
	extern int errno;
	fd = open(CHAR_DEV_NAME, O_RDWR | O_NDELAY);
	if(fd < 0)
	{
		printf("open failed! error %d\n", errno);
		return -1;
	}
	scanf("%s", calc);
	write(fd,calc,32);
	read(fd, res, 32);
	printf("%s\n", res);
	close(fd);
	return 0;
}