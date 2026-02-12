bin_PROGRAMS = freeboard system_info_example
lib_LTLIBRARIES = libfreeboard.la

freeboard_SOURCES = main.cpp AppScanner.mm FreeboardAPI.cpp
freeboard_CPPFLAGS = $(libX11_CFLAGS) -I$(srcdir)
freeboard_LDADD = $(libX11_LIBS) libfreeboard.la
freeboard_LDFLAGS = -framework Foundation
freeboard_OBJCXXFLAGS = $(libX11_CFLAGS) -I$(srcdir)

# Library for the API
libfreeboard_la_SOURCES = FreeboardAPI.cpp AppScanner.mm
libfreeboard_la_CPPFLAGS = $(libX11_CFLAGS) -I$(srcdir)
libfreeboard_la_LDFLAGS = -framework Foundation -version-info 1:0:0

# Example program
system_info_example_SOURCES = system_info_example.c
system_info_example_CPPFLAGS = -I$(srcdir)
system_info_example_LDADD = libfreeboard.la
system_info_example_LDFLAGS = -framework Foundation
