 #import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

/**
Setting
 */
@interface DAConfig : NSObject
/**
 User Custom language , if nil equal to resetSystemLanguage
 */
@property (class, nonatomic, strong, nullable) NSString *userLanguage;
/**
 Reset system language
 */
+ (void)resetSystemLanguage;

@end

NS_ASSUME_NONNULL_END
