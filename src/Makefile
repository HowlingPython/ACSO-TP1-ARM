CC := gcc
CFLAGS := -std=c11 -g -O0 -Wall -Wextra -Wno-unused-parameter
DEPFLAGS := -MMD -MP

SRCS := shell.c sim.c decode.c execute.c utils.c
OBJS := $(SRCS:.c=.o)
DEPS := $(OBJS:.o=.d)

all: sim

sim: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

-include $(DEPS)

clean:
	rm -f $(OBJS) $(DEPS) sim *~ dumpsim
