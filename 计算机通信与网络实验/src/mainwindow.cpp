// =================================================
// @Created on 2021 Nov 3
// @Version 1.0.0
// @Description A TFTP Client
// =================================================
#include "mainwindow.h"
#include "ui_mainwindow.h"
#define MAX_DATA_SIZE 512
// ip of server and client
sockaddr_in serverAddress, clientAddress;
// socket of client
SOCKET sock;
// the length of ip
unsigned int lenOfAddress;
unsigned long Opt = 1;
double bytesOfTrans, timeCost;
FILE* logPointer;
char logBuf[MAX_DATA_SIZE];
time_t initTime;
tm* info;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initUI();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initUI(){
    WSADATA wsaData;
    lenOfAddress = sizeof(struct sockaddr_in);
    // initialize winsock
    int nRC = WSAStartup(0x0101, &wsaData);
    if (nRC)
    {
        ui->output->append("Initialization error!");
    }
    if (wsaData.wVersion != 0x0101)
    {
        ui->output->append("Client's winsock version is wrong!");
        WSACleanup();
    }
}
void MainWindow::on_PathChoose_pressed(){
    QDir dir;
    // chooseMode file path
    QString PathName = QFileDialog::getOpenFileName(this,tr(""),"",tr("file(*)"));
    // show filename on gui
    ui->PathShow->setText(PathName);
}
void MainWindow::on_uploadButton_pressed(){
    ui->output->clear();
    QByteArray Qfilename = ui->PathShow->text().toLatin1();
    QByteArray QserverIP = ui->uploadServerIP->text().toLatin1();
    QByteArray QclientIP = ui->uploadLocalIP->text().toLatin1();
    char * filePath = Qfilename.data();
    char * serverIP = QserverIP.data();
    char * clientIP = QclientIP.data();
    char buf[MAX_DATA_SIZE], filename[MAX_DATA_SIZE];
    int temp = 0;
    for(int i = 0; filePath[i]!='\0'; i++, temp++){
        if(filePath[i] == '/'){
            i++;
            temp = 0;
            filename[temp] = filePath[i];
        }
        else{
            filename[temp] = filePath[i];
        }
    }
    filename[temp] = '\0';
    logPointer = fopen("tftp.log", "a");
    if (logPointer == NULL) {
        ui->output->append("Failed to open log file!");
    }
    // set server address family(ipv4) port ip
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(69);
    serverAddress.sin_addr.S_un.S_addr = inet_addr(serverIP);
    // set client address family(ipv4) port ip
    clientAddress.sin_family = AF_INET;
    clientAddress.sin_port = htons(0);
    clientAddress.sin_addr.S_un.S_addr = inet_addr(clientIP);
    // create client socket
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    ioctlsocket(sock, FIONBIO, &Opt);
    if (sock == INVALID_SOCKET)
    {
        // failed to create
        sprintf(buf, "Client socket creation error!");
        ui->output->append(buf);
        fclose(logPointer);
        return;
    }
    sprintf(buf, "Client socket create successfully!");
    ui->output->append(buf);
    // socket bind
    bind(sock,(LPSOCKADDR)&clientAddress, sizeof(clientAddress));

    // record time
    clock_t start, end;
    // the byte of transmission
    bytesOfTrans = 0;
    // to save the ip and port that server sends
    sockaddr_in sender;
    // time to wait and size of recieved packet
    int time_wait_ack, receiveSize, chooseMode, resent = 0;

    tftpData sendPacket, recievePacket;
    // Send WRQ
    sendPacket.code = htons(CMD_WRQ);
    // written filename and transport pattern
    chooseMode = ui->uploadMode->currentIndex();
    sprintf(buf, "chooseMode=%d", chooseMode);
    ui->output->append(buf);
    if(chooseMode == 0)
        sprintf(sendPacket.filename, "%s%c%s%c", filename, 0, "netascii", 0);
    else
        sprintf(sendPacket.filename, "%s%c%s%c", filename, 0, "octet", 0);
    // send request packet
    sendto(sock, (char*)&sendPacket, sizeof(tftpData), 0, (struct sockaddr*) & serverAddress, lenOfAddress);
    // wait for ACK && at most 3s && flash every 20ms
    for (time_wait_ack = 0; time_wait_ack < PKT_RCV_TIMEOUT; time_wait_ack += 20) {

        // try to recieve
        receiveSize = recvfrom(sock, (char*)&recievePacket,
                               sizeof(tftpData), 0, (struct sockaddr*) & sender, (int*)&lenOfAddress);
        if (receiveSize > 0 && receiveSize < 4) {
            // find exception
            sprintf(buf, "Bad packet: receiveSize=%d", receiveSize);
            ui->output->append(buf);
        }
        if (receiveSize >= 4 && recievePacket.code == htons(CMD_ACK) && recievePacket.block == htons(0)) {
            // get ACK
            break;
        }
        Sleep(20);
    }
    // WRQ TIME OUT
    if (time_wait_ack >= PKT_RCV_TIMEOUT) {
        // time out
        sprintf(buf, "Could not receive from server.");
        ui->output->append(buf);
        time(&initTime);
        info = localtime(&initTime);
        sprintf(logBuf, "%s ERROR: upload %s, mode: %s, %s\n",
            asctime(info), filename, chooseMode == 0 ? ("netascii") : ("octet"),
            "Could not receive from server.");
        for (int i = 0; i < MAX_DATA_SIZE; i++) {
            if (logBuf[i] == '\n') {
                logBuf[i] = ' ';
                break;
            }
        }
        fwrite(logBuf, 1, strlen(logBuf), logPointer);
        fclose(logPointer);
        //WRQ ERROR END PROGRAM
        return;
    }
    // open file
    FILE* fp = NULL;
    if(chooseMode == 0)
        fp = fopen(filePath, "r");
    else
        fp = fopen(filePath, "rb");
    // FILE NOT EXIST
    if (fp == NULL) {
        sprintf(buf, "File does not exist!");
        ui->output->append(buf);
        // write to log file
        // get initTime
        time(&initTime);
        // transport it to local time
        info = localtime(&initTime);
        sprintf(logBuf, "%s ERROR: upload %s, mode: %s, %s\n",
            asctime(info), filename, chooseMode == 0 ? ("netascii") : ("octet"),
            "File not exists!");
        for (int i = 0; i < MAX_DATA_SIZE; i++) {
            if (logBuf[i] == '\n') {
                logBuf[i] = ' ';
                break;
            }
        }
        fwrite(logBuf, 1, strlen(logBuf), logPointer);
        fclose(logPointer);

        return;
    }
    int sendSize = 0;
    int rxmt;
    unsigned short block = 1;
    // send DATA
    sendPacket.code= htons(CMD_DATA);

    start = clock();
    do {
        // clear data
        memset(sendPacket.data, 0, sizeof(sendPacket.data));
        // write block number
        sendPacket.block = htons(block);
        // read data
        sendSize = fread(sendPacket.data, 1, DATA_SIZE, fp);
        bytesOfTrans += sendSize;
        // reupload at most 3 times
        for (rxmt = 0; rxmt < PKT_MAX_RXMT; rxmt++) {
            // send data packet
            sendto(sock, (char*)&sendPacket, sendSize + 4, 0, (struct sockaddr*) & sender, lenOfAddress);
            sprintf(buf, "Send the %d block", block);
            ui->output->append(buf);
            // wait for ACK && at most 3s
            for (time_wait_ack = 0; time_wait_ack < PKT_RCV_TIMEOUT; time_wait_ack += 20) {
                receiveSize = recvfrom(sock, (char*)&recievePacket,
                                       sizeof(tftpData), 0, (struct sockaddr*) & sender, (int*)&lenOfAddress);
                if (receiveSize > 0 && receiveSize < 4) {
                    sprintf(buf, "Bad packet: receiveSize=%d", receiveSize);
                    ui->output->append(buf);
                }
                if (receiveSize >= 4 && recievePacket.code == htons(CMD_ACK)
                        && recievePacket.block == htons(block)) {
                    break;
                }
                Sleep(20);
            }
            if (time_wait_ack < PKT_RCV_TIMEOUT) {
                // send successfully
                break;
            }
            else {
                // get no ACK
                // write to log file
                time(&initTime);
                // transport it to local time
                info = localtime(&initTime);
                sprintf(logBuf, "%s WARN: upload %s, mode: %s, %s\n",
                    asctime(info), filename, chooseMode == 0 ? ("netascii") : ("octet"),
                    "Can't receive ACK, resent");
                for (int i = 0; i < MAX_DATA_SIZE; i++) {
                    if (logBuf[i] == '\n') {
                        logBuf[i] = ' ';
                        break;
                    }
                }
                fwrite(logBuf, 1, strlen(logBuf), logPointer);
                // resent
                resent++;
                continue;
            }
        }
        if (rxmt >= PKT_MAX_RXMT) {
            // failed to reupload 3 times
            sprintf(buf, "Could not receive from server.");
            ui->output->append(buf);
            fclose(fp);
            // write to log file
            // get initTime
            time(&initTime);
            // transport it to local time
            info = localtime(&initTime);
            sprintf(logBuf, "%s ERROR: upload %s, mode: %s, Wait for ACK timeout\n",
                asctime(info), filename, chooseMode == 0 ? ("netascii") : ("octet"));
            for (int i = 0; i < MAX_DATA_SIZE; i++) {
                if (logBuf[i] == '\n') {
                    logBuf[i] = ' ';
                    break;
                }
            }
            fwrite(logBuf, 1, strlen(logBuf), logPointer);
            fclose(logPointer);
            return;
        }
        // transport next data block
        block++;
        // if the data block is not full then it's the last and end loop
    } while (sendSize == DATA_SIZE);
    end = clock();
    sprintf(buf, "Send file successfully.");
    ui->output->append(buf);

    fclose(fp);
    timeCost = ((double)(end - start)) / CLK_TCK;
    sprintf(buf, "upload file size: %.1f kB time cost: %.2f s", bytesOfTrans/ 1024, timeCost);
    ui->output->append(buf);
    sprintf(buf, "upload speed: %.1f kB/s", bytesOfTrans/(1024 * timeCost));
    ui->output->append(buf);
    sprintf(buf, "resent packet:%d, packet loss probability:%.2f%%", resent, 100 * ((double)resent / (resent + block - 1)));
    ui->output->append(buf);

    // get initTime
    time(&initTime);
    // transport it to local time
    info = localtime(&initTime);
    sprintf(logBuf, "%s INFO: upload %s, mode: %s, size: %.1f kB, time cost: %.2f s\n",
        asctime(info), filename, chooseMode == 0 ? ("netascii") : ("octet"), bytesOfTrans / 1024, timeCost);
    for (int i = 0; i < MAX_DATA_SIZE; i++) {
        if (logBuf[i] == '\n') {
            logBuf[i] = ' ';
            break;
        }
    }
    fwrite(logBuf, 1, strlen(logBuf), logPointer);
    fclose(logPointer);
}
void MainWindow::on_downloadButton_pressed(){
    ui->output->clear();
    QByteArray QServerFile = ui->downloadServerFilename->text().toLatin1();
    QByteArray QlocalFile = ui->downloadLocalFilename->text().toLatin1();
    QByteArray QserverIP = ui->downloadServerIP->text().toLatin1();
    QByteArray QclientIP = ui->downloadLocalIP->text().toLatin1();
    char * localFile = QlocalFile.data();
    char * remoteFile = QServerFile.data();
    char * serverIP = QserverIP.data();
    char * clientIP = QclientIP.data();
    char buf[MAX_DATA_SIZE];
    ui->output->append(remoteFile);
    ui->output->append(localFile);

    logPointer = fopen("tftp.log", "a");
    if (logPointer == NULL) {
        ui->output->append("Failed to open log file!");
    }


    // set server address family(ipv4) port ip
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(69);
    serverAddress.sin_addr.S_un.S_addr = inet_addr(serverIP);
    // set client address family(ipv4) port ip
    clientAddress.sin_family = AF_INET;
    clientAddress.sin_port = htons(0);
    clientAddress.sin_addr.S_un.S_addr = inet_addr(clientIP);
    // create client socket
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    ioctlsocket(sock, FIONBIO, &Opt);
    if (sock == INVALID_SOCKET)
    {
        // failed to create
        sprintf(buf, "Client socket creation error!");
        ui->output->append(buf);
        fclose(logPointer);
        return;
    }
    sprintf(buf, "Client socket create successfully!");
    ui->output->append(buf);
    // socket bind
    bind(sock,(LPSOCKADDR)&clientAddress, sizeof(clientAddress));

    // record time
    clock_t start, end;
    // the bytes of transmission
    bytesOfTrans = 0;
    // to save the ip and port that server sends
    sockaddr_in sender;
    // time to wait and size of recieved packet
    int receiveSize = 0, chooseMode = 0, resent = 0;

    tftpData sendPacket, recievePacket;


    int timeWaitingForData;
    // initial block is 1
    unsigned short block = 1;

    // send RRQ
    sendPacket.code = htons(CMD_RRQ);
    // written filename and transport pattern
    chooseMode = ui->downloadMode->currentIndex();
    if(chooseMode == 0)
        sprintf(sendPacket.filename, "%s%c%s%c", remoteFile, 0, "netascii", 0);
    else
        sprintf(sendPacket.filename, "%s%c%s%c", remoteFile, 0, "octet", 0);
    // send request data packet
    sendto(sock, (char *)&sendPacket, sizeof(tftpData), 0, (struct sockaddr*) & serverAddress, lenOfAddress);

    // create local file to write
    FILE* fp = NULL;
    if (chooseMode == 0)
        fp = fopen(localFile, "w");
    else
        fp = fopen(localFile, "wb");
    if (fp == NULL) {
        sprintf(buf, "Create file \"%s\" error.", localFile);
        ui->output->append(buf);

        // get initTime
        time(&initTime);
        // transport it to local time
        info = localtime(&initTime);
        sprintf(logBuf, "%s ERROR: download %s as %s, mode: %s, Create file \"%s\" error.\n",
            asctime(info), remoteFile, localFile, chooseMode == 0 ? ("netascii") : ("octet"),
            localFile);
        for (int i = 0; i < MAX_DATA_SIZE; i++) {
            if (logBuf[i] == '\n') {
                logBuf[i] = ' ';
                break;
            }
        }
        fwrite(logBuf, 1, strlen(logBuf), logPointer);
        fclose(logPointer);
        return;
    }
    // get data
    start = clock();
    sendPacket.code = htons(CMD_ACK);
    do {
        // time waiting is for-loop variable
        for (timeWaitingForData = 0; timeWaitingForData < PKT_RCV_TIMEOUT * PKT_MAX_RXMT; timeWaitingForData += 50) {
            // Try receive(Nonblock receive).
            receiveSize = recvfrom(sock, (char *)&recievePacket,
                                   sizeof(tftpData), 0, (struct sockaddr*) & sender,
                                   (int *)&lenOfAddress);
            // RRQ sending error
            if (timeWaitingForData == PKT_RCV_TIMEOUT && block == 1) {
                // can't connect to server
                sprintf(buf, "Could not receive from server.");
                ui->output->append(buf);
                // get initTime
                time(&initTime);
                // transport it to local time
                info = localtime(&initTime);
                sprintf(logBuf, "%s ERROR: download %s as %s, mode: %s, Could not receive from server.\n",
                    asctime(info), remoteFile, localFile, chooseMode == 0 ? ("netascii") : ("octet"));
                for (int i = 0; i < MAX_DATA_SIZE; i++) {
                    if (logBuf[i] == '\n') {
                        logBuf[i] = ' ';
                        break;
                    }
                }
                fwrite(logBuf, 1, strlen(logBuf), logPointer);
                fclose(fp);
                fclose(logPointer);
                // RRQ ERROR END PROGRAM
                return;
            }
            if (receiveSize > 0 && receiveSize < 4) {
                sprintf(buf, "Bad packet: receiveSize=%d", receiveSize);
                ui->output->append(buf);
            }
            // normally recieve:recievePacket.code == htons(CMD_DATA)
            else if (receiveSize >= 4 && recievePacket.code == htons(CMD_DATA) && recievePacket.block == htons(block)) {
                sprintf(buf, "DATA: block=%d, data_size=%d\n", ntohs(recievePacket.block), receiveSize - 4);
                ui->output->append(buf);
                // Send ACK.
                sendPacket.block = recievePacket.block;
                sendto(sock, (char*)&sendPacket, sizeof(tftpData), 0, (struct sockaddr*) & sender, lenOfAddress);
                fwrite(recievePacket.data, 1, receiveSize - 4, fp);
                break;
            }
            else {
                if (timeWaitingForData != 0 && timeWaitingForData % PKT_RCV_TIMEOUT == 0) {
                    // resend ACK
                    sendto(sock, (char*)&sendPacket, sizeof(tftpData), 0, (struct sockaddr*) & sender, lenOfAddress);
                    // get initTime
                    time(&initTime);
                    // transport it to local time
                    info = localtime(&initTime);
                    sprintf(logBuf, "%s WARN: download %s as %s, mode: %s, Can't receive DATA #%d, resent\n",
                        asctime(info), remoteFile, localFile, chooseMode == 0 ? ("netascii") : ("octet"),
                        block);
                    for (int i = 0; i < MAX_DATA_SIZE; i++) {
                        if (logBuf[i] == '\n') {
                            logBuf[i] = ' ';
                            break;
                        }
                    }
                    fwrite(logBuf, 1, strlen(logBuf), logPointer);
                    resent++;
                }
            }
            Sleep(50);
        }
        bytesOfTrans += (receiveSize - 4);
        //time out
        if (timeWaitingForData >= PKT_RCV_TIMEOUT * PKT_MAX_RXMT) {
            sprintf(buf, "Wait for DATA #%d timeout.\n", block);
            ui->output->append(buf);
            fclose(fp);
            // get initTime
            time(&initTime);
            // transport it to local time
            info = localtime(&initTime);
            sprintf(logBuf, "%s ERROR: download %s as %s, mode: %s, Wait for DATA #%d timeout.\n",
                asctime(info), remoteFile, localFile, chooseMode == 0 ? ("netascii") : ("octet"),
                block);
            for (int i = 0; i < MAX_DATA_SIZE; i++) {
                if (logBuf[i] == '\n') {
                    logBuf[i] = ' ';
                    break;
                }
            }
            fwrite(logBuf, 1, strlen(logBuf), logPointer);
            fclose(logPointer);
            return;
        }
        block++;
    } while (receiveSize == DATA_SIZE + 4);
    end = clock();
    timeCost = ((double)(end - start)) / CLK_TCK;
    sprintf(buf, "download file size: %.1f kB time cost: %.2f s", bytesOfTrans/ 1024, timeCost);
    ui->output->append(buf);
    sprintf(buf, "download speed: %.1f kB/s", bytesOfTrans/(1024 * timeCost));
    ui->output->append(buf);
    sprintf(buf, "resent packet:%d, packet loss probability:%.2f%%", resent, 100 * ((double)resent / (resent + block - 1)));
    ui->output->append(buf);
    fclose(fp);
    // get initTime
    time(&initTime);
    // transport it to local time
    info = localtime(&initTime);
    sprintf(logBuf, "%s INFO: download %s as %s, mode: %s, size: %.1f kB, time cost: %.2f s,download speed: %.1f kB/s\n",
        asctime(info), remoteFile, localFile, chooseMode == 0 ? ("netascii") : ("octet"),
            bytesOfTrans / 1024, timeCost,bytesOfTrans/(1024 * timeCost));
    for (int i = 0; i < MAX_DATA_SIZE; i++) {
        if (logBuf[i] == '\n') {
            logBuf[i] = ' ';
            break;
        }
    }
    fwrite(logBuf, 1, strlen(logBuf), logPointer);
    fclose(logPointer);
}
