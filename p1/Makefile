OBJS=wcat wgrep wzip wunzip zcat
%.o: %.c
	gcc -c -o $< $@ -Wall -Werror

all: $(OBJS)

%: %.o
	gcc -o $< $@ -Wall -Werror

clean:
	rm -f $(OBJS) *.o
