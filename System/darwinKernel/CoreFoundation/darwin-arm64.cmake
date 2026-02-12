set(CMAKE_SYSTEM_NAME Darwin)
set(CMAKE_SYSTEM_PROCESSOR arm64)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang)

set(CMAKE_C_FLAGS "--target=arm64-apple-darwin")
set(CMAKE_CXX_FLAGS "--target=arm64-apple-darwin")

set(CMAKE_SYSROOT /Volumes/FileSystemMac/Developer/Projects/PocketDarwin/System/darwinKernel/sysroot)

set(CMAKE_FIND_ROOT_PATH /Volumes/FileSystemMac/Developer/Projects/PocketDarwin/System/darwinKernel/sysroot)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
