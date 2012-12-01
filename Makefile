CFLAGS+=-Wall -g -O0
PREFIX=/usr/local

all:
	$(CC) $(CFLAGS) -o status status.c
	$(CC) $(CFLAGS) -o graph graph.c -lpng

clean:
	rm -rf status graph

install:
	install -d $(PREFIX)/lib/cgi-bin
	install -m 755 -t $(PREFIX)/lib/cgi-bin status graph
