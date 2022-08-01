#############################################################################################################

## Description :  This codes is for test two USB2CAN module commuincation
##                on one Raspberry Pi.        
##                CAN0 send base frame/extended frame/remote frame
##                CAN1 receivd frame and print         

## Author      :  Calvin (calvin@inno-maker.com)/ www.inno-maker.com
              
                
## Date        :  2022.08.01

## Environment :  Hardware            ----------------------  Raspberry Pi 4
##                SYstem of RPI       ----------------------  2022-04-04-raspbian-buster-full.img 64 bit
##                Version of Python   ----------------------  Python 3.7.3(Default in the system)
## Toinstall dependencies:
## sudo pip3 install python-can

#############################################################################################################
import os
import can

def can_test():
    #Set CAN0 speed to 1M bps
    os.system('sudo ifconfig can0 down')
    os.system('sudo ip link set can0 type can bitrate 1000000')
    os.system("sudo ifconfig can0 txqueuelen 100000")
    os.system('sudo ifconfig can0 up')

    #Set CAN1 speed to 1M bps
    os.system('sudo ifconfig can1 down')
    os.system('sudo ip link set can1 type can bitrate 1000000')
    os.system("sudo ifconfig can1 txqueuelen 100000")
    os.system('sudo ifconfig can1 up')
    
    can0 = can.interface.Bus(channel = 'can0', bustype = 'socketcan')
    can1 = can.interface.Bus(channel = 'can1', bustype = 'socketcan')
    
    ##  CAN0 send Base frame , CAN 1 receive this frame and print.     
    sff_frame = can.Message(arbitration_id=0x123, data=[0,1,2,3,4,5,6,7])
    can0.send(sff_frame)
    msg = can1.recv(10.0)
    if msg is None:
        print("USB2CAN hardware connection failure.")
    else:
        print(f"Received base frame: \n{msg}\n")
    ######################################################################    
    ##  CAN0 send extended frame, CAN 1 receive this frame and print. 
    eff_frame = can.Message(arbitration_id=0x1FFF6666, data=[7,6,5,4,3,2,1,0],is_extended_id = True)
    can0.send(eff_frame)
    msg = can1.recv(10.0)
    if msg is None:
        print("USB2CAN hardware connection failure.")
    else:
        print(f"Received extended frame: \n{msg}\n")
    ######################################################################    
    ##CAN0 send remote frame, CAN 1 receive this frame and print.
    rtr_frame = can.Message(arbitration_id=0x321, data=[0,1,2,3,4,5,6,7],is_remote_frame = True)    
    can0.send(rtr_frame)
    msg = can1.recv(10.0)
    if msg is None:
        print("USB2CAN hardware connection failure.")
    else:
        print(f"Received remote frame: \n{msg}\n")    
    ###################################################################### 
    os.system('sudo ifconfig can1 down')
    os.system('sudo ifconfig can0 down')
    
    
if __name__ == '__main__':
    can_test()



