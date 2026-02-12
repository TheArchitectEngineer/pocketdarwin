# configure.ac for interlude boot system
# Autoconf configuration for PocketDarwin Interlude

AC_INIT([interlude], [1.0.0], [pocketdarwin@example.com])
AC_CONFIG_SRCDIR([darwin_interlude.c])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign subdir-objects -Wall -Werror])
AM_SILENT_RULES([yes])

# Check for programs
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_MAKE_SET
LT_INIT

# Check for headers
AC_CHECK_HEADERS([stdlib.h string.h unistd.h sys/stat.h sys/types.h sys/utsname.h sys/wait.h errno.h])

# Check for types
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

# Check for functions
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset strdup strerror])

# Optional libfdt support
AC_ARG_WITH([libfdt],
            [AS_HELP_STRING([--with-libfdt],
                            [Enable libfdt support for device tree handling])],
            [],
            [with_libfdt=no])

AS_IF([test "x$with_libfdt" != "xno"],
      [AC_CHECK_HEADER([libfdt.h],
                       [AC_CHECK_LIB([fdt], [fdt_first_property_offset],
                                     [LIBFDT_LIBS="-lfdt"
                                      with_libfdt=yes],
                                     [with_libfdt=no
                                      AC_MSG_ERROR([libfdt library not found])],
                                     [$LIBFDT_LIBS])],
                       [with_libfdt=no
                        AC_MSG_ERROR([libfdt headers not found])])])

# Set variables based on libfdt support
AM_CONDITIONAL([L2D_USE_LIBFDT], [test "x$with_libfdt" = "xyes"])

AS_IF([test "x$with_libfdt" = "xyes"],
      [AC_DEFINE([L2D_USE_LIBFDT], [1], [Use libfdt for device tree handling])
       INTERLUDE_CXX_SOURCES="Linux2DarwinBridge.cxx"
       TOOLS="l2d_dump"],
      [INTERLUDE_CXX_SOURCES=""
       TOOLS=""])

# Substitute variables
AC_SUBST([INTERLUDE_CXX_SOURCES])
AC_SUBST([TOOLS])
AC_SUBST([LIBFDT_LIBS])

# Check for platform-specific features
AC_CANONICAL_HOST

# Set platform-specific flags
case $host_os in
    darwin*)
        CFLAGS="$CFLAGS -D_DARWIN_C_SOURCE"
        CXXFLAGS="$CXXFLAGS -D_DARWIN_C_SOURCE"
        ;;
    linux*)
        CFLAGS="$CFLAGS -D_GNU_SOURCE"
        CXXFLAGS="$CXXFLAGS -D_GNU_SOURCE"
        ;;
    *)
        ;;
esac

# Default compiler flags
if test -z "$CFLAGS"; then
    CFLAGS="-O2 -Wall -Wextra -std=c11"
fi

if test -z "$CXXFLAGS"; then
    CXXFLAGS="-O2 -Wall -Wextra -std=c++17"
fi

# Output files
AC_CONFIG_FILES([
    Makefile
    build-aux/Makefile
])

# Summary
AC_MSG_NOTICE([Interlude Boot System Configuration:])
AC_MSG_NOTICE([  libfdt support: $with_libfdt])
AC_MSG_NOTICE([  Additional tools: $TOOLS])
AC_MSG_NOTICE([  Host: $host])
AC_MSG_NOTICE([  CFLAGS: $CFLAGS])
AC_MSG_NOTICE([  CXXFLAGS: $CXXFLAGS])

AC_OUTPUT