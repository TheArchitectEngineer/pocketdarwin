HFS_CFLAGS ?=
HFS_LDFLAGS ?=

BUILD_DIR := ../../build/hfs

SRCS := hfs_rw_stub.c
OBJS := $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRCS))

CC ?= $(CROSS_PREFIX)gcc
CFLAGS := -O2 -Wall $(HFS_CFLAGS)

.PHONY: all clean

all: $(BUILD_DIR)/hfs_rw

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/hfs_rw: $(OBJS)
	$(CC) $(LDFLAGS) $(HFS_LDFLAGS) -o $@ $^

clean:
	 rm -rf $(BUILD_DIR)

help:
	@echo "This target builds a small userspace HFS+ helper when an HFS library is provided."
