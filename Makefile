CFLAGS+=-Wall -g -O0

all:
	$(CC) $(CFLAGS) -o status status.c
	$(CC) $(CFLAGS) -o graph graph.c -lpng
