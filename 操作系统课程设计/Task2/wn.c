// =============================================
//                     wn.c
//                Non-Blocking Write
//                   非阻塞式写
// =============================================
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

int main(char argc, char* argv[])
{
    char val[] = { 
        "X123456789X123456789X123456789X123456789X123456789X123456789X123456789X123456789" 
    };
    // 第二个参数代表非阻塞式
    int d = open("/dev/mydev", O_RDWR|O_NONBLOCK);
    if (d < 0) {
        printf("Failed to open device!\n");
    }
    for (int i = 0; i < 80; i++) {
        int n = write(d, val + i, 1);
        if (n > 0){
            printf("Write char : %c\n", val[i]);
        } else{
            printf("Didn't get char : %c\n", val[i]);
        }
    }

    return 0;
}

