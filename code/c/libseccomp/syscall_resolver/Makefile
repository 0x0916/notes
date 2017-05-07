CC=gcc
CFLAGS=-Wall -g `pkg-config --cflags --libs libseccomp`
all: syscall_resolver

syscall_resolver: resolver.c
	$(CC) -o syscall_resolver resolver.c $(CFLAGS)

.PHONY: clean
clean:
	rm -f syscall_resolver
