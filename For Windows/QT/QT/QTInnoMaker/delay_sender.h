#ifndef DELAY_SENDER_H
#define DELAY_SENDER_H
#include <QObject>
#include "InnoMakerUsb2CanLib.h"
#include <QMetaType>
#include <QThread>
#include <formatter.h>
struct MessageA {
    int length;
    BYTE data[24];
};

Q_DECLARE_METATYPE(MessageA);

class DelaySender:public QObject
{
    Q_OBJECT
public:
    DelaySender(InnoMakerUsb2CanLib *usbCanLib = nullptr,
                InnoMakerUsb2CanLib::InnoMakerDevice *device = nullptr,
                InnoMakerUsb2CanLib::innomaker_can * can = nullptr,
                QString delaySendFrameId ="00000000",
                QString delaySendFrameData="0000000000000000",
                int interval=1000,int numberNeedSend = 1,bool idIncrease=false,bool dataIncrease=false,bool extend=false,bool remote = false);

    void exit();

private:
    int bytesToInt(BYTE* bytes, int size = 4);
    int DelaySender::bytesToInt1(BYTE* bytes, int size = 4);
    QString increaseHexString(QString data, int bit);
    InnoMakerUsb2CanLib::innomaker_host_frame buildStandardFrame(QString frameId,QString frameData,int echoId);

    int _interval;
    int _numberSend;
    int _numberNeedSend;
    bool _idIncrease;
    bool _dataIncrease;
    QString _delaySendFrameId;
    QString _delaySendFrameData;
    InnoMakerUsb2CanLib *_usbCanLib;
    InnoMakerUsb2CanLib::InnoMakerDevice *_device;
    InnoMakerUsb2CanLib::innomaker_can * _can;
    bool _exit;
    bool _extend;
    bool _remote;
    Formatter *formatter = new Formatter();
    int Can_Eff_Flag = 0x80000000;
    int Can_Rtr_Flag = 0x40000000;
signals:
    void finished();
    void error(QString err);
    void send( MessageA);
public slots:
    void process();
};

#endif // DELAY_SENDER_H
