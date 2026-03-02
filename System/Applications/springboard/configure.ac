# configure.ac
# SpringBoard — PocketDarwin
#
# Autoconf 2.61+ required (ships with Xcode 3.1 / Mac OS X 10.5).
# Cross-compilation target: arm-apple-darwin9 (iPhoneOS 2.0, armv6).
#
# Typical invocation:
#   ./configure \
#       --host=arm-apple-darwin9 \
#       --with-sysroot=/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS2.0.sdk \
#       CC=arm-apple-darwin9-gcc \
#       OBJC=arm-apple-darwin9-gcc \
#       CFLAGS="-arch armv6 -miphoneos-version-min=2.0 -Os" \
#       OBJCFLAGS="-arch armv6 -miphoneos-version-min=2.0 -Os -fobjc-exceptions" \
#       LDFLAGS="-arch armv6 -miphoneos-version-min=2.0 \
#                -framework UIKit -framework Foundation \
#                -framework CoreGraphics -framework QuartzCore"

AC_PREREQ([2.61])

AC_INIT([SpringBoard],
        [2.0],
        [springboard],
        [https://github.com/oakymacintosh/pocketdarwin])

AC_CONFIG_SRCDIR([Sources/main.m])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])

# We produce a native application bundle, not a shared library,
# so we use a plain "foreign" Automake strictness level.
AM_INIT_AUTOMAKE([foreign -Wall -Werror no-define])

# ------------------------------------------------------------------
# Cross-compilation / sysroot
# ------------------------------------------------------------------

AC_ARG_WITH([sysroot],
    [AS_HELP_STRING([--with-sysroot=PATH],
        [iPhoneOS SDK sysroot (default: auto-detected from Xcode)])],
    [SYSROOT="$withval"],
    [
        # Try to find the iPhoneOS 2.0 SDK automatically via xcode-select
        XCODE_DIR=$(xcode-select --print-path 2>/dev/null || echo /Developer)
        SDK_SEARCH="$XCODE_DIR/Platforms/iPhoneOS.platform/Developer/SDKs"
        SYSROOT=$(ls -d "$SDK_SEARCH/iPhoneOS2.0.sdk" 2>/dev/null | head -1)
        if test -z "$SYSROOT"; then
            echo "configure: WARNING: iPhoneOS 2.0 SDK not found under $SDK_SEARCH; set --with-sysroot" >&2
            SYSROOT=""
        fi
    ])
AC_SUBST([SYSROOT])

if test -n "$SYSROOT"; then
    AC_MSG_NOTICE([Using SDK sysroot: $SYSROOT])
    # Inject -isysroot into all compile/link flags
    CFLAGS="$CFLAGS -isysroot $SYSROOT"
    OBJCFLAGS="$OBJCFLAGS -isysroot $SYSROOT"
    LDFLAGS="$LDFLAGS -isysroot $SYSROOT"
fi

# ------------------------------------------------------------------
# Architecture / deployment target
# ------------------------------------------------------------------

AC_ARG_WITH([arch],
    [AS_HELP_STRING([--with-arch=ARCH],
        [CPU architecture to compile for (default: armv6)])],
    [TARGET_ARCH="$withval"],
    [TARGET_ARCH="armv6"])
AC_SUBST([TARGET_ARCH])

AC_ARG_WITH([iphoneos-version-min],
    [AS_HELP_STRING([--with-iphoneos-version-min=VER],
        [Minimum iPhoneOS deployment version (default: 2.0)])],
    [IPHONEOS_VERSION_MIN="$withval"],
    [IPHONEOS_VERSION_MIN="2.0"])
AC_SUBST([IPHONEOS_VERSION_MIN])

ARCH_FLAGS="-arch $TARGET_ARCH -miphoneos-version-min=$IPHONEOS_VERSION_MIN"
CFLAGS="$CFLAGS $ARCH_FLAGS"
OBJCFLAGS="$OBJCFLAGS $ARCH_FLAGS"
LDFLAGS="$LDFLAGS $ARCH_FLAGS"

# ------------------------------------------------------------------
# Compilers
# ------------------------------------------------------------------

# C compiler (used for Autoconf probes and any .c helpers)
AC_PROG_CC

# Objective-C compiler — the same cross-gcc handles .m files
AC_PROG_OBJC

# Ranlib / ar (needed even for app builds when Automake links objects)
AC_PROG_RANLIB
AM_PROG_AR

# ------------------------------------------------------------------
# Objective-C flags
# ------------------------------------------------------------------

AC_DEFUN([PD_CHECK_COMPILE_FLAG], [
    AC_MSG_CHECKING([whether $OBJC supports $1])
    pd_saved_OBJCFLAGS="$OBJCFLAGS"
    OBJCFLAGS="$OBJCFLAGS $1"
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([], [])], [$2], [$3])
    OBJCFLAGS="$pd_saved_OBJCFLAGS"
    AC_MSG_RESULT([done])
])

# -fobjc-exceptions: needed for @try/@catch in UIKit callbacks
PD_CHECK_COMPILE_FLAG([-fobjc-exceptions],
    [OBJCFLAGS="$OBJCFLAGS -fobjc-exceptions"],
    [echo "configure: WARNING: -fobjc-exceptions not supported; exception handling disabled" >&2])

