#############################################################################################################################################################################
##
## Description :  This codes is based on gs-usb 0.3.0 demo from pypi.org released o（https://pypi.org/project/gs-usb/0.3.0/） Much appreciated to the original author.
##                You could test all series innomaker usb2can device(usb2can module/usb2can-c/usb2can-x2/usb2can-core) by this python code
##
##
##
##
## Author      :  Calvin (calvin@inno-maker.com)/ www.inno-maker.com
##
##
## Date        :  2023.12.10
##
## Environment :  Hardware            ----------------------  Computer and innomaker usb2can module
##                System              ----------------------  Windows 11、10
##                Version of Python   ----------------------  Python 3.11 64bit
##
## reference   :  gs-usb              ----------------------  pip install gs-usb==0.3.0
##                libusb              ----------------------  libusb-win32 (v1.2.7.3) install by zading
#
##
## Notes       :  1. Use the libusb and zadig and do not install the libusb-win32 on your computer. libusb-win32 will case the device incorrectly recognize
##
##                2.  If this demo can't check the usb2can device after you install all driver. Download the libusb-1.0.20 packet from below link
##                      https://sourceforge.net/projects/libusb/files/libusb-1.0/libusb-1.0.20/libusb-1.0.20.7z/download
##                      Copy the MS64\dll\libusb-1.0.dll to your computer C:\Windows\System32
##                3. need to modify the  gs-usb.py(local-packages\Python311\site-packages\gs_usb  which you have downloaded)
##                    Comment out the self.gs_usb.reset() And save the files
##
## support:          Any problem and suggestion, feel free to contact support@inno-maker.com/calvin@inno-maker.com

##############################################################################################################################################################################

from gs_usb.gs_usb import GsUsb
from gs_usb.gs_usb_frame import GsUsbFrame
from gs_usb.constants import (
    CAN_EFF_FLAG,
    CAN_ERR_FLAG,
    CAN_RTR_FLAG,
)


# gs_usb general also can import from gs_usb_structures.py
GS_USB_ECHO_ID = 0
GS_USB_NONE_ECHO_ID = 0xFFFFFFFF


#innomaker usb2can device do not support the NO_ECHO_BACK mode
#below macro is for gs_usb 0.2.9
#from gs_usb.gs_usb import (
#    GS_USB_MODE_NORMAL ,
#    GS_USB_MODE_LISTEN_ONLY ,
#    GS_USB_MODE_LOOP_BACK ,
#    GS_USB_MODE_ONE_SHOT ,
#    GS_USB_MODE_NO_ECHO_BACK,
#)

#below macro is for gs_usb 0.3.0 and above

GS_CAN_MODE_NORMAL = 0
GS_CAN_MODE_LISTEN_ONLY = (1 << 0)
GS_CAN_MODE_LOOP_BACK = (1 << 1)
#GS_CAN_MODE_TRIPLE_SAMPLE = (1 << 2)
GS_CAN_MODE_ONE_SHOT = (1 << 3)
GS_CAN_MODE_HW_TIMESTAMP = (1 << 4)


def main():
    devs = GsUsb.scan()
    if len(devs) == 0:
        print("Can not find gs_usb device")
        return

    #set can device handle from 0 to 1,2,3...  choosing the right device serial number accroding to the print
    #I found defualt usb2can device is devs[0] with gs_usb 0.2.9 ,and devs[1] with gs_usb 0.3.0
    dev = devs[0]
    print(dev)
    # Close before Start device in case the device was not properly stop last time
    # If do not stop the device, bitrate setting will be fail.
    dev.stop()

    # Configuration Modify the Baudrate you want here
    if not dev.set_bitrate(250000):
        print("Can not set bitrate for gs_usb")
        return

    # Start device, If you have only one device for test, pls use the loop-back mode, otherwise you will get a lot of error frame
    # If you have already connect to a aviailable CAN-BUS, you could set as NORMAL mode
    #dev.start(GS_CAN_MODE_LOOP_BACK)
    dev.start(GS_CAN_MODE_NORMAL)


    # Read all the time and send message in each second
    while True:
        iframe = GsUsbFrame()
        if dev.read(iframe, 1):
            # if you don't want to receive the error frame. filter out it.
            # otherwise you will receive a lot of error frame when your device do not connet to CAN-BUS
            #if iframe.can_id & CAN_ERR_FLAG != CAN_ERR_FLAG:

            # filter out the echo frame. Otherwise It will read back the frame has been sent successfully
            if iframe.echo_id == GS_USB_NONE_ECHO_ID:
                print("RX  {}".format(iframe))

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        # When press keyboard to kill the codes. The usb2can will not auto-stop.It may risk error when you open it next time.
        #So added below code to stop it.
        devs = GsUsb.scan()
        dev = devs[0]
        dev.stop()
        pass
