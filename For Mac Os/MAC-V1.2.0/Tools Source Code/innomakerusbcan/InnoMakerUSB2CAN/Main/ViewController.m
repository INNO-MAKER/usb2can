//
//  ViewController.m
//  InnoMakerUSB2CAN
//
//  Created by Inno-Maker on 2020/4/3.
//  Copyright © 2020 Inno-Maker. All rights reserved.
//

#import "ViewController.h"
#import "UsbIO.h"
#import "USBIO+USBCAN.h"
#import "FrameViewModel.h"
#import "NSData+HexPresention.h"
#import "DAConfig.h"
#import "NSBundle+DAUtils.h"



typedef enum _FrameFormat {
    FrameFormatStandard = 0,
    FrameFormatExtend = 1
} FrameFormat;

/// Frame Type
typedef enum _FrameType {
    FrameTypeData = 0,
    FrameTypeRemote = 1
} FrameType;

/// Can Mode
typedef enum _CanMode{
    CanModeNormal = 0
}CanMode;

/// Bitrate
typedef enum _Bitrate{
    BitrateTenMillionsBit = 0
}Bitrate ;

/// Check ID
typedef enum _CheckIDType{
    CheckIDTypeNone = 0,
    CheckIDTypeIncrease = 1
}CheckIDType;

/// Check data
typedef enum _CheckDataType {
    CheckDataTypeNone = 0,
    CheckDataTypeIncrease = 1
}CheckDataType;

/// ErrorFrame flag
typedef enum _CheckErrorFrame {
    _CheckErrorFrameClose = 0,
    _CheckErrorFrameOpen = 1
}CheckErrorFrame;

/// System language
typedef enum _SystemLanguage {
    DefaultLanguage = 0,
    EnglishLanguage = 1,
    ChineseLanguage = 2
}SystemLanguage;

@interface ViewController()<InnoMakerDeviceDelegate,
NSTextFieldDelegate,
NSTableViewDataSource,
NSTableViewDelegate,
NSComboBoxDataSource,
NSComboBoxDelegate>
@property (nonatomic,assign) FrameFormat frameFormat;
@property (nonatomic,assign) FrameType frameType;
@property (nonatomic,assign) CanMode canMode;
@property (nonatomic,assign) Bitrate bitrate;
@property (nonatomic,assign) CheckIDType checkIdType;
@property (nonatomic,assign) CheckDataType checkDataType;
@property (nonatomic,assign) CheckErrorFrame checkErrorFrame;
@property (nonatomic,strong) UsbIO *usbIO;
@property (nonatomic,strong) NSMutableArray *devIndexes;
@property (nonatomic,strong) NSMutableArray *bitrateIndexes;
@property (nonatomic,strong) NSMutableArray <NSData *>*recvFrame;
@property (nonatomic,strong) NSMutableArray <FrameViewModel *>*recvFrameCopy;
@property (nonatomic,assign) OSSpinLock recvFrameLock;
@property (nonatomic,assign) SystemLanguage currentLanguage;
/// Recv timer
@property (nonatomic,strong) NSTimer *recvTimer;
/// Send timer
@property (nonatomic,strong) dispatch_source_t sendTimer;
/// Current device
@property (nonatomic,strong) InnoMakerDevice *currentDevice;
/// Current select bitrate
@property (nonatomic,assign) int curBitrateSelectIndex;
/// Current work mode
@property (nonatomic,assign) int curWorkModeSelectIndex;
/// Current device
@property (nonatomic,assign) struct gs_can can;
/// Recv thread
@property (nonatomic,strong) NSThread *recvThread;
/// Send thread
@property (nonatomic,strong) NSThread *sendThread;
@property (nonatomic,strong) NSThread *workerThread;
@property (nonatomic,assign) BOOL sendThreadExit;
@property (nonatomic,assign) BOOL recvThreadExit;
@property (nonatomic,assign) BOOL workerThreadExit;
@property (nonatomic,assign) NSUInteger numberSended;
@property (nonatomic,assign) NSUInteger numberNeedToSend;
@property (nonatomic,assign) int txNum;
@property (nonatomic,assign) int rxNum ;
@property (nonatomic,assign) int rxErrorNum;

@end

@implementation ViewController

#pragma mark - life cycle
- (void)viewDidLoad {
    [super viewDidLoad];
    
    // Do any additional setup after loading the view.
    _logoInfo.stringValue = @"www.inno-maker.com  wiki.inno-maker.com  sales@inno-maker.com  support@inno-maker.com";
    
    /// Default standard frame format
    _frameFormat = FrameFormatStandard;
    /// Default data frame type
    _frameType = FrameTypeData;
    /// Default normal mode
    _canMode = CanModeNormal;
    /// Default 10MBits
    _bitrate = BitrateTenMillionsBit;
    /// Default check id none
    _checkIdType = CheckIDTypeNone;
    /// Default check data none
    _checkDataType = CheckDataTypeNone;
    /// Default check error close
    _checkErrorFrame = _CheckErrorFrameClose;
    
    /// Default one frame
    _totalFrameTextField.stringValue = @"1";
    /// Default one second
    _periodTextField.stringValue = @"1000";
    _recvFrame = [NSMutableArray array];
    _recvFrameCopy = [NSMutableArray array];
    _curBitrateSelectIndex = -1;
    _curWorkModeSelectIndex = -1;
    
    /// Init USBIO Instance
    _usbIO = [[UsbIO alloc]init];
    [_usbIO setDelegate:self];
    
    /// Default frame id ,frame data
    _frameIdTextField.stringValue = @"00 00 00 00";
    _frameDataTextField.stringValue = @"00 00 00 00 00 00 00 00";
    
    /// Init Dev Indexes
    _devIndexes = [[NSMutableArray alloc]init];
    _bitrateIndexes = [NSMutableArray arrayWithObjects:
                       @"20K",
                       @"33.33K",
                       @"40K",
                       @"50K",
                       @"66.66K",
                       @"80K",
                       @"83.33K",
                       @"100K",
                       @"125K",
                       @"200K",
                       @"250K",
                       @"400K",
                       @"500K",
                       @"666K",
                       @"800K",
                       @"1000K",
                       nil];
    
    
    _bitrateComboBox.stringValue = @"";
    _bitrateComboBox.editable = false;
    _workModeComboBox.stringValue = @"";
    _workModeComboBox.editable = false;
    _devComboBox.stringValue = @"";
    _devComboBox.editable = false;
    _langComboBox.editable = false;
    _tableView.dataSource = self;
    _tableView.delegate = self;
    
    
    _openDeviceBtn.title = NSLocalizedStringFromTable(@"open-device", @"Main", @"open-device");
    _openDeviceBtn.tag = 0;
    _mutleSendBtn.tag = 0;
    _frameFormatComboBox.enabled = true;
    _frameTypeComboBox.enabled = true;
    /// Read local language
    if ([DAConfig userLanguage] == nil) {
        _currentLanguage = DefaultLanguage;
        _langComboBox.stringValue = @"English";
        _langTextField.stringValue = @"Language";
    } else if([[DAConfig userLanguage] isEqualToString:@"zh-Hans"]) {
        _currentLanguage = ChineseLanguage;
        _langComboBox.stringValue = @"Chinese";
        _langTextField.stringValue = @"语言";
    } else if([[DAConfig userLanguage] isEqualToString:@"en"]){
        _currentLanguage = EnglishLanguage;
        _langComboBox.stringValue = @"English";
        _langTextField.stringValue = @"Language";
    }
    
    _txNum = 0;
    _rxNum = 0;
    _rxErrorNum = 0;
    
}


- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];
    
    // Update the view, if already loaded.
}


#pragma mark - datasource

- (NSInteger)numberOfItemsInComboBox:(NSComboBox *)aComboBox
{
    if (aComboBox == _devComboBox) {
        return [_devIndexes count];
    }
    if (aComboBox == _bitrateComboBox) {
        return [_bitrateIndexes count];
    }
    return 0;
}

- (id)comboBox:(NSComboBox *)aComboBox objectValueForItemAtIndex:(NSInteger)index
{
    if (aComboBox == _devComboBox) {
        return [_devIndexes objectAtIndex:index];
    }
    if (aComboBox == _bitrateComboBox) {
        return [_bitrateIndexes objectAtIndex:index];
    }
    return @"";
}

- (void)comboBoxSelectionDidChange:(NSNotification *)notification {
    NSComboBox *comboBox = (NSComboBox *)notification.object;
    if (comboBox == _langComboBox) {
        if (comboBox.indexOfSelectedItem == 0) {
            [self chanageCurrentLanguage:EnglishLanguage];
        } else {
            [self chanageCurrentLanguage:ChineseLanguage];
        }
    }
    if (comboBox == _devComboBox) {
        _currentDevice = [_usbIO getInnoMakerDevice:[comboBox indexOfSelectedItem]];
        if (_currentDevice == nil) {
            NSLog(@"Device is null!");
        }
    }
    if (comboBox == _bitrateComboBox) {
        _curBitrateSelectIndex =  (int)[comboBox indexOfSelectedItem];
    }
    if (comboBox == _workModeComboBox) {
        _curWorkModeSelectIndex = (int)[comboBox indexOfSelectedItem];
    }
    if (comboBox == _frameFormatComboBox) {
        if ([comboBox indexOfSelectedItem] == 0) {
            _frameFormat = FrameFormatStandard;
        } else {
            _frameFormat = FrameFormatExtend;
        }
    }
    if (comboBox == _frameTypeComboBox) {
        if ([comboBox indexOfSelectedItem] == 0) {
            _frameType = FrameTypeData;
        } else {
            _frameType = FrameTypeRemote;
        }
    }
}
#pragma mark - delegate
- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView {
    return _recvFrameCopy.count;
}

- (NSView *)tableView:(NSTableView *)tableView viewForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row {
    FrameViewModel *model = [_recvFrameCopy objectAtIndex:row];
    NSTableCellView *cellView = [self configColumnWithTable:tableView model:model column:tableColumn];
    return cellView;
}

- (void)controlTextDidBeginEditing:(NSNotification *)obj {
    
}

- (void)controlTextDidChange:(NSNotification *)obj {
    
    if (obj.object == _frameIdTextField) {
        /// Limit Four Byte
        [self adjustTextFieldToHex:obj.object limitLength:8 needSpacePadding:true];
    }
    else if (obj.object == _frameDataTextField) {
        /// Limite Eight Bytes
        [self adjustTextFieldToHex:obj.object limitLength:16 needSpacePadding:true];
    }
    else if (obj.object == _totalFrameTextField) {
        [self adjustTextFieldToNumber:obj.object minValue:1 maxValue:2000000];
    }
    else if (obj.object == _periodTextField) {
        [self adjustTextFieldToNumber:obj.object minValue:0 maxValue:5000];
    }
    
}

- (void)controlTextDidEndEditing:(NSNotification *)obj {
    if (obj.object == _totalFrameTextField) {
        [self adjustTextFieldToNumber:obj.object minValue:1 maxValue:2000000];
        
    }
    else if (obj.object == _periodTextField) {
        [self adjustTextFieldToNumber:obj.object minValue:0 maxValue:5000];
    }
}


/*
 Add device notify
 */
- (void)addDeviceNotify:(InnoMakerDevice*)device {
    dispatch_async(dispatch_get_main_queue(), ^{
   
        [self updateDevs];
    });
}

/*
 Remove device notify
 */
- (void)removeDeviceNotify:(InnoMakerDevice*)device {
    dispatch_async(dispatch_get_main_queue(), ^{
        [self updateDevs];
        
        if (self.currentDevice == nil ||  self.currentDevice == device) {
            self.devComboBox.stringValue = @"";
            self.bitrateComboBox.stringValue = @"";
            self.curBitrateSelectIndex = -1;
            self.currentDevice = nil;
            [self _closeDevice];
        }
    });
}

- (void)readDeviceDataNotify:(InnoMakerDevice *)device
                        data:(Byte *)dataByte
                      length:(NSUInteger)length {
    
    /// The length must equal to gs_host_frame format, it is the frame to send and recv
    if (length != sizeof(struct innomaker_host_frame)) {
        return;
    }
    
    Byte echodIdByte[4];
    echodIdByte[0] = dataByte[0];
    echodIdByte[1] = dataByte[1];
    echodIdByte[2] = dataByte[2];
    echodIdByte[3] = dataByte[3];
    
    uint32_t echoId = 0;
    memcpy(&echoId, echodIdByte, 4);
    /// NOT RECV BUFFER
    if (echoId != 0xFFFFFFFF) {
        struct gs_tx_context *txc = gs_get_tx_context(&self->_can, echoId);
        ///bad devices send bad echo_ids.
        if (!txc) {
            NSLog(@"Unexpected unused echo id %d\n",echoId);
            return;
        }
        gs_free_tx_context(txc);
    }
    NSData *data = [NSData dataWithBytes:dataByte length:length];
    OSSpinLockLock(&_recvFrameLock);
    [_recvFrame addObject:data];
    OSSpinLockUnlock(&_recvFrameLock);
}

- (void)writeDeviceDataNotify:(InnoMakerDevice *)device
                         data:(Byte *)dataByte
                       length:(NSUInteger)length {
    /// Send Success, Here USB2CAN Use Recv Data To Verfiy If Send
}

