#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include "if.h"
#include "html.h"

HTML *html;
static const char title[]="Status";
static const char stylesheet[]="/style.css";
static const char favicon[]="/favicon.png";

char *uptime_string() {
	static char s[64];
	static struct sysinfo info;
	long sec, min, hour, day;
	s[0]=0;
	sysinfo(&info);
	long uptime=info.uptime;
	
	min=uptime/60;
	sec=uptime%60;
	if(!min) {
		sprintf(s, "%li s", sec);
		return s;
	}
	hour=min/60;
	min=min%60;
	if(!hour) {
		sprintf(s, "%li min, %li s", min, sec);
		return s;
	}
	day=hour/24;
	hour=hour%24;
	if(!day) {
		sprintf(s, "%li h, %li min, %li s", hour, min, sec);
		return s;
	}
	
	sprintf(s, "%li d, %li h, %li min, %li s", day, hour, min, sec);
	return s;
}

char *bytes_string(unsigned int bytes) {
	/*it is up to the caller to call free when done*/
	static const char *suffix[]={"B", "kB", "MB", "GB"};
	char *ret;
	int i;
	for(i=0; bytes/1024&&i<sizeof(suffix); i++)
		bytes=(bytes+(bytes%1024>512)*1024)/1024;
	
	ret=malloc(32);
	sprintf(ret, "%u %s", bytes, suffix[i]);
	return ret;
}

void status_system() {
	static char hostname[256];
	static struct utsname uts;
	gethostname(hostname, sizeof(hostname));
	uname(&uts);
	
	html_body_add(html, html_tag_double("h2", NULL, html_tag_text("System Status")));
	HTML_TAG *table=html_tag_double("table", NULL, NULL);
	
	html_tag_add(table, html_tag_double("tr", NULL, html_stack(2, 
		html_tag_double("th", NULL, html_tag_text("Hostname")),
		html_tag_double("td", NULL, html_tag_text(hostname))
	)));
	
	html_tag_add(table, html_tag_double("tr", NULL, html_stack(2, 
		html_tag_double("th", NULL, html_tag_text("System")),
		html_tag_double("td", NULL, html_tag_text(uts.sysname))
	)));
	
	html_tag_add(table, html_tag_double("tr", NULL, html_stack(2, 
		html_tag_double("th", NULL, html_tag_text("Release")),
		html_tag_double("td", NULL, html_tag_text(uts.release))
	)));
	
	html_tag_add(table, html_tag_double("tr", NULL, html_stack(2, 
		html_tag_double("th", NULL, html_tag_text("Version")),
		html_tag_double("td", NULL, html_tag_text(uts.version))
	)));
	
	html_tag_add(table, html_tag_double("tr", NULL, html_stack(2, 
		html_tag_double("th", NULL, html_tag_text("Architecture")),
		html_tag_double("td", NULL, html_tag_text(uts.machine))
	)));
	
	html_body_add(html, table);
	table=html_tag_double("table", NULL, NULL);
	
	html_tag_add(table, html_tag_double("tr", NULL, html_stack(2, 
		html_tag_double("th", NULL, html_tag_text("Uptime")),
		html_tag_double("td", NULL, html_tag_text(uptime_string()))
	)));
	
	html_body_add(html, table);
	html_body_add(html, html_tag_single("hr", NULL));
}

void status_lan() {
	const char *s_addr;
	char *addr;
	INTERFACE *eth0=ifstatus("eth0");
	html_body_add(html, html_tag_double("h2", NULL, html_tag_text("LAN Status")));
	if(eth0&&(eth0->flags&IFF_UP)) {
		html_body_add(html, html_tag_double("p", NULL, html_stack(2, 
			html_tag_text("LAN is "),
			html_tag_double("span", html_tag_attributes(1, "class", "connected"), 
				html_tag_text("connected")
			)
		)));
		HTML_TAG *table=html_tag_double("table", NULL, NULL);
		
		html_tag_add(table, html_tag_double("tr", NULL, html_stack(2,
			html_tag_double("th", NULL, html_tag_text("Interface")),
			html_tag_double("td", NULL, html_tag_text(eth0->name))
		)));
		
		s_addr=inet_ntoa(eth0->addr.sin_addr);
		addr=malloc(strlen(s_addr)+1);
		strcpy(addr, s_addr);
		html_tag_add(table, html_tag_double("tr", NULL, html_stack(2, 
			html_tag_double("th", NULL, html_tag_text("IP Address")),
			html_tag_double("td", NULL, html_tag_text(addr))
		)));
		
		s_addr=inet_ntoa(eth0->netmask.sin_addr);
		addr=malloc(strlen(s_addr)+1);
		strcpy(addr, s_addr);
		html_tag_add(table, html_tag_double("tr", NULL, html_stack(2, 
			html_tag_double("th", NULL, html_tag_text("Netmask")),
			html_tag_double("td", NULL, html_tag_text(addr))
		)));
		
		s_addr=inet_ntoa(eth0->broadaddr.sin_addr);
		addr=malloc(strlen(s_addr)+1);
		strcpy(addr, s_addr);
		html_tag_add(table, html_tag_double("tr", NULL, html_stack(2, 
			html_tag_double("th", NULL, html_tag_text("Broadcast")),
			html_tag_double("td", NULL, html_tag_text(addr))
		)));
		
		html_body_add(html, table);
		table=html_tag_double("table", NULL, NULL);
		
		html_tag_add(table, html_tag_double("tr", NULL, html_stack(2, 
			html_tag_double("th", NULL, html_tag_text("Received")),
			html_tag_double("td", NULL, html_tag_text(bytes_string(eth0->stats.rx_bytes)))
		)));
		
		html_tag_add(table, html_tag_double("tr", NULL, html_stack(2, 
			html_tag_double("th", NULL, html_tag_text("Sent")),
			html_tag_double("td", NULL, html_tag_text(bytes_string(eth0->stats.tx_bytes)))
		)));
		
		html_body_add(html, table);
	} else
		html_body_add(html, html_tag_double("p", NULL, html_stack(2, 
			html_tag_text("LAN is "),
			html_tag_double("span", html_tag_attributes(1, "class", "disconnected"), 
				html_tag_text("disconnected")))));
	html_body_add(html, html_tag_single("hr", NULL));
}

