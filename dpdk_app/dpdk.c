#include "http_server.h"

int numa_on = 1;
int promiscuous_on = 0;
struct rte_eth_link link;
int nb_txd = RTE_TEST_TX_DESC_DEFAULT;
int nb_rxd = RTE_TEST_RX_DESC_DEFAULT;
struct rte_mempool* pktmbuf_pool[NB_SOCKETS];
struct lcore_conf lcore_conf[NUM_LCORE];
struct rte_eth_conf port_conf = {
	.link_speed = 0,
	.link_duplex = 0,
	.rxmode = {
		.split_hdr_size = 0,
		.header_split = 0,
		.hw_ip_checksum = 1,
		.hw_vlan_filter = 0,
		.jumbo_frame = 0,
		.hw_strip_crc = 0,
	},
	.rx_adv_conf = {
		.rss_conf = {
			.rss_key = NULL, /* Receive Side Scaling uses a random hash*/
			.rss_hf = ETH_RSS_IPV4, /*Applies to all IPV4 packets */
		},
	},
	.txmode = {
		/* no members, future extension */
	},
	/* Interrupts + Flow Director not initialized - defaults to disable */
};
const struct rte_eth_rxconf rx_conf = {
	.rx_thresh = {
		.pthresh = RX_PTHRESH,
		.hthresh = RX_HTHRESH,
		.wthresh = RX_WTHRESH,
	},
};
const struct rte_eth_txconf tx_conf = {
	.tx_thresh = {
		.pthresh = TX_PTHRESH,
		.hthresh = TX_HTHRESH,
		.wthresh = TX_WTHRESH,
	},
	.tx_free_thresh = 0,
	.tx_rs_thresh = 0,
};
struct ether_addr ports_eth_addr;
void send_burst(__attribute__((unused)) struct lcore_conf* qconf, __attribute__((unused)) uint16_t len, __attribute__((unused)) uint8_t port) {


}

__attribute__((noreturn)) int dpdk_driver(__attribute__((unused)) void *dummy) {
	unsigned lcoreid;
	struct lcore_conf *qconf;
	int nb_rx, j;
	struct rte_mbuf *pkts_burst[MAX_PKT_BURST];
	
	lcoreid = rte_lcore_id();
	qconf = &lcore_conf[lcoreid-START_CORE];

	while (1) {
		if(qconf->tx_mbufs.len != 0) {
			send_burst(qconf, qconf->tx_mbufs.len, 0);
			qconf->tx_mbufs.len = 0;
		}
		nb_rx = rte_eth_rx_burst(0, qconf->rx_queue_id, pkts_burst, MAX_PKT_BURST);
		for (j=0; j < nb_rx; j++) {
			send_to_lwip_network_stack(pkts_burst[j], 0); 
		}
	}		

}

int init_mem(void) {
	int lcoreid = 0;
	char s[64];
	int socketid;

	for (lcoreid = START_CORE; lcoreid < NUM_LCORE+START_CORE; lcoreid++) {
		if (rte_lcore_is_enabled(lcoreid) == 0)
			rte_exit(EXIT_FAILURE, "Wrong lcore mask\n");
		if (numa_on)
			socketid = rte_lcore_to_socket_id(lcoreid);
		else 
			socketid = 0;
		if (pktmbuf_pool[socketid] == NULL) {
			rte_snprintf(s, sizeof(s), "mbuf_pool_%d", socketid);
			pktmbuf_pool[socketid] = rte_mempool_create(s, NB_MBUF, MBUF_SIZE, 32, sizeof(struct rte_pktmbuf_pool_private), rte_pktmbuf_pool_init, NULL, rte_pktmbuf_init, NULL, socketid, 0);
			if(pktmbuf_pool[socketid] == NULL)
				rte_exit(EXIT_FAILURE, "Cannot init mbuf pool on socket%d\n", socketid);
			else 
				printf("Allocated mbuf pool on socket%d\n", socketid);
		
		}

	}
	return 0;

}

