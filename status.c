#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>

static struct {
	struct ifreq ifreqs[32];
	int num;
} interfaces;

char title[]="Status";
char style[]="/style.css";
char body[]=
	"<h1>Status</h1>"
	"<h2>System</h2>"
	"<h2>3G</h2>"
	"<img src=\"graph\" alt=\"graph\" />"
	"";
char *html[]={
	"<!DOCTYPE html>",
	"<html>",
	"	<head>",
	"		<link rel=\"stylesheet\" type=\"text/css\" href=\"", style, "\" />\n"
	"		<title>", title, "</title>",
	"	</head>",
	"	<body>",
	body,
	"	</body>",
	"</html>",
	NULL,
};

void ifconf() {
	struct ifconf ifconf;
	int sock;
	int i;
	memset(&ifconf, 0, sizeof(ifconf));
	ifconf.ifc_req=interfaces.ifreqs;
	ifconf.ifc_len=sizeof(interfaces.ifreqs)-1;
	if((sock=socket(PF_INET, SOCK_STREAM, 0))<0)
		return;
	if(ioctl(sock, SIOCGIFCONF, (char *) &ifconf))
		return;
	interfaces.num=ifconf.ifc_len/sizeof(struct ifreq);
	for(i=0; i<interfaces.num; i++) {
		if(ioctl(sock, SIOCGIFFLAGS, (char *) &interfaces.ifreqs[i]))
			continue;
		//printf("interface %s is %s, address %s\n", ifreqs[i].ifr_name, ifreqs[i].ifr_flags&IFF_UP?"up":"down", inet_ntoa(((struct sockaddr_in *) &ifreqs[i].ifr_addr)->sin_addr));
	}
	close(sock);
}

struct ifreq* ifstatus(char *interface) {
	int i;
	for(i=0; i<interfaces.num; i++)
		if(!strcmp(interface, interfaces.ifreqs[i].ifr_name))
			return &interfaces.ifreqs[i];
	return NULL;
}

void status_lan() {
	struct ifreq *eth0=ifstatus("eth0");
	puts("<h2>LAN</h2>");
	if(eth0&&(eth0->ifr_flags&IFF_UP)) {
		puts("<p>LAN is connected</p>");
	} else {
		puts("<p>LAN is disconnected</p>");
	}
	puts("<hr />");
}

int main(int argc, char **argv) {
	printf("Content-type: text/html; charset=utf-8\nStatus: 200 OK\n\n");
	int i;
	ifconf();
	for(i=0; html[i]; i++)
		printf("%s", html[i]);
	status_lan();
	return 0;
}
