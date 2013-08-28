#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/queue.h>
#include <setjmp.h>
#include <stdarg.h>
#include <errno.h>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>

#include "http_server.h"
#include "dpdk.h"

#include "lwip/opt.h"
#include "lwip/init.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/sys.h"
#include "lwip/stats.h"
#include "lwip/tcp_impl.h"
#include "lwip/inet_chksum.h"
#include "lwip/tcpip.h"
#include "lwip/sockets.h"
#include "netif/unixif.h"
#include "netif/tcpdump.h"
#include "lwip/ip_addr.h"
#include "arch/perf.h"
#include "httpd.h"
#include "lwip/icmp.h"
#include "lwip/raw.h"

/* THE FOLLOWING VALUES CAN BE SET */
/*--------------------------------------------------------*/
#define RX_PTHRESH 8
#define RX_HTHRESH 8
#define RX_WTHRESH 4
#define RTE_TEXT_RX_DESC_DEFAULT 128

#define TX_PTHRESH 36
#define TX_HTHRESH 0
#define TX_WTHRESH 0
#define RTE_TEST_TX_DESC_DEFAULT 512

#define NUM_PORTS 1
#define NUM_TX_QUEUES_PER_LCORE NUM_PORTS
#define NUM_RX_QUEUES_PER_LCORE 1
#define NB_SOCKETS 2 /*IF NUMA TURNED OFF, CHANGE TO 1*/

#define NETMASK 255.255.255.0
#define GATEWAY 198.162.52.254
#define IP_ADDR 198.162.52.211

/*-------------------------------------------------------*/

#define NB_MBUF 8192
#define MBUF_SIZE (2048 + sizeof(struct rte_mbuf) + RTE_PKTMBUF_HEADROOM)

unsigned char ping_flag = 1;
ip_addr_t ipaddr, netmask, gw;
 

int main(int argc, char** argv) {
	int ret = 0;
	argc -= ret;
	argv += ret;	
	printf("Hello\n");
	return 0;
	
}	

