# Project Settings
FRAMEWORK_NAME = CoreOS
TOOL_NAME = coreosctl
CC = clang

# Framework Structure Paths
FRAMEWORK_BUNDLE = $(FRAMEWORK_NAME).framework
VERSIONS_DIR = $(FRAMEWORK_BUNDLE)/Versions/A
HEADERS_DIR = $(VERSIONS_DIR)/Headers
RESOURCES_DIR = $(VERSIONS_DIR)/Resources

# Files
SHARED_OBJC_FILES = FDSystem.m FDUserSession.m FDProcessInfo.m FDFileSystem.m FDServiceManager.m
FRAMEWORK_HEADERS = CoreOS.h FDCompatibility.h FDSystem.h FDUserSession.h FDProcessInfo.h FDFileSystem.h FDServiceManager.h
TOOL_MAIN = main.m

# Compiler & Linker Flags
CFLAGS = -fobjc-arc -Wall -I$(HEADERS_DIR)
LDFLAGS = -framework Foundation

.PHONY: all clean framework_structure

all: $(FRAMEWORK_BUNDLE) $(TOOL_NAME)

# 1. Create the Framework Bundle
$(FRAMEWORK_BUNDLE): $(SHARED_OBJC_FILES) framework_structure
	# Compile the dynamic library into the Versions/A directory
	$(CC) $(CFLAGS) $(LDFLAGS) -dynamiclib -o $(VERSIONS_DIR)/$(FRAMEWORK_NAME) $(SHARED_OBJC_FILES) \
		-install_name @rpath/$(FRAMEWORK_NAME).framework/Versions/A/$(FRAMEWORK_NAME)

	# Create the standard macOS symbolic links
	ln -sfh A $(FRAMEWORK_BUNDLE)/Versions/Current
	ln -sfh Versions/Current/$(FRAMEWORK_NAME) $(FRAMEWORK_BUNDLE)/$(FRAMEWORK_NAME)
	ln -sfh Versions/Current/Headers $(FRAMEWORK_BUNDLE)/Headers
	ln -sfh Versions/Current/Resources $(FRAMEWORK_BUNDLE)/Resources

# 2. Build the CLI Tool (Linking against the local framework)
$(TOOL_NAME): $(TOOL_MAIN) $(FRAMEWORK_BUNDLE)
	$(CC) $(CFLAGS) $(LDFLAGS) -F. -framework $(FRAMEWORK_NAME) -o $(TOOL_NAME) $(TOOL_MAIN)

# Internal: Setup directory hierarchy and copy headers
framework_structure:
	mkdir -p $(HEADERS_DIR)
	mkdir -p $(RESOURCES_DIR)
	cp $(FRAMEWORK_HEADERS) $(HEADERS_DIR)/

clean:
	rm -rf $(FRAMEWORK_BUNDLE) $(TOOL_NAME)

