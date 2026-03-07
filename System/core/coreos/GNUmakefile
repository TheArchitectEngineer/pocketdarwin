include $(GNUSTEP_MAKEFILES)/common.make

LIBRARY_NAME = CoreOS

CoreOS_OBJC_FILES = \
    FDSystem.m \
    FDUserSession.m \
    FDProcessInfo.m \
    FDFileSystem.m \
    FDServiceManager.m

CoreOS_HEADER_FILES = \
    CoreOS.h \
    FDCompatibility.h \
    FDSystem.h \
    FDUserSession.h \
    FDProcessInfo.h \
    FDFileSystem.h \
    FDServiceManager.h

CoreOS_INSTALL_DIR = $(GNUSTEP_LOCAL_LIBRARY)/Frameworks

include $(GNUSTEP_MAKEFILES)/library.make

TOOL_NAME = coreosctl

coreosctl_OBJC_FILES = \
    main.m \
    FDSystem.m \
    FDUserSession.m \
    FDProcessInfo.m \
    FDFileSystem.m \
    FDServiceManager.m

coreosctl_TOOL_LIBS = -lobjc -lgnustep-base

include $(GNUSTEP_MAKEFILES)/tool.make
