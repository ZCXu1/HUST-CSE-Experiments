#pragma warning(disable:4996)
#include <io.h>
#include <stdio.h>
#include <string.h>

#define  MAXPATH 20
#define  MAXFILENUM 1000
char     filename[MAXFILENUM][MAXPATH];


void SearchDirFile(char* path, int& MaxFileNum)//搜索当前目录的所有文件
{
    struct _finddata_t data;
    long hnd = _findfirst(path, &data);
    if (hnd < 0)
    {
        perror(path);
    }
    int  nRet = (hnd < 0) ? -1 : 1;
    int  Index = 0;
    while (nRet >= 0)
    {
        if (data.attrib == _A_ARCH) {
            //  printf("   %s\n", data.name );
            int i = 0;
            while (data.name[i] != '\0')
            {
                filename[Index][i] = data.name[i];
                i++;
            }
            Index++;
        }
        nRet = _findnext(hnd, &data);
    }
    MaxFileNum = Index;
    _findclose(hnd);
}
int is_exe(FILE* fd) {//判断是否是可执行文件
    char dos_mark[3];
    long address_of_nt_header;
    long offset = 0x3c;
    char nt_mark[3];
    if (fd == NULL)
        return 0;
    fgets(dos_mark, 3, fd);
    if (strcmp(dos_mark, "MZ"))
        return 0;
    fseek(fd, offset, SEEK_SET);
    fread(&address_of_nt_header, 4, 1, fd);
    fseek(fd, address_of_nt_header, SEEK_SET);
    fgets(nt_mark, 3, fd);
    if (strcmp(nt_mark, "PE"))
        return 0;
    return 1;
}
