OBJS = hyg.o bbatt.o hwaddr.o

CFLAGS += -g
CFLAGS += -O2

all:	hyg

hyg:	$(OBJS)
	$(CC) -o $@ $^

%.o:	%.c
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -fr hyg $(OBJS)
