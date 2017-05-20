all: libjsmn.a jsmn-simple

libjsmn.a: jsmn.o
	$(AR) rc $@ $^

%.o: %.c jsmn.h
	$(CC) -c -g $(CFLAGS) $< -o $@

jsmn-simple: jsmn-simple.o libjsmn.a
	$(CC) -g $(LDFLAGS) $^ -o $@

clean:
	rm -f *.o
	rm -f *.a *.so
	rm -f jsmn-simple

.PHONY: all clean 

