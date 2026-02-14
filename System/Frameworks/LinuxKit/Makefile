# Makefile for LinuxKit Framework

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -fPIC
AR = ar
RANLIB = ranlib

# Source files
SOURCES = LinuxKit.c
OBJECTS = $(SOURCES:.c=.o)
HEADERS = LinuxKit.h

# Library name
LIBRARY = liblinuxkit.a
SHARED_LIB = liblinuxkit.so

.PHONY: all clean install uninstall

all: $(LIBRARY) $(SHARED_LIB)

$(LIBRARY): $(OBJECTS)
	$(AR) rcs $@ $^
	$(RANLIB) $@

$(SHARED_LIB): $(OBJECTS)
	$(CC) -shared -o $@ $^

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(LIBRARY) $(SHARED_LIB)

install: $(LIBRARY) $(SHARED_LIB)
	install -d $(PREFIX)/lib
	install -m 644 $(LIBRARY) $(PREFIX)/lib/
	install -m 755 $(SHARED_LIB) $(PREFIX)/lib/
	install -d $(PREFIX)/include/LinuxKit
	install -m 644 $(HEADERS) $(PREFIX)/include/LinuxKit/

uninstall:
	rm -f $(PREFIX)/lib/$(LIBRARY)
	rm -f $(PREFIX)/lib/$(SHARED_LIB)
	rm -f $(PREFIX)/include/LinuxKit/LinuxKit.h

test: $(LIBRARY)
	$(CC) -o test_framework test_framework.c -L. -llinuxkit -lm
	./test_framework

help:
	@echo "LinuxKit Framework Makefile"
	@echo "==========================="
	@echo "Targets:"
	@echo "  all       - Build static and shared libraries"
	@echo "  clean     - Remove built files"
	@echo "  install   - Install to system"
	@echo "  uninstall - Remove from system"
	@echo "  test      - Build and run tests"
	@echo "  help      - Show this help"
