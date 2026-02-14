# FreeBoard Autotools Integration

## Overview

FreeBoard now uses GNU autotools for build system configuration, providing cross-platform compatibility and proper dependency management.

## Build System Structure

### Autotools Files
- `configure.ac` - Autoconf configuration script
- `Makefile.am` - Automake makefile template
- `autogen.sh` - Bootstrap script
- `m4/` - Directory for custom macros
- `config.h.in` - Generated configuration header template

### Generated Files
- `configure` - Generated configuration script
- `Makefile` - Generated makefile
- `config.h` - Generated configuration header
- `libtool` - Generated libtool script

## Quick Start

### Bootstrap (for developers)
```bash
./autogen.sh
```

### Configure and Build
```bash
./configure
make
```

### Install
```bash
make install
```

### Clean
```bash
make clean      # Remove built files
make distclean  # Remove all generated files
```

## Build Targets

### Available Programs
- `freeboard` - Original iOS 5/6 style launcher
- `freeboard_new` - Modern OpenGL-based launcher
- `system_info_example` - System information example

### Build Specific Targets
```bash
make freeboard          # Build original freeboard only
make freeboard_new      # Build modern freeboard only
make all                # Build all targets (default)
```

## Dependencies

### Required Libraries
- **X11** - Window system (checked via pkg-config)
- **OpenGL** - Graphics rendering
- **GLUT** - OpenGL utilities (macOS only)
- **Foundation** - macOS framework

### Build Tools
- **autoconf** >= 2.69
- **automake** >= 1.14
- **libtool** >= 2.4
- **pkg-config**
- **C++17 compatible compiler**

## Configuration Options

### Standard Options
```bash
./configure --help
```

### Common Options
- `--prefix=PREFIX` - Installation directory (default: /usr/local)
- `--enable-debug` - Enable debug symbols
- `--disable-shared` - Build static libraries only

### Platform-Specific
The configure script automatically detects the platform and configures appropriate OpenGL libraries:

**macOS:**
```bash
OPENGL_LIBS="-framework OpenGL -framework GLUT"
```

**Linux/Unix:**
```bash
OPENGL_LIBS="-lGL -lGLU"
```

## Cross-Platform Support

### macOS
- Uses OpenGL frameworks
- Supports both X11 and native macOS
- Handles glibtoolize vs libtoolize

### Linux/Unix
- Uses standard OpenGL libraries
- Requires X11 development headers
- Standard libtool

### Build Detection
```bash
# Check for OpenGL
case $host_os in
  darwin*)
    OPENGL_LIBS="-framework OpenGL -framework GLUT"
    ;;
  *)
    AC_CHECK_LIB([GL], [glBegin], [OPENGL_LIBS="-lGL"])
    AC_CHECK_LIB([GLU], [gluPerspective], [OPENGL_LIBS="$OPENGL_LIBS -lGLU"])
    ;;
esac
```

## Development Workflow

### Initial Setup
```bash
# Clone or checkout the repository
cd freeboard

# Bootstrap the build system
./autogen.sh

# Configure for development
./configure --enable-debug --prefix=/usr/local

# Build
make

# Test
./freeboard_new --help
```

### Making Changes
1. Edit source files
2. Run `make` to rebuild
3. Test changes
4. Commit changes

### Release Process
```bash
# Create distribution package
make dist

# Create signed distribution
make distcheck

# Install system-wide
sudo make install
```

## Troubleshooting

### Common Issues

#### Missing Build Tools
```bash
# macOS
brew install autoconf automake libtool pkg-config

# Ubuntu/Debian
sudo apt-get install autoconf automake libtool pkg-config

# CentOS/RHEL
sudo yum install autoconf automake libtool pkg-config
```

#### Missing OpenGL Headers
```bash
# macOS
xcode-select --install

# Ubuntu/Debian
sudo apt-get install libgl1-mesa-dev libglu1-mesa-dev

# CentOS/RHEL
sudo yum install mesa-libGL-devel mesa-libGLU-devel
```

#### Missing X11 Headers
```bash
# Ubuntu/Debian
sudo apt-get install libx11-dev libxext-dev

# CentOS/RHEL
sudo yum install libX11-devel libXext-devel
```

### Build Failures

#### Objective-C++ Compilation Issues
If you encounter issues with the original freeboard (Objective-C++), build only the modern version:
```bash
make freeboard_new
```

#### C++17 Support
The configure script checks for C++17 support. If compilation fails:
```bash
# Ensure you have a C++17 compatible compiler
g++ --version

# Or specify compiler explicitly
./configure CXX=g++-9
```

## File Structure After Build

```
freeboard/
├── configure           # Generated configure script
├── Makefile           # Generated makefile
├── config.h           # Generated config header
├── libtool            # Generated libtool script
├── .deps/             # Dependency files
├── .libs/             # Libtool libraries
├── freeboard          # Original executable
├── freeboard_new      # Modern executable
├── libfreeboard.la    # Shared library
├── system_info_example # Example program
└── m4/                # Autoconf macros
    ├── ax_cxx_compile_stdcxx.m4
    ├── libtool.m4
    ├── ltoptions.m4
    ├── ltsugar.m4
    ├── ltversion.m4
    └── lt~obsolete.m4
```

## Integration with PocketDarwin

The autotools system integrates seamlessly with the PocketDarwin build environment:

### Dependencies
- Uses existing X11 configuration from PocketDarwin
- Compatible with PocketDarwin's OpenGL setup
- Maintains original freeboard API compatibility

### Installation
```bash
# Install to PocketDarwin prefix
./configure --prefix=/opt/PocketDarwin
make install
```

### Packaging
The autotools system supports creating distribution packages:
```bash
# Create tar.gz package
make dist

# Create signed package
make distcheck
```

## Performance Considerations

### Build Optimization
- Uses `-O2` optimization by default
- Parallel builds supported: `make -j$(nproc)`
- Debug builds available with `--enable-debug`

### Runtime Performance
- Modern freeboard_new uses hardware-accelerated OpenGL
- Efficient texture management
- Optimized rendering pipeline

## Future Enhancements

### Potential Improvements
1. **OpenGL ES Support** - For embedded systems
2. **Wayland Support** - Next-generation display server
3. **Static Linking Options** - For portable binaries
4. **Cross-Compilation** - Build for multiple architectures

### Configuration Extensions
```bash
# Future options might include:
./configure --enable-opengl-es --enable-wayland --with-static-linking
```

## Conclusion

The autotools integration provides FreeBoard with:
- ✅ **Cross-platform compatibility**
- ✅ **Proper dependency management**
- ✅ **Standard build procedures**
- ✅ **Easy installation and distribution**
- ✅ **Development-friendly workflow**

This modern build system ensures FreeBoard can be easily built, installed, and maintained across different platforms while maintaining compatibility with the PocketDarwin ecosystem.
