CC ?= cc
CXX ?= c++
CFLAGS ?= -O2 -Wall -Wextra -std=c11
CXXFLAGS ?= -O2 -Wall -Wextra -std=c++17

# Build toggles / overrides:
#   make L2D_WITH_LIBFDT=1 LIBFDT_CFLAGS="-I/path" LIBFDT_LDFLAGS="-L/path -lfdt"
LIBFDT_CFLAGS ?=
LIBFDT_LDFLAGS ?=

L2D_WITH_LIBFDT ?= 0
TOOLS :=
INTERLUDE_SOURCES := darwin_interlude.c JSONParser.c InterludeIO.c
INTERLUDE_CXX_SOURCES :=

ifneq ($(L2D_WITH_LIBFDT),0)
TOOLS += l2d_dump
CXXFLAGS += -DL2D_USE_LIBFDT $(LIBFDT_CFLAGS)
CFLAGS += -DL2D_USE_LIBFDT $(LIBFDT_CFLAGS)
INTERLUDE_CXX_SOURCES += Linux2DarwinBridge.cxx
LDFLAGS += $(LIBFDT_LDFLAGS) -lfdt
endif

all: darwin_interlude $(TOOLS)

darwin_interlude: $(INTERLUDE_SOURCES) $(INTERLUDE_CXX_SOURCES)
ifeq ($(L2D_WITH_LIBFDT),0)
	$(CC) $(CFLAGS) -o $@ $(INTERLUDE_SOURCES)
else
	$(CXX) $(CXXFLAGS) -o $@ $(INTERLUDE_SOURCES) $(INTERLUDE_CXX_SOURCES) $(LDFLAGS)
endif

l2d_dump: l2d_dump.cpp Linux2DarwinBridge.cxx
	$(CXX) $(CXXFLAGS) -o $@ l2d_dump.cpp Linux2DarwinBridge.cxx $(LDFLAGS)

clean:
	rm -f darwin_interlude l2d_dump
