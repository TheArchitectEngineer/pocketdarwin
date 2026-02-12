# Frameworks.cmake
# PocketDarwin Frameworks Configuration
# This file defines all available frameworks for building PocketDarwin applications

cmake_minimum_required(VERSION 3.16)

# PocketDarwin Frameworks Directory
set(PD_FRAMEWORKS_DIR "${CMAKE_CURRENT_LIST_DIR}")

# Available Frameworks
set(PD_AVAILABLE_FRAMEWORKS
    GnuCoreFoundation
    L2DIOBridgeKit  
    LinuxKit
    pdUIKit
)

# Framework Information
set(PD_FRAMEWORK_GNUCOREFOUNDATION
    NAME "GnuCoreFoundation"
    DESCRIPTION "GNUStep Core Foundation compatibility layer"
    PATH "${PD_FRAMEWORKS_DIR}/GnuCoreFoundation"
    TYPE "Foundation"
    DEPENDS ""
    PUBLIC_HEADERS "Headers"
    SOURCES "Source"
    LIBRARIES "gnustep-base"
    INCLUDE_DIRS "Headers"
    VERSION "1.0.0"
)

set(PD_FRAMEWORK_L2DIOBRIDGEKIT
    NAME "L2DIOBridgeKit"
    DESCRIPTION "Level 2 I/O Bridge Kit for hardware abstraction"
    PATH "${PD_FRAMEWORKS_DIR}/L2DIOBridgeKit"
    TYPE "Hardware"
    DEPENDS "GnuCoreFoundation"
    PUBLIC_HEADERS "DarwinKit"
    SOURCES "DarwinKit;Libraries"
    LIBRARIES "GnuCoreFoundation"
    INCLUDE_DIRS "DarwinKit"
    VERSION "1.0.0"
)

set(PD_FRAMEWORK_LINUXKIT
    NAME "LinuxKit"
    DESCRIPTION "Linux compatibility layer for PocketDarwin"
    PATH "${PD_FRAMEWORKS_DIR}/LinuxKit"
    TYPE "Platform"
    DEPENDS "GnuCoreFoundation"
    PUBLIC_HEADERS ""
    SOURCES ""
    LIBRARIES "GnuCoreFoundation"
    INCLUDE_DIRS ""
    VERSION "1.0.0"
)

set(PD_FRAMEWORK_PDUKIT
    NAME "pdUIKit"
    DESCRIPTION "Touch-oriented UIKit adaptation of GNUStep AppKit"
    PATH "${PD_FRAMEWORKS_DIR}/pdUIKit"
    TYPE "UI"
    DEPENDS "GnuCoreFoundation"
    PUBLIC_HEADERS "pdUIKit"
    SOURCES "pdUIKit"
    LIBRARIES "GnuCoreFoundation;gnustep-gui"
    INCLUDE_DIRS "pdUIKit"
    VERSION "1.0.0"
)

# Framework Types
set(PD_FRAMEWORK_TYPES
    Foundation  # Core foundation classes
    Hardware     # Hardware abstraction
    Platform     # Platform compatibility
    UI           # User interface
)

# Helper Functions

# Function to add a framework to a target
function(pd_add_framework TARGET FRAMEWORK_NAME)
    if(NOT PD_FRAMEWORK_${FRAMEWORK_NAME})
        message(FATAL_ERROR "Unknown framework: ${FRAMEWORK_NAME}")
    endif()
    
    # Get framework info
    set(FRAMEWORK_INFO ${PD_FRAMEWORK_${FRAMEWORK_NAME}})
    
    # Add include directories
    if(PD_FRAMEWORK_${FRAMEWORK_NAME}_INCLUDE_DIRS)
        foreach(INCLUDE_DIR ${PD_FRAMEWORK_${FRAMEWORK_NAME}_INCLUDE_DIRS})
            target_include_directories(${TARGET} PRIVATE 
                "${PD_FRAMEWORK_${FRAMEWORK_NAME}_PATH}/${INCLUDE_DIR}")
        endforeach()
    endif()
    
    # Add library dependencies
    if(PD_FRAMEWORK_${FRAMEWORK_NAME}_LIBRARIES)
        target_link_libraries(${TARGET} PRIVATE ${PD_FRAMEWORK_${FRAMEWORK_NAME}_LIBRARIES})
    endif()
    
    # Add framework dependencies
    if(PD_FRAMEWORK_${FRAMEWORK_NAME}_DEPENDS)
        foreach(DEP ${PD_FRAMEWORK_${FRAMEWORK_NAME}_DEPENDS})
            pd_add_framework(${TARGET} ${DEP})
        endforeach()
    endif()
    
    message(STATUS "Added framework ${FRAMEWORK_NAME} to ${TARGET}")
