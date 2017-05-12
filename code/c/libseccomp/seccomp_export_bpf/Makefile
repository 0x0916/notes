CC=gcc
CFLAGS=-Wall -g `pkg-config --cflags --libs libseccomp`
all: export_bpf

export_bpf: main.c
	$(CC) -o export_bpf  main.c $(CFLAGS)

.PHONY: clean
clean:
	rm -f export_bpf
