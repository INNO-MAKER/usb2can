#############################################################################################################################################################################
##
## Description :  This codes is based on gs-usb 0.2.9 demo from pypi.org released on Feb 25, 2022. Much appreciated to the original author.   
##                You could test all series innomaker usb2can device(usb2can module/usb2can-c/usb2can-x2/usb2can-core) by this python code     
##                 
##                
##                
##
## Author      :  Calvin (calvin@inno-maker.com)/ www.inno-maker.com
##
##                
## Date        :  2021.07.02
##
## Environment :  Hardware            ----------------------  Computer and innomaker usb2can module
##                System              ----------------------  Windows 10
##                Version of Python   ----------------------  Python 3.7 64bit
##
## reference   :  gs-usb              ----------------------  pip install gs-usb
##                libusb              ----------------------  https://sourceforge.net/projects/libusb/
##                Winusb              ----------------------  https://udomain.dl.sourceforge.net/project/libusb-win32/libusb-win32-releases/1.2.6.0/libusb-win32-bin-1.2.6.0.zip  
##
## Notes       :  1. Use the libusb and zadig and do not install the libusb-win32 on your computer. libusb-win32 will case the device incorrectly recognize
##
##                2.  If this demo can't check the usb2can device after you install all driver. Download the libusb-1.0.20 packet from below link
##                      https://sourceforge.net/projects/libusb/files/libusb-1.0/libusb-1.0.20/libusb-1.0.20.7z/download
##                      Copy the MS64\dll\libusb-1.0.dll to your computer C:\Windows\System32 
##
## support:          Any problem and suggestion, feel free to contact support@inno-maker.com/calvin@inno-maker.com

##############################################################################################################################################################################


import time

from gs_usb.gs_usb import GsUsb
from gs_usb.gs_usb_frame import GsUsbFrame
from gs_usb.constants import (
    CAN_EFF_FLAG,
    CAN_ERR_FLAG,
    CAN_RTR_FLAG,
)

#innomaker usb2can device do not support the GS_USB_MODE_NO_ECHO_BACK mode
from gs_usb.gs_usb import (
    GS_USB_MODE_NORMAL ,
    GS_USB_MODE_LISTEN_ONLY ,
    GS_USB_MODE_LOOP_BACK ,
    GS_USB_MODE_ONE_SHOT ,
    #GS_USB_MODE_NO_ECHO_BACK,
)


def main():
    devs = GsUsb.scan()
    if len(devs) == 0:
        print("Can not find gs_usb device")
        return

    #set can device handle from 0 to 1,2,3...  choosing the right device serial number accroding to the print
    print(devs)    
    dev = devs[0]
    # Close before Start device in case the device was not properly stop last time
    # If do not stop the device, bitrate setting will be fail.
    dev.stop() 

    # Configuration
    if not dev.set_bitrate(1000000):
        print("Can not set bitrate for gs_usb")
        return
    
    # Start device, If you have only one device for test, pls use the loop-back mode,
    dev.start(GS_USB_MODE_LOOP_BACK)
    #dev.start(GS_USB_MODE_NORMAL)
    
    # Prepare frames
    data = b"\x12\x34\x56\x78\x9A\xBC\xDE\xF0"
    sff_frame = GsUsbFrame(can_id=0x7FF, data=data)
    sff_none_data_frame = GsUsbFrame(can_id=0x7FF)
    err_frame = GsUsbFrame(can_id=0x7FF | CAN_ERR_FLAG, data=data)
    eff_frame = GsUsbFrame(can_id=0x12345678 | CAN_EFF_FLAG, data=data)
    eff_none_data_frame = GsUsbFrame(can_id=0x12345678 | CAN_EFF_FLAG)
    rtr_frame = GsUsbFrame(can_id=0x7FF | CAN_RTR_FLAG)
    rtr_with_eid_frame = GsUsbFrame(can_id=0x12345678 | CAN_RTR_FLAG | CAN_EFF_FLAG)
    rtr_with_data_frame = GsUsbFrame(can_id=0x7FF | CAN_RTR_FLAG, data=data)
    frames = [
        sff_frame,
        sff_none_data_frame,
        err_frame,
        eff_frame,
        eff_none_data_frame,
        rtr_frame,
        rtr_with_eid_frame,
        rtr_with_data_frame,
    ]

    # Read all the time and send message in each second
    end_time, n = time.time() + 1, -1
    while True:
        iframe = GsUsbFrame()
        if dev.read(iframe, 1):
            # if you don't want to receive the error frame. filter out it.
            # otherwise you will receive a lot of error frame when your device do not connet to CAN-BUS
            if iframe.can_id & CAN_ERR_FLAG != CAN_ERR_FLAG:
                print("RX  {}".format(iframe))

        if time.time() - end_time >= 0:
            end_time = time.time() + 1
            n += 1
            n %= len(frames)

            if dev.send(frames[n]):
                print("TX  {}".format(frames[n]))



if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        pass

