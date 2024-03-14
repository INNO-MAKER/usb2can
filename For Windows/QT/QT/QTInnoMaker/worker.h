#ifndef WORKER_H
#define WORKER_H
#include <QObject>
#include "InnoMakerUsb2CanLib.h"
#include <QMetaType>

struct Message {
    int length;
    BYTE data[24];
};

Q_DECLARE_METATYPE(Message);


class Worker:public QObject {
    Q_OBJECT
public:
    Worker(InnoMakerUsb2CanLib *usbCanLib = nullptr, InnoMakerUsb2CanLib::InnoMakerDevice *device = nullptr,  InnoMakerUsb2CanLib::innomaker_can * can = nullptr);
    ~Worker();
    bool exit;
public slots:
    void process();
signals:
    void finished();
    void error(QString err);
    void recv( Message);
private:
         // add your variables here
    InnoMakerUsb2CanLib *usbCanLib;
    InnoMakerUsb2CanLib::InnoMakerDevice *device;
    InnoMakerUsb2CanLib::innomaker_can * can;
 int bytesToInt(BYTE* bytes, int size = 4);
};

#endif // WORKER_H
