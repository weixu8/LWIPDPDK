#ifndef _HTTP_SERVER_H
#define _HTTP_SERVER_H
#include "lwip/opt.h"
#include "lwip/init.h"
#include "lwip/mem.h"
#include "lwip/sys.h"
#include "lwip/stats.h"
#include "lwip/tcp_impl.h"
#include "lwip/inet_chksum.h"
#include "lwip/tcpip.h"
#include "lwip/sockets.h"
#include "netif/unixif.h"
#include "netif/tcpdump.h"
#include "lwip/ip_addr.h"
#include "httpd.h"
#include "lwip/icmp.h"
#include "lwip/raw.h"
#include "dpdk.h"
int main(int argc, char **argv);
void send_to_lwip_network_stack(struct rte_mbuf*, uint8_t);
#endif
