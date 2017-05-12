CC=gcc
CFLAGS=-Wall -g `pkg-config --cflags --libs libseccomp`
all: export_pfc

export_pfc: main.c
	$(CC) -o export_pfc  main.c $(CFLAGS)

.PHONY: clean
clean:
	rm -f export_pfc
