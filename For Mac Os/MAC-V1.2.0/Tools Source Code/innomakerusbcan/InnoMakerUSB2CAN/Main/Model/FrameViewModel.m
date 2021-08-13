//
//  FrameModel.m
//  InnoMakerUSB2CAN
//
//  Created by Inno-Maker on 2020/4/6.
//  Copyright © 2020 Inno-Maker. All rights reserved.
//

#import "FrameViewModel.h"

@implementation FrameViewModel

- (NSString *)presentString {
    NSMutableString *mString = [NSMutableString string];
    [mString appendFormat:@"seqID:%@ ",_seqID];
    [mString appendFormat:@"systemTime:%@ ",_systemTime];
    [mString appendFormat:@"timeIdentifier:%@ ",_timeIdentifier];
    [mString appendFormat:@"canChannel:%@ ",_canChannel];
    [mString appendFormat:@"direction:%@ ",_direction];
    [mString appendFormat:@"identifier:%@ ",_identifier];
    [mString appendFormat:@"frameType:%@ ",_frameType];
    [mString appendFormat:@"frameFormat:%@ ",_frameFormat];
    [mString appendFormat:@"length:%@ ",_length];
    [mString appendFormat:@"data:%@ ",_data];
    return mString;
}


@end
