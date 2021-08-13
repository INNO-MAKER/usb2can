//
//  OnlyIntegerValueFormatter.h
//  InnoMakerUSB2CAN
//
//  Created by SUGAR Dev on 2020/4/13.
//  Copyright © 2020 Inno-Maker. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

/// OnlyIntegerValueFormatter
/// Support minValue , maxValue , Only Number
@interface OnlyIntegerValueFormatter : NSNumberFormatter
@property (nonatomic,assign)int minValue;
@property (nonatomic,assign)int maxValue;
@end


NS_ASSUME_NONNULL_END
