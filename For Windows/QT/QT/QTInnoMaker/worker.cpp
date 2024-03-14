#include "worker.h"

Worker::Worker(InnoMakerUsb2CanLib *_usbCanLib , InnoMakerUsb2CanLib::InnoMakerDevice *_device,  InnoMakerUsb2CanLib::innomaker_can * _can ) { // Constructor
    // you could copy data from constructor arguments to internal variables here.
    qRegisterMetaType<Message>("Message");
    usbCanLib = _usbCanLib;
    device = _device;
    can = _can;
    exit =false;
}

Worker::~Worker() { // Destructor
    // free resources
}


int Worker::bytesToInt(BYTE* bytes, int size)
{
    int a = bytes[0] & 0xFF;
    a |= ((bytes[1] << 8) & 0xFF00);
    a |= ((bytes[2] << 16) & 0xFF0000);
    a |= ((bytes[3] << 24) & 0xFF000000);
    return a;
}

void Worker::process() { // Process. Start processing data
    while (exit != true && device != NULL) {
        unsigned char recvBuffer[sizeof(InnoMakerUsb2CanLib::innomaker_host_frame)];
        memset(recvBuffer, 0, sizeof(InnoMakerUsb2CanLib::innomaker_host_frame));
        bool result = usbCanLib->recvInnoMakerDeviceBuf(device, recvBuffer, sizeof(InnoMakerUsb2CanLib::innomaker_host_frame), 1000);
        if (result) {
            int echoId = bytesToInt(recvBuffer);

            if (echoId != 0xffffffff)
            {
                Message msg;
                memcpy(msg.data,recvBuffer,24);
                msg.length = sizeof(InnoMakerUsb2CanLib::innomaker_host_frame);
                recv(msg);

                /// Find the context that transfer before by echoid
                InnoMakerUsb2CanLib::innomaker_tx_context *txc = usbCanLib->innomaker_get_tx_context(can, echoId);
                ///bad devices send bad echo_ids.
                if (txc == NULL)
                {
                    printf("RECV FAIL:Bad Devices Send Bad Echo_ids %d",echoId);
                    continue;
                }
                qDebug("Free TX Context %d",txc->echo_id);
                /// Free context
                usbCanLib->innomaker_free_tx_context(txc);

            }
            else {
                Message msg;
                memcpy(msg.data,recvBuffer,24);
                msg.length = sizeof(InnoMakerUsb2CanLib::innomaker_host_frame);
                recv(msg);
            }
        }


    }
    emit finished();
}
