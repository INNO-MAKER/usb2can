##############################################################################

## Description :  This codes is for test two USB2CAN module commuincation
##                on one Raspberry Pi.        
##                CAN0 send "0x55, 0xaa, 0x5a,0xa5" to CAN1
##                After that, CAN1 will "0x55, 0xaa, 0x5a,0xa5" to CAN0
##                Finally, Print the test result

## Author      :  Calvin (calvin@inno-maker.com)/ www.inno-maker.com
              
                
## Date        :  2019.11.30

## Environment :  Hardware            ----------------------  Raspberry Pi 4
##                SYstem of RPI       ----------------------  2019-09-26-raspbian-buster-full.img
##                Version of Python   ----------------------  Python 3.7.3(Default in the system)
## Toinstall dependencies:
## sudo pip install python-can


###############################################################################
import os
import can

#Set CAN0 speed to 1M bps
os.system('sudo ip link set can0 type can bitrate 1000000')
os.system('sudo ifconfig can0 up')

#Set CAN1 speed to 1M bps
os.system('sudo ip link set can1 type can bitrate 1000000')
os.system('sudo ifconfig can1 up')

can0 = can.interface.Bus(channel = 'can0', bustype = 'socketcan')
can1 = can.interface.Bus(channel = 'can1', bustype = 'socketcan')

def rs485_enable():
##  CAN0 SEND AND CAN1 RECEIVE    
    msg = can.Message(arbitration_id=0x123, data=[0x55,0xaa,0x5a,0xa5],
                      extended_id=False)

    can0.send(msg)

    msg = can1.recv(10.0)

    if((msg.data[0]==0x55)and(msg.data[1]==0xaa)and
       (msg.data[2]==0x5a)and(msg.data[3]==0xa5)):
    ##  CAN1 SEND AND CAN0 RECEIVE     
        msg = can.Message(arbitration_id=0x123, data=[0x55,0xaa,0x5a,0xa5],
                          extended_id=False)

        print(msg)
        can1.send(msg)
        msg = can0.recv(30.0)
        if((msg.data[0]==0x55)and(msg.data[1]==0xaa)and(msg.data[2]==0x5a)and(msg.data[3]==0xa5)):
            print(msg)
            print('Both USB2CAN module communication test successful')
        elif msg is None:    ## No data
            print('Timeout USB2CAN module test failure.\r\n'*3)
        else :
            print('Data validation errors\r\n'*3)
            print(msg)                       ##print the err data
        
    elif msg is None: ## No data
        print('Timeout USB2CAN module test failure.\r\n'*3)
    else :     ##print the err data
        print('Data validation errors\r\n'*3)
        print(msg)      
    
if __name__ == '__main__':
    rs485_enable()

    os.system('sudo ifconfig can1 down')
    os.system('sudo ifconfig can0 down')


