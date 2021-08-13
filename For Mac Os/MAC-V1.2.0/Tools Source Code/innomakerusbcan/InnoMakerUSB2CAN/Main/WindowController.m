//
//  WindowController.m
//  InnoMakerUSB2CAN
//
//  Created by SUGAR Dev on 2020/4/26.
//  Copyright © 2020 Yanci. All rights reserved.
//

#import "WindowController.h"

@interface WindowController ()<NSWindowDelegate>

@end

@implementation WindowController

- (void)windowDidLoad {
    [super windowDidLoad];
    
    // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
}

- (BOOL)windowShouldClose:(NSWindow *)sender {
    [NSApp hide:nil];
    return false;
}

@end
