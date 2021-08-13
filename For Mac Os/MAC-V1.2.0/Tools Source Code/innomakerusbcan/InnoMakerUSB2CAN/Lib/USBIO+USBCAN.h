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

#import "UsbIO.h"

#define CAN_EFF_FLAG 0x80000000U
#define CAN_RTR_FLAG 0x40000000U
#define CAN_ERR_FLAG 0x20000000U

#define CAN_SFF_MASK 0x000007FFU
#define CAN_EFF_MASK 0x1FFFFFFFU
#define CAN_ERR_MASK 0x1FFFFFFFU
#define CAN_ID_MASK  0x1FFFFFFF

/* error class (mask) in can_id */
#define CAN_ERR_CRTL 0x00000004U
#define CAN_ERR_BUSOFF 0x00000040U
#define CAN_ERR_RESTARTED 0x00000100U

/* error status of CAN-controller / data[1] */
#define CAN_ERR_CRTL_UNSPEC     0x00 
#define CAN_ERR_CRTL_RX_OVERFLOW 0x01
#define CAN_ERR_CRTL_TX_OVERFLOW 0x02
#define CAN_ERR_CRTL_RX_WARNING 0x04
#define CAN_ERR_CRTL_TX_WARNING 0x08
#define CAN_ERR_CRTL_RX_PASSIVE 0x10
#define CAN_ERR_CRTL_TX_PASSIVE 0x20

#define CAN_SFF_ID_BITS        11
#define CAN_EFF_ID_BITS        29


#define GS_MAX_TX_URBS 10

struct innomaker_host_frame {
    uint32_t echo_id;
    uint32_t can_id;
    uint8_t can_dlc;
    uint8_t channel;
    uint8_t flags;
    uint8_t reserved;
    uint8_t data[8];
    uint32_t timestamp_us;
};

struct innomaker_device_mode {
    uint32 mode;
    uint32 flags;
};

struct innomaker_device_bittiming {
    uint32 prop_seg;
    uint32 phase_seg1;
    uint32 phase_seg2;
    uint32 sjw;
    uint32 brp;
};

struct innomaker_identify_mode {
    uint32 mode;
};

typedef enum : NSUInteger {
    UsbCanModeNormal,
    UsbCanModeLoopback,
    UsbCanModeListenOnly,
}UsbCanMode;


struct gs_tx_context {
    unsigned int echo_id;
};

struct gs_can {
    /* This lock prevents a race condition between xmit and receive. */
    OSSpinLock tx_ctx_lock;
    struct gs_tx_context tx_context[GS_MAX_TX_URBS];
};

struct gs_tx_context *gs_alloc_tx_context(struct gs_can *dev);
void gs_free_tx_context(struct gs_tx_context *txc);
struct gs_tx_context *gs_get_tx_context(struct gs_can *dev,
                                        unsigned int id);
@interface UsbIO (USBCan)


/*!
 Urb reset device
 @param device target device
 */
- (BOOL)UrbResetDevice:(InnoMakerDevice *)device;

/*!
 Urb setup device
 @param dev target device
 @param mode dev mode
 @param bittiming bittiming
 */
- (BOOL)UrbSetupDevice:(InnoMakerDevice *)dev
                  mode:(UsbCanMode) mode
              bittiming:(struct innomaker_device_bittiming)bittiming;

 
@end


