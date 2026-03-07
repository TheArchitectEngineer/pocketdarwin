#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

extern NSString * const FDStartupErrorDomain;

@interface FDStartup : NSObject {
    NSString *_displayName;
    NSString *_bundleIdentifier;
    NSString *_bundlePath;
    NSString *_executablePath;
    UIImage *_icon;
}

@property (nonatomic, copy) NSString *displayName;
@property (nonatomic, copy) NSString *bundleIdentifier;
@property (nonatomic, copy) NSString *bundlePath;
@property (nonatomic, copy) NSString *executablePath;
@property (nonatomic, retain) UIImage *icon;

- (id)initWithBundlePath:(NSString *)bundlePath;

+ (NSArray *)startupBundlesFromConfiguration:(NSString *)configurationPath;
- (BOOL)launchAndReturnError:(NSError **)error;

@end
