#include <stdio.h>

char title[]="Status";
char body[]="<img src=\"graph\" alt=\"graph\" />\n";
char *html[]={
	"<!DOCTYPE html>",
	"<html>",
	"	<head>",
	"		<title>", title, "</title>",
	"	</head>",
	"	<body>",
	body,
	"	</body>",
	"</html>",
	NULL,
};

int main(int argc, char **argv) {
	printf("Content-type: text/html; charset=utf-8\nStatus: 200 OK\n\n");
	int i;
	for(i=0; html[i]; i++)
		printf("%s\n", html[i]);
	return 0;
}
