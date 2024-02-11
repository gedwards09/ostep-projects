
CFLAGS=-Wall -Werror -pthread -O

%.o: %.c utilities.h node.h hashmap.h
	gcc -c -o $@ $< $(CFLAGS)

pzip: pzip.o utilities.o hashmap.o node.o
	gcc -o $@ $^ $(CFLAGS)

test_hashmap: test_hashmap.o utilities.o hashmap.o node.o
	gcc -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o
	
