//
//  ViewController.h
//  InnoMakerUSB2CAN
//
//  Created by Inno-Maker on 2020/4/3.
//  Copyright © 2020 Inno-Maker. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface ViewController : NSViewController
@property (assign) IBOutlet NSButton *openDeviceBtn;
@property (assign) IBOutlet NSButton *singleSendBtn;
@property (assign) IBOutlet NSButton *mutleSendBtn;
@property (assign) IBOutlet NSButton *scanDeviceBtn;

@property (assign) IBOutlet NSButton *clearBtn;
@property (assign) IBOutlet NSComboBox *devComboBox;
@property (assign) IBOutlet NSComboBox *bitrateComboBox;
@property (assign) IBOutlet NSComboBox *langComboBox;
@property (assign) IBOutlet NSComboBox *workModeComboBox;
@property (assign) IBOutlet NSTextField *langTextField;
@property (assign) IBOutlet NSTextField *frameIdTextField;
@property (assign) IBOutlet NSTextField *frameDataTextField;
@property (assign) IBOutlet NSTextField *totalFrameTextField;
@property (assign) IBOutlet NSTextField *periodTextField;
@property (assign) IBOutlet NSTableView *tableView;
@property (assign) IBOutlet NSTextField *logoInfo;
@property (assign) IBOutlet NSComboBox *frameFormatComboBox;
@property (assign) IBOutlet NSComboBox *frameTypeComboBox;

- (IBAction)controlSetFrameFormat:(id)sender;
- (IBAction)controlSetFrameType:(id)sender;
- (IBAction)controlSetWorkMode:(id)sender;
- (IBAction)controlCheckId:(id)sender;
- (IBAction)controlCheckData:(id)sender;
- (IBAction)controlErrorFrame:(id)sender;
- (IBAction)controlSend:(id)sender;
- (IBAction)controlSingleSend:(id)sender;
- (IBAction)clear:(id)sender;
- (IBAction)scanDevices:(id)sender;
- (IBAction)changeLanguage:(id)sender;
- (IBAction)exportFile:(id)sender;
- (IBAction)openDevice:(id)sender;
@end

