# PocketDarwin Frameworks

This directory contains all the core frameworks for building PocketDarwin applications. The `Frameworks.cmake` file provides a unified CMake configuration for building with any combination of frameworks.

## Available Frameworks

### **GnuCoreFoundation**
- **Type**: Foundation
- **Description**: GNUStep Core Foundation compatibility layer
- **Dependencies**: None
- **Usage**: Core data types, collections, file I/O, networking
- **Headers**: `Headers/`
- **Sources**: `Source/`
- **Libraries**: `gnustep-base`

### **L2DIOBridgeKit**
- **Type**: Hardware
- **Description**: Level 2 I/O Bridge Kit for hardware abstraction
- **Dependencies**: GnuCoreFoundation
- **Usage**: Hardware access, device I/O, system integration
- **Headers**: `DarwinKit/`
- **Sources**: `DarwinKit/`, `Libraries/`
- **Libraries**: `GnuCoreFoundation`

### **LinuxKit**
- **Type**: Platform
- **Description**: Linux compatibility layer for PocketDarwin
- **Dependencies**: GnuCoreFoundation
- **Usage**: Linux system calls, compatibility layer
- **Status**: Empty (placeholder for future Linux support)

### **pdUIKit**
- **Type**: UI
- **Description**: Touch-oriented UIKit adaptation of GNUStep AppKit
- **Dependencies**: GnuCoreFoundation
- **Usage**: User interface, touch handling, view management
- **Headers**: `pdUIKit/`
- **Sources**: `pdUIKit/`
- **Libraries**: `GnuCoreFoundation`, `gnustep-gui`

## Building with CMake

### **Basic Usage**

```cmake
cmake_minimum_required(VERSION 3.16)
project(MyPocketDarwinApp)

# Include frameworks configuration
include(${CMAKE_CURRENT_SOURCE_DIR}/Frameworks/Frameworks.cmake)

# Add executable
add_executable(MyApp main.m)

# Add framework (automatically handles dependencies)
pd_add_framework(MyApp pdUIKit)
```

### **Multiple Frameworks**

```cmake
# Add multiple frameworks
pd_find_and_link_frameworks(MyApp 
    GnuCoreFoundation
    L2DIOBridgeKit
    pdUIKit
)
```

### **Building All Frameworks**

```cmake
# Build all available frameworks
option(BUILD_ALL_FRAMEWORKS "Build all PocketDarwin frameworks" ON)

if(BUILD_ALL_FRAMEWORKS)
    pd_create_framework(GnuCoreFoundation)
    pd_create_framework(L2DIOBridgeKit)
    pd_create_framework(pdUIKit)
endif()
```

## Framework Functions

### **pd_add_framework(TARGET FRAMEWORK_NAME)**
Adds a single framework to a target, automatically handling:
- Include directories
- Library linking
- Framework dependencies

### **pd_create_framework(FRAMEWORK_NAME)**
Creates a build target for the framework:
- Collects source files
- Sets up headers
- Configures build properties
- Handles installation

### **pd_find_and_link_frameworks(TARGET FRAMEWORK1 FRAMEWORK2 ...)**
Adds multiple frameworks to a target with validation.

### **pd_list_frameworks()**
Prints information about all available frameworks.

### **pd_validate_framework_dependencies(FRAMEWORK_NAME)**
Validates that all dependencies of a framework are available.

## Configuration Options

### **PD_BUILD_ALL_FRAMEWORKS**
Build all available frameworks when set to ON.

### **PD_VALIDATE_FRAMEWORKS**
Validate framework dependencies during configuration (default: ON).

## Example Projects

### **pdUIKit Application**
```objc
#import <pdUIKit/UIKit.h>

int main(int argc, char * argv[]) {
    @autoreleasepool {
        UIApplication *app = [UIApplication sharedApplication];
        
        UIViewController *rootVC = [[UIViewController alloc] init];
        UIWindow *window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
        window.rootViewController = rootVC;
        [window makeKeyAndVisible];
        
        app.window = window;
        [app run];
    }
    return 0;
}
```

