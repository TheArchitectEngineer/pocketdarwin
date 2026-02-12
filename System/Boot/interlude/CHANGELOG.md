# CHANGELOG.md for Interlude Boot System
# Version history and changes

## [1.0.0] - 2026-02-12

### Added
- **Autotools Build System**: Complete migration from Make to autotools
- **configure.ac**: Autoconf configuration with feature detection
- **Makefile.am**: Automake configuration with conditional compilation
- **libfdt Support**: Optional device tree handling with libfdt library
- **Platform Detection**: Automatic Darwin/Linux platform detection
- **Feature Configuration**: Configurable build options and dependencies
- **Installation System**: Proper install/uninstall targets
- **Development Tools**: Debug, release, and test build targets
- **Documentation**: Comprehensive README and build instructions

### Features
- **Boot Management**: Core PocketDarwin boot process handling
- **Device Tree Support**: Enhanced device tree parsing and processing
- **JSON Configuration**: Flexible JSON-based configuration system
- **VMApple Integration**: Complete VMApple virtualization support
- **Linux Bridge**: Robust Linux-to-Darwin compatibility layer
- **Debug Tools**: Device tree dumping and analysis utilities

### Technical Details
- **Autoconf 2.69+**: Modern autotools configuration
- **Automake 1.13+**: Advanced build system features
- **Libtool Support**: Optional shared library building
- **Cross-Platform**: Darwin and Linux platform support
- **Conditional Compilation**: Optional feature selection
- **Dependency Management**: Automatic library detection and linking

### Build Targets
- **Standard**: all, clean, install, uninstall, distclean
- **Development**: debug, release, test, check-syntax, help
- **Configuration**: --with-libfdt, --enable-debug, --enable-release
- **Installation**: Custom prefix and directory configuration

### Programs
- **darwin_interlude**: Main boot manager (always built)
- **vmapple**: VMApple tools (always built)
- **l2d_dump**: Device tree tool (requires libfdt)

### Dependencies
- **Required**: GCC/Clang, standard C library
- **Optional**: libfdt (for enhanced device tree support)
- **Build**: autoconf, automake, libtool

### Installation
- **Default**: /usr/local/sbin for system binaries
- **Configurable**: Custom prefix and directory structure
- **Package**: Distribution tarball generation support

### Documentation
- **README.md**: Comprehensive usage and build instructions
- **CHANGELOG.md**: Version history and changes
- **configure --help**: Available configuration options
- **make help**: Build target descriptions

### Migration from Make
- **Compatibility**: Maintains original Makefile functionality
- **Enhancement**: Adds autotools features and flexibility
- **Portability**: Improved cross-platform support
- **Maintenance**: Easier dependency and configuration management

### Quality Improvements
- **Error Handling**: Better error detection and reporting
- **Validation**: Configuration and dependency validation
- **Testing**: Built-in test targets and syntax checking
- **Debugging**: Enhanced debug build options

### Future Enhancements
- **Package Management**: Native package creation
- **Integration**: Better PocketDarwin system integration
- **Performance**: Optimized build configurations
- **Features**: Additional boot system capabilities
