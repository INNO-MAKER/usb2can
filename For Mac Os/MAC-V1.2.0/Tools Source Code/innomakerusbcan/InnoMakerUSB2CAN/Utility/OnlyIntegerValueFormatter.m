//
//  OnlyIntegerValueFormatter.m
//  InnoMakerUSB2CAN
//
//  Created by SUGAR Dev on 2020/4/13.
//  Copyright © 2020 Inno-Maker. All rights reserved.
//

#import "OnlyIntegerValueFormatter.h"



@implementation OnlyIntegerValueFormatter

- (instancetype)init
{
    self = [super init];
    if (self) {
        _minValue = 0;
        _maxValue = 100;
    }
    return self;
}

- (BOOL)isPartialStringValid:(NSString*)partialString newEditingString:(NSString**)newString errorDescription:(NSString**)error
{
    if([partialString length] == 0) {
        return YES;
    }
    
    NSScanner* scanner = [NSScanner scannerWithString:partialString];
    if(!([scanner scanInt:0] && [scanner isAtEnd])) {
        return NO;
    }
    
    int value = partialString.intValue;
    if (value < _minValue || value > _maxValue) {
        return NO;
    }
    
    return YES;
}

@end
