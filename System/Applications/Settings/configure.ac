# configure.ac
# Settings.app — PocketDarwin
#
# Autoconf 2.61+ required.
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
#                -framework CoreGraphics"

AC_PREREQ([2.61])

AC_INIT([Settings],
        [2.0],
        [pocketdarwin-dev@example.org],
        [settings],
        [https://github.com/pocketdarwin/settings])

AC_CONFIG_SRCDIR([Sources/main.m])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([foreign -Wall -Werror no-define])

# ------------------------------------------------------------------
# Cross-compilation / sysroot
# ------------------------------------------------------------------

AC_ARG_WITH([sysroot],
    [AS_HELP_STRING([--with-sysroot=PATH],
        [iPhoneOS SDK sysroot (default: auto-detected from Xcode)])],
    [SYSROOT="$withval"],
    [
        XCODE_DIR=$(xcode-select --print-path 2>/dev/null || echo /Developer)
        SDK_SEARCH="$XCODE_DIR/Platforms/iPhoneOS.platform/Developer/SDKs"
        SYSROOT=$(ls -d "$SDK_SEARCH/iPhoneOS2.0.sdk" 2>/dev/null | head -1)
        if test -z "$SYSROOT"; then
            AC_MSG_WARN([iPhoneOS 2.0 SDK not found under $SDK_SEARCH; set --with-sysroot])
            SYSROOT=""
        fi
    ])
AC_SUBST([SYSROOT])

if test -n "$SYSROOT"; then
    AC_MSG_NOTICE([Using SDK sysroot: $SYSROOT])
    CFLAGS="$CFLAGS -isysroot $SYSROOT"
    OBJCFLAGS="$OBJCFLAGS -isysroot $SYSROOT"
    LDFLAGS="$LDFLAGS -isysroot $SYSROOT"
fi

# ------------------------------------------------------------------
# Architecture / deployment target
# ------------------------------------------------------------------

AC_ARG_WITH([arch],
    [AS_HELP_STRING([--with-arch=ARCH],
        [CPU architecture (default: armv6)])],
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

AC_PROG_CC
AC_PROG_OBJC
AC_PROG_RANLIB
AM_PROG_AR

# ------------------------------------------------------------------
# Objective-C flags
# ------------------------------------------------------------------

AX_CHECK_COMPILE_FLAG([-fobjc-exceptions],
    [OBJCFLAGS="$OBJCFLAGS -fobjc-exceptions"],
    [AC_MSG_WARN([-fobjc-exceptions not supported])])

AX_CHECK_COMPILE_FLAG([-Wall],   [OBJCFLAGS="$OBJCFLAGS -Wall"])
AX_CHECK_COMPILE_FLAG([-Wextra], [OBJCFLAGS="$OBJCFLAGS -Wextra"])

# No ARC — iPhoneOS 2.0 uses manual retain/release throughout
AX_CHECK_COMPILE_FLAG([-fno-objc-arc],
    [OBJCFLAGS="$OBJCFLAGS -fno-objc-arc"])

# ------------------------------------------------------------------
# NSInvocation / runtime check
# ------------------------------------------------------------------
# PSTableCell and PSViewController use NSInvocation to call typed
# getter/setter selectors dynamically.  Verify the ObjC runtime
# exposes it (it always does on iPhoneOS 2.0, but check anyway).

AC_LANG_PUSH([Objective C])

AC_MSG_CHECKING([for NSInvocation])
AC_COMPILE_IFELSE(
    [AC_LANG_PROGRAM(
        [#import <Foundation/Foundation.h>],
        [NSMethodSignature *sig = [NSObject instanceMethodSignatureForSelector:@selector(init)];
         NSInvocation *inv = [NSInvocation invocationWithMethodSignature:sig];
         (void)inv;])],
    [AC_MSG_RESULT([yes])],
    [AC_MSG_FAILURE([NSInvocation not available — PSTableCell requires it])])

AC_LANG_POP([Objective C])

# ------------------------------------------------------------------
# Required Apple frameworks
# ------------------------------------------------------------------

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

# QuartzCore is optional for Settings (no CALayer animations needed)
AC_MSG_CHECKING([for -framework QuartzCore (optional)])
saved_LDFLAGS="$LDFLAGS"
LDFLAGS="$LDFLAGS -framework QuartzCore"
AC_LINK_IFELSE(
    [AC_LANG_PROGRAM([],[])],
    [AC_MSG_RESULT([yes])
     FRAMEWORK_LIBS="$FRAMEWORK_LIBS -framework QuartzCore"],
    [AC_MSG_RESULT([no (optional — skipped)])])
LDFLAGS="$saved_LDFLAGS"

AC_LANG_POP([Objective C])

AC_SUBST([FRAMEWORK_LIBS])

LIBS="$LIBS -lobjc"

# ------------------------------------------------------------------
# sysctl availability (used by PSAboutController to read hw.machine)
# ------------------------------------------------------------------

AC_CHECK_HEADERS([sys/sysctl.h], [],
    [AC_MSG_WARN([sys/sysctl.h not found — PSAboutController will show "Unknown" for hardware info])])

AC_CHECK_FUNC([sysctlbyname], [],
    [AC_MSG_WARN([sysctlbyname() not available])])

# ------------------------------------------------------------------
# Required SDK headers
# ------------------------------------------------------------------

AC_LANG_PUSH([Objective C])

AC_CHECK_HEADER([UIKit/UIKit.h], [],
    [AC_MSG_FAILURE([UIKit/UIKit.h not found — is --with-sysroot correct?])])

AC_CHECK_HEADER([Foundation/Foundation.h], [],
    [AC_MSG_FAILURE([Foundation/Foundation.h not found])])

# UITableView grouped style (iPhoneOS 2.0)
AC_MSG_CHECKING([for UITableViewStyleGrouped])
AC_COMPILE_IFELSE(
    [AC_LANG_PROGRAM(
        [#import <UIKit/UIKit.h>],
        [UITableView *tv = [[UITableView alloc] initWithFrame:CGRectZero
                                                       style:UITableViewStyleGrouped];
         (void)tv;])],
    [AC_MSG_RESULT([yes])],
    [AC_MSG_FAILURE([UITableViewStyleGrouped not available — SDK is not iPhoneOS 2.0])])

# UINavigationController — required for the master/detail push navigation
AC_MSG_CHECKING([for UINavigationController])
AC_COMPILE_IFELSE(
    [AC_LANG_PROGRAM(
        [#import <UIKit/UIKit.h>],
        [UINavigationController *nc =
             [[UINavigationController alloc] initWithRootViewController:nil];
         (void)nc;])],
    [AC_MSG_RESULT([yes])],
    [AC_MSG_FAILURE([UINavigationController not available])])

# UISwitch — used in PSTableCell for toggle rows
AC_MSG_CHECKING([for UISwitch])
AC_COMPILE_IFELSE(
    [AC_LANG_PROGRAM(
        [#import <UIKit/UIKit.h>],
        [UISwitch *sw = [[UISwitch alloc] initWithFrame:CGRectZero]; (void)sw;])],
    [AC_MSG_RESULT([yes])],
    [AC_MSG_FAILURE([UISwitch not available])])

# UIActionSheet — used in PSGeneralController for destructive confirmations
AC_MSG_CHECKING([for UIActionSheet])
AC_COMPILE_IFELSE(
    [AC_LANG_PROGRAM(
        [#import <UIKit/UIKit.h>],
        [UIActionSheet *as = [[UIActionSheet alloc]
              initWithTitle:nil delegate:nil
              cancelButtonTitle:nil
              destructiveButtonTitle:nil
              otherButtonTitles:nil]; (void)as;])],
    [AC_MSG_RESULT([yes])],
    [AC_MSG_WARN([UIActionSheet not available — reset confirmations will not work])])

AC_LANG_POP([Objective C])

# ------------------------------------------------------------------
# Optional: check for NSClassFromString (dynamic controller dispatch)
# ------------------------------------------------------------------

AC_LANG_PUSH([Objective C])

AC_MSG_CHECKING([for NSClassFromString (runtime class lookup)])
AC_COMPILE_IFELSE(
    [AC_LANG_PROGRAM(
        [#import <Foundation/Foundation.h>],
        [Class c = NSClassFromString(@"NSObject"); (void)c;])],
    [AC_MSG_RESULT([yes])
     AC_DEFINE([HAVE_NSCLASSFROMSTRING], [1],
               [Define if NSClassFromString is available])],
    [AC_MSG_WARN([NSClassFromString not available — link navigation will be disabled])])

AC_LANG_POP([Objective C])

# ------------------------------------------------------------------
# NSUserDefaults — preference persistence backend
# ------------------------------------------------------------------

AC_LANG_PUSH([Objective C])

AC_MSG_CHECKING([for NSUserDefaults])
AC_COMPILE_IFELSE(
    [AC_LANG_PROGRAM(
        [#import <Foundation/Foundation.h>],
        [[[NSUserDefaults standardUserDefaults] synchronize];])],
    [AC_MSG_RESULT([yes])],
    [AC_MSG_WARN([NSUserDefaults may not function correctly on PocketDarwin])])

AC_LANG_POP([Objective C])

# ------------------------------------------------------------------
# Installation paths
# ------------------------------------------------------------------

if test "$prefix" = NONE; then
    prefix="/Applications"
fi

AC_SUBST([BUNDLE_NAME],    [Settings.app])
AC_SUBST([BUNDLE_INSTALL], ['$(prefix)/Settings.app'])

# ------------------------------------------------------------------
# Output
# ------------------------------------------------------------------

AC_CONFIG_FILES([Makefile])

AC_OUTPUT

AC_MSG_NOTICE([
-------------------------------------------------------
  Settings.app — PocketDarwin — configuration summary
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
