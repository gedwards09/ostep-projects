OBJS=xcheck.o
CFLAGS=-Wall -Werror -O

%.o: %.c
	gcc -c -o $@ $< $(CFLAGS)

xcheck: xcheck.o
	gcc -o $@ $^

clean:
	rm -f *.o