#pragma mark - user events

- (IBAction)controlSetFrameFormat:(id)sender {
    NSComboBox *comboBox = (NSComboBox *)sender;
    switch (comboBox.selectedTag) {
        case 0:
            break;
        default:
            break;
    }
}


- (IBAction)controlSetFrameType:(id)sender {
    NSComboBox *comboBox = (NSComboBox *)sender;
    switch (comboBox.selectedTag) {
        case 0:
            break;
        default:
            break;
    }
}


- (IBAction)controlSetWorkMode:(id)sender {
    NSComboBox *comboBox = (NSComboBox *)sender;
    switch (comboBox.selectedTag) {
        case 0:
            break;
        default:
            break;
    }
}

- (IBAction)controlCheckId:(id)sender {
    NSButton *button = (NSButton *)sender;
    if (button.state == NSControlStateValueOn) {
        _checkIdType = CheckIDTypeIncrease;
    }
    else {
        _checkIdType = CheckIDTypeNone;
    }
}

- (IBAction)controlCheckData:(id)sender {
    NSButton *button = (NSButton *)sender;
    if (button.state == NSControlStateValueOn) {
        _checkDataType = CheckDataTypeIncrease;
    }
    else {
        _checkDataType = CheckDataTypeNone;
    }
}

- (IBAction)controlErrorFrame:(id)sender  {
    NSButton *button = (NSButton *)sender;
    if (button.state == NSControlStateValueOn) {
        _checkErrorFrame = _CheckErrorFrameOpen;
    }
    else {
        _checkErrorFrame = _CheckErrorFrameClose;
    }
}

- (IBAction)controlSend:(id)sender {
    
    /// If sending,Stop Send Message
    if (_mutleSendBtn.tag == 1) {
        [self cancelSendThread];
        self.mutleSendBtn.tag = 0;
        self.mutleSendBtn.title = NSLocalizedStringFromTable(@"lzK-xu-4Vr.title",@"Main", @"Delayed Send");
        return;
    }
    /// Start Send message
    /// Get Frame Id
    NSString *frameId = _frameIdTextField.stringValue;
    /// Get Frame Data
    NSString *frameData = _frameDataTextField.stringValue;
    /// Get Total Frame
    int totalFrame = _totalFrameTextField.intValue;
    /// Get Period Time (ms)
    int periodTime = _periodTextField.intValue;
    /// Cancel Timer
    [self cancelSendThread];
    
    /// Check if device selected
    if (_currentDevice == nil || _currentDevice.isOpen == false) {
        [self alertWithMsg:NSLocalizedStringFromTable(@"device-not-open", @"Main", @"device-not-open")];
        return;
    }
    /// Check if set bitrate
    if (_curBitrateSelectIndex == -1) {
        [self alertWithMsg:NSLocalizedStringFromTable(@"baudrate-not-right", @"Main", @"baudrate-not-right")];
        return;
    }
    
    /// Check if set bitrate
    if (_curWorkModeSelectIndex == -1) {
        [self alertWithMsg:NSLocalizedStringFromTable(@"workmode-not-right", @"Main", @"workmode-not-right")];
        return;
    }
    
    /// Invalid frame id
    if (frameId.length == 0)  {
        [self alertWithMsg:NSLocalizedStringFromTable(@"frameid-not-right", @"Main", @"frameid-not-right")];
        return;
    }
    /// Invalid frame data
    if (frameData.length == 0) {
        [self alertWithMsg:NSLocalizedStringFromTable(@"framedata-not-right", @"Main", @"framedata-not-right")];
        return;
    }
    /// Frame Range
    if (totalFrame < 1 || totalFrame > 2000000) {
        [self alertWithMsg:NSLocalizedStringFromTable(@"framerange-not-right", @"Main", @"framerange-not-right")];
        return;
    }
    /// Time Range
    if (periodTime < 0 || periodTime > 5000) {
        [self alertWithMsg:NSLocalizedStringFromTable(@"timerange-not-right", @"Main", @"timerange-not-right")];
        return;
    }
    /// Start Send
    self.mutleSendBtn.tag = 1;
    self.mutleSendBtn.title = NSLocalizedStringFromTable(@"stop-send", @"Main", @"stop-send");
    
    [self setupSendThread:periodTime frameId:frameId frameData:frameData totalFrame:totalFrame];
}

- (IBAction)controlSingleSend:(id)sender {
    
    /// Get Frame Id
    NSString *frameId = _frameIdTextField.stringValue;
    /// Get Frame Data
    NSString *frameData = _frameDataTextField.stringValue;
    
    /// Check if device selected
    if (_currentDevice == nil ||  _currentDevice.isOpen == false) {
        [self alertWithMsg:NSLocalizedStringFromTable(@"device-not-open", @"Main", @"device-not-open")];
        return;
    }
    /// Check if set bitrate
    
    if (_curBitrateSelectIndex == -1) {
        [self alertWithMsg:NSLocalizedStringFromTable(@"baudrate-not-right", @"Main", @"baudrate-not-right")];
        return;
    }
    
    /// Check if set bitrate
    if (_curWorkModeSelectIndex == -1) {
        [self alertWithMsg:NSLocalizedStringFromTable(@"workmode-not-right", @"Main", @"workmode-not-right")];
        return;
    }
    
    /// Invalid frame id
    if (frameId.length == 0)  {
        [self alertWithMsg:NSLocalizedStringFromTable(@"frameid-not-right", @"Main", @"frameid-not-right")];
        return;
    }
    /// Invalid frame data
    if (frameData.length == 0) {
        [self alertWithMsg:NSLocalizedStringFromTable(@"framedata-not-right", @"Main", @"framedata-not-right")];
        return;
    }
    /* find an empty context to keep track of transmission */
    struct gs_tx_context *txc = gs_alloc_tx_context(&self->_can);
    if (!txc) {
        NSLog(@"NETDEV_TX_BUSY");
        [self alertWithMsg:@"发送繁忙 ERROR:[NETDEV_TX_BUSY]"];
        return;
    }
    
    
    NSData *standardFrameData = [self buildStandardFrame:frameId
                                               frameData:frameData
                                                  echoId:txc->echo_id];
    __weak typeof(self)strongSelf = self;
    dispatch_async(dispatch_get_main_queue(), ^{
        [strongSelf.usbIO asyncSendInnoMakerDeviceBuf:strongSelf.currentDevice
                                            andBuffer:(Byte*)standardFrameData.bytes
                                              andSize:(int)standardFrameData.length
                                           andTimeOut:100];
    });
    
    
}

