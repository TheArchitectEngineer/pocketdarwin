# CoreDarwin Makefile
# Open Source CoreFoundation Replacement for PocketDarwin

CC = clang
CFLAGS = -Wall -Wextra -O2 -std=c99 -fPIC
INCLUDES = -I.
LDFLAGS = -shared
LIBS = -lpthread

# Source files
SOURCES = CoreDarwin.c RunLoop.c PropertyList.c
OBJECTS = $(SOURCES:.c=.o)
HEADERS = CoreDarwin.h

# Library name
LIB_NAME = libCoreDarwin
STATIC_LIB = $(LIB_NAME).a
SHARED_LIB = $(LIB_NAME).dylib

# Build targets
all: $(STATIC_LIB) $(SHARED_LIB)

$(STATIC_LIB): $(OBJECTS)
	ar rcs $@ $^

$(SHARED_LIB): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Install targets
install: $(STATIC_LIB) $(SHARED_LIB)
	install -d /usr/local/lib
	install -d /usr/local/include/CoreDarwin
	install $(STATIC_LIB) /usr/local/lib/
	install $(SHARED_LIB) /usr/local/lib/
	install $(HEADERS) /usr/local/include/CoreDarwin/

uninstall:
	rm -f /usr/local/lib/$(STATIC_LIB)
	rm -f /usr/local/lib/$(SHARED_LIB)
	rm -rf /usr/local/include/CoreDarwin

# Clean targets
clean:
	rm -f $(OBJECTS) $(STATIC_LIB) $(SHARED_LIB)

# Test targets
test: test_program
	./test_program

test_program: test.c $(STATIC_LIB)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $< -L. -lCoreDarwin

# Debug build
debug: CFLAGS += -g -DDEBUG
debug: all

# Help target
help:
	@echo "CoreDarwin Build System"
	@echo "======================="
	@echo ""
	@echo "Targets:"
	@echo "  all        - Build static and shared libraries"
	@echo "  static     - Build static library only"
	@echo "  shared     - Build shared library only"
	@echo "  install    - Install libraries and headers"
	@echo "  uninstall  - Remove installed files"
	@echo "  clean      - Remove build artifacts"
	@echo "  test       - Build and run tests"
	@echo "  debug      - Build with debug symbols"
	@echo "  help       - Show this help message"
	@echo ""
	@echo "Libraries:"
	@echo "  $(STATIC_LIB) - Static library"
	@echo "  $(SHARED_LIB) - Shared library"

# Phony targets
.PHONY: all install uninstall clean test debug help static shared

# Individual library targets
static: $(STATIC_LIB)
shared: $(SHARED_LIB)
