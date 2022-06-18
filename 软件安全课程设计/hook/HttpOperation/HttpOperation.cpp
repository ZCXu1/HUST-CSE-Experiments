#include <winsock2.h>
#include <windows.h>
#include <iostream>
#pragma comment(lib,"ws2_32.lib")
using namespace std;
int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "WSAStartup failed.\n";
        getchar();
        return 1;
    }
    SOCKET Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct hostent* host;
    host = gethostbyname("www.baidu.com");
    SOCKADDR_IN SockAddr;
    SockAddr.sin_port = htons(80);
    SockAddr.sin_family = AF_INET;
    SockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);
    cout << "Connecting...\n";
    if (connect(Socket, (SOCKADDR*)(&SockAddr), sizeof(SockAddr)) != 0) {
        cout << "Could not connect";
        getchar();
        return 1;
    }
    cout << "Connected.\n";
    send(Socket, "GET / HTTP/1.1\r\nHost: www.baidu.com\r\nConnection: close\r\n\r\n", strlen("GET / HTTP/1.1\r\nHost: www.baidu.com\r\nConnection: close\r\n\r\n"), 0);
    char buffer[10000];
    int nDataLength;
    while ((nDataLength = recv(Socket, buffer, 10000, 0)) > 0) {
        int i = 0;
        while (buffer[i] >= 32 || buffer[i] == '\n' || buffer[i] == '\r') {
            cout << buffer[i];
            i += 1;
        }
    }
    closesocket(Socket);
    WSACleanup();
    getchar();
    return 0;
}