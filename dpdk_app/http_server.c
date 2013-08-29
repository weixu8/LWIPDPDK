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

#define NETMASK "255.255.255.0"
#define GATEWAY "198.162.52.254"
#define IP_ADDR "198.162.52.211"

/*-------------------------------------------------------*/

#define NB_MBUF 8192
#define MBUF_SIZE (2048 + sizeof(struct rte_mbuf) + RTE_PKTMBUF_HEADROOM)

unsigned char ping_flag = 1;
ip_addr_t ipaddr, netmask, gw;

__attribute__((noreturn)) int network_stack(__attribute__((unused)) void *dummy) {
	while (1);
} 

int main(int argc, char** argv) {
	int ret = 0;
	int lcoreid = 0;
	in_addr inaddr;

	ret = init_dpdk(argc, argv);
	argc -= ret;
	argv += ret;	

	inet_aton(IP_ADDR, &inaddr);
	ipaddr.addr = inaddr.s_addr;
	inet_aton(GATEWAY, &inaddr);
	gw.addr = inaddr.s_addr;
	inet_aton(NETMASK, &inaddr);
	netmask.addr = inaddr.s_addr;

	printf(" Host at %s mask %s gateway %s\n", IP_ADDR, NETMASK, GATEWAY);
	printf(" Initializing LWIP TCP/IP Stack\n");

 	

	rte_eal_mp_remote_launch(network_stack, NULL, CALL_MASTER);
	RTE_LCORE_FOREACH_SLAVE(lcoreid) {
		if(rte_eal_wait_lcore(lcoreid) <0) 
			return -1;
	}	
	return 0;
	
}	