- (IBAction)clear:(id)sender {
    _rxNum = _txNum = _rxErrorNum = 0;
    [self.recvFrameCopy removeAllObjects];
    [_tableView reloadData];
}

- (IBAction)scanDevices:(id)sender {
    _currentDevice = nil;
    _devComboBox.stringValue = @"";
    [_usbIO scanInnoMakerDevices];
    [self updateDevs];
}

- (IBAction)changeLanguage:(id)sender {
    if (_currentLanguage == ChineseLanguage) {
        [self chanageCurrentLanguage:EnglishLanguage];
    } else {
        [self chanageCurrentLanguage:ChineseLanguage];
    }
}

- (IBAction)exportFile:(id)sender {
    [self exportLogFile];
}

- (IBAction)openDevice:(id)sender {
    /// Open Device
    if (_openDeviceBtn.tag == 0) {
        [self _openDevice];
    }
    /// Close Device
    else {
        
        [self _closeDevice];
        
    }
}

- (IBAction)changeToChinese:(id)sender {
    [self chanageCurrentLanguage:ChineseLanguage];
}

- (IBAction)changeToEnglish:(id)sender {
    [self chanageCurrentLanguage:EnglishLanguage];
}
#pragma mark - functions
/*
 Update devices
 */
- (void)updateDevs {
    UInt8 devCount = [_usbIO getInnoMakerDeviceCount];
    // Remove devices
    [_devIndexes removeAllObjects];
    // Fill devices
    InnoMakerDevice *device;
    NSString *devStr;
    
    for(int i = 0; i < devCount ;i++) {
        device = [_usbIO getInnoMakerDevice:i];
        devStr = [NSString stringWithFormat:@"device %d, ID:%x",i,[device deviceID]];
        [_devIndexes addObject:devStr];
    }
    [_devComboBox reloadData];
}

/// Read Dev Data
- (void)inputFromDev{
    InnoMakerDevice *currentDevice =  _currentDevice;
    if (_currentDevice == nil || currentDevice.isOpen == false ) {
        [self cancelRecvThread];
        [self cancelWorkerThread];
        return;
    }
    
    int size = sizeof(struct innomaker_host_frame);
    Byte inputBytes[size];
    UInt32 validSize = size;
    if (kIOReturnSuccess == [_usbIO syncGetInnoMakerDeviceBuf:_currentDevice
                                                     andBuffer:inputBytes
                                                       andSize:validSize
                                                    andTimeOut:INT16_MAX]) {
    } else {
        
    }
}

/// Change current  language
/// @param language language
- (void)chanageCurrentLanguage:(SystemLanguage)language {
    _currentLanguage = language;
    switch (_currentLanguage) {
        case DefaultLanguage:
            [DAConfig resetSystemLanguage];
            break;
        case EnglishLanguage:
            [DAConfig setUserLanguage:@"en"];
            break;
        case ChineseLanguage:
            [DAConfig setUserLanguage:@"zh-Hans"];
            break;;
        default:
            break;
    }
    
    /// Close curren device
    if (_currentDevice.isOpen) {
        [_usbIO closeInnoMakerDevice:_currentDevice];
        _currentDevice = nil;
    }
    /// Cancel recv timer
    [self cancelRecvThread];
    /// Cancel worker thread
    [self cancelWorkerThread];
    /// Cancel send timer
    [self cancelSendThread];
    
    dispatch_async(dispatch_get_main_queue(), ^{
        
        NSViewController *mainViewController = [[NSStoryboard storyboardWithName:@"Main" bundle:[NSBundle mainBundle]] instantiateControllerWithIdentifier:@"MainViewController"];
        [NSApplication sharedApplication].keyWindow.contentViewController =  mainViewController;
    });
}

- (void)_openDevice {
    /// Check if device selected
    if (_currentDevice == nil ) {
        [self alertWithMsg:NSLocalizedStringFromTable(@"device-not-open", @"Main", @"device-not-open")];
        return;
    }
    
    /// Check if set bitrate
    if (_curWorkModeSelectIndex == -1) {
        [self alertWithMsg:NSLocalizedStringFromTable(@"workmode-not-right", @"Main", @"workmode-not-right")];
        return;
    }
    
    /// Check if set bitrate
    if (_curBitrateSelectIndex == -1) {
        [self alertWithMsg:NSLocalizedStringFromTable(@"baudrate-not-right", @"Main", @"baudrate-not-right")];
        return;
    }
    
    [_usbIO openInnoMakerDevice:_currentDevice];
    UsbCanMode usbCanMode = UsbCanModeNormal;
    if (_curWorkModeSelectIndex == 0) {
        usbCanMode = UsbCanModeNormal;
    } else if (_curWorkModeSelectIndex == 1) {
        usbCanMode = UsbCanModeLoopback ;
    } else if (_curWorkModeSelectIndex == 2) {
        usbCanMode = UsbCanModeListenOnly;
    }
    
    [_usbIO UrbSetupDevice:_currentDevice
                      mode:usbCanMode
                 bittiming:[self getBittiming:_curBitrateSelectIndex]];
    
    /// Reset current device tx_context
    for (int i = 0; i < GS_MAX_TX_URBS; i++) {
        _can.tx_context[i].echo_id = GS_MAX_TX_URBS;
    }
    [self setupRecvThread];
    [self setupWorkerThread];
    _openDeviceBtn.tag = 1;
    _openDeviceBtn.title = NSLocalizedStringFromTable(@"close-device", @"Main", @"close-device");
    _bitrateComboBox.enabled = false;
    _devComboBox.enabled = false;
    _workModeComboBox.enabled = false;
    _scanDeviceBtn.enabled = false;
}


- (void)_closeDevice {
    
    [self cancelSendThread];
    [self cancelRecvThread];
    [self cancelWorkerThread];
    if (_currentDevice && _currentDevice.isOpen) {
        
        /// Reset current device tx_context
        for (int i = 0; i < GS_MAX_TX_URBS; i++) {
            _can.tx_context[i].echo_id = GS_MAX_TX_URBS;
        }
        [_usbIO UrbResetDevice:_currentDevice];
        [_usbIO closeInnoMakerDevice:_currentDevice];
    }
    
    _bitrateComboBox.enabled = true;
    _devComboBox.enabled = true;
    _scanDeviceBtn.enabled = true;
    _workModeComboBox.enabled = true;
    _openDeviceBtn.tag = 0;
    _openDeviceBtn.title = NSLocalizedStringFromTable(@"open-device", @"Main", @"open-device");
    self.mutleSendBtn.tag = 0;
    self.mutleSendBtn.title = NSLocalizedStringFromTable(@"lzK-xu-4Vr.title",@"Main", @"Delayed Send");
}

