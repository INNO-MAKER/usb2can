/*This is a socket CAN transmiter test programmer*/
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
unsigned char  find_can(const int port)
{
 char buf[128]={0};
 sprintf(buf,"/sys/class/net/can%d/can_bittiming/bitrate",port);
 return((access(buf,0)==0));
}

int main()
{
    int ret;
    int s, nbytes;
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame;
    memset(&frame, 0, sizeof(struct can_frame));
  /* if(!find_can(0))
  	  {
	   printf("Can0 device dose not exist!\n ");
	   return -1;
	  }
*/
    system("sudo ifconfig can0 down");// must close can device before set baud rate!
    //below mean depend on iprout tools ,not ip tool with busybox!
	 system("sudo ip link set can0 type can bitrate 1000000");
    //system("sudo echo 1000000 > /sys/class/net/can0/can_bittiming/bitrate");
    system("sudo ifconfig can0 up");
    printf("This is a socket can transmit demo program ,can0 with 1Mbps baud rate\r\n");
        
    /*Create socket*/
    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s < 0) {
        perror("Create socket PF_CAN failed!");
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
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    ret = bind(s, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0) {
        perror("bind failed!");
        return 1;
    }
    
    /*Disable filtering rules,this program only send message do not receive packets */
    setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);

    /*assembly  message data! */
    frame.can_id = 0x123;
    frame.can_dlc = 8;
    frame.data[0] = 1;
    frame.data[1] = 2;
    frame.data[2] = 3;
    frame.data[3] = 4;
    frame.data[4] = 5;
    frame.data[5] = 6;
    frame.data[6] = 7;
    frame.data[7] = 8;
    //if(frame.can_id&CAN_EFF_FLAG==0)
    if(!(frame.can_id&CAN_EFF_FLAG))
		printf("Transmit standard frame!\n");
	else
		printf("Transmit extended frame!\n");
    printf("can_id  = 0x%X\r\n", frame.can_id);
    printf("can_dlc = %d\r\n", frame.can_dlc);
    int i = 0;
    for(i = 0; i < 8; i++)
        printf("data[%d] = %d\r\n", i, frame.data[i]);
    
    /*Send message out */
    nbytes = write(s, &frame, sizeof(frame)); 
    if(nbytes != sizeof(frame)) {
        printf("Send  frame incompletely!\r\n");
        system("sudo ifconfig can0 down");
    }
    
    /*Close the socket and can0 */
    close(s);
    system("sudo ifconfig can0 down");
    return 0;
}
