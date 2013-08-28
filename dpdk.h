#ifndef _DPDK_H
#define _DPDK_H

struct mbuf_table {
	uint16_t len;
	struct rte_mbuf *m_table[MAX_PKT_BURST];
} __rte_cache_aligned;

struct lcore_conf {
	uint16_t n_rx_queue;
	uint16_t rx_queue_id;
	uint16_t tx_queue_id;
	struct mbuf_table tx_mbufs;
} __rte_cache_aligned;

int lcore_setup(int, int, struct lcore_conf*);

#endif