void status_3g() {
	const char *s_addr;
	char *addr;
	INTERFACE *ppp0=ifstatus("ppp0");
	html_body_add(html, html_tag_double("h2", NULL, html_tag_text("3G Status")));
	if(ppp0&&(ppp0->flags&IFF_UP)) {
		html_body_add(html, html_tag_double("p", NULL, html_stack(2, 
			html_tag_text("3G is "),
			html_tag_double("span", html_tag_attributes(1, "class", "connected"), 
				html_tag_text("connected")
			)
		)));
		HTML_TAG *table=html_tag_double("table", NULL, NULL);
		
		html_tag_add(table, html_tag_double("tr", NULL, html_stack(2,
			html_tag_double("th", NULL, html_tag_text("Interface")),
			html_tag_double("td", NULL, html_tag_text(ppp0->name))
		)));
		
		s_addr=inet_ntoa(ppp0->addr.sin_addr);
		addr=malloc(strlen(s_addr)+1);
		strcpy(addr, s_addr);
		html_tag_add(table, html_tag_double("tr", NULL, html_stack(2, 
			html_tag_double("th", NULL, html_tag_text("IP Address")),
			html_tag_double("td", NULL, html_tag_text(addr))
		)));
		
		s_addr=inet_ntoa(ppp0->netmask.sin_addr);
		addr=malloc(strlen(s_addr)+1);
		strcpy(addr, s_addr);
		html_tag_add(table, html_tag_double("tr", NULL, html_stack(2, 
			html_tag_double("th", NULL, html_tag_text("Netmask")),
			html_tag_double("td", NULL, html_tag_text(addr))
		)));
		
		s_addr=inet_ntoa(ppp0->destaddr.sin_addr);
		addr=malloc(strlen(s_addr)+1);
		strcpy(addr, s_addr);
		html_tag_add(table, html_tag_double("tr", NULL, html_stack(2, 
			html_tag_double("th", NULL, html_tag_text("Destination")),
			html_tag_double("td", NULL, html_tag_text(addr))
		)));
		
		html_body_add(html, table);
		table=html_tag_double("table", NULL, NULL);
		
		html_tag_add(table, html_tag_double("tr", NULL, html_stack(2, 
			html_tag_double("th", NULL, html_tag_text("Received")),
			html_tag_double("td", NULL, html_tag_text(bytes_string(ppp0->stats.rx_bytes)))
		)));
		
		html_tag_add(table, html_tag_double("tr", NULL, html_stack(2, 
			html_tag_double("th", NULL, html_tag_text("Sent")),
			html_tag_double("td", NULL, html_tag_text(bytes_string(ppp0->stats.tx_bytes)))
		)));
		
		html_body_add(html, table);
	} else
		html_body_add(html, html_tag_double("p", NULL, html_stack(2, 
			html_tag_text("3G is "),
			html_tag_double("span", html_tag_attributes(1, "class", "disconnected"), 
				html_tag_text("disconnected")
			)
		)));
	html_body_add(html, html_tag_single("hr", NULL));
}

int main(int argc, char **argv) {
	printf("Content-type: text/html; charset=utf-8\nStatus: 200 OK\n\n");
	
	ifconf();
	
	html=html_create(title);
	html_head_add(html, html_tag_single("link", html_tag_attributes(3, "rel", "stylesheet", "type", "text/css", "href", stylesheet)));
	html_head_add(html, html_tag_single("link", html_tag_attributes(3, "rel", "favicon", "type", "image/png", "href", favicon)));
	html_body_add(html, html_tag_double("h1", NULL, html_tag_text(title)));
	
	status_system();
	status_lan();
	status_3g();
	
	html_body_add(html, html_tag_double("small", NULL, html_stack(2, 
		html_tag_text("cgi-status by"),
		html_tag_double("a", html_tag_attributes(1, "href", "http://h4xxel.org/"), 
			html_tag_text("h4xxel")
		)
	)));
	
	html_write(html);
	return 0;
}
