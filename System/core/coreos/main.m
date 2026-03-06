/* 
        This CoreOS userland implementations relies on
        GNUStep's FoundationKit (works as well with Apple's FoundationKit).

        If you don't have an Objective-C Compiler read the docs for more info.

*/


#define _GNU_SOURCE

#import <Foundation/Foundation.h>
#include <Foundation/NSObjCRuntime.h>
#import "include/SystemHeader.h"

int main() {
    NSLog(@"CoreOS booting on %@", [FDSystemTypeIdentifier currentPlatformName]);
    return 0;
}
