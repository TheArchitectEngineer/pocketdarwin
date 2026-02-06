#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface MCMobileEnvironmentInfo : NSObject

@property (nonatomic, readonly) NSString *externalStorageRoot;
@property (nonatomic, readonly) NSString *mobileEnvironmentRoot;
@property (nonatomic, readonly) BOOL internalStorageAllowed;

@property (nonatomic, readonly) uint64_t uartBase;
@property (nonatomic, readonly) uint64_t uartSize;
@property (nonatomic, readonly) int uartIRQ;

@property (nonatomic, readonly) uint64_t fbBase;
@property (nonatomic, readonly) uint64_t fbSize;
@property (nonatomic, readonly) int fbWidth;
@property (nonatomic, readonly) int fbHeight;
@property (nonatomic, readonly) int fbStride;

+ (nullable instancetype)currentInfo;

@end

NS_ASSUME_NONNULL_END
