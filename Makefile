PROG = suck-clock
SRCS = main.c
OBJS = $(SRCS:.c=.o)

CFLAGS = -g -Wall -Wextra -O2
LDFLAGS = -lxcb

${PROG}: $(OBJS)
	@rm -f $(PROG)
	$(CC) $(LDFLAGS) -o $(PROG) $(OBJS)

clean:
	rm -f $(PROG) $(OBJS)
