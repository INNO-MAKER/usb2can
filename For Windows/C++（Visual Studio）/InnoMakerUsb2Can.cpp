// InnoMakerUsb2Can.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "InnoMakerUsb2CanLib.h"
#include <thread>

InnoMakerUsb2CanLib::innomaker_can *can;
BOOL exitRecvThread = false;

int bytesToInt(byte* bytes, int size = 4)
{
	int a = bytes[0] & 0xFF;
	a |= ((bytes[1] << 8) & 0xFF00);
	a |= ((bytes[2] << 16) & 0xFF0000);
	a |= ((bytes[3] << 24) & 0xFF000000);
	return a;
}

void sendTask(InnoMakerUsb2CanLib *usbCanLib, InnoMakerUsb2CanLib::InnoMakerDevice *firstDevice) {
	/// Test Send 
	int counter = 10000;
	while (counter-- > 0) {
		InnoMakerUsb2CanLib::innomaker_host_frame frame;
		frame.data[0] = 0;
		frame.data[1] = 0;
		frame.data[2] = 0;
		frame.data[3] = 0;
		frame.data[4] = 0;
		frame.data[5] = 0;
		frame.data[6] = 0;
		frame.data[7] = 0;
		frame.can_id = 0;

		frame.can_dlc = 8;
		frame.channel = 0;
		frame.flags = 0;
		frame.reserved = 0;


		// Find an empty context to keep track of transmission 
		InnoMakerUsb2CanLib::innomaker_tx_context *txc = usbCanLib->innomaker_alloc_tx_context(can);
		if (txc->echo_id == 0xff)
		{
			printf("SEND FAIL: NETDEV_BUSY \n");
			Sleep(1000);
			continue;
		}

		frame.echo_id = txc->echo_id;

		BYTE sendBuffer[sizeof(InnoMakerUsb2CanLib::innomaker_host_frame)];
		memcpy(sendBuffer, &frame, sizeof(InnoMakerUsb2CanLib::innomaker_host_frame));
		bool result = usbCanLib->sendInnoMakerDeviceBuf(firstDevice, sendBuffer, sizeof(InnoMakerUsb2CanLib::innomaker_host_frame), 10);
		if (result) {
			printf("Send a frame \n");
		}
		else {
			/// If send fail,free the context by echo id 
			InnoMakerUsb2CanLib::innomaker_tx_context *txc1 = usbCanLib->innomaker_get_tx_context(can, txc->echo_id);
			if (txc1 != NULL)
			{
				usbCanLib->innomaker_free_tx_context(txc1);
			}
		}
		Sleep(1000);
	}

}

void recvTask(InnoMakerUsb2CanLib *usbCanLib, InnoMakerUsb2CanLib::InnoMakerDevice *firstDevice) {
	while (1) {
		if (exitRecvThread) break;
		BYTE recvBuffer[sizeof(InnoMakerUsb2CanLib::innomaker_host_frame)];
		memset(recvBuffer, 0, sizeof(InnoMakerUsb2CanLib::innomaker_host_frame));
		bool result = usbCanLib->recvInnoMakerDeviceBuf(firstDevice, recvBuffer, sizeof(InnoMakerUsb2CanLib::innomaker_host_frame), 10);
		if (result) {
			int echoId = bytesToInt(recvBuffer);
			if (echoId != 0xffffffff)
			{
				printf("Recv a echo frame \n");

				/// Find the context that transfer before by echoid 
				InnoMakerUsb2CanLib::innomaker_tx_context *txc = usbCanLib->innomaker_get_tx_context(can, echoId);
				///bad devices send bad echo_ids.
				if (txc == NULL)
				{
					printf("RECV FAIL:Bad Devices Send Bad Echo_ids");
					continue;
				}
				/// Free context
				usbCanLib->innomaker_free_tx_context(txc);
			}
			else {
				printf("Recv a frame \n");
			}
		}
	}
}

int main()
{
	/// Setup
	InnoMakerUsb2CanLib *usbCanLib = new InnoMakerUsb2CanLib();
	usbCanLib->setup();

	/// Scan devices
	usbCanLib->scanInnoMakerDevice();
	int deviceNums = usbCanLib->getInnoMakerDeviceCount();
	printf("Find Device Count = %d \n", deviceNums);

	if (deviceNums <= 0) return 0;

	/// Get first devices
	InnoMakerUsb2CanLib::InnoMakerDevice *firstDevice = usbCanLib->getInnoMakerDevice(0);

	/// Set usb can mode
	InnoMakerUsb2CanLib::UsbCanMode usbCanMode = InnoMakerUsb2CanLib::UsbCanMode::UsbCanModeLoopback;
	InnoMakerUsb2CanLib::Innomaker_device_bittming bittming;

	/// 20K
	bittming.prop_seg = 6;
	bittming.phase_seg1 = 7;
	bittming.phase_seg2 = 2;
	bittming.sjw = 1;
	bittming.brp = 150;


	//usbCanLib->openInnoMakerDevice(firstDevice);
	//usbCanLib->urbResetDevice(firstDevice);
	//usbCanLib->closeInnoMakerDevice(firstDevice);
	int result = usbCanLib->urbSetupDevice(firstDevice, usbCanMode, bittming);
	if (!result) {
		printf("setup fail");
		return 0;
	}


	can = new InnoMakerUsb2CanLib::innomaker_can();


	/// Init tx context 
	for (int i = 0; i < usbCanLib->innomaker_MAX_TX_URBS; i++)
	{
		can->tx_context[i].echo_id = usbCanLib->innomaker_MAX_TX_URBS;
	}

	exitRecvThread = false;
	/// Start Send Thread
	thread send_thread = std::thread(sendTask, usbCanLib, firstDevice);
	/// Start Recv Thread
	thread recv_thread = std::thread(recvTask, usbCanLib, firstDevice);
	send_thread.join();
	exitRecvThread = true;
	recv_thread.join();
	usbCanLib->urbResetDevice(firstDevice);
	usbCanLib->closeInnoMakerDevice(firstDevice);
	usbCanLib->setdown();
	return 0;
}

