CC=gcc
CFLAGS=-Wall -g `pkg-config --cflags --libs libseccomp`
all: version

version: main.c
	$(CC) -o version  main.c $(CFLAGS)

.PHONY: clean
clean:
	rm -f version
