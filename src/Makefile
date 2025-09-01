CC := gcc
CFLAGS := -std=c11 -g -O0 -Wall -Wextra -Wno-unused-parameter

SRCS := shell.c sim.c decode.c execute.c utils.c
OBJDIR := obj
OBJS := $(addprefix $(OBJDIR)/,$(SRCS:.c=.o))

all: sim

sim: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJDIR)/%.o: %.c
	mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(OBJDIR) sim *~ dumpsim