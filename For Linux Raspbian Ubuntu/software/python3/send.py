##############################################################################

## Description :  This codes is for test two USB2CAN module commuincation
##                one as sender and the other as receiver.
##                sender send  '0,1,2,3,4,5,6'
 
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

#check system name, in linux will print 'posix' and in windows will print 'nt'
print(os.name)

os.system('sudo ifconfig can1 down')
os.system('sudo ip link set can1 type can bitrate 1000000')
os.system('sudo ifconfig can1 up')

can1 = can.interface.Bus(channel = 'can1', bustype = 'socketcan')

msg = can.Message(arbitration_id=0x123, data=[0, 1, 2, 3,4,5,6])
can1.send(msg)

os.system('sudo ifconfig can1 down')

