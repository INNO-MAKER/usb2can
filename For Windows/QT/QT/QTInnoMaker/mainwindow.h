#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "libusb.h"
#include "InnoMakerUsb2CanLib.h"
#include <thread>
#include <qthread.h>
#include "worker.h"
#include<QStandardItemModel>
#include<delay_sender.h>
#include <formatter.h>
#include "initguid.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

DEFINE_GUID( GUID_USB2CAN  ,0x88bae032, 0x5a81, 0x49f0, 0xbc, 0x3d, 0xa4, 0xff, 0x13, 0x82, 0x16, 0xd6);

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_scanDevice_clicked();

    void on_pushButton_openDevice_clicked();

    void on_pushButton_delaysend_clicked();

    void on_pushButton_send_clicked();

    void on_pushButton_clear_clicked();

    void on_pushButton_export_clicked();

    void on_lineEdit_frameId_textEdited(const QString &arg1);

    void on_lineEdit_data_textEdited(const QString &arg1);

private:

    int bytesToInt(unsigned char* bytes, int size = 4);
    int bytesToInt1(unsigned char* bytes, int size= 4);
    void saveToCSV();
    InnoMakerUsb2CanLib::Innomaker_device_bittming getBittming(int index);
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);
    QString getDevicePath(libusb_device *dev);
    Ui::MainWindow *ui;
    InnoMakerUsb2CanLib *usbCanLib;
    InnoMakerUsb2CanLib::innomaker_can *can;
    InnoMakerUsb2CanLib::InnoMakerDevice *curDevice = NULL;
    QThread *recvThread = NULL;
    QThread *sendThread = NULL;
    Worker* worker = NULL;
    DelaySender *delaySender=NULL;
    QStandardItemModel *model;
    Formatter *formatter = new Formatter();
    int rxNum = 0;
    int txNum = 0;
    int rxErrorNum = 0;
    int scrollToBottomCounter = 0;
    int Can_Eff_Flag = 0x80000000;
    int Can_Rtr_Flag = 0x40000000;
    int Can_Err_Flag = 0x20000000;

    int  Can_Err_Ctrl = 0x00000004;
    int  Can_Err_BusOff = 0x00000040;
    int  Can_Err_Restarted = 0x00000100;
    int  Can_Err_Ctrl_UnSpec = 0x00;
    int  Can_Err_Ctrl_Rx_Overflow = 0x01;
    int  Can_Err_Ctrl_Tx_Overflow = 0x02;
    int  Can_Err_Ctrl_Rx_Warning = 0x04;
    int  Can_Err_Ctrl_Tx_Warning = 0x08;
    int  Can_Err_Ctrl_Rx_Passive = 0x10;
    int  Can_Err_Ctrl_Tx_Passive = 0x20;
    int Can_Id_Mask = 0x1fffffff;

public slots:
    void recv( Message message);
    void send(MessageA message);
    void errorString(QString);
    void delaySenderFinish();

};
#endif // MAINWINDOW_H

