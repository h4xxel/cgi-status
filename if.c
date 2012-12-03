#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <asm/types.h>
#include <linux/rtnetlink.h>
#include "if.h"

static struct {
	INTERFACE interface[32];
	int num;
} interfaces;

int ifconf() {
	static struct ifreq ifreqs[32];
	struct ifconf ifconf;
	int sock;
	int i;
	memset(&ifconf, 0, sizeof(ifconf));
	ifconf.ifc_req=ifreqs;
	ifconf.ifc_len=sizeof(ifreqs);
	if((sock=socket(PF_INET, SOCK_STREAM, 0))<0)
		return 1;
	if(ioctl(sock, SIOCGIFCONF, (char *) &ifconf))
		return 2;
	interfaces.num=ifconf.ifc_len/sizeof(struct ifreq);
	for(i=0; i<interfaces.num; i++) {
		strcpy(interfaces.interface[i].name, ifreqs[i].ifr_name);
		memcpy(&interfaces.interface[i].addr, &ifreqs[i].ifr_addr, sizeof(struct sockaddr_in));
		if(!ioctl(sock, SIOCGIFINDEX, (char *) &ifreqs[i]))
			interfaces.interface[i].index=ifreqs[i].ifr_ifindex;
		if(!ioctl(sock, SIOCGIFFLAGS, (char *) &ifreqs[i]))
			interfaces.interface[i].flags=ifreqs[i].ifr_flags;
		if(!ioctl(sock, SIOCGIFBRDADDR, (char *) &ifreqs[i]))
			memcpy(&interfaces.interface[i].broadaddr, &ifreqs[i].ifr_broadaddr, sizeof(struct sockaddr_in));
		if(!ioctl(sock, SIOCGIFNETMASK, (char *) &ifreqs[i]))
			memcpy(&interfaces.interface[i].netmask, &ifreqs[i].ifr_netmask, sizeof(struct sockaddr_in));
		if(!ioctl(sock, SIOCGIFDSTADDR, (char *) &ifreqs[i]))
			memcpy(&interfaces.interface[i].destaddr, &ifreqs[i].ifr_dstaddr, sizeof(struct sockaddr_in));
	}
	close(sock);
	return ifstats()*2;
}

int ifstats() {
	int sock, i;
	struct {
		struct nlmsghdr nl;
		struct ifinfomsg ifinfo;
		char buf[8192];
	} req; 
	memset(&req, 0, sizeof(req));
	struct msghdr msg={0};
	struct iovec iov={0};
	struct sockaddr_nl addr={.nl_family=AF_NETLINK, .nl_pid=getpid()};
	
	if((sock=socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE))<0)
		return 1;
	if(bind(sock, (struct sockaddr *) &addr, sizeof(addr)))
		return 2;
	
	req.nl.nlmsg_len=NLMSG_LENGTH(sizeof(struct ifinfomsg));
	req.nl.nlmsg_flags=NLM_F_REQUEST|NLM_F_MATCH;
	req.nl.nlmsg_type=RTM_GETLINK;
	req.ifinfo.ifi_family=AF_UNSPEC;
	req.ifinfo.ifi_change=0xffffffff;
	
	addr.nl_pid=0;
	msg.msg_name = (void *) &addr;
	msg.msg_namelen = sizeof(addr);
	iov.iov_base=(void *) &req.nl;
	iov.iov_len=req.nl.nlmsg_len;
	msg.msg_iov=&iov;
	msg.msg_iovlen=2;
	sendmsg(sock, &msg, 0);
	
	memset(req.buf, 0, sizeof(req.buf));
	struct nlmsghdr *p=(void *) req.buf;
	unsigned int bytes=0;
	do {
		p=((void *) p)+bytes;
		bytes=recv(sock, (void *) p, sizeof(req.buf)-(((void *) p)-((void *) req.buf)), 0);
	} while(p->nlmsg_type!=NLMSG_DONE);
	
	bytes=((void *) p)-((void *) req.buf);
	p=(void *) req.buf;
	struct rtattr *attr;
	for(; NLMSG_OK(p, bytes); p=NLMSG_NEXT(p, bytes)) {
		unsigned int payload_size=0;
		struct ifinfomsg *ifinfo=NLMSG_DATA(p);
		int interface=ifinfo->ifi_index;
		attr=(struct rtattr *) IFLA_RTA(ifinfo);
		payload_size=RTM_PAYLOAD(p);
		for(; RTA_OK(attr, payload_size); attr=RTA_NEXT(attr, payload_size))
			if(attr->rta_type==IFLA_STATS) {
				struct rtnl_link_stats *stats=RTA_DATA(attr);
				for(i=0; i<interfaces.num; i++)
					if(interfaces.interface[i].index==interface) {
						interfaces.interface[i].stats.rx_packets=stats->rx_packets;
						interfaces.interface[i].stats.tx_packets=stats->tx_packets;
						interfaces.interface[i].stats.rx_bytes=stats->rx_bytes;
						interfaces.interface[i].stats.tx_bytes=stats->tx_bytes;
						interfaces.interface[i].stats.rx_errors=stats->rx_errors;
						interfaces.interface[i].stats.tx_errors=stats->tx_errors;
						interfaces.interface[i].stats.rx_dropped=stats->rx_dropped;
						interfaces.interface[i].stats.tx_dropped=stats->tx_dropped;
						break;
					}
			}
	}
	
	close(sock);
	return 0;
}

INTERFACE* ifstatus(char *interface) {
	int i;
	for(i=0; i<interfaces.num; i++)
		if(!strcmp(interface, interfaces.interface[i].name))
			return &interfaces.interface[i];
	return NULL;
}