- (void)setupRecvThread {
    [self cancelRecvThread];
    self.recvThread = [[NSThread alloc]initWithTarget:self selector:@selector(recvThreadEntry) object:nil];
    [self.recvThread start];
}

- (void)recvThreadEntry {
    _recvThreadExit = false;
    while (!_recvThreadExit ||
           !NSThread.currentThread.isCancelled) {
        [self inputFromDev];
    }
}

-(void)cancelRecvThread {
    if (self.recvThread != NULL) {
        _recvThreadExit = true;
        [_recvThread cancel];
        _recvThread = NULL;
    }

}

- (void)setupWorkerThread {
    [self cancelWorkerThread];
    
    self.workerThread = [[NSThread alloc] initWithTarget:self selector:@selector(workerThreadEntry) object:nil];
    [self.workerThread start];
}

- (void)workerThreadEntry {
    _workerThreadExit = false;
    while (!_workerThreadExit || !NSThread.currentThread.isCancelled ) {
        
        NSMutableArray *copyFrame;
        if (self.recvFrame.count > 0) {
            OSSpinLockLock(&_recvFrameLock);
            copyFrame = [self.recvFrame mutableCopy];
            [self.recvFrame removeAllObjects];
            OSSpinLockUnlock(&_recvFrameLock);
        }
        
        if (copyFrame.count > 0) {
            dispatch_async(dispatch_get_main_queue(), ^{
                if (self.recvFrameCopy.count == 0) {
                    
                    NSMutableArray *copyFrameModels = [NSMutableArray array];
                    for (int i = 0; i < copyFrame.count; i++) {
                        [copyFrameModels addObject:[self dataToModel:[copyFrame objectAtIndex:i]]];
                    }
                    
                    [self.recvFrameCopy addObjectsFromArray:copyFrameModels];
                    [self.tableView reloadData];
                    if ([self isScrollToBottom] ) {
                        [self.tableView scrollRowToVisible:self.recvFrameCopy.count - 1];
                    }
                } else {
                    
                    NSMutableArray *copyFrameModels = [NSMutableArray array];
                    for (int i = 0; i < copyFrame.count; i++) {
                        [copyFrameModels addObject:[self dataToModel:[copyFrame objectAtIndex:i]]];
                    }
                    NSIndexSet *insertIndexSet = [[NSIndexSet alloc]initWithIndexesInRange:NSMakeRange(self.recvFrameCopy.count, copyFrameModels.count)];
                    [self.recvFrameCopy addObjectsFromArray:copyFrameModels];
                    [self.tableView beginUpdates];
                    [self.tableView insertRowsAtIndexes:insertIndexSet withAnimation:NSTableViewAnimationEffectNone];
                    [self.tableView endUpdates];
                    if ([self isScrollToBottom] ) {
                        [self.tableView scrollRowToVisible:self.recvFrameCopy.count - 1];
                    }
                }
            });
        }
        [NSThread sleepForTimeInterval:0.5];
    }
}

- (void)cancelWorkerThread {
    if (self.workerThread != NULL) {
        [self.workerThread cancel];
        self.workerThread = NULL;
    }
    
    
    OSSpinLockLock(&_recvFrameLock);
    [self.recvFrame removeAllObjects];
    OSSpinLockUnlock(&_recvFrameLock);
}

- (void)setupSendThread:(int)periodTime
                frameId:(NSString *)frameId
              frameData:(NSString *)frameData
             totalFrame:(int)totalFrame {
    [self cancelSendThread];
    self.sendThread = [[NSThread alloc]initWithTarget:self
                                             selector:@selector(sendTheadEntry:)
                                               object:@{@"periodTime":@(periodTime),
                                                        @"totalFrame":@(totalFrame),
                                                        @"frameId":frameId,
                                                        @"frameData":frameData
                                               }];
    [self.sendThread start];
}

- (void)sendTheadEntry:(NSDictionary *)params {
    NSNumber *intervalNum = (NSNumber *)[params objectForKey:@"periodTime"];
    NSNumber *totalFrameNum = (NSNumber *)[params objectForKey:@"totalFrame"];
    NSString *frameId = (NSString *)[params objectForKey:@"frameId"];
    NSString *frameData = (NSString *)[params objectForKey:@"frameData"];
    
    NSMutableString *MFrameId = [NSMutableString stringWithString:frameId];
    NSMutableString *MFrameData = [NSMutableString stringWithString:frameData];
    
    _numberSended = 0;
    _sendThreadExit = false;
    _numberNeedToSend = totalFrameNum.intValue;
    
    while (!_sendThreadExit ||
           !NSThread.currentThread.isCancelled) {
        [self sendToDevSustain:MFrameId frameData:MFrameData];
        [NSThread sleepForTimeInterval:intervalNum.doubleValue / 1000.0];
    }
}

- (void)sendToDevSustain:(NSMutableString *)frameId frameData:(NSMutableString *)frameData {
    struct gs_tx_context *txc = gs_alloc_tx_context(&self->_can);
    if (!txc) {
        NSLog(@"NETDEV_TX_BUSY");
        return;
    }
    
    NSData *standardFrameData = [self buildStandardFrame:frameId
                                               frameData:frameData
                                                  echoId:txc->echo_id];
    IOReturn result = [self.usbIO asyncSendInnoMakerDeviceBuf:self.currentDevice
                                                    andBuffer:(Byte*)standardFrameData.bytes
                                                      andSize:(int)standardFrameData.length
                                                   andTimeOut:1000];
    if (result != kIOReturnSuccess) {
        gs_free_tx_context(txc);
        return;
    }
    
    if (_numberSended + 1 >= _numberNeedToSend) {
        [self cancelSendThread];
        dispatch_async(dispatch_get_main_queue(), ^{
            self.mutleSendBtn.tag = 0;
            self.mutleSendBtn.title = NSLocalizedStringFromTable(@"lzK-xu-4Vr.title",@"Main", @"Delayed Send");
        });
        return;
    }
    
    if (self.currentDevice == nil || self.currentDevice.isOpen == false ) {
        [self cancelSendThread];
        dispatch_async(dispatch_get_main_queue(), ^{
   
            self.mutleSendBtn.tag = 0;
            self.mutleSendBtn.title = NSLocalizedStringFromTable(@"lzK-xu-4Vr.title",@"Main", @"Delayed Send");
        });
        return;
    }
    
    _numberSended++;
    if (self.checkIdType == CheckIDTypeIncrease) {
        [frameId setString:[self increaseFrameIdHexString:frameId]];
    }
    if(self.checkDataType == CheckDataTypeIncrease) {
        [frameData setString:[self increaseFrameDataHexString:frameData]];
    }
}

