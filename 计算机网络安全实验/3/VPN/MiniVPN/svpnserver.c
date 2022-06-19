#include <arpa/inet.h>
#include <crypt.h>
#include <fcntl.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <netdb.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <pthread.h>
#include <shadow.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define CHK_SSL(err)                     \
    do {                                 \
        if ((err) < 1) {                 \
            ERR_print_errors_fp(stderr); \
            exit(2);                     \
        }                                \
    } while (0)
#define CHK_ERR(err, s) \
    if ((err) == -1) {  \
        perror(s);      \
        exit(1);        \
    }

SSL *setupTLSServer() {
    SSL_METHOD *meth;
    SSL_CTX *ctx;
    SSL *ssl;   

    // Step 0: OpenSSL library initialization
    // This step is no longer needed as of version 1.1.0.
    SSL_library_init();
    SSL_load_error_strings();
    SSLeay_add_ssl_algorithms();

    // Step 1: SSL context initialization
    meth = (SSL_METHOD *)TLSv1_2_method();
    ctx = SSL_CTX_new(meth);
    // 服务器模式：服务器不会向客户端发送客户端证书请求，因此客户端不会发送证书
    SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);
    // Step 2: Set up the server certificate and private key
    SSL_CTX_use_certificate_file(ctx, "./cert_server/server-lyg-crt.pem", SSL_FILETYPE_PEM);
    SSL_CTX_use_PrivateKey_file(ctx, "./cert_server/server-lyg-key.pem", SSL_FILETYPE_PEM);
    // Step 3: Create a new SSL structure for a connection
    ssl = SSL_new(ctx);
    
    return ssl;
}

int setupTCPServer() {
    struct sockaddr_in sa_server;
    int listen_sock;

    listen_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    CHK_ERR(listen_sock, "socket");
    memset(&sa_server, '\0', sizeof(sa_server));
    sa_server.sin_family = AF_INET;
    sa_server.sin_addr.s_addr = INADDR_ANY;
    sa_server.sin_port = htons(4433);
    int err = bind(listen_sock, (struct sockaddr *)&sa_server, sizeof(sa_server));
    CHK_ERR(err, "bind");
    err = listen(listen_sock, 5);
    CHK_ERR(err, "listen");
    return listen_sock;
}

int createTunDevice() {
    int tunfd;
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));

    ifr.ifr_flags = IFF_TUN | IFF_NO_PI;

    tunfd = open("/dev/net/tun", O_RDWR);
    ioctl(tunfd, TUNSETIFF, &ifr);

    return tunfd;
}

typedef struct pipe_data {
    char *pipe_file;
    SSL *ssl;
} PIPEDATA;

void *listen_tun(void *tunfd) {
    int fd = *((int *)tunfd);
    char buff[2000];
    while (1) {
        int len = read(fd, buff, 2000);
        if (len > 19 && buff[0] == 0x45) {
            printf("Receive TUN: len = %d | ip.des = 192.168.53.%d\n", len, (int)buff[19]);
            char pipe_file[10];
            sprintf(pipe_file, "./pipe/%d", (int)buff[19]);
            int fd = open(pipe_file, O_WRONLY);
            if (fd == -1) {
                printf("[WARN] File %s is not exist.\n", pipe_file);
            } else {
                write(fd, buff, len);
            }
        }
    }
}

int login(char *user, char *passwd) {
    struct spwd *pw;
    char *epasswd;
    pw = getspnam(user);
    if (pw == NULL) {
        printf("[ERRO] Password is NULL.\n");
        return 0;
    }

    printf("Login name: %s\n", pw->sp_namp);
    printf("Passwd    : %s\n", pw->sp_pwdp);

    epasswd = crypt(passwd, pw->sp_pwdp);
    if (strcmp(epasswd, pw->sp_pwdp)) {
        printf("[ERRO] Incorrect password.\n");
        return 0;
    }
    return 1;
}

void *listen_pipe(void *threadData) {
    PIPEDATA *ptd = (PIPEDATA*)threadData;
    int pipefd = open(ptd->pipe_file, O_RDONLY);

    char buff[2000];
    int len;
    do {
        len = read(pipefd, buff, 2000);
        SSL_write(ptd->ssl, buff, len);
    } while (len > 0);
    printf("%s read 0 byte. Close connection and remove file.\n", ptd->pipe_file);
    remove(ptd->pipe_file);
}

void sendOut(SSL *ssl, int sock, int tunfd) {
    int len;
    do {
        char buf[1024];
        len = SSL_read(ssl, buf, sizeof(buf) - 1);
        write(tunfd, buf, len);
        buf[len] = '\0';
        printf("Received SSL: %d\n", len);
    } while (len > 0);
    printf("[INFO] SSL shutdown.\n");
}

int main() {
    int err;
    struct sockaddr_in sa_client;
    size_t client_len;
    /*------ TLS Initialization ------*/    
    SSL *ssl = setupTLSServer();
    /*------ TCP Connect ------*/
    int listen_sock = setupTCPServer();
    // tunnal init, redirect and forward
    int tunfd = createTunDevice();
    system("sudo ifconfig tun0 192.168.53.1/24 up && sudo sysctl net.ipv4.ip_forward=1");
    // pipe folder init
    system("rm -rf pipe");
    mkdir("pipe", 0666);
    // creat a listen tunnel
    pthread_t listen_tun_thread;
    pthread_create(&listen_tun_thread, NULL, listen_tun, (void *)&tunfd);

    while (1) {
        int sock = accept(listen_sock, (struct sockaddr *)&sa_client, &client_len);  // block, if try connect
        if (fork() == 0) {                                                           // The child process
            close(listen_sock);

            SSL_set_fd(ssl, sock);
            int err = SSL_accept(ssl);
            CHK_SSL(err);
            printf("[INFO] SSL connection established!\n");

            // login messages
            char user[1024], passwd[1024], last_ip_buff[1024];
            user[SSL_read(ssl, user, sizeof(user) - 1)] = '\0';
            passwd[SSL_read(ssl, passwd, sizeof(passwd) - 1)] = '\0';
            last_ip_buff[SSL_read(ssl, last_ip_buff, sizeof(last_ip_buff) - 1)] = '\0';

            if (login(user, passwd)) {
                printf("[INFO] Login successfully!\n");
                // check IP and create pipe file
                char pipe_file[10];
                sprintf(pipe_file, "./pipe/%s", last_ip_buff);
                if (mkfifo(pipe_file, 0666) == -1) {
                    printf("[WARN] This IP(%s) has been occupied.", last_ip_buff);
                } else {
                    pthread_t listen_pipe_thread;
                    PIPEDATA pipeData;
                    pipeData.pipe_file = pipe_file;
                    pipeData.ssl = ssl;
                    pthread_create(&listen_pipe_thread, NULL, listen_pipe, (void *)&pipeData);
                    sendOut(ssl, sock, tunfd);
                    pthread_cancel(listen_pipe_thread);
                    remove(pipe_file);
                }
            } else {
                printf("[ERRO] Login failed!\n");
            }
            SSL_shutdown(ssl);
            SSL_free(ssl);
            close(sock);
            printf("[INFO] Close sock and return 0.\n");
            return 0;
        } else {  // The parent process
            close(sock);
        }
    }
}
