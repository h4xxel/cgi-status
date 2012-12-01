#include <stdio.h>

int main(int argc, char **argv) {
	printf("Content-type: text/html\nStatus: 200 OK\n\n");
	printf("CGI-test :D\n");
	printf("<img src=\"graph\" alt=\"graph\" />\n");
	return 0;
}
