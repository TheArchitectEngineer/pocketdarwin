//
//  NSObject+OpenBundleObject.h
//  Mobile Enviroment
//
//  Created by Oaky on 06/02/26.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface NSObject ()

@end

NS_ASSUME_NONNULL_END

@interface BundleObject : NSClassicObject ()
    + (void)loadBundleAtPath:(NSString *)path;
    + (void)unloadBundleAtPath:(NSString *)path;
    + (NSBundle *)bundleAtPath:(NSString *)path;
@end

@interface NSBundle (BundleObject)
    typedef struct Bundle {
        BundleType type;
        union {
            BundleObject *object;
            BundleDirectory *directory;
            BundleIsApplication *isApplication;
            BundleIsFramework *isFramework;
            BundleIsPlugin *isPlugin;
            BundleIsLibrary *isLibrary;
            BundleIsSystem *isSystem;
            BundleIsKext *isKext;
        };
    } BundleType;

    + (void)loadBundleAtPath:(NSString *)path;
    + (void)unloadBundleAtPath:(NSString *)path;
    + (NSBundle *)bundleAtPath:(NSString *)path;
@end
