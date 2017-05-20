all: libjsmn.a jsmn-internal

libjsmn.a: jsmn.o
	$(AR) rc $@ $^

%.o: %.c jsmn.h
	$(CC) -c -g $(CFLAGS) $< -o $@

jsmn-internal: jsmn-internal.o libjsmn.a
	$(CC) -g $(LDFLAGS) $^ -o $@

clean:
	rm -f *.o
	rm -f *.a *.so
	rm -f jsmn-internal

.PHONY: all clean 

