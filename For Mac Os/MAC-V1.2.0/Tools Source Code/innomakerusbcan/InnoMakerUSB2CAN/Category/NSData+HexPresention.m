//
//  NSData+HexPresention.m
//  InnoMakerUSB2CAN
//
//  Created by SUGAR Dev on 2020/4/13.
//  Copyright © 2020 Inno-Maker. All rights reserved.
//

#import "NSData+HexPresention.h"

 
@implementation NSData (HexRepresentation)

- (NSString *)hexString {
    const unsigned char *bytes = (const unsigned char *)self.bytes;
    NSMutableString *hex = [NSMutableString new];
    [hex appendString:@"0x|"];
    for (NSInteger i = 0; i < self.length; i++) {
        [hex appendFormat:@"%02x ", bytes[i]];
    }
    return [hex copy];
}
@end
 
