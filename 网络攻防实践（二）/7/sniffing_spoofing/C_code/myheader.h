
/* ethernet headers are always exactly 14 bytes [1] */
#define SIZE_ETHERNET 14

/* Ethernet addresses are 6 bytes */
#define ETHER_ADDR_LEN  6

#define PACKET_LEN   1500


/* Ethernet header */
struct ethheader {
    u_char  ether_dhost[ETHER_ADDR_LEN];    /* destination host address */
    u_char  ether_shost[ETHER_ADDR_LEN];    /* source host address */
    u_short ether_type;                     /* IP? ARP? RARP? etc */
};


/* IP Header */
struct ipheader {
	unsigned char      iph_ihl:4, iph_ver:4; //IP Header length & Version.
	unsigned char      iph_tos; //Type of service
	unsigned short int iph_len; //IP Packet length (Both data and header)
	unsigned short int iph_ident; //Identification
	unsigned short int iph_flag:3, iph_offset:13; //Flags and Fragmentation offset
	unsigned char      iph_ttl; //Time to Live
	unsigned char      iph_protocol; //Type of the upper-level protocol
	unsigned short int iph_chksum; //IP datagram checksum
	struct  in_addr    iph_sourceip; //IP Source address (In network byte order)
	struct  in_addr    iph_destip;//IP Destination address (In network byte order)
};

/* ICMP Header */
struct icmpheader {
	unsigned char icmp_type; //ICMP message type
	unsigned char icmp_code; //Error code
	unsigned short int icmp_chksum; //Checksum for ICMP Header and data
	unsigned short int icmp_id; //Used in echo request/reply to identify request
 	unsigned short int icmp_seq;//Identifies the sequence of echo messages, 
				    //if more than one is sent.
};


/* TCP Header */
struct tcpheader {
    u_short tcp_sport;               /* source port */
    u_short tcp_dport;               /* destination port */
    u_int   tcp_seq;                 /* sequence number */
    u_int   tcp_ack;                 /* acknowledgement number */
    u_char  tcp_offx2;               /* data offset, rsvd */
#define TH_OFF(th)      (((th)->tcp_offx2 & 0xf0) >> 4)
    u_char  tcp_flags;
#define TH_FIN  0x01
#define TH_SYN  0x02
#define TH_RST  0x04
#define TH_PUSH 0x08
#define TH_ACK  0x10
#define TH_URG  0x20
#define TH_ECE  0x40
#define TH_CWR  0x80
#define TH_FLAGS        (TH_FIN|TH_SYN|TH_RST|TH_ACK|TH_URG|TH_ECE|TH_CWR)
    u_short tcp_win;                 /* window */
    u_short tcp_sum;                 /* checksum */
    u_short tcp_urp;                 /* urgent pointer */
};


/* UDP Header */
struct udpheader
{
  u_int16_t udp_sport;           /* source port */
  u_int16_t udp_dport;           /* destination port */
  u_int16_t udp_ulen;            /* udp length */
  u_int16_t udp_sum;             /* udp checksum */
};

struct pseudo_tcp
{
        unsigned saddr, daddr;
        unsigned char mbz;
        unsigned char ptcl;
        unsigned short tcpl;
        struct tcpheader tcp;
        char payload[PACKET_LEN];
};

// DNS layer header's structure
struct dnsheader {
	unsigned short int query_id;
	unsigned short int flags;
	unsigned short int QDCOUNT;
	unsigned short int ANCOUNT;
	unsigned short int NSCOUNT;
	unsigned short int ARCOUNT;
};

