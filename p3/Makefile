BINS=pzip test_hashmap
OBJS=utilities.o hashmap.o node.o
CFLAGS=-Wall -Werror -pthread -O

%.o: %.c utilities.h node.h hashmap.h
	gcc -c -o $@ $< $(CFLAGS)

all: pzip test_hashmap

pzip: pzip.o $(OBJS)
	gcc -o $@ $^ $(CFLAGS)

test_hashmap: test_hashmap.o $(OBJS) 
	gcc -o $@ $^ $(CFLAGS)

clean:
	rm -f $(BINS) *.o
