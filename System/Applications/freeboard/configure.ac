AC_INIT([freeboard], [0.1.0a])
AM_INIT_AUTOMAKE([foreign])
AC_PROG_CC
AC_PROG_CXX
AC_PROG_OBJCXX
AC_LANG_PUSH([C])
AC_COMPILE_IFELSE(
  [AC_LANG_PROGRAM([], [[
     return 0;
  ]])],
  [],
  [AC_MSG_ERROR([C compiler does not work])]
)
AC_LANG_POP([C])
PKG_PROG_PKG_CONFIG

PKG_CHECK_MODULES([libX11], [x11], [], [AC_MSG_ERROR([libX11 not found])])

AC_LANG_PUSH([Objective C])
AC_COMPILE_IFELSE(
  [AC_LANG_PROGRAM([], [[
     id obj = 0;
     return obj ? 0 : 0;
  ]])],
  [],
  [AC_MSG_ERROR([Objective-C compiler does not work])]
)
# check for sys/socket.h
AC_LANG_PUSH([C])
AC_CHECK_HEADERS([sys/socket.h], [], [AC_MSG_ERROR([sys/socket.h not found])])
AC_LANG_POP([C])
AC_LANG_PUSH([C++])
AC_COMPILE_IFELSE(
  [AC_LANG_PROGRAM([], [[
     return 0;
  ]])],
  [],
  [AC_MSG_ERROR([C++ compiler does not work])]
)
AC_LANG_POP([C++])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT

