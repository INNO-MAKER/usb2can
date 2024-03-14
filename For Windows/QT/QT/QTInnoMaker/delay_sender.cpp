#include "delay_sender.h"
DelaySender::DelaySender(InnoMakerUsb2CanLib *usbCanLib  ,
                         InnoMakerUsb2CanLib::InnoMakerDevice *device ,
                         InnoMakerUsb2CanLib::innomaker_can * can  ,
                         QString delaySendFrameId  ,
                         QString delaySendFrameData ,
                         int interval,int numberNeedSend,bool idIncrease,bool dataIncrease,bool extend,bool remote) {
    qRegisterMetaType<MessageA>("MessageA");
    _usbCanLib = usbCanLib;
    _device = device;
    _can = can;
    _delaySendFrameData = delaySendFrameData;
    _delaySendFrameId = delaySendFrameId;
    _numberSend = 0;
    _interval = interval;
    _numberNeedSend = numberNeedSend;
    _idIncrease = idIncrease;
    _dataIncrease = dataIncrease;
    _exit = false;
    _extend = extend;
    _remote = remote;
}

void DelaySender::exit() {
    _exit = true;
}

void DelaySender::process()
{
    while(!_exit && _device != NULL) {

        // Find an empty context to keep track of transmission
        InnoMakerUsb2CanLib::innomaker_tx_context *txc = _usbCanLib->innomaker_alloc_tx_context(_can);
        if (txc->echo_id == 0xff)
        {
            qDebug("SEND FAIL: NETDEV_BUSY \n");
            continue;
        }
         qDebug("Alloc TX Context %d",txc->echo_id);

        InnoMakerUsb2CanLib::innomaker_host_frame frame = this->buildStandardFrame(_delaySendFrameId,_delaySendFrameData,txc->echo_id);
        BYTE sendBuffer[sizeof(InnoMakerUsb2CanLib::innomaker_host_frame)];
        memcpy(sendBuffer, &frame, sizeof(InnoMakerUsb2CanLib::innomaker_host_frame));
        bool result = _usbCanLib->sendInnoMakerDeviceBuf(_device, sendBuffer, sizeof(InnoMakerUsb2CanLib::innomaker_host_frame), 1000);
        if (result) {

            /// 如果达到了发送次0数，则停止发送
            _numberSend++;
            if(_numberSend >= _numberNeedSend) {
                _exit = true;
                continue;
            }
        }
        else {
            qDebug("SEND FAIL: NETDEV_BUSY ECHODID = %d\n",txc->echo_id);
            /// If send fail,free the context by echo id
            InnoMakerUsb2CanLib::innomaker_tx_context *txc1 = _usbCanLib->innomaker_get_tx_context(_can, txc->echo_id);
            if (txc1 != NULL)
            {
                 qDebug("Free TX Context %d",txc1->echo_id);
                _usbCanLib->innomaker_free_tx_context(txc1);
            }
        }


        if(_idIncrease) {
            _delaySendFrameId = increaseHexString(_delaySendFrameId,8);
        }

        if(_dataIncrease) {
            int dlc = _delaySendFrameData.split(' ').size();
            _delaySendFrameData = increaseHexString(_delaySendFrameData,dlc * 2);
        }
        if(_interval == 0) {
          //  QThread::msleep(0);
        } else {
            QThread::msleep(_interval);
        }
    }

    emit finished();
}

InnoMakerUsb2CanLib::innomaker_host_frame DelaySender::buildStandardFrame(QString frameId,QString frameData,int echoId) {

    InnoMakerUsb2CanLib::innomaker_host_frame frame;
    memset(&frame, 0, sizeof(InnoMakerUsb2CanLib::innomaker_host_frame));

    frameId = formatter->formatStringToHex(frameId,8,true);
    frameData = formatter->formatStringToHex(frameData,16,true);

    /// Data Hex String
    int dataArrSize = frameData.split(" ").size();
    QString dataHexStr = frameData.replace(" ","");


    for (int i = 0; i < dataArrSize; i++) {
        QString hexByte = dataHexStr.mid(i*2, 2);
        frame.data[i] = hexByte.toUShort(nullptr, 16);
    }

    /// Id Hex String
    int idArrSize = frameId.split(" ").size();
    QString idHexStr =frameId.replace(" ","");


    unsigned char idArr[4] = {0,0,0,0};
    for (int i = 0; i < idArrSize; i++) {
        QString hexByte = idHexStr.mid(i*2, 2);
        idArr[i] = hexByte.toUShort(nullptr, 16);
    }

    frame.can_id = bytesToInt1(idArr);

    if (_extend == true)
    {
        frame.can_id |= Can_Eff_Flag;
    }

    if (_remote == true)
    {
        frame.can_id |= Can_Rtr_Flag;
    }

    frame.can_dlc = dataArrSize;
    frame.channel = 0;
    frame.flags = 0;
    frame.reserved = 0;
    frame.echo_id = echoId;
    return frame;
}

int DelaySender::bytesToInt(BYTE* bytes, int size)
{
    int a = bytes[0] & 0xFF;
    a |= ((bytes[1] << 8) & 0xFF00);
    a |= ((bytes[2] << 16) & 0xFF0000);
    a |= ((bytes[3] << 24) & 0xFF000000);
    return a;
}

int DelaySender::bytesToInt1(BYTE* bytes, int size)
{
    int a = bytes[3] & 0xFF;
    a |= ((bytes[2] << 8) & 0xFF00);
    a |= ((bytes[1] << 16) & 0xFF0000);
    a |= ((bytes[0] << 24) & 0xFF000000);
    return a;
}

QString DelaySender::increaseHexString(QString frameId, int bit) {
    QString increaseFrameId = "";

    frameId = formatter->formatStringToHex(frameId, bit, true);

    QStringList dataByte = frameId.split(' ');
    BYTE *frameIdBytes = new BYTE[dataByte.size()];
    bool increaseBit = true;

    for (int i = dataByte.size() - 1; i >= 0; i--)
    {
        QString byteValue = dataByte[i];
        frameIdBytes[i] = byteValue.toUShort(nullptr, 16);
        if (increaseBit)
        {
            if (frameIdBytes[i] + 1 > 0xff)
            {
                frameIdBytes[i] = 0x00;
                increaseBit = true;
            }
            else
            {
                frameIdBytes[i] = (BYTE)(frameIdBytes[i] + 1);
                increaseBit = false;
            }
        }
    }

    for (int i = 0; i < dataByte.size(); i++)
    {
        increaseFrameId += QString("%1").arg(frameIdBytes[i],2,16,QLatin1Char('0'));
        if (i != dataByte.size() - 1)
        {
            increaseFrameId += ' ';
        }
    }
    delete[] frameIdBytes;
    return increaseFrameId;
}
