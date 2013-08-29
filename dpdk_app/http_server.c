#include <fcntl.h>
#include <unistd.h>

#include "http_server.h"

/* THE FOLLOWING VALUES CAN BE SET */
/*--------------------------------------------------------*/

#define NETMASK "255.255.255.0"
#define GATEWAY "198.162.52.254"
#define IP_ADDR "198.162.52.211"

/*-------------------------------------------------------*/

unsigned char ping_flag = 1;
ip_addr_t ipaddr, netmask, gw;
 
void send_to_lwip_network_stack(__attribute__((unused)) struct rte_mbuf* m, __attribute__((unused)) uint8_t portid) {

}

int main(int argc, char** argv) {
	int ret = 0;
	int lcoreid = 0;
        struct in_addr inaddr;

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
	

	rte_eal_mp_remote_launch(dpdk_driver, NULL, CALL_MASTER);
	RTE_LCORE_FOREACH_SLAVE(lcoreid) {
		if(rte_eal_wait_lcore(lcoreid) <0) 
			return -1;
	}	
	return 0;
	
}	

