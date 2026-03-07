include $(GNUSTEP_MAKEFILES)/common.make

FRAMEWORK_NAME = CoreOS

${FRAMEWORK_NAME}_OBJC_FILES = \
    FDSystem.m \
    FDUserSession.m \
    FDProcessInfo.m \
    FDFileSystem.m \
    FDServiceManager.m

${FRAMEWORK_NAME}_HEADER_FILES = \
    CoreOS.h \
    FDCompatibility.h \
    FDSystem.h \
    FDUserSession.h \
    FDProcessInfo.h \
    FDFileSystem.h \
    FDServiceManager.h

${FRAMEWORK_NAME}_INSTALL_DIR = $(GNUSTEP_LOCAL_LIBRARY)/Frameworks

include $(GNUSTEP_MAKEFILES)/framework.make

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
