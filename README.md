# This link is for below USB2CAN series products:

## Release Notice for V3.4.3



We plan to roll out firmware version **V3.4.3** starting in **January 2026**. This update involves **no changes to the hardware**; it is solely a firmware update to the MCU. You can check the this version number via **Device Manager in Windows**, or by using the `dmesg` command in **Linux**. 

Latest version **V3.3** has been in use since **August 2022**, serving thousands of customers worldwide over the past three years. During this time, we have received a lot of encouragement and positive feedback, as well as valuable comments and criticism. We have been carefully recording this input and continuously using it to improve our product. We may not be perfect yet, but please rest assured that we are continuously improving.  This version has undergone long-term testing and has proven to be stable with excellent performance. If you encounter any issues or have feedback or suggestions, please feel free to contact us directly at  **[support@inno-maker.com](mailto:support@inno-maker.com)**.



Below is a summary of the optimizations and improvements included in **V3.4.3**：

1. **For the USB2CAN device, when a down + up operation is performed during communication, the MCU will no longer retain any previously unsent CAN frames or error frames.** 

   After the interface is **down**, all CAN data buffered in the MCU is completely cleared. The original retransmission mechanism was designed so that certain important data could be resent even after a restart. However, this also caused some customers’ systems to receive old data again after reboot, which led to confusion.



2. **The transmission rate of error frames has been changed from continuous transmission to one frame per second.** 

   This is intended to reduce the load on customer applications and the USB bandwidth under error conditions. Error frames are used to inform the application that the CAN bus is currently in a **BUS-OFF** state. For applications that do not make use of error frames, you can continue to **mask the error frame ID** as before.

   

3. **In some very extreme usage scenarios, the USB2CAN can still communicate normally, but the status LED may not blink**. 

   This issue only occurs in a few rare edge cases and does not affect the vast majority of normal use. And this does not affect normal CAN communication, only the LED blinking status will be abnormal. It will return to normal after bringing the interface down + up again. After investigation, we found that it was caused by a problem in the STM32 library used at that time. This has been fixed in the V3.4.3 version.

   

4. **We have added and enabled a third version number.** 

   Originally, this third digit was used only for our internal testing, but to better track customer feedback and version history, we have now made the third version number publicly visible.



Finally, regarding the V3.3 version

1. **We will continue to supply V3.3 and currently have no end-of-sale date or discontinuation plan.** 

   For bulk orders, customers can contact us directly or ask their existing supplier to provide the V3.3 version. For retail customers, May I recommend switching to version V3.4.3, as it will be easier for you to purchase in-stock units directly from all of our sales channels. 

2. **V3.3 devices cannot be directly upgraded to V3.4.3 because the onboard MCU is locked with STM32 RDP Level 2.** 

   Many customers have asked why we locked the MCU and why we did not implement an IAP (in-application programming) upgrade function.

   This is based on our many years of field and industry experience. To protect our industrial customers’ products and prevent devices from being reprogrammed on-site for data theft, reverse engineering, or malicious damage, we chose to lock the MCU and not provide any firmware upgrade mechanism.
   
   

​                                                                                                                                                                                  			 Calvin (calvin@inno-maker.com)

​                                                                       																                        2025.12.10

## Porducts Instuction 

![](https://github.com/INNO-MAKER/Images-Folder/blob/main/USB2CAN%20Family.jpg)




## Folder Instuction
### Folder Document   : Including all documents for usb2can device. Read the user manual first.

### Folder For Windows: Including InnoMaker Software,Busmaster And API For Windows.

### Folder For Linux  : Including InnoMaker Python/C#/CAN-Utils Software And Source Code.
### Folder For Mac Os : Including InnoMaker Software And API For Mac OS.
### Folder CE(EMC) FCC(SDOC) UKCA ICES Verification: All verification files




## <u>Related Project and Demo Link</u>

Below are the some project and demo which based on usb2can device, Thanks very much for the author kindly sharing.

If I missed anything or have any suggestions,  feel free to let me know. (calvin@inno-maker.com)



### (1) [CANOpenRobotController](https://github.com/UniMelbHumanRoboticsLab/CANOpenRobotController)

### (2) **[YZ-AIM-USB2CAN-Driver](https://github.com/NicoPowers/YZ-AIM-USB2CAN-Driver)**

### (3) [NU-Formula-Racing](https://github.com/NU-Formula-Racing)

### (4)**[AgIsoStack++ /Formerly Isobus++(Open-Agriculture)](https://github.com/Open-Agriculture/AgIsoStack-plus-plus)**

### (5)**[Python-Interface_InnoMaker_USB2CAN](https://github.com/Dani-W/Python-Interface_InnoMaker_USB2CAN)**

### (6)**[python-can-loxone-heating-integration](https://github.com/fknipp/python-can-loxone-heating-integration)**

### (7)[Usb2CanPlayground](https://github.com/cuinixam/Usb2CanPlayground)

### (8)**[openinverter-can-tool](https://github.com/davefiddes/openinverter-can-tool)**

