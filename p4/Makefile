HDRS=\
	list.h\
	listnode.h\
	mapreduce.h\
	partition.h\
	treemap.h\
	treenode.h\
	utilities.h\

OBJS=\
	list.o\
	listnode.o\
	mapreduce.o\
	partition.o\
	treemap.o\
	treenode.o\
	utilities.o\

CFLAGS=-Wall -Werror -pthread -O

%.o: %.c $(HDRS)
	gcc -c -o $@ $< $(CFLAGS)

all: client-wordcount test_treemap

client-wordcount: client-wordcount.o $(OBJS)
	gcc -o $@ $^ $(CFLAGS)

test_treemap: test_treemap.o $(OBJS)
	gcc -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o

