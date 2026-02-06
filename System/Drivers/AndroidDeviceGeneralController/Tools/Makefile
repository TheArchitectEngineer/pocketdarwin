CC ?= cc
CFLAGS ?= -O2 -Wall -Wextra

all: adgc_dump

adgc_dump: adgc_dump.c
	$(CC) $(CFLAGS) -o $@ $< -framework IOKit -framework CoreFoundation

clean:
	rm -f adgc_dump