- (void)cancelSendThread {
    if (self.sendThread != NULL) {
        _sendThreadExit = true;
        [_sendThread cancel];
        _sendThread = NULL;
    }
}

- (struct innomaker_device_bittiming)getBittiming:(int)index {
    struct innomaker_device_bittiming bittming;
    
    switch (index) {
        case 0: // 20K
            bittming.prop_seg = 6;
            bittming.phase_seg1  = 7;
            bittming.phase_seg2 = 2;
            bittming.sjw = 1;
            bittming.brp = 150;
            break;
        case 1: // 33.33K
            bittming.prop_seg = 3;
            bittming.phase_seg1  = 3;
            bittming.phase_seg2 = 1;
            bittming.sjw = 1;
            bittming.brp = 180;
            break;
        case 2: // 40K
            bittming.prop_seg = 6;
            bittming.phase_seg1  = 7;
            bittming.phase_seg2 = 2;
            bittming.sjw = 1;
            bittming.brp = 75;
            break;
        case 3: // 50K
            bittming.prop_seg = 6;
            bittming.phase_seg1  = 7;
            bittming.phase_seg2 = 2;
            bittming.sjw = 1;
            bittming.brp = 60;
            break;
        case 4: // 66.66K
            bittming.prop_seg = 3;
            bittming.phase_seg1  = 3;
            bittming.phase_seg2 = 1;
            bittming.sjw = 1;
            bittming.brp = 90;
            break;
        case 5: // 80K
            bittming.prop_seg = 3;
            bittming.phase_seg1  = 3;
            bittming.phase_seg2 = 1;
            bittming.sjw = 1;
            bittming.brp = 75;
            break;
        case 6: // 83.33K
            bittming.prop_seg = 3;
            bittming.phase_seg1  = 3;
            bittming.phase_seg2 = 1;
            bittming.sjw = 1;
            bittming.brp = 72;
            break;
            
            
        case 7: // 100K
            bittming.prop_seg = 6;
            bittming.phase_seg1  = 7;
            bittming.phase_seg2 = 2;
            bittming.sjw = 1;
            bittming.brp = 30;
            break;
        case 8: // 125K
            bittming.prop_seg = 6;
            bittming.phase_seg1  = 7;
            bittming.phase_seg2 = 2;
            bittming.sjw = 1;
            bittming.brp = 24;
            break;
        case 9: // 200K
            bittming.prop_seg = 6;
            bittming.phase_seg1  = 7;
            bittming.phase_seg2 = 2;
            bittming.sjw = 1;
            bittming.brp = 15;
            break;
        case 10: // 250K
            bittming.prop_seg = 6;
            bittming.phase_seg1  = 7;
            bittming.phase_seg2 = 2;
            bittming.sjw = 1;
            bittming.brp = 12;
            break;
        case 11: // 400K
            bittming.prop_seg = 3;
            bittming.phase_seg1  = 3;
            bittming.phase_seg2 = 1;
            bittming.sjw = 1;
            bittming.brp = 15;
            break;
        case 12: // 500K
            bittming.prop_seg = 6;
            bittming.phase_seg1  = 7;
            bittming.phase_seg2 = 2;
            bittming.sjw = 1;
            bittming.brp = 6;
            break;
        case 13: // 666K
            bittming.prop_seg = 3;
            bittming.phase_seg1  = 3;
            bittming.phase_seg2 = 2;
            bittming.sjw = 1;
            bittming.brp = 8;
            break;
        case 14: /// 800K
            bittming.prop_seg = 7;
            bittming.phase_seg1  = 8;
            bittming.phase_seg2 = 4;
            bittming.sjw = 1;
            bittming.brp = 3;
            break;
        case 15: /// 1000K
            bittming.prop_seg = 5;
            bittming.phase_seg1  = 6;
            bittming.phase_seg2 = 4;
            bittming.sjw = 1;
            bittming.brp = 3;
            break;
        default: /// 1000K
            bittming.prop_seg = 5;
            bittming.phase_seg1  = 6;
            bittming.phase_seg2 = 4;
            bittming.sjw = 1;
            bittming.brp = 3;
            break;
    }
    return bittming;
}

#pragma mark - helper
/// Format string
/// @param originString origin string
/// @param charactersInString limit string
- (NSString *)formatString:(NSString *)originString charactersInString:(NSString *)charactersInString {
    NSCharacterSet *charSet = [NSCharacterSet characterSetWithCharactersInString:charactersInString];
    char *stringResult = malloc(originString.length * 2);
    int cpt = 0;
    for (int i = 0; i < [originString length]; i++) {
        unichar c = [originString characterAtIndex:i];
        if ([charSet characterIsMember:c]) {
            stringResult[cpt]=c;
            cpt++;
        }
    }
    stringResult[cpt]='\0';
    NSString *string = [NSString stringWithCString:stringResult encoding:NSUTF8StringEncoding];
    free(stringResult);
    return string;
}

/// Sperate every two char use space
/// @param number origin number string
- (NSString *)dealWithString:(NSString *)number
{
    char *chars = malloc(number.length * 2);
    int count = 0;
    int j = 0;
    
    for (int i = 0; i < number.length; i++) {
        count++;
        char c = [number characterAtIndex:i];
        chars[j++] = c;
        if (count == 2 && i < number.length - 1) {
            chars[j++] = ' ';
            count = 0;
        }
    }
    
    /// add end character
    chars[j] = '\0';
    NSString *formatString =  [NSString stringWithUTF8String:chars];
    free(chars);
    
    return formatString;
}

/// Limit textfield 16 binary system format
/// @param textField target textfield
/// @param limitLength limit length
/// @param spacePadding if use space to sperate
- (void)adjustTextFieldToHex:(NSTextField *)textField
                 limitLength:(int)limitLength
            needSpacePadding:(BOOL)spacePadding
{
    
    NSString *originString = textField.stringValue;
    if (spacePadding ) {
        originString = [textField.stringValue stringByReplacingOccurrencesOfString:@" " withString:@""];
    }
    NSString *string =  [self formatString:originString charactersInString:@"0123456789ABCDEFabcdef"];
    if (string.length > limitLength) {
        string = [string substringWithRange:NSMakeRange(0, limitLength)];
    }
    
    if (spacePadding) {
        string = [self dealWithString:string];
    }
    
    textField.stringValue = string;
}

