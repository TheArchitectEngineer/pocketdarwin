CC ?= cc
CFLAGS ?= -O2 -Wall -Wextra -std=c11

all: darwin_interlude

darwin_interlude: darwin_interlude.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f darwin_interlude
