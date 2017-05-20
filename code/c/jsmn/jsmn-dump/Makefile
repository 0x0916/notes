all: libjsmn.a jsmn-dump

libjsmn.a: jsmn.o
	$(AR) rc $@ $^

%.o: %.c jsmn.h
	$(CC) -c -g $(CFLAGS) $< -o $@

jsmn-dump: jsmn-dump.o libjsmn.a
	$(CC) -g $(LDFLAGS) $^ -o $@

clean:
	rm -f *.o
	rm -f *.a *.so
	rm -f jsmn-dump

.PHONY: all clean 

