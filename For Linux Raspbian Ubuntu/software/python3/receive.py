

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
        print(f'Received frame: \n{msg}\n')

os.system('sudo ifconfig can0 down')
