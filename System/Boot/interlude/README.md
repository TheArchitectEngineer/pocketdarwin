# README.md for Interlude Boot System
# PocketDarwin Interlude Boot Manager

## Overview

Interlude is the boot system for PocketDarwin, providing a bridge between Linux boot processes and Darwin/macOS compatibility. It handles device tree parsing, configuration management, and system initialization.

## Features

### **Core Functionality**
- **Boot Management**: Handles PocketDarwin boot process
- **Device Tree Support**: Parses and processes device tree data
- **JSON Configuration**: Manages system configuration via JSON
- **VMApple Integration**: Supports VMApple virtualization
- **Linux Bridge**: Provides Linux-to-Darwin compatibility layer

### **Optional Features**
- **libfdt Support**: Enhanced device tree handling with libfdt
- **Debug Tools**: Device tree dumping and analysis tools
- **Configuration Validation**: JSON schema validation

## Building with Autotools

### **Prerequisites**
- GCC or Clang compiler
- Autoconf 2.69+
- Automake 1.13+
- libtool (optional)
- libfdt (optional, for device tree support)

### **Basic Build**

```bash
# Generate configure script
autoreconf -fiv

# Configure build
./configure

# Build
make

# Install (optional)
sudo make install
```

### **Build Options**

#### **With libfdt Support**
```bash
./configure --with-libfdt
make
```

#### **Debug Build**
```bash
./configure --enable-debug
make debug
```

#### **Optimized Build**
```bash
./configure --enable-release
make release
```

#### **Custom Installation**
```bash
./configure --prefix=/usr/local --sbindir=/usr/local/sbin
make
sudo make install
```

### **Configuration Options**

| Option | Description | Default |
|--------|-------------|---------|
| `--with-libfdt` | Enable libfdt device tree support | no |
| `--enable-debug` | Enable debug symbols | no |
| `--enable-release` | Enable optimization | no |
| `--prefix=PREFIX` | Installation prefix | /usr/local |
| `--sbindir=DIR` | System binary directory | PREFIX/sbin |

## Build Targets

### **Standard Targets**
```bash
make              # Build all programs
make clean        # Remove build artifacts
make install      # Install programs
make uninstall    # Remove installed programs
make distclean    # Remove all generated files
```

### **Development Targets**
```bash
make debug        # Build with debug flags
make release      # Build optimized
make test         # Run basic tests
make check-syntax # Check syntax only
make help         # Show available targets
```

## Programs

### **darwin_interlude**
Main interlude boot manager binary.

**Usage:**
```bash
darwin_interlude [options]
```

**Options:**
- `--config PATH`: Path to configuration file
- `--serial PATH`: Path to serial device
- `--help`: Show help message
- `--version`: Show version information

### **vmapple**
VMApple virtualization support tools.

**Usage:**
```bash
vmapple [options] command
```

### **l2d_dump** (requires libfdt)
Device tree dumping and analysis tool.

**Usage:**
```bash
l2d_dump [options] device-tree-file
```

## Configuration

### **Default Configuration**
- **Config Path**: `/sdcard/OCMobileConfig.json`
- **Serial Device**: `/dev/ttyACM0`

### **Configuration Format**
```json
{
  "boot": {
    "timeout": 10,
    "default": "pocketdarwin"
  },
  "devices": {
    "serial": "/dev/ttyACM0",
    "storage": "/sdcard"
  },
  "debug": {
    "enabled": false,
    "level": "info"
  }
}
```

## Development

### **Project Structure**
```
interlude/
├── configure.ac          # Autoconf configuration
├── Makefile.am          # Automake configuration
├── darwin_interlude.c   # Main boot manager
├── JSONParser.c/.h      # JSON parsing utilities
├── InterludeIO.c/.h     # I/O utilities
├── Linux2DarwinBridge.cxx/.h  # Linux-Darwin bridge
├── l2d_dump.cpp         # Device tree tool
└── vmapple/             # VMApple tools
    ├── vmapple.c
    ├── vmapple-select.c
    └── vmapple.h
```

### **Adding New Features**
1. Add source files to appropriate `Makefile.am` variables
2. Update `configure.ac` for new dependencies
3. Add tests to the `test` target
4. Update documentation

### **Debugging**
```bash
# Enable debug build
./configure --enable-debug CFLAGS="-g -O0"

# Run with debug output
./darwin_interlude --debug --verbose

# Use debugger
gdb ./darwin_interlude
```

## Troubleshooting

### **Common Issues**

#### **Configure Fails**
```bash
# Check for missing dependencies
autoreconf --version
automake --version
libtoolize --version

# Install missing tools (macOS)
brew install autoconf automake libtool

# Install missing tools (Linux)
sudo apt-get install autoconf automake libtool
```

#### **Build Fails**
```bash
# Check compiler
gcc --version
clang --version

# Clean and rebuild
make distclean
./configure
make
```

#### **libfdt Issues**
```bash
# Install libfdt (macOS)
brew install libfdt

# Install libfdt (Linux)
sudo apt-get install libfdt-dev

# Verify installation
pkg-config --exists libfdt
```

### **Getting Help**
```bash
# Show configuration summary
./configure --help

# Show build targets
make help

# Check build log
cat config.log
```

## Installation

### **System Installation**
```bash
./configure --prefix=/usr
make
sudo make install
```

### **User Installation**
```bash
./configure --prefix=$HOME/.local
make
make install
```

### **Package Installation**
```bash
# Create distribution
make dist

# Install from package
tar xf interlude-1.0.0.tar.gz
cd interlude-1.0.0
./configure
make
sudo make install
```

## License

This project is part of PocketDarwin and follows the same licensing terms as the main project.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests if applicable
5. Update documentation
6. Submit a pull request

## Support

For support and questions:
- Check the troubleshooting section
- Review the build logs
- Contact the PocketDarwin team
