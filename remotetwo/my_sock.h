#include        <arpa/inet.h>   /* inet(3) functions */
#include        <asm/errno.h>
#include    <asm/ioctl.h>
#include        <errno.h>
#include        <fcntl.h>               /* for nonblocking */
#include        <in_cksum.h>       
#include    <linux/if.h>
#include    <linux/filter.h>    //for socket filters
#include    <linux/socket.h>
#include    <linux/sockios.h>
#include    <net/ethernet.h>       //ether_addr header
#include        <netinet/in_systm.h>
#include        <netinet/ip.h>
#include        <netinet/ip_icmp.h>
#include        <netinet/tcp.h> 
#include        <netinet/udp.h> 
#include        <netinet/in.h>  /* sockaddr_in{} and other Internet defns */
#include        <netdb.h>
#include        <sys/types.h>   /* basic system data types */
#include        <sys/socket.h>  /* basic socket definitions */
#include        <sys/time.h>    /* timeval{} for select() */
#include        <sys/stat.h>    /* for S_xxx file mode constants */
#include        <sys/uio.h>             /* for iovec{} and readv/writev */
#include        <sys/wait.h>
#include        <sys/un.h> 
#include        <signal.h>
#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include        <unistd.h>

#if __GLIBC__ >= 2 && __GLIBC_MINOR >= 1
       #include <netpacket/packet.h> 
       #include <net/ethernet.h>     /* the L2 protocols */
       #else
       #include <asm/types.h>
       #include <linux/if_packet.h>
       #include <linux/if_ether.h>   /* The L2 protocols */
#endif

//  tcp flags       need to use BSD style tcp header for these #define __FAVOR_BSD to enable it b4 linking 2 library
#define     TCP_FIN     0x01
#define     TCP_SYN     0x02
#define     TCP_RST     0x04
#define     TCP_PUSH    0x08
#define     TCP_ACK     0x10
#define     TCP_URG     0x20

//  internet headers
#define         IP_HDRLEN       20              //duhhh
#define         ICMP_HDRLEN     8
#define         TCP_HDRLEN      20
#define     UDP_HDRLEN  8
#define     ETH_HDRLEN  14

//  useful defines
#define     ETH_BUFSIZE 1500        //max size of ethernet frame w/o header
#define         ADDR_LEN        16              //max len of ascii ip address + NULL terminator

//  LSF OFFSETS these are also useful for otther stuff  as they contain the exact byte offsets off
//          almost all of the header fields. useful to get a value quickly 
#define     LSF_IP_V    0x0 //for consistency   ../ mask 0xf0  }  to get these 4 bit values we use
#define     LSF_IP_HL   0x0 //..            ../ mask 0x0f  }  these masks with bitwise & 
#define     LSF_IP_TOS  0x1
#define     LSF_IP_TL   0x2
#define     LSF_IP_ID   0x4
#define     LSF_IP_FL   0x6
#define     LSF_IP_TTL  0x8
#define     LSF_IP_P    0x9
#define     LSF_IP_CS   0xa
#define     LSF_IP_SIP  0xc
#define     LSF_IP_DIP  0x10

#define     LSF_UDP_SP  0x0
#define     LSF_UDP_DP  0x2
#define     LSF_UDP_L   0x4
#define     LSF_UDP_CS  0x6

#define     LSF_TCP_SP  0x0
#define     LSF_TCP_DP  0x2
#define     LSF_TCP_SQ  0x4
#define     LSF_TCP_AK  0x8
#define     LSF_TCP_HL  0xc
#define     LSF_TCP_FL  0xd
#define     LSF_TCP_WN  0xe
#define     LSF_TCP_CS  0x10
#define     LSF_TCP_UR  0x12

#define     LSF_E_P     0xc

#define     PS_LEN      0xc
//each of these is bitwise OR'ed to match TCP flags
#define FIN_M           0x01    //...notice the pattern? i hope so! since each flag represents a single bit
#define SYN_M           0x02    //we test for the exact number that indicates that power of 2
#define RST_M           0x04
#define PSH_M           0x08
#define ACK_M           0x10
#define URG_M           0x20

//useful port numbers in hex
#define TLNT            0x17            //23            port numbers in hex
#define FTP             0x15            //21
#define SMTP            0x19            //25
#define POP             0x6e            //110
#define TFTP            0x45            //69
#define HTTP        0x50        //80

#define LEN(x) (sizeof(x)/sizeof(struct sock_filter))   //macro for getting code length

//  useful structures       //need to rename pseudohdr to tcp_ps b/c we have a udp one as well to make
struct pseudohdr        //tcp pseudo header used in calculating checksum
{   
    struct in_addr saddr;   //ip source address     04 bytes
        struct in_addr daddr;   //ip destination address    04 bytes
    u_char pad;     //padding           01 bytes
    u_char protocol;    //protocol          01 bytes
    u_short length;     //tcp total length      02 bytes    --->> sum of this 12 bytes
    struct tcphdr tcpheader;    //tcpheader     20 bytes
};

//ripped right from da kernel
/*
 *  *      Display an IP address in readable format.
 *   */

#define NIPQUAD(addr) \
        ((unsigned char *)&addr)[0], \
        ((unsigned char *)&addr)[1], \
        ((unsigned char *)&addr)[2], \
        ((unsigned char *)&addr)[3]

#define HIPQUAD(addr) \
        ((unsigned char *)&addr)[3], \
        ((unsigned char *)&addr)[2], \
        ((unsigned char *)&addr)[1], \
        ((unsigned char *)&addr)[0]
