/*This is a socket CAN receive demo program*/
/*This program default use can0 device can receive standid 0x123 and extend id 0x1234567*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
/*Special address description flags for CAN_ID*/
#define CAN_EFF_FLAG 0x80000000U
#define CAN_RTR_FLAG 0x40000000U
#define CAN_ERR_FLAG 0x20000000U

unsigned char find_can(const int port)
{
	char buf[128]={0};
	sprintf(buf,"/sys/class/net/can%d/can_bittiming/bitrate",port);
	return ((access(buf,0)==0));
}

int main()
{
    int ret;
    int s, nbytes,i;
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame;
    memset(&frame, 0, sizeof(struct can_frame));
    
   /*if(!find_can(0))
   {
    printf("Can0 device dose not exist!\n");
	 return -1;
   }*/
    system("sudo ifconfig can0 down");//must close device before set bitrate!
    //below mean depends on iprout tools,not ip tool in busybox!
	system("sudo ip link set can0 type can bitrate 1000000");
    //system("sudo echo 1000000 > /sys/class/net/can0/can_bittiming/bitrate");
    system("sudo ifconfig can0 up");
    printf("This is a can receive demo,can0 with 1Mbps!\r\n");
    /*Create socket*/
    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s < 0) {
        perror("Create socket PF_CAN failed");
        return 1;
    }
    
    /*Specify can0 device*/
    strcpy(ifr.ifr_name, "can0");
    ret = ioctl(s, SIOCGIFINDEX, &ifr);
    if (ret < 0) {
        perror("ioctl interface index failed!");
        return 1;
    }

    /*Bind the socket to can0*/
    addr.can_family = PF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    ret = bind(s, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0) {
        perror("bind failed");
        return 1;
    }
    
    /*Define receive filter rules,we can set more than one filter rule!*/
    struct can_filter rfilter[2];
    rfilter[0].can_id = 0x123;//Standard frame id !
    rfilter[0].can_mask = CAN_SFF_MASK;
    rfilter[1].can_id = 0x12345678;//extend frame id!
    rfilter[1].can_mask = CAN_EFF_MASK;
    //mask below sentense to receive all the message from CAN BUS!
	//setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));

    /*Receive data and exit*/
    while(1) {
        nbytes = read(s, &frame, sizeof(frame));
        if(nbytes > 0) {
            if(!(frame.can_id&CAN_EFF_FLAG))
            //if(frame.can_id&0x80000000==0)
				printf("Received standard frame!\n");
			else
				printf("Received extended frame!\n");
            printf("can_id = 0x%X\r\ncan_dlc = %d \r\n", frame.can_id&0x1FFFFFFF, frame.can_dlc);
            for(i = 0; i < 8; i++)
                printf("data[%d] = %d\r\n", i, frame.data[i]);
		   // mask below sentense to receive all the time other wise can only receive one time!
           // break;
        }
    }
    
    /*Close can0 device and destroy socket!*/
    close(s);
    system("sudo ifconfig can0 down");
    
    return 0;
}
