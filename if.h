#ifndef IF_H
#define IF_H

#include <arpa/inet.h>
#include <net/if.h>

typedef struct {
	unsigned int rx_packets;
	unsigned int tx_packets;
	unsigned int rx_bytes;
	unsigned int tx_bytes;
	unsigned int rx_errors;
	unsigned int tx_errors;
	unsigned int rx_dropped;
	unsigned int tx_dropped;
} INTERFACE_STATS;

typedef struct INTERFACE {
	int index;
	char name[IFNAMSIZ];
	struct sockaddr_in addr;
	struct sockaddr_in destaddr;
	struct sockaddr_in netmask;
	struct sockaddr_in broadaddr;
	short flags;
	INTERFACE_STATS stats;
} INTERFACE;

int ifconf();
int ifstats();
INTERFACE* ifstatus(char *interface);

#endif
