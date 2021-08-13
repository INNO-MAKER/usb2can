//
//  USBIO.h
//  InnoMakerUSB2CAN
//
//  Created by Inno-Maker on 2020/4/5.
//  Copyright © 2020 Inno-Maker. All rights reserved.
//

/**********************************************************************
 Class : UsbIO
 Author : Inno-Maker
 Date: 07.20.2021
 Version : 1.2.0
 Routines to Proivde InnoMaker Client Device Applicate Interface
 ***********************************************************************/

#import <Foundation/Foundation.h>
#import <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/IOMessage.h>
#include <IOKit/IOCFPlugIn.h>
#include <IOKit/usb/IOUSBLib.h>
#import <unistd.h>

@interface InnoMakerDevice : NSObject {
    IOUSBDeviceInterface245 **InnoMakerDev;
    IOUSBInterfaceInterface245 **InnoMakerIntf;
    UInt8 pipeIn;
    UInt8 pipeOut;
    UInt32 deviceID;
    UInt16 maxPacketSizeIn;
    UInt16 maxPacketSizeOut;
    BOOL _open;
}
@property(nonatomic,assign) IOUSBDeviceInterface245 **InnoMakerDev;
@property(nonatomic,assign) IOUSBInterfaceInterface245 **InnoMakerIntf;
@property(nonatomic,assign) UInt8 pipeIn;
@property(nonatomic,assign) UInt8 pipeOut;
@property(nonatomic,assign) UInt8 controlPipe;
@property(nonatomic,assign) UInt32 deviceID;
@property(nonatomic,assign) UInt16 maxPacketSizeIn;
@property(nonatomic,assign) UInt16 maxPacketSizeOut;
@property BOOL isOpen;
@end


/*!
 @protocol	 InnoMakerDeviceDelegate
 @discussion  It is notified when the device status changed
 such as a new device found , or a device remove
 */
@protocol InnoMakerDeviceDelegate

/*!
 @method     addDeviceNotify:
 @param      device The new find device
 */
- (void)addDeviceNotify:(InnoMakerDevice *)device;

/*!
 @method     removeDeviceNotify:
 @param		device The remove device
 */
- (void)removeDeviceNotify:(InnoMakerDevice *)device;


/**
 Read Device Data Notify When Async/sync Read
 
 @param device device
 @param data data
 @param length length
 */
- (void)readDeviceDataNotify:(InnoMakerDevice *)device
                        data:(Byte *)data
                      length:(NSUInteger)length;

/**
 Write Device Data Notify When Async/sync Write
 
 @param device device
 @param data data
 @param length length
 */
- (void)writeDeviceDataNotify:(InnoMakerDevice *)device
                         data:(Byte *)data
                       length:(NSUInteger)length;
@end


/*!
 @class			UsbIO
 */
@interface UsbIO : NSObject {
    NSMutableArray *InnoMakerDevices;
    NSThread *monitorThread;
    UInt32 PID;
    UInt32 VID;
}

@property (nonatomic,strong) NSMutableArray *InnoMakerDevices;
@property (nonatomic,weak) NSObject<InnoMakerDeviceDelegate>*delegate;

/*!
 @method     getDllVersion
 @abstract   Get current dll verison
 @result     Dll version characters
 */
- (NSString *)getDllVersion;

/*!
 @method    scanInnoMakerDevices
 @abstract  Scan all the device
 @result	kIOReturnSuccess , other fail
 */
- (IOReturn)scanInnoMakerDevices;

/*!
 @method    getInnoMakerDeviceCount
 @abstract  Get the amount of the devices
 @result	The amount of the devices
 */
- (NSUInteger)getInnoMakerDeviceCount;

/*!
 @method    openInnoMakerDevice:
 @abstract  Open the device
 @param		dev The device wanted to open
 @result	true,Open success;false Open fail
 */
- (BOOL)openInnoMakerDevice:(InnoMakerDevice *)dev;

/*!
 @method    closeInnoMakerDevice:
 @abstract  Close the device
 @param		dev The device wanted to close
 @result	true, Close success ; false, Close fail
 */
- (BOOL)closeInnoMakerDevice:(InnoMakerDevice *)dev;

/*!
 @method    getInnoMakerDevice:
 @abstract  Get the device
 @param		devIndex The device index
 @result	The destination device
 */
- (InnoMakerDevice *)getInnoMakerDevice:(UInt8)devIndex;

/*!
 @method    getInnoMakerDeviceBuf:andBuffer:andSize:
 @abstract	Get buffer from the device async(will also call readDeviceDataNotify),pSize must equal or less than maxPipeIn size
 @param		dev The device for getting buffer from
 @param		buf The buffer wanted to fill in
 @param		pSize  It is the buffer in length
 @result	kIOReturnSuccess , other fail
 */
- (IOReturn)asyncGetInnoMakerDeviceBuf:(InnoMakerDevice *)dev
                             andBuffer:(Byte *)buf
                               andSize:(UInt32)pSize
                               andTimeOut:(UInt32)timeout;

/*!
 @method    sendInnoMakerDeviceBuf:andBuffer:andSize:
 @abstract  Send buffer to the device async(will also call writeDeviceDataNotify),pSize must equal or less than maxPipeOut size
 @param		dev The device for sending buffer to
 @param		buf The buffer wanted to send
 @param		pSize It is the buffer out length
 @result	kIOReturnSuccess , other fail
 */
- (IOReturn)asyncSendInnoMakerDeviceBuf:(InnoMakerDevice *)dev
                              andBuffer:(Byte *)buf
                                andSize:(UInt32)pSize
                             andTimeOut:(UInt32)timeout;

/*!
 @method    getInnoMakerDeviceBuf:andBuffer:andSize:
 @abstract    Get buffer from the device sync (will also call readDeviceDataNotify),pSize must equal or less than maxPipeIn size
 @param        dev The device for getting buffer from
 @param        buf The buffer wanted to fill in
 @param        pSize  It is the buffer in length
 @result    kIOReturnSuccess , other fail
 */
- (IOReturn)syncGetInnoMakerDeviceBuf:(InnoMakerDevice *)dev
                             andBuffer:(Byte *)buf
                               andSize:(UInt32)pSize
                           andTimeOut:(UInt32)timeout;

/*!
 @method    sendInnoMakerDeviceBuf:andBuffer:andSize:
 @abstract  Send buffer to the device sync (will also call writeDeviceDataNotify),pSize must equal or less than maxPipeOut size
 @param        dev The device for sending buffer to
 @param        buf The buffer wanted to send
 @param        pSize It is the buffer out length
 @result    kIOReturnSuccess , other fail
 */
- (IOReturn)syncSendInnoMakerDeviceBuf:(InnoMakerDevice *)dev
                              andBuffer:(Byte *)buf
                                andSize:(UInt32)pSize
                            andTimeOut:(UInt32)timeout;
@end
