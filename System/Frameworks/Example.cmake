# Example CMakeLists.txt for building with PocketDarwin Frameworks
# This demonstrates how to use the Frameworks.cmake file

cmake_minimum_required(VERSION 3.16)
project(PocketDarwinExample)

# Include the frameworks configuration
include(${CMAKE_CURRENT_SOURCE_DIR}/../Frameworks/Frameworks.cmake)

# Example 1: Using pdUIKit for a touch-oriented application
add_executable(pdUIKitExample
    examples/pdUIKit_example.m
)

# Add pdUIKit framework (automatically includes dependencies)
pd_add_framework(pdUIKitExample pdUIKit)

# Example 2: Using GnuCoreFoundation for foundation services
add_executable(foundationExample
    examples/foundation_example.m
)

# Add GnuCoreFoundation framework
pd_add_framework(foundationExample GnuCoreFoundation)

# Example 3: Using L2DIOBridgeKit for hardware access
add_executable(hardwareExample
    examples/hardware_example.m
)

# Add L2DIOBridgeKit framework
pd_add_framework(hardwareExample L2DIOBridgeKit)

# Example 4: Using multiple frameworks
add_executable(complexExample
    examples/complex_example.m
)

# Add multiple frameworks
pd_find_and_link_frameworks(complexExample 
    GnuCoreFoundation
    L2DIOBridgeKit
    pdUIKit
)

# Example 5: Building all frameworks
option(BUILD_ALL_FRAMEWORKS "Build all PocketDarwin frameworks" OFF)
if(BUILD_ALL_FRAMEWORKS)
    # This will build all available frameworks
    pd_create_framework(GnuCoreFoundation)
    pd_create_framework(L2DIOBridgeKit)
    pd_create_framework(pdUIKit)
    # LinuxKit is empty, so we skip it
endif()

# Print available frameworks
pd_list_frameworks()

# Installation
install(TARGETS pdUIKitExample foundationExample hardwareExample complexExample
    RUNTIME DESTINATION bin
)
