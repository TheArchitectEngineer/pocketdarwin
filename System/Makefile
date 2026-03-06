SYSTEM_NAME = PocketDarwin
SYSTEM_VERSION = 1.0
SYSTEM_ARCH = armv7
SYSTEM_TYPE = Base
SYSTEM_CONTENTS_DIR = build/Staging/System
DATA_CONTENTS_DIR = build/Staging/Data

APPS_SRC := Applications

# System apps

springboard:
	$(MAKE) -C $(APPS_SRC)/springboard
	ls -l $(APPS_SRC)/springboard/SpringBoard.app

include $(SRCROOT)/Frameworks/MakeKit/makefiles/systemBundle.mk