int lcore_setup(void) {
	int lcoreid=0;
	for (lcoreid = START_CORE; lcoreid < NUM_LCORE+START_CORE; lcoreid++) {
		lcore_conf[lcoreid - START_CORE].n_rx_queue = NUM_RX_QUEUES_PER_LCORE;
		lcore_conf[lcoreid - START_CORE].rx_queue_id = lcoreid-START_CORE;
		lcore_conf[lcoreid - START_CORE].tx_queue_id = lcoreid-START_CORE;	
		lcore_conf[lcoreid - START_CORE].rx_port_id = 0;
		lcore_conf[lcoreid - START_CORE].tx_port_id = 0;
	}	
	return 0;
}

int init_dpdk(int argc, char** argv) {
	int ret;
	int read;
	int lcoreid;
	uint16_t nb_rx_queue, nb_tx_queue;
	uint8_t socketid;
	ret = rte_eal_init(argc, argv);
	read = ret;
	if (ret < 0) {
		rte_exit(EXIT_FAILURE, "Invalid EAL parameters\n");
	}
	ret = lcore_setup();
	if (ret < 0) {
		rte_exit(EXIT_FAILURE, "Error in setting up per lcore config\n");
	}	
	ret = init_mem();
	if (ret < 0) {
		rte_exit(EXIT_FAILURE, "Error in initializing memory\n");
	}
	ret = rte_ixgbe_pmd_init();
	if (ret < 0) {
		rte_exit(EXIT_FAILURE, "Cannot attach to ixgbe pmd\n");
	}
	ret = rte_eal_pci_probe();
	if (ret < 0) {
		rte_exit(EXIT_FAILURE, "Cannot attach to pci\n");
	}
	
	if (rte_eth_dev_count() != 1) {
		rte_exit(EXIT_FAILURE, "This program only works with one port\n");
	}
	if (rte_lcore_count() != NUM_LCORE) {
		rte_exit(EXIT_FAILURE, "This program only works with %d lcores\n", NUM_LCORE);
	}

	/* Initialize port */
	printf("Initializing port 0\n");
	fflush(stdout);
	nb_rx_queue = nb_tx_queue = NUM_LCORE;
        printf("Creating queues: nb_rxq:%hu, nb_txq:%hu ...\n", nb_rx_queue, nb_tx_queue);
	ret = rte_eth_dev_configure(0, nb_rx_queue, nb_tx_queue, &port_conf);
	if (ret < 0) 
		rte_exit(EXIT_FAILURE, "Cannot configure device: err=%d, port=0\n", ret);
	rte_eth_macaddr_get(0, &ports_eth_addr);	
	/*Set up transmit queues */
	for (lcoreid = START_CORE; lcoreid < NUM_LCORE+START_CORE; lcoreid++) {
		if(numa_on)
			socketid = (uint8_t) rte_lcore_to_socket_id(lcoreid);
		else 
			socketid = 0;
		ret = rte_eth_tx_queue_setup(0, lcore_conf[lcoreid-START_CORE].tx_queue_id, nb_txd, socketid, &tx_conf);
		if (ret < 0)
			rte_exit(EXIT_FAILURE, "rte_eth_tx_queue_setup: err = %d, port=0\n", ret);
	}
  	for (lcoreid = START_CORE; lcoreid < NUM_LCORE+START_CORE; lcoreid++) {
		if (numa_on)
			socketid = (uint8_t) rte_lcore_to_socket_id(lcoreid);
		else
			socketid = 0;
		ret = rte_eth_rx_queue_setup(0, lcore_conf[lcoreid-START_CORE].rx_queue_id, nb_rxd, socketid, &rx_conf, pktmbuf_pool[socketid]);
		if (ret < 0) 
			rte_exit(EXIT_FAILURE, "rte_eth_rx_queue_setup: err = %d, port = 0\n", ret);
	}
	/* start port */
	ret = rte_eth_dev_start(0);
	if (ret < 0)
		rte_exit(EXIT_FAILURE, "rte_eth_dev_starts: err=%d, port=0\n", ret);
	rte_eth_link_get(0, &link);
	if (link.link_status) {
		printf(" Link Up - speed %u Mbps - %s\n", (unsigned) link.link_speed, (link.link_duplex == ETH_LINK_FULL_DUPLEX) ? ("full-duplex"):("half-duplex"));
	}
	else {
		printf(" Link Down\n");
	}
	if (promiscuous_on)
		rte_eth_promiscuous_enable(0);
	
	return read;
}