# Strict warnings
PD_CHECK_COMPILE_FLAG([-Wall],   [OBJCFLAGS="$OBJCFLAGS -Wall"], [:])
PD_CHECK_COMPILE_FLAG([-Wextra], [OBJCFLAGS="$OBJCFLAGS -Wextra"], [:])

# iPhoneOS 2.0: no ARC.  If the compiler erroneously enables it, turn it off.
PD_CHECK_COMPILE_FLAG([-fno-objc-arc],
    [OBJCFLAGS="$OBJCFLAGS -fno-objc-arc"],
    [:])

# ------------------------------------------------------------------
# Required Apple frameworks (link-time)
# ------------------------------------------------------------------
# Autoconf has no built-in framework checker; we implement a simple probe.

AC_DEFUN([PD_CHECK_FRAMEWORK], [
    AC_MSG_CHECKING([for -framework $1])
    saved_LDFLAGS="$LDFLAGS"
    LDFLAGS="$LDFLAGS -framework $1"
    AC_LINK_IFELSE(
        [AC_LANG_PROGRAM([],[])],
        [AC_MSG_RESULT([yes])],
        [AC_MSG_FAILURE([required framework $1 not found in SDK])]
    )
    LDFLAGS="$saved_LDFLAGS"
    FRAMEWORK_LIBS="$FRAMEWORK_LIBS -framework $1"
])

AC_LANG_PUSH([Objective C])

PD_CHECK_FRAMEWORK([Foundation])
PD_CHECK_FRAMEWORK([UIKit])
PD_CHECK_FRAMEWORK([CoreGraphics])
PD_CHECK_FRAMEWORK([QuartzCore])

AC_LANG_POP([Objective C])

AC_SUBST([FRAMEWORK_LIBS])

# libobjc is always required when linking ObjC programs
LIBS="$LIBS -lobjc"

# ------------------------------------------------------------------
# Required headers (sanity-check the SDK)
# ------------------------------------------------------------------

AC_LANG_PUSH([Objective C])

AC_CHECK_HEADER([UIKit/UIKit.h],
    [],
    [AC_MSG_FAILURE([UIKit/UIKit.h not found — is --with-sysroot correct?])])

AC_CHECK_HEADER([Foundation/Foundation.h],
    [],
    [AC_MSG_FAILURE([Foundation/Foundation.h not found])])

AC_CHECK_HEADER([QuartzCore/QuartzCore.h],
    [],
    [echo "configure: WARNING: QuartzCore/QuartzCore.h not found; CALayer animations may not compile" >&2])

AC_LANG_POP([Objective C])

# ------------------------------------------------------------------
# iPhoneOS 2.0 API availability checks
# ------------------------------------------------------------------

AC_LANG_PUSH([Objective C])

# UIPageControl was introduced in iPhoneOS 2.0
AC_MSG_CHECKING([for UIPageControl (iPhoneOS 2.0+)])
AC_COMPILE_IFELSE(
    [AC_LANG_PROGRAM(
        [#import <UIKit/UIKit.h>],
        [UIPageControl *pc = (UIPageControl *)0; (void)pc;])],
    [AC_MSG_RESULT([yes])],
    [AC_MSG_FAILURE([UIPageControl not available — SDK is not iPhoneOS 2.0])])

# CAKeyframeAnimation (QuartzCore, available since iPhoneOS 2.0)
AC_MSG_CHECKING([for CAKeyframeAnimation])
AC_COMPILE_IFELSE(
    [AC_LANG_PROGRAM(
        [#import <QuartzCore/QuartzCore.h>],
        [CAKeyframeAnimation *a = (CAKeyframeAnimation *)0; (void)a;])],
    [AC_MSG_RESULT([yes])],
    [echo "configure: WARNING: CAKeyframeAnimation not found; icon wiggle will be disabled" >&2])

AC_LANG_POP([Objective C])

# ------------------------------------------------------------------
# Installation paths
# ------------------------------------------------------------------

# Default install prefix for PocketDarwin system apps
if test "$prefix" = NONE; then
    prefix="/System/Library/CoreServices"
fi

# The .app bundle sits directly under prefix
AC_SUBST([BUNDLE_NAME],    [SpringBoard.app])
AC_SUBST([BUNDLE_INSTALL], ['$(prefix)/SpringBoard.app'])

# ------------------------------------------------------------------
# Output
# ------------------------------------------------------------------

AC_CONFIG_FILES([Makefile])

AC_OUTPUT

AC_MSG_NOTICE([
-------------------------------------------------------
  SpringBoard — PocketDarwin — configuration summary
-------------------------------------------------------
  Host triplet  : $host
  C compiler    : $CC
  ObjC compiler : $OBJC
  CFLAGS        : $CFLAGS
  OBJCFLAGS     : $OBJCFLAGS
  LDFLAGS       : $LDFLAGS
  Frameworks    : $FRAMEWORK_LIBS
  SDK sysroot   : $SYSROOT
  Architecture  : $TARGET_ARCH
  iOS min ver   : $IPHONEOS_VERSION_MIN
  Install prefix: $prefix
-------------------------------------------------------
  Run 'make' then 'make install' to deploy.
])
