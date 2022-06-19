#include <arpa/inet.h>
#include <fcntl.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <netdb.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <pthread.h>
#include <string.h>
#include <sys/ioctl.h>

#define CHK_SSL(err)                     \
    do {                                 \
        if ((err) < 1) {                 \
            ERR_print_errors_fp(stderr); \
            exit(2);                     \
        }                                \
    } while (0)
#define CA_DIR "./ca_client"

int verify_callback(int preverify_ok, X509_STORE_CTX *x509_ctx) {
    char buf[300];

    X509 *cert = X509_STORE_CTX_get_current_cert(x509_ctx);
    X509_NAME_oneline(X509_get_subject_name(cert), buf, 300);
    printf("subject = %s\n", buf);

    if (preverify_ok == 1) {
        printf("[INFO] Verification passed.\n");
    } else {
        int err = X509_STORE_CTX_get_error(x509_ctx);
        printf("[ERRO] Verification failed: %s.\n", X509_verify_cert_error_string(err));
    }
}

SSL *setupTLSClient(const char *hostname) {
    // Step 0: OpenSSL library initialization
    // This step is no longer needed as of version 1.1.0.
    SSL_library_init();
    SSL_load_error_strings();
    SSLeay_add_ssl_algorithms();

    SSL_METHOD *meth;
    SSL_CTX *ctx;
    SSL *ssl;

    meth = (SSL_METHOD *)TLSv1_2_method();
    ctx = SSL_CTX_new(meth);

    /* 客户端模式：服务器证书已验证。
     * 如果验证过程失败，则TLS/SSL握手会立即终止，并显示一条警告消息，其中包含验证失败的原因。
     * 如果未发送服务器证书，则由于使用了匿名密码，因此将忽略SSL_VERIFY_PEER。
     */
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, verify_callback);
    if (SSL_CTX_load_verify_locations(ctx, NULL, CA_DIR) < 1) {
        printf("[ERRO] Error setting the verify locations. \n");
        exit(0);
    }
    ssl = SSL_new(ctx);

    /* https://www.openssl.org/docs/man1.0.2/man3/SSL_get0_param.html
     * 检查主机名称是否匹配
     */
    X509_VERIFY_PARAM *vpm = SSL_get0_param(ssl);
    X509_VERIFY_PARAM_set1_host(vpm, hostname, 0);

    return ssl;
}

int setupTCPClient(const char *hostname, int port) {
    struct sockaddr_in server_addr;

    // Get the IP address from hostname
    struct hostent *hp = gethostbyname(hostname);

    // Create a TCP socket
    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // Fill in the destination information (IP, port #, and family)
    memset(&server_addr, '\0', sizeof(server_addr));
    memcpy(&(server_addr.sin_addr.s_addr), hp->h_addr, hp->h_length);
    server_addr.sin_port = htons(port);
    server_addr.sin_family = AF_INET;

    // Connect to the destination
    connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    return sockfd;
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

typedef struct thread_data {
    int tunfd;
    SSL *ssl;
} THDATA, *PTHDATA;

char *last_ip;

void *listen_tun(void *threadData) {
    PTHDATA ptd = (PTHDATA)threadData;
    while (1) {
        int len;
        char buff[2000];

        bzero(buff, 2000);
        len = read(ptd->tunfd, buff, 2000);
        if (len > 19 && buff[0] == 0x45) {
            if ((int)buff[15] == atoi(last_ip)) {
                printf("Receive TUN: %d\n", len);
                SSL_write(ptd->ssl, buff, len);
            } else {
                printf("[ERRO] Incorrect IP: 192.168.53.%s", last_ip);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    char *hostname = "lyg.com";
    int port = 4433;

    if (argc < 6) {
        printf("[ERRO] Missing arguments.\n");
        exit(1);
    }

    hostname = argv[1];
    port = atoi(argv[2]);
    last_ip = argv[5];

    /*------ TLS initialization ------*/
    SSL *ssl = setupTLSClient(hostname);
    printf("[INFO] Set up TLS client successfully!\n");

    /*------ TCP connection ------*/
    int sockfd = setupTCPClient(hostname, port);
    printf("[INFO] Set up TCP client successfully!\n");

    /*------ TLS handshake ------*/
    SSL_set_fd(ssl, sockfd);
    int err = SSL_connect(ssl);
    CHK_SSL(err);
    printf("[INFO] SSL connection is successful\n");
    printf("[INFO] SSL connection using %s\n", SSL_get_cipher(ssl));

    /*------ Authenticating Client by user/passwd ------*/
    SSL_write(ssl, argv[3], strlen(argv[3]));  // username
    SSL_write(ssl, argv[4], strlen(argv[4]));  // password
    SSL_write(ssl, argv[5], strlen(argv[5]));  // local last IP

    /*------ Send/Receive data ------*/
    int tunfd = createTunDevice();
    pthread_t listen_tun_thread;
    THDATA threadData;
    threadData.tunfd = tunfd;
    threadData.ssl = ssl;
    pthread_create(&listen_tun_thread, NULL, listen_tun, (void *)&threadData);

    // redirect and routing
    char cmd[100];
    sprintf(cmd, "sudo ifconfig tun0 192.168.53.%s/24 up && sudo route add -net 192.168.51.0/24 tun0",
            argv[5]);
    system(cmd);

    int len;
    do {
        char buf[9000];
        len = SSL_read(ssl, buf, sizeof(buf) - 1);
        write(tunfd, buf, len);
        printf("Receive SSL: %d\n", len);
    } while (len > 0);
    pthread_cancel(listen_tun_thread);
    printf("[INFO] Close connection.\n");
    return 0;
}