endfunction()

# Function to create a framework target
function(pd_create_framework FRAMEWORK_NAME)
    if(NOT PD_FRAMEWORK_${FRAMEWORK_NAME})
        message(FATAL_ERROR "Unknown framework: ${FRAMEWORK_NAME}")
    endif()
    
    set(FRAMEWORK_PATH "${PD_FRAMEWORK_${FRAMEWORK_NAME}_PATH}")
    set(FRAMEWORK_SOURCES "")
    set(FRAMEWORK_HEADERS "")
    
    # Collect source files
    if(PD_FRAMEWORK_${FRAMEWORK_NAME}_SOURCES)
        foreach(SOURCE_DIR ${PD_FRAMEWORK_${FRAMEWORK_NAME}_SOURCES})
            file(GLOB_RECURSE SOURCE_FILES "${FRAMEWORK_PATH}/${SOURCE_DIR}/*.m" "${FRAMEWORK_PATH}/${SOURCE_DIR}/*.c")
            list(APPEND FRAMEWORK_SOURCES ${SOURCE_FILES})
        endforeach()
    endif()
    
    # Collect header files
    if(PD_FRAMEWORK_${FRAMEWORK_NAME}_PUBLIC_HEADERS)
        foreach(HEADER_DIR ${PD_FRAMEWORK_${FRAMEWORK_NAME}_PUBLIC_HEADERS})
            file(GLOB_RECURSE HEADER_FILES "${FRAMEWORK_PATH}/${HEADER_DIR}/*.h")
            list(APPEND FRAMEWORK_HEADERS ${HEADER_FILES})
        endforeach()
    endif()
    
    # Create framework target
    add_library(${FRAMEWORK_NAME} 
        SHARED
        ${FRAMEWORK_SOURCES}
    )
    
    # Set framework properties
    set_target_properties(${FRAMEWORK_NAME} PROPERTIES
        FRAMEWORK TRUE
        VERSION ${PD_FRAMEWORK_${FRAMEWORK_NAME}_VERSION}
        SOVERSION ${PD_FRAMEWORK_${FRAMEWORK_NAME}_VERSION}
        PUBLIC_HEADER "${FRAMEWORK_HEADERS}"
        OUTPUT_NAME ${FRAMEWORK_NAME}
    )
    
    # Install framework
    install(TARGETS ${FRAMEWORK_NAME}
        FRAMEWORK
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
    
    # Install headers
    if(FRAMEWORK_HEADERS)
        install(FILES ${FRAMEWORK_HEADERS}
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${FRAMEWORK_NAME}
        )
    endif()
    
    message(STATUS "Created framework target: ${FRAMEWORK_NAME}")
endfunction()

# Function to find and link frameworks
function(pd_find_and_link_frameworks TARGET)
    set(FRAMEWORKS_TO_LINK ${ARGN})
    
    foreach(FRAMEWORK_NAME ${FRAMEWORKS_TO_LINK})
        if(PD_FRAMEWORK_${FRAMEWORK_NAME})
            pd_add_framework(${TARGET} ${FRAMEWORK_NAME})
        else()
            message(WARNING "Framework ${FRAMEWORK_NAME} not found, skipping...")
        endif()
    endforeach()
endfunction()

# Function to validate framework dependencies
function(pd_validate_framework_dependencies FRAMEWORK_NAME)
    if(NOT PD_FRAMEWORK_${FRAMEWORK_NAME})
        message(FATAL_ERROR "Unknown framework: ${FRAMEWORK_NAME}")
    endif()
    
    set(DEPS ${PD_FRAMEWORK_${FRAMEWORK_NAME}_DEPENDS})
    foreach(DEP ${DEPS})
        if(NOT PD_FRAMEWORK_${DEP})
            message(FATAL_ERROR "Framework ${FRAMEWORK_NAME} depends on unknown framework: ${DEP}")
        endif()
    endforeach()
endfunction()

# Function to print framework information
function(pd_print_framework_info FRAMEWORK_NAME)
    if(NOT PD_FRAMEWORK_${FRAMEWORK_NAME})
        message(FATAL_ERROR "Unknown framework: ${FRAMEWORK_NAME}")
    endif()
    
    message(STATUS "")
    message(STATUS "Framework: ${PD_FRAMEWORK_${FRAMEWORK_NAME}_NAME}")
    message(STATUS "  Description: ${PD_FRAMEWORK_${FRAMEWORK_NAME}_DESCRIPTION}")
    message(STATUS "  Path: ${PD_FRAMEWORK_${FRAMEWORK_NAME}_PATH}")
    message(STATUS "  Type: ${PD_FRAMEWORK_${FRAMEWORK_NAME}_TYPE}")
    message(STATUS "  Version: ${PD_FRAMEWORK_${FRAMEWORK_NAME}_VERSION}")
    message(STATUS "  Dependencies: ${PD_FRAMEWORK_${FRAMEWORK_NAME}_DEPENDS}")
    message(STATUS "")
endfunction()

# Function to list all available frameworks
function(pd_list_frameworks)
    message(STATUS "Available PocketDarwin Frameworks:")
    message(STATUS "")
    
    foreach(FRAMEWORK_NAME ${PD_AVAILABLE_FRAMEWORKS})
        pd_print_framework_info(${FRAMEWORK_NAME})
    endforeach()
endfunction()

# Function to validate all frameworks
function(pd_validate_all_frameworks)
    message(STATUS "Validating framework dependencies...")
    
    foreach(FRAMEWORK_NAME ${PD_AVAILABLE_FRAMEWORKS})
        pd_validate_framework_dependencies(${FRAMEWORK_NAME})
    endforeach()
    
    message(STATUS "All framework dependencies are valid!")
endfunction()

# Default configuration
option(PD_BUILD_ALL_FRAMEWORKS "Build all available frameworks" OFF)
option(PD_VALIDATE_FRAMEWORKS "Validate framework dependencies" ON)

# Auto-validation
if(PD_VALIDATE_FRAMEWORKS)
    pd_validate_all_frameworks()
endif()

# Build all frameworks if requested
if(PD_BUILD_ALL_FRAMEWORKS)
    message(STATUS "Building all PocketDarwin frameworks...")
    foreach(FRAMEWORK_NAME ${PD_AVAILABLE_FRAMEWORKS})
        if(EXISTS "${PD_FRAMEWORK_${FRAMEWORK_NAME}_PATH}")
            pd_create_framework(${FRAMEWORK_NAME})
        else()
            message(WARNING "Framework path does not exist: ${PD_FRAMEWORK_${FRAMEWORK_NAME}_PATH}")
        endif()
    endforeach()
endif()

# Export functions
export(pd_add_framework)
export(pd_create_framework)
export(pd_find_and_link_frameworks)
export(pd_validate_framework_dependencies)
export(pd_print_framework_info)
export(pd_list_frameworks)
export(pd_validate_all_frameworks)

# Convenience variables for external projects
set(PD_FRAMEWORKS_AVAILABLE ${PD_AVAILABLE_FRAMEWORKS})
set(PD_FRAMEWORKS_FOUND TRUE)

message(STATUS "PocketDarwin Frameworks configuration loaded")
message(STATUS "Available frameworks: ${PD_AVAILABLE_FRAMEWORKS}")