bin_PROGRAMS = freeboard freeboard_new freeboard_fb freeboard_lk system_info_example
lib_LTLIBRARIES = libfreeboard.la

# Original freeboard (iOS 5/6 style)
freeboard_SOURCES = main.cpp AppScanner.mm FreeboardAPI.cpp
freeboard_CPPFLAGS = $(libX11_CFLAGS) -I$(srcdir)
freeboard_LDADD = $(libX11_LIBS) libfreeboard.la
freeboard_LDFLAGS = -framework Foundation
freeboard_OBJCXXFLAGS = $(libX11_CFLAGS) -I$(srcdir)

# New freeboard with modern OpenGL UI
freeboard_new_SOURCES = main_new.cpp FreeBoardUI.cpp
freeboard_new_CPPFLAGS = $(libX11_CFLAGS) -I$(srcdir) -DSTB_IMAGE_IMPLEMENTATION
freeboard_new_LDADD = $(libX11_LIBS) $(OPENGL_LIBS) $(GLUT_LIBS) -lm
freeboard_new_LDFLAGS = 
freeboard_new_CXXFLAGS = -std=c++17 -Wall -Wextra

# Framebuffer-based freeboard using LinuxKit (original)
freeboard_fb_SOURCES = freeboard_framebuffer.c
freeboard_fb_CPPFLAGS = -I$(srcdir)/../Frameworks/LinuxKit
freeboard_fb_LDADD = ../Frameworks/LinuxKit/liblinuxkit.la -lm
freeboard_fb_CFLAGS = -Wall -Wextra -std=c99

# Real LinuxKit-based freeboard with device framebuffer access
freeboard_lk_SOURCES = freeboard_linuxkit.c
freeboard_lk_CPPFLAGS = -I$(srcdir)/../Frameworks/LinuxKit
freeboard_lk_LDADD = ../Frameworks/LinuxKit/liblinuxkit.la -lm
freeboard_lk_CFLAGS = -Wall -Wextra -std=c99

# Library for the API
libfreeboard_la_SOURCES = FreeboardAPI.cpp AppScanner.mm
libfreeboard_la_CPPFLAGS = $(libX11_CFLAGS) -I$(srcdir)
libfreeboard_la_LDFLAGS = -framework Foundation -version-info 1:0:0
libfreeboard_la_OBJCXXFLAGS = $(libX11_CFLAGS) -I$(srcdir)

# Example program
system_info_example_SOURCES = system_info_example.c
system_info_example_CPPFLAGS = -I$(srcdir)
system_info_example_LDADD = libfreeboard.la
system_info_example_LDFLAGS = -framework Foundation