/// Limit textfield number format
/// @param textField target textfield
/// @param minValue min value
/// @param maxValue max value
- (void)adjustTextFieldToNumber:(NSTextField *)textField
                       minValue:(int)minValue
                       maxValue:(int)maxValue {
    NSString *originString = [textField.stringValue stringByReplacingOccurrencesOfString:@" "
                                                                              withString:@""];
    NSString *string = [self formatString:originString charactersInString:@"0123456789"];
    int value = string.intValue;
    if (value < minValue) {
        string = [NSString stringWithFormat:@"%d", minValue];
    }
    if (value > maxValue) {
        string = [NSString stringWithFormat:@"%d", maxValue];
    }
    
    textField.stringValue = string;
}

/// Increase frame id
/// @param frameId frame id str
- (NSString *)increaseFrameIdHexString:(NSString *)frameId {
    NSMutableString *increaseFrameId = [NSMutableString string];
    
    NSArray *dataByte = [frameId componentsSeparatedByString:@" "];
    Byte frameIdBytes[dataByte.count];
    BOOL increaseBit = true;
    /// FrameID increase one inverted order
    for (int i = (int)dataByte.count - 1; i >= 0; i--) {
        NSString *byteValue = [dataByte objectAtIndex:i];
        frameIdBytes[i] = strtoul([byteValue UTF8String],0,16);
        if (increaseBit) {
            if (frameIdBytes[i] + 1 > 0xff) {
                frameIdBytes[i] = 0x00;
                increaseBit = true;
            }
            else {
                frameIdBytes[i] = frameIdBytes[i] + 1;
                increaseBit = false;
            }
        }
    }
    
    for (unsigned long i = 0; i < dataByte.count;i++) {
        [increaseFrameId appendFormat:@"%02x",frameIdBytes[i]];
        if (i != dataByte.count - 1) {
            [increaseFrameId appendString:@" "];
        }
    }
    return increaseFrameId;
}

/// Increase frame data
/// @param frameData frame data str
- (NSString *)increaseFrameDataHexString:(NSString*)frameData {
    NSMutableString *increaseFrameData = [NSMutableString string];
    Byte frameDataBytes[frameData.length];
    NSArray *dataByte = [frameData componentsSeparatedByString:@" "];
    BOOL increaseBit = true; /// increase one
    /// FrameData increase one in order
    for (int i = (int)dataByte.count - 1; i >= 0; i--) {
        NSString *byteValue = [dataByte objectAtIndex:i];
        frameDataBytes[i] = strtoul([byteValue UTF8String],0,16);
        if (increaseBit) {
            if (frameDataBytes[i] + 1 > 0xff) {
                frameDataBytes[i] = 0x00;
                increaseBit = true;
            }
            else {
                frameDataBytes[i] = frameDataBytes[i] + 1;
                increaseBit = false;
            }
        }
    }
    
    for (unsigned long i = 0; i < dataByte.count;i ++) {
        [increaseFrameData appendFormat:@"%02x",frameDataBytes[i]];
        if (i != dataByte.count - 1) {
            [increaseFrameData appendString:@" "];
        }
    }
    return increaseFrameData;
}

/// Build standard frame
/// @param frameId frame id
/// @param frameData frame data
/// @return standard frame
- (NSData *)buildStandardFrame:(NSString *)frameId
                     frameData:(NSString *)frameData
                        echoId:(uint32_t)echoId{
    struct innomaker_host_frame frame;
    memset(&frame, 0, sizeof(frame));
    frame.echo_id = echoId;
    frame.can_id = frameId.intValue;
    frame.can_dlc = (uint32_t)[frameData componentsSeparatedByString:@" "].count;
    frame.channel = 0;
    frame.flags = 0;
    frame.reserved = 0;
    
    NSArray *canIdByte = [frameId componentsSeparatedByString:@" "];
    Byte canId[4] = {0x00,0x00,0x00,0x00};
    
    /// Use Little Endian Mode (From High To Low)
    for (int i = 0; i < canIdByte.count; i++) {
        NSString  *b = canIdByte[i];
        canId[3 - i] = strtoul([b UTF8String],0,16);
    }
    memcpy(&frame.can_id, canId, 4);
    
    /// Build Standard Frame
    /// Set Extend Flag
    if (_frameFormat == FrameFormatExtend) {
        frame.can_id |= CAN_EFF_FLAG;
    }
    /// Set Remote Flag
    if  (_frameType == FrameTypeRemote) {
        frame.can_id |= CAN_RTR_FLAG;
    }
    
    
    NSArray *dataByte = [frameData componentsSeparatedByString:@" "];
    for (int i = 0; i < dataByte.count; i++) {
        NSString *byteValue = [dataByte objectAtIndex:i];
        frame.data[i] = strtoul([byteValue UTF8String],0,16);
    }
    
    NSData *standardFrameData = [NSData dataWithBytes:&frame length:sizeof(frame)];
    NSLog(@"Standard Frame Data = %@",standardFrameData);
    return standardFrameData;
}


