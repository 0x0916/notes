CC=gcc
CFLAGS=-Wall -g `pkg-config --cflags --libs libseccomp`
all: arch

arch: main.c
	$(CC) -o arch  main.c $(CFLAGS)

.PHONY: clean
clean:
	rm -f arch
