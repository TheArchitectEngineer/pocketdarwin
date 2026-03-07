#ifndef COREOS_FD_COMPATIBILITY_H
#define COREOS_FD_COMPATIBILITY_H

/*
 * Shared compatibility import for FreeDarwin CoreOS.
 *
 * Prefer Apple's Foundation when available, and fall back to FoundationKit
 * for FreeDarwin / GNUstep-like environments.
 */
#if __has_include(<Foundation/Foundation.h>)
#import <Foundation/Foundation.h>
#elif __has_include(<FoundationKit/FoundationKit.h>)
#import <FoundationKit/FoundationKit.h>
#else
#error "CoreOS requires either <Foundation/Foundation.h> or <FoundationKit/FoundationKit.h>."
#endif

#endif /* COREOS_FD_COMPATIBILITY_H */