### **Foundation Application**
```objc
#import <GnuCoreFoundation/GnuCoreFoundation.h>

int main(int argc, char * argv[]) {
    @autoreleasepool {
        // Use Foundation classes
        NSString *path = @"/tmp/test.txt";
        NSString *content = @"Hello, PocketDarwin!";
        
        [content writeToFile:path atomically:YES encoding:NSUTF8StringEncoding error:nil];
        
        NSLog(@"File written to: %@", path);
    }
    return 0;
}
```

### **Hardware Application**
```objc
#import <L2DIOBridgeKit/L2DIOBridgeKit.h>

int main(int argc, char * argv[]) {
    @autoreleasepool {
        // Access hardware through L2DIOBridgeKit
        DeviceInfo *info = [L2DIOBridgeKit getDeviceInfo];
        
        NSLog(@"Device: %@", info.model);
        NSLog(@"Architecture: %@", info.architecture);
        NSLog(@"Memory: %ld MB", info.totalMemory / (1024*1024));
    }
    return 0;
}
```

## Building

### **Prerequisites**
- CMake 3.16 or later
- GNUStep base libraries
- Xcode or compatible compiler
- PocketDarwin SDK

### **Build Commands**

```bash
# Configure build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

# Build specific target
cmake --build build --target MyApp

# Build all frameworks
cmake --build build --target all

# Install
cmake --install build
```

### **Building Individual Frameworks**

```bash
# Build only pdUIKit
cd pdUIKit
xcodebuild -project pdUIKit.xcodeproj -scheme pdUIKit

# Build only GnuCoreFoundation
cd GnuCoreFoundation
./configure && make && make install
```

## Framework Development

### **Adding New Frameworks**

1. Create framework directory in `System/Frameworks/`
2. Add framework to `PD_AVAILABLE_FRAMEWORKS` in `Frameworks.cmake`
3. Define `PD_FRAMEWORK_<NAME>` variables
4. Implement framework with headers and sources
5. Update documentation

### **Framework Structure**
```
FrameworkName/
├── Headers/           # Public headers
├── Source/            # Implementation files
├── Tests/             # Unit tests
├── Documentation/      # API documentation
├── Examples/          # Usage examples
├── FrameworkName.xcodeproj  # Xcode project
└── README.md          # Framework-specific docs
```

## Installation

### **System Installation**
```bash
# Install all frameworks
sudo make -C /System/Frameworks install

# Install specific framework
sudo make -C /System/Frameworks/FrameworkName install
```

### **Development Installation**
```bash
# Install to development directory
cmake -S . -B build -DCMAKE_INSTALL_PREFIX=~/pocketdarwin-dev
cmake --install build
```

## Troubleshooting

### **Common Issues**

1. **Framework not found**: Ensure framework path exists and is defined in `Frameworks.cmake`
2. **Missing dependencies**: Check that all required frameworks are available
3. **Build failures**: Verify GNUStep environment is properly configured
4. **Link errors**: Ensure library dependencies are correctly specified

### **Getting Help**

```bash
# List available frameworks
cmake -P Frameworks.cmake -DPD_LIST_FRAMEWORKS=ON

# Validate dependencies
cmake -P Frameworks.cmake -DPD_VALIDATE_FRAMEWORKS=ON

# Get framework info
cmake -P Frameworks.cmake -DPD_PRINT_FRAMEWORK=pdUIKit
```

## Contributing

When adding new frameworks or modifying existing ones:

1. Follow the established naming conventions
2. Update `Frameworks.cmake` with new framework information
3. Add comprehensive documentation
4. Include example usage
5. Test with all supported platforms

## License

Each framework may have its own license. Refer to individual framework documentation for specific licensing terms.
