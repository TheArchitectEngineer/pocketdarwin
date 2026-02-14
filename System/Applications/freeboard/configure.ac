AC_INIT([freeboard], [0.2.0])
AM_INIT_AUTOMAKE([foreign subdir-objects])
LT_INIT
AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_HEADERS([config.h])

# Checks for programs
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

# Check for OpenGL
case $host_os in
  darwin*)
    # macOS - set OpenGL frameworks
    OPENGL_LIBS="-framework OpenGL -framework GLUT"
    ;;
  *)
    # Linux/Unix - check for GL/GLX
    AC_CHECK_LIB([GL], [glBegin], [OPENGL_LIBS="-lGL"], 
                 [AC_MSG_ERROR([OpenGL library not found])])
    AC_CHECK_LIB([GLU], [gluPerspective], [OPENGL_LIBS="$OPENGL_LIBS -lGLU"], 
                 [AC_MSG_ERROR([GLU library not found])])
    ;;
esac

AC_SUBST([OPENGL_LIBS])

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

