//
//  FrameModel.h
//  InnoMakerUSB2CAN
//
//  Created by Inno-Maker on 2020/4/6.
//  Copyright © 2020 Inno-Maker. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

NS_ASSUME_NONNULL_BEGIN

@interface FrameViewModel : NSObject

@property (nonatomic,copy) NSString *seqID;

@property (nonatomic,copy) NSString *systemTime;

@property (nonatomic,copy) NSString *timeIdentifier;

@property (nonatomic,copy) NSString *canChannel;

@property (nonatomic,copy) NSString *direction;

@property (nonatomic,copy) NSString *identifier;

@property (nonatomic,copy) NSString *frameType;

@property (nonatomic,copy) NSString *frameFormat;

@property (nonatomic,copy) NSString *length;

@property (nonatomic,copy) NSString *data;

@property (nonatomic,strong) NSColor *color;

@property (nonatomic,assign) BOOL isDataValid;

@property (nonatomic,copy) NSString *messge;

- (NSString *)presentString;

@end

NS_ASSUME_NONNULL_END