/// Convert Data to view model
/// @param data data
- (FrameViewModel *)dataToModel:(NSData *)data{
    if (data.length < sizeof(struct innomaker_host_frame)) return [FrameViewModel new];
    
    Byte echodIdByte[4];
    [data getBytes:echodIdByte range:NSMakeRange(0, 4)];
    uint32_t echoId = 0;
    memcpy(&echoId, echodIdByte, 4);
    
    BOOL isIn = false;
    if (echoId == 0xFFFFFFFF) {
        isIn = true;
    }
    
    if (_rxNum > 4999999)
    {
        _rxNum = 0;
    }
    if (_txNum > 4999999)
    {
        _txNum = 0;
    }
    if (_rxErrorNum > 4999999)
    {
        _rxErrorNum = 0;
    }

    Byte *dataByte = (Byte*)data.bytes;
    
    /// Caculate time
    Byte timeUs[4];
    timeUs[0] = dataByte[20];
    timeUs[1] = dataByte[21];
    timeUs[2] = dataByte[22];
    timeUs[3] = dataByte[23];
    
    uint32_t timeStamp = 0;
    memcpy(&timeStamp,timeUs, 4);
    
    uint32_t s = timeStamp/1000000;
    uint32_t ms = (timeStamp % 1000000) / 1000;
    uint32_t us = timeStamp % 1000;
    NSString *dateString = [NSString stringWithFormat:@"%d.%d.%d",s,ms,us];
    
    
    /// Init Frame View Model
    FrameViewModel *model = [FrameViewModel new];
    model.seqID = [NSString stringWithFormat:@"%d",_txNum+_rxNum+_rxErrorNum];
    model.systemTime = dateString;
    model.timeIdentifier = @"";
    model.canChannel = [NSString stringWithFormat:@"%d", dataByte[10]];
    model.direction =  isIn ? [NSString stringWithFormat:@"Recv [%d]",_rxNum] : [NSString stringWithFormat:@"Send [%d]",_txNum] ;
    
    
    /// Caculate Frame Id
    Byte frameId[4];
    frameId[0] = dataByte[4];
    frameId[1] = dataByte[5];
    frameId[2] = dataByte[6];
    frameId[3] = dataByte[7];
    
    uint32_t intFrameId = 0;
    memcpy(&intFrameId,frameId, 4);
    
    /// Check If is error or not
    if ( (intFrameId & CAN_ERR_FLAG) && isIn) {
        model.color = NSColor.systemYellowColor;
        model.isDataValid = false;
        model.direction = [NSString stringWithFormat:@"Recv ERROR  [%d]",_rxErrorNum++];
        if (intFrameId & CAN_ERR_RESTARTED) {
            model.messge = @"Fail: CAN_STATE_ERROR_ACTIVE";
        }
        else if(intFrameId & CAN_ERR_BUSOFF){
            model.messge = @"Fail: CAN_STATE_BUS_OFF";
        }
        else if(intFrameId & CAN_ERR_CRTL){
            /// dataByte[13] = Data[1]
            if ((dataByte[13] & CAN_ERR_CRTL_TX_WARNING)
                || (dataByte[13] & CAN_ERR_CRTL_RX_WARNING)) {
                model.messge = @"Fail: CAN_STATE_ERROR_WARNING";
            } else if  ((dataByte[13] & CAN_ERR_CRTL_TX_PASSIVE)
                        || (dataByte[13] & CAN_ERR_CRTL_RX_PASSIVE)) {
                model.messge = @"Fail: CAN_STATE_ERROR_PASSIVE";
            }  else if ((dataByte[13] & CAN_ERR_CRTL_RX_OVERFLOW)
                        || (dataByte[13] & CAN_ERR_CRTL_TX_OVERFLOW)) {
                model.messge = @"Fail: CAN_STATE_ERROR_OVERFLOW";
            } else  {
                model.messge = @"Fail: CAN_STATE_ERROR_ACTIVE";
            }
        }else {
            model.messge = @"Fail";
        }
    } else if (isIn){
        model.color = NSColor.systemGreenColor;
        model.isDataValid =  true;
        model.messge = @"Success";
        _rxNum++;
    } else {
        model.color = NSColor.blackColor;
        model.isDataValid = true;
        model.messge = @"Success";
        _txNum++;
    }
    
    /// Check extend format
    if (intFrameId & CAN_EFF_FLAG) {
        model.frameFormat = NSLocalizedStringFromTable(@"extended_format", @"Main", @"Extend Frame")  ;
        model.identifier = [NSString stringWithFormat:@"%04x",intFrameId & CAN_ID_MASK];
    } else {
        model.frameFormat = NSLocalizedStringFromTable(@"standard_format", @"Main", @"Standard Frame")  ;
        model.identifier = [NSString stringWithFormat:@"%04x",intFrameId & CAN_ID_MASK];
    }
    
    /// Check remote frame
    if (intFrameId & CAN_RTR_FLAG) {
        model.frameType = NSLocalizedStringFromTable(@"remote_frame", @"Main", @"Remote Frame")  ;
    } else {
        model.frameType = NSLocalizedStringFromTable(@"data_frame", @"Main", @"Data Frame")  ;
    }
    
   
    /// Get Data
    Byte dlc = dataByte[8];
    model.length = [NSString stringWithFormat:@"%d",dlc];
    NSData *frameData = [NSData dataWithBytes:dataByte + 12 length:dlc];
    if (intFrameId & CAN_RTR_FLAG) {
        model.data =  _currentLanguage == ChineseLanguage ? @"远程请求" : @"Remote Request";
    } else {
        model.data = [frameData hexString];
    }
    
    
    return model;
}

- (NSString *)displayTextForColumn:(NSTableColumn *)tableColumn
                             model:(FrameViewModel *)model {
    NSDictionary *dictionary = @{
        @"SeqIdentifier": model.seqID,
        @"SystemTimeIdentifier":model.systemTime,
        @"TimeIdentifierIdenftifier":model.timeIdentifier,
        @"CanChannelIdentifier":model.canChannel,
        @"DirectionIdentifier":model.direction,
        @"IDIdentifier":model.identifier,
        @"FrameTypeIdentifier":model.frameType,
        @"FrameFormatIdentifier":model.frameFormat,
        @"FrameLengthIdentifier":model.length,
        @"FrameDataIdentifier":model.data,
        @"MessageIdentifier":model.messge
    };
    
    return dictionary[tableColumn.identifier];
}

- (NSTableCellView *)configColumnWithTable:(NSTableView *)tableView
                                     model:(FrameViewModel*)model
                                    column:(NSTableColumn *)tableColumn {
    
    NSString *displayText = [self displayTextForColumn:tableColumn model:model];
    NSTableCellView *cellView = [_tableView makeViewWithIdentifier:tableColumn.identifier owner:nil];
    if (cellView) {
        cellView.textField.textColor = model.color;
        cellView.textField.stringValue = displayText;
        return cellView;
    }
    return cellView;
}


/// Alert message
/// @param message message
- (void)alertWithMsg:(NSString *)message {
    NSAlert *alert = [[NSAlert alloc] init];
    alert.alertStyle = NSWarningAlertStyle;
    [alert addButtonWithTitle: NSLocalizedStringFromTable(@"confirm-title", @"Main", @"Confirm")];
    alert.messageText = NSLocalizedStringFromTable(@"tip", @"Main", @"tip");
    alert.informativeText = message;
    
    [alert beginSheetModalForWindow:[NSApplication sharedApplication].keyWindow completionHandler:^(NSModalResponse returnCode) {
        if (returnCode == NSAlertFirstButtonReturn) {
            
        } else if (returnCode == NSAlertSecondButtonReturn) {
            
        } else {
            
        }
    }];
}

- (BOOL)isScrollToBottom {
    return true;
}


- (void)exportLogFile
{
    NSWindow*       window = [NSApplication sharedApplication].keyWindow;
    NSSavePanel*    panel = [NSSavePanel savePanel];
    [panel setNameFieldStringValue:@"log.txt"];
    
    __weak typeof(self) weakSelf = self;
    [panel beginSheetModalForWindow:window completionHandler:^(NSInteger result) {
        if (result == NSFileHandlingPanelOKButton) {
            NSURL*  theFile = [panel URL];
            // Write the contents in the new format.
            NSMutableString *writeString = [NSMutableString string];
            for (FrameViewModel *model in weakSelf.recvFrame) {
                [writeString appendFormat:@"%@\n",model.presentString];
            }
            [writeString writeToURL:theFile atomically:true encoding:NSUTF8StringEncoding error:nil];
        }
    }];
}

#pragma mark - notification
#pragma mark - getter and setter



@end
