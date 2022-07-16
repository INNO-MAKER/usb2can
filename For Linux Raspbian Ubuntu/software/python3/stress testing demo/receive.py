

import os
import can
import time

os.system('sudo ifconfig can0 down')
os.system('sudo ip link set can0 type can bitrate 1000000')
os.system('sudo ifconfig can0 up')

can0 = can.interface.Bus(channel = 'can0', bustype = 'socketcan')
rec_count = 0
while True:
    msg = can0.recv(10.0)
    #print (msg)
    if msg is None:
        print('No message was received')
    else:
        if (msg.arbitration_id == 0x123):
            rec_count = rec_count + 1
        else:
            while True:
                print("Get error arbitration_id!!!!!!!!!!!!!!!!!!")
                print(msg.arbitration_id)
                print("Currecnt rec count:", rec_count)
                time.sleep(1)
    print("Currecnt rec count:", rec_count)
       
os.system('sudo ifconfig can0 down')
