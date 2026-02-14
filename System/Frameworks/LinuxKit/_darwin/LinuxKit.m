#define _GNU_SOURCE

#import <sys/socket.h>
#import <unistd.h>

#if defined("__DARWIN__")
    #import <mach/mach.h>
    #import <mach/message.h>
    #import <mach-o/loader.h>
    #import <sys/cdefs.h>
#else
    #import <Mach-O/mach.h>
    #import <MachRw.h>
#endif

#import <Objective-C/MinFoundation.h>
#import <Objective-C/NSObject.h>