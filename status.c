#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>

char title[]="Status";
char style[]="/style.css";
char body[]="<img src=\"graph\" alt=\"graph\" />\n";
char *html[]={
	"<!DOCTYPE html>",
	"<html>",
	"	<head>",
	"		<link rel=\"stylesheet\" type=\"text/css\" href=\"", style, "\" />"
	"		<title>", title, "</title>",
	"	</head>",
	"	<body>",
	body,
	"	</body>",
	"</html>",
	NULL,
};

void ifconf() {
	static struct ifreq ifreqs[32];
	struct ifconf ifconf;
	int sock;
	int i;
	memset(&ifconf, 0, sizeof(ifconf));
	ifconf.ifc_req=ifreqs;
	ifconf.ifc_len=sizeof(ifreqs)-1;
	if((sock=socket(PF_INET, SOCK_STREAM, 0))<0)
		return;
	if(ioctl(sock, SIOCGIFCONF, (char *) &ifconf))
		return;
	for(i=0; i<ifconf.ifc_len/sizeof(struct ifreq); i++) {
		if(ioctl(sock, SIOCGIFFLAGS, (char *) &ifreqs[i]))
			continue;
		printf("interface %s is %s, address %s\n", ifreqs[i].ifr_name, ifreqs[i].ifr_flags&IFF_UP?"up":"down", inet_ntoa(((struct sockaddr_in *) &ifreqs[i].ifr_addr)->sin_addr));
	}
	close(sock);
}

int main(int argc, char **argv) {
	printf("Content-type: text/html; charset=utf-8\nStatus: 200 OK\n\n");
	int i;
	for(i=0; html[i]; i++)
		printf("%s\n", html[i]);
	
	return 0;
}
