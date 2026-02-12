# Makefile.am for interlude boot system
# Automake configuration for PocketDarwin Interlude

ACLOCAL_AMFLAGS = -I m4

# Source files
INTERLUDE_SOURCES = \
	darwin_interlude.c \
	JSONParser.c \
	InterludeIO.c

# Header files
noinst_HEADERS = \
	InterludeIO.h \
	JSONParser.h \
	Linux2DarwinBridge.h

# VMApple sources
VMAPPLE_SOURCES = \
	vmapple/vmapple.c \
	vmapple/vmapple-select.c

# Additional tools (conditionally built)
EXTRA_DIST = \
	l2d_dump.cpp \
	Linux2DarwinBridge.cxx \
	vmapple/vmapple.h

# Main interlude binary
bin_PROGRAMS = darwin_interlude vmapple

darwin_interlude_SOURCES = $(INTERLUDE_SOURCES)

# Conditional compilation for libfdt support
if L2D_USE_LIBFDT
darwin_interlude_SOURCES += Linux2DarwinBridge.cxx
darwin_interlude_LDADD = @LIBFDT_LIBS@
endif

# VMApple binary
vmapple_SOURCES = $(VMAPPLE_SOURCES)

# Additional tools (built conditionally)
if L2D_USE_LIBFDT
bin_PROGRAMS += l2d_dump
l2d_dump_SOURCES = l2d_dump.cpp Linux2DarwinBridge.cxx
l2d_dump_LDADD = @LIBFDT_LIBS@
endif

# Compiler flags
AM_CFLAGS = -Wall -Wextra -std=c11
AM_CXXFLAGS = -Wall -Wextra -std=c++17

# Platform-specific flags
AM_CPPFLAGS = \
	-D_GNU_SOURCE

# Installation directories
interludedir = $(pkglibdir)
interlude_DATA = \
	README.md \
	CHANGELOG.md

# Documentation
dist_doc_DATA = \
	README.md \
	CHANGELOG.md

# Clean targets
clean-local:
	rm -f *.o vmapple/*.o

# Install targets
install-exec-hook:
	$(MKDIR_P) $(DESTDIR)$(sbindir)
	$(INSTALL_PROGRAM) darwin_interlude $(DESTDIR)$(sbindir)/darwin_interlude
	$(INSTALL_PROGRAM) vmapple $(DESTDIR)$(sbindir)/vmapple
if L2D_USE_LIBFDT
	$(INSTALL_PROGRAM) l2d_dump $(DESTDIR)$(sbindir)/l2d_dump
endif

# Uninstall targets
uninstall-hook:
	rm -f $(DESTDIR)$(sbindir)/darwin_interlude
	rm -f $(DESTDIR)$(sbindir)/vmapple
if L2D_USE_LIBFDT
	rm -f $(DESTDIR)$(sbindir)/l2d_dump
endif

# Development targets
.PHONY: debug release

debug:
	$(MAKE) CFLAGS="-g -O0 -Wall -Wextra -std=c11" \
	        CXXFLAGS="-g -O0 -Wall -Wextra -std=c++17"

release:
	$(MAKE) CFLAGS="-O3 -Wall -Wextra -std=c11" \
	        CXXFLAGS="-O3 -Wall -Wextra -std=c++17"

# Test targets
.PHONY: test check-syntax

test: $(bin_PROGRAMS)
	@echo "Running basic tests..."
	./darwin_interlude --help || true
	./vmapple --help || true
if L2D_USE_LIBFDT
	./l2d_dump --help || true
endif

check-syntax:
	@echo "Checking syntax..."
	$(CC) $(CFLAGS) -fsyntax-only $(INTERLUDE_SOURCES)
if L2D_USE_LIBFDT
	$(CXX) $(CXXFLAGS) -fsyntax-only Linux2DarwinBridge.cxx l2d_dump.cpp
endif
	$(CC) $(CFLAGS) -fsyntax-only $(VMAPPLE_SOURCES)

# Package targets
.PHONY: distclean-local

distclean-local:
	rm -rf build-aux autom4te.cache
	rm -f config.log config.status

# Help target
.PHONY: help

help:
	@echo "Interlude Boot System Build Targets:"
	@echo ""
	@echo "  all          - Build all programs"
	@echo "  darwin_interlude - Build main interlude binary"
	@echo "  vmapple      - Build VMApple tools"
	@echo "  l2d_dump     - Build device tree dump tool (requires libfdt)"
	@echo ""
	@echo "  install      - Install programs"
	@echo "  uninstall    - Remove installed programs"
	@echo ""
	@echo "  clean        - Remove build artifacts"
	@echo "  distclean    - Remove all generated files"
	@echo ""
	@echo "  debug        - Build with debug flags"
	@echo "  release      - Build optimized"
	@echo ""
	@echo "  test         - Run basic tests"
	@echo "  check-syntax - Check syntax only"
	@echo ""
	@echo "Configuration options:"
	@echo "  --with-libfdt    Enable libfdt support"
	@echo "  --enable-debug   Enable debug build"
	@echo "  --enable-release Enable optimized build"