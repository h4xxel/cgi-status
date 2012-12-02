#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include "html.h"

typedef struct INTERFACE {
	char name[IFNAMSIZ];
	struct sockaddr_in addr;
	struct sockaddr destaddr;
	struct sockaddr broadaddr;
	short flags;
} INTERFACE;

static struct {
	INTERFACE interface[32];
	int num;
} interfaces;

HTML *html;
static const char title[]="Status";
static const char stylesheet[]="/style.css";

void ifconf() {
	static struct ifreq ifreqs[32];
	struct ifconf ifconf;
	int sock;
	int i;
	memset(&ifconf, 0, sizeof(ifconf));
	ifconf.ifc_req=ifreqs;
	ifconf.ifc_len=sizeof(ifreqs);
	if((sock=socket(PF_INET, SOCK_STREAM, 0))<0)
		return;
	if(ioctl(sock, SIOCGIFCONF, (char *) &ifconf))
		return;
	interfaces.num=ifconf.ifc_len/sizeof(struct ifreq);
	for(i=0; i<interfaces.num; i++) {
		strcpy(interfaces.interface[i].name, ifreqs[i].ifr_name);
		memcpy(&interfaces.interface[i].addr, &ifreqs[i].ifr_addr, sizeof(struct sockaddr_in));
		if(ioctl(sock, SIOCGIFFLAGS, (char *) &ifreqs[i]))
			continue;
		interfaces.interface[i].flags=ifreqs[i].ifr_flags;
		//printf("interface %s is %s, address %s\n", ifreqs[i].ifr_name, ifreqs[i].ifr_flags&IFF_UP?"up":"down", inet_ntoa(((struct sockaddr_in *) &ifreqs[i].ifr_addr)->sin_addr));
	}
	close(sock);
}

struct INTERFACE* ifstatus(char *interface) {
	int i;
	for(i=0; i<interfaces.num; i++)
		if(!strcmp(interface, interfaces.interface[i].name))
			return &interfaces.interface[i];
	return NULL;
}

void status_lan() {
	INTERFACE *eth0=ifstatus("eth0");
	html_body_add(html, html_tag_double("h2", NULL, html_tag_text("LAN Status")));
	if(eth0&&(eth0->flags&IFF_UP)) {
		html_body_add(html, html_tag_double("p", NULL, html_tag_text("LAN is connected")));
		HTML_TAG *row, *table=html_tag_double("table", NULL, NULL);
		row=html_tag_double("tr", NULL, NULL);
		html_tag_add(row, html_tag_double("th", NULL, html_tag_text("Interface")));
		html_tag_add(row, html_tag_double("td", NULL, html_tag_text(eth0->name)));
		html_tag_add(table, row);
		row=html_tag_double("tr", NULL, NULL);
		html_tag_add(row, html_tag_double("th", NULL, html_tag_text("IP Address")));
		const char *s_ip=inet_ntoa(eth0->addr.sin_addr);
		char *ip=malloc(strlen(s_ip)+1);
		strcpy(ip, s_ip);
		html_tag_add(row, html_tag_double("td", NULL, html_tag_text(ip)));
		html_tag_add(table, row);
		html_body_add(html, table);
	} else
		html_body_add(html, html_tag_double("p", NULL, html_tag_text("LAN is disconnected")));
	html_body_add(html, html_tag_single("hr", NULL));
}

int main(int argc, char **argv) {
	printf("Content-type: text/html; charset=utf-8\nStatus: 200 OK\n\n");
	ifconf();
	html=html_create(title);
	html_head_add(html, html_tag_single("link", html_tag_attributes(3, "rel", "stylesheet", "type", "text/css", "href", stylesheet)));
	html_body_add(html, html_tag_double("h1", NULL, html_tag_text(title)));
	status_lan();
	html_write(html);
	return 0;
}
