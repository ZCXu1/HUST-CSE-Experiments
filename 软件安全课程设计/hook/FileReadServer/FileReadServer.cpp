#include <stdio.h>  
#include <winsock2.h>  
#include <iostream>
using namespace std;

#pragma comment(lib,"ws2_32.lib")  
char s[1024] = "\0";

void readFile();
int main(int argc, char* argv[])
{
    //初始化WSA  
    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA wsaData;
    if (WSAStartup(sockVersion, &wsaData) != 0)
    {
        return 0;
    }

    //创建套接字  
    SOCKET slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (slisten == INVALID_SOCKET)
    {
        printf("socket error !");
        return 0;
    }

    //绑定IP和端口  
    sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(1234);
    sin.sin_addr.S_un.S_addr = INADDR_ANY;
    if (bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
    {
        printf("bind error !");
    }

    //开始监听  
    if (listen(slisten, 5) == SOCKET_ERROR)
    {
        printf("listen error !");
        return 0;
    }

    //循环接收数据  
    SOCKET sClient;
    sockaddr_in remoteAddr;
    int nAddrlen = sizeof(remoteAddr);
    char revData[255];
    STARTUPINFOA startupInfo;
    PROCESS_INFORMATION  processInfo;
    ZeroMemory(&startupInfo, sizeof(startupInfo));
    ZeroMemory(&processInfo, sizeof(processInfo));
    CreateProcessA("D:\\hook\\InjectDll\\Debug\\FileReadClient.exe", NULL, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &startupInfo, &processInfo);
    int i = 0;
    while (i < 50)
    {
        printf("Waiting for connection...\n\n");
        sClient = accept(slisten, (SOCKADDR*)&remoteAddr, &nAddrlen);
        if (sClient == INVALID_SOCKET)
        {
            printf("accept error !");
            continue;
        }
        printf("Received a connection：%s \r\n\n", inet_ntoa(remoteAddr.sin_addr));

        //接收数据  
        int ret = recv(sClient, revData, 255, 0);
        if (ret > 0)
        {
            revData[ret] = 0x00;
            printf(revData);
            printf("\n");
        }

        readFile();

        //发送数据  
        const char* sendData = s;
        send(sClient, sendData, strlen(sendData), 0);
        i++;
        closesocket(sClient);
      
    }

    closesocket(slisten);
    WSACleanup();
    return 0;
}

void readFile() {
    bool flag;
    HANDLE h = (HANDLE)CreateFile(L"D:\\hook\\InjectDll\\Debug\\in.txt", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, NULL, NULL);
    if (h == INVALID_HANDLE_VALUE) {
        h = NULL;
        return;
    }


    flag = ReadFile(h, s, 1024, NULL, NULL);
    if (flag) {
        cout << "成功读入文件 内容如下" << endl;
        cout << s << endl;
    }
    FlushFileBuffers(h);

}
