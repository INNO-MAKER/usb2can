#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include<QDebug>
#include <QMetaType>
#include <QMessageBox>
#include "worker.h"
#include <qfile.h>
#include <QDateTime>
#include <QWidget>
#include <dbt.h>
#include <windows.h>
#include <QFileDialog>
#include "SetupAPI.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    can = new InnoMakerUsb2CanLib::innomaker_can();
    usbCanLib = new InnoMakerUsb2CanLib();
    usbCanLib->setup();

    this->ui->checkBox_hideErrorFrame->setChecked(true);

    this->model = new QStandardItemModel;
    this->ui->tableView->setModel(this->model);
    this->ui->tableView->verticalHeader()->setHidden(true);
    this->model->setHorizontalHeaderItem(0,new QStandardItem("SeqID"));
    this->model->setHorizontalHeaderItem(1,new QStandardItem("TimeStamp"));
    this->model->setHorizontalHeaderItem(2,new QStandardItem("Channel"));
    this->model->setHorizontalHeaderItem(3,new QStandardItem("Direction"));
    this->model->setHorizontalHeaderItem(4,new QStandardItem("FrameId"));
    this->model->setHorizontalHeaderItem(5,new QStandardItem("FrameType"));
    this->model->setHorizontalHeaderItem(6,new QStandardItem("FrameFormat"));
    this->model->setHorizontalHeaderItem(7,new QStandardItem("Length"));
    this->model->setHorizontalHeaderItem(8,new QStandardItem("FrameData"));
    this->model->setHorizontalHeaderItem(9,new QStandardItem("Message"));

    this->ui->lineEdit_frameId->setText("00 00 00 00");
    this->ui->lineEdit_data->setText("00 00 00 00 00 00 00 00");

    this->ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);


}

MainWindow::~MainWindow()
{

    usbCanLib->setdown();
    delete ui;
}


void MainWindow::on_pushButton_scanDevice_clicked()
{
    qDebug("on_pushButton_scanDevice_clicked");
    usbCanLib->scanInnoMakerDevice();
    int deviceNums = usbCanLib->getInnoMakerDeviceCount();
    this->ui->comboBox_deviceIndex->clear();
    for(int i = 0; i < deviceNums; i++) {
        this->ui->comboBox_deviceIndex->addItem("CAN" + QString::number(i));
    }

}


void MainWindow::on_pushButton_openDevice_clicked()
{
    if(curDevice != NULL) {


        if(recvThread != NULL) {
            worker->exit = true;
            recvThread->quit();
            recvThread->wait();
            worker = NULL;
            recvThread = NULL;
        }

        if(sendThread != NULL) {
            delaySender->exit();
            sendThread->quit();
            sendThread->wait();

            delaySender = NULL;
            sendThread = NULL;
            this->ui->pushButton_delaysend->setText("Delay Send");
        }
        /// Close Device
        usbCanLib->urbResetDevice(curDevice);
        usbCanLib->closeInnoMakerDevice(curDevice);

        /// Init tx context
        for (int i = 0; i < usbCanLib->innomaker_MAX_TX_URBS; i++)
        {
            can->tx_context[i].echo_id = usbCanLib->innomaker_MAX_TX_URBS;
        }


        curDevice = NULL;
        ui->pushButton_openDevice->setText("Open Device");
        ui->comboBox_deviceIndex->setEnabled(true);
        ui->comboBox_baudrate->setEnabled(true);
        ui->comboBox_mode->setEnabled(true);
        return;
    }

    int currentIndex = this->ui->comboBox_deviceIndex->currentIndex();
    if (currentIndex < 0) {
        QMessageBox msgBox;
        msgBox.setText("Select Device Empty.");
        msgBox.exec();
        return;
    }

    int deviceCount = this->ui->comboBox_deviceIndex->count();
    if(currentIndex < deviceCount) {
        curDevice = usbCanLib->getInnoMakerDevice(this->ui->comboBox_deviceIndex->currentIndex());
    }

    if(curDevice == NULL) {
        QMessageBox msgBox;
        msgBox.setText("Select Device Empty.");
        msgBox.exec();
        return;
    }

    /// Set usb can mode
    InnoMakerUsb2CanLib::UsbCanMode usbCanMode = InnoMakerUsb2CanLib::UsbCanMode::UsbCanModeLoopback;
    if(ui->comboBox_mode->currentIndex() == 0) {
        usbCanMode = InnoMakerUsb2CanLib::UsbCanModeNormal;
    }
    if(ui->comboBox_mode->currentIndex() == 1) {
        usbCanMode = InnoMakerUsb2CanLib::UsbCanModeLoopback;
    }
    if(ui->comboBox_mode->currentIndex() == 2) {
        usbCanMode = InnoMakerUsb2CanLib::UsbCanModeListenOnly;
    }

    InnoMakerUsb2CanLib::Innomaker_device_bittming bittming = this->getBittming(ui->comboBox_baudrate->currentIndex());
    int result = usbCanLib->urbSetupDevice(curDevice, usbCanMode, bittming);
    if (!result) {
        QMessageBox msgBox;
        msgBox.setText("Setup fail.");
        msgBox.exec();

        // Close Device
        usbCanLib->urbResetDevice(curDevice);
        usbCanLib->closeInnoMakerDevice(curDevice);
        curDevice = NULL;
        ui->pushButton_openDevice->setText("Open Device");
        ui->comboBox_deviceIndex->setEnabled(true);
        ui->comboBox_baudrate->setEnabled(true);
        ui->comboBox_mode->setEnabled(true);



        return;
    }

    /// Init tx context
    for (int i = 0; i < usbCanLib->innomaker_MAX_TX_URBS; i++)
    {
        can->tx_context[i].echo_id = usbCanLib->innomaker_MAX_TX_URBS;
    }

    /// Start Recv Thread
    recvThread= new QThread();
    worker = new Worker(usbCanLib,curDevice,can);
    worker->moveToThread(recvThread);
    connect( worker, &Worker::error, this, &MainWindow::errorString);
    connect( recvThread, &QThread::started, worker, &Worker::process);
    connect(worker,SIGNAL(recv(Message)),SLOT(recv(Message)),Qt::QueuedConnection );

    recvThread->start();
    ui->pushButton_openDevice->setText("Close Device");
    ui->comboBox_deviceIndex->setEnabled(false);
    ui->comboBox_baudrate->setEnabled(false);
    ui->comboBox_mode->setEnabled(false);

}



void MainWindow::on_pushButton_delaysend_clicked()
{

    if(sendThread != NULL) {
        delaySender->exit();
        sendThread->quit();
        sendThread->wait();

        delaySender = NULL;
        sendThread = NULL;
        this->ui->pushButton_delaysend->setText("Delay Send");
        return;
    }


    if(curDevice == NULL || curDevice->isOpen == false) {
        QMessageBox::information(NULL,"Tips","Device Not Open");
        return;
    }

    QString frameId = this->ui->lineEdit_frameId->text();
    if(frameId.size() == 0) {
        QMessageBox::information(NULL,"Tips","ID Empty");
        return;
    }
    QString frameData = this->ui->lineEdit_data->text();
    if(frameData.size() == 0) {
        QMessageBox::information(NULL,"Tips","Data Empty");
        return;
    }
    int frameToSend = this->ui->lineEdit_numToSend->text().toInt();
    if(frameToSend < 1 || frameToSend > 5000000) {
        QMessageBox::information(NULL,"Tips","Frame To Send Not Valid");

        return;
    }
    int interval = this->ui->lineEdit_sendInterval->text().toInt();
    if (interval < 1 || interval > 5000)
    {     QMessageBox::information(NULL,"Tips","Interval Not Valid");
        return;
    }

    sendThread = new QThread();
    delaySender = new DelaySender(usbCanLib,curDevice,can,frameId,frameData,interval,frameToSend,this->ui->checkBox_IDAutoInc->isChecked(),this->ui->checkBox_DataAutoInc->isChecked(),this->ui->comboBox_format->currentIndex() ==1,this->ui->comboBox_type->currentIndex() ==1);
    delaySender->moveToThread(sendThread);
    connect( delaySender, &DelaySender::error, this, &MainWindow::errorString);
    connect( delaySender, &DelaySender::finished, this, &MainWindow::delaySenderFinish);
    connect( sendThread, &QThread::started, delaySender, &DelaySender::process);
    connect(delaySender,SIGNAL(send(MessageA)),SLOT(send(MessageA)),Qt::QueuedConnection );
    sendThread->start();
    this->ui->pushButton_delaysend->setText("stop");
}

// 01 02 03 04 ==> 04 03 02 01
int MainWindow::bytesToInt(unsigned char* bytes, int size)
{
    int a = bytes[0] & 0xFF;
    a |= ((bytes[1] << 8) & 0xFF00);
    a |= ((bytes[2] << 16) & 0xFF0000);
    a |= ((bytes[3] << 24) & 0xFF000000);
    return a;
}

// 01 02 03 04 ==> 01 02 03 04
int MainWindow::bytesToInt1(unsigned char* bytes, int size)
{
    int a = bytes[3] & 0xFF;
    a |= ((bytes[2] << 8) & 0xFF00);
    a |= ((bytes[1] << 16) & 0xFF0000);
    a |= ((bytes[0] << 24) & 0xFF000000);
    return a;
}


void MainWindow::on_pushButton_send_clicked()
{
    qDebug("on_pushButton_send_clicked");
    InnoMakerUsb2CanLib::innomaker_host_frame frame;
    memset(&frame, 0, sizeof(InnoMakerUsb2CanLib::innomaker_host_frame));

    if(curDevice == NULL || curDevice->isOpen == false) {
        QMessageBox::information(NULL,"Tips","Device Not Open");
        return;
    }

    /// Data Hex String
    QString dataHexStr = this->ui->lineEdit_data->text().replace(" ","");
    if(dataHexStr.size() <= 0) {
        QMessageBox::information(NULL,"Tips","Data Empty");
        return;
    }
    int dataArrSize = dataHexStr.length() / 2;
    for (int i = 0; i < dataArrSize; i++) {
        QString hexByte = dataHexStr.mid(i*2, 2);
        frame.data[i] = hexByte.toUShort(nullptr, 16);
    }

    /// Id Hex String
    QString idHexStr = this->ui->lineEdit_frameId->text().replace(" ","");
    if(idHexStr.size() <= 0)  {
        QMessageBox::information(NULL,"Tips","ID","Empty");
        return;
    }
    int idArrSize = idHexStr.length() / 2;
    unsigned char idArr[4] = {0,0,0,0};
    for (int i = 0; i < idArrSize; i++) {
        QString hexByte = idHexStr.mid(i*2, 2);
        idArr[i] = hexByte.toUShort(nullptr, 16);
    }

    frame.can_id = bytesToInt1(idArr);
    if (this->ui->comboBox_format->currentIndex() == 1)
    {
        frame.can_id |= Can_Eff_Flag;
    }

    if (this->ui->comboBox_type->currentIndex() == 1)
    {
        frame.can_id |= Can_Rtr_Flag;
    }

    frame.can_dlc = dataArrSize;
    frame.channel = 0;
    frame.flags = 0;
    frame.reserved = 0;
    frame.timestamp_us = 0;

    // Find an empty context to keep track of transmission
    InnoMakerUsb2CanLib::innomaker_tx_context *txc = usbCanLib->innomaker_alloc_tx_context(can);
    if (txc->echo_id == 0xff)
    {
        qDebug("SEND FAIL: NETDEV_BUSY \n");
        return;
    }

    frame.echo_id = txc->echo_id;

    BYTE sendBuffer[sizeof(InnoMakerUsb2CanLib::innomaker_host_frame)];

    memcpy(sendBuffer, &frame, sizeof(InnoMakerUsb2CanLib::innomaker_host_frame));
    bool result = usbCanLib->sendInnoMakerDeviceBuf(curDevice, sendBuffer, sizeof(InnoMakerUsb2CanLib::innomaker_host_frame), 10);
    if (result) {

    }
    else {
        /// If send fail,free the context by echo id
        InnoMakerUsb2CanLib::innomaker_tx_context *txc1 = usbCanLib->innomaker_get_tx_context(can, txc->echo_id);
        if (txc1 != NULL)
        {
            usbCanLib->innomaker_free_tx_context(txc1);
        }
         qDebug("SEND FAIL: Free Context \n");
    }
}


void MainWindow::on_pushButton_clear_clicked()
{
    qDebug("on_pushButton_clear_clicked");
    this->model->clear();
    this->model->setHorizontalHeaderItem(0,new QStandardItem("SeqID"));
    this->model->setHorizontalHeaderItem(1,new QStandardItem("TimeStamp"));
    this->model->setHorizontalHeaderItem(2,new QStandardItem("Channel"));
    this->model->setHorizontalHeaderItem(3,new QStandardItem("Direction"));
    this->model->setHorizontalHeaderItem(4,new QStandardItem("FrameId"));
    this->model->setHorizontalHeaderItem(5,new QStandardItem("FrameType"));
    this->model->setHorizontalHeaderItem(6,new QStandardItem("FrameFormat"));
    this->model->setHorizontalHeaderItem(7,new QStandardItem("Length"));
    this->model->setHorizontalHeaderItem(8,new QStandardItem("FrameData"));
    this->model->setHorizontalHeaderItem(9,new QStandardItem("Message"));

    txNum = 0;
    rxNum = 0;
    rxErrorNum = 0;

}


void MainWindow::on_pushButton_export_clicked()
{
    qDebug("on_pushButton_export_clicked");
    saveToCSV();



}

void MainWindow::send( MessageA msg) {

}

void MainWindow::recv( Message msg) {

    int echoId = bytesToInt(msg.data);
    int frameId = bytesToInt(&msg.data[4]);
    BYTE dlc = msg.data[8];
    if (dlc > 8) {dlc = 8; }
    BYTE frameDataBytes[8];
    memset(frameDataBytes,0,8);
    memcpy(frameDataBytes,&msg.data[12],dlc);

    unsigned int timeStamp = (unsigned int)bytesToInt(&msg.data[20]);
    QString seqStr = QString::number(txNum + rxNum + rxErrorNum);
    QString timeStampStr= QString("%1.%2.%3").arg(timeStamp / 1000000,3,10,QChar('0')).arg((timeStamp % 1000000) / 1000,3,10,QChar('0')).arg( timeStamp % 1000,3,10,QChar('0'));
    QString channelStr ="0";
    QString frameIdStr = QString::number(frameId & Can_Id_Mask,16);
    QString frameLengthStr = QString::number(dlc,16);

    std::string data_str = "";

    char buf[3];
    for (int i = 0; i < dlc; i++) {
        sprintf_s(buf, "%02X", (unsigned int)frameDataBytes[i]);
        data_str += buf;
    }
    QString frameDataStr =QString(data_str.c_str());
    frameDataStr = "0X|"+ formatter->formatStringToHex(frameDataStr,dlc * 2,true);
    QString directionStr = "";
    QString frameFormatStr = "";
    QString frameTypeStr = "";

    if (rxNum > 4999999)
    {
        rxNum = 0;
    }
    if (txNum > 4999999)
    {
        txNum = 0;
    }
    if (rxErrorNum > 4999999)
    {
        rxErrorNum = 0;
    }

    directionStr = echoId == 0xffffffff ? QString("Recv[%1]").arg(rxNum) : QString("Send[%1]").arg(txNum) ;
    frameFormatStr = ((frameId & Can_Eff_Flag) != 0x00) ? "Extend Frame" : "Standard Frame";
    frameTypeStr = ((frameId & Can_Rtr_Flag) != 0x00) ? "Remote Frame" : "Data Frame";

    if (echoId == 0xffffffff)
    {

        if((frameId & Can_Err_Flag) != 0 && ui->checkBox_hideErrorFrame->isChecked()) {
            return ;
        }

        QString msgStr = "Success";
        if((frameId & Can_Err_Flag) != 0) {
            directionStr = QString("Recv Error[%1]").arg(rxErrorNum++);
            if((frameId & Can_Err_Restarted) != 0) {
                msgStr = QString("Fail: CAN_STATE_ERROR_ACTIVE");
            }
            if((frameId & Can_Err_BusOff) != 0) {
                msgStr =  QString("Fail: CAN_STATE_BUS_OFF");
            }
            if((frameId & Can_Err_Ctrl) != 0) {
                if ((frameDataBytes[1] & Can_Err_Ctrl_Tx_Warning) != 0x00 ||
                    (frameDataBytes[1] & Can_Err_Ctrl_Rx_Warning) != 0x00)
                {
                    msgStr =  QString("Fail: CAN_STATE_ERROR_WARNING");

                }
                else if ((frameDataBytes[1] & Can_Err_Ctrl_Tx_Passive) != 0x00 ||
                         (frameDataBytes[1] & Can_Err_Ctrl_Rx_Passive) != 0x00)
                {
                    msgStr =  QString("Fail: CAN_STATE_ERROR_PASSIVE");

                }
                else if ((frameDataBytes[1] & Can_Err_Ctrl_Tx_Overflow) != 0x00 ||
                         (frameDataBytes[1] & Can_Err_Ctrl_Rx_Overflow) != 0x00)
                {
                    msgStr =  QString("Fail: CAN_STATE_ERROR_OVERFLOW");

                }
                else
                {
                    msgStr =  QString("Fail: Other Reason");

                }
            }
        }



        QList<QStandardItem *> items;
        items.append(new QStandardItem(seqStr));
        items.append(new QStandardItem(timeStampStr));
        items.append(new QStandardItem(channelStr));
        items.append(new QStandardItem(directionStr));
        items.append(new QStandardItem(frameIdStr));
        items.append(new QStandardItem(frameTypeStr));
        items.append(new QStandardItem(frameFormatStr));
        items.append(new QStandardItem(frameLengthStr));
        items.append(new QStandardItem(frameDataStr));
        items.append(new QStandardItem(msgStr));
        this->model->appendRow(items);
        rxNum++;
        // qDebug("RX:%d",rxNum);
    } else {

        QList<QStandardItem *> items;
        items.append(new QStandardItem(seqStr));
        items.append(new QStandardItem(timeStampStr));
        items.append(new QStandardItem(channelStr));
        items.append(new QStandardItem(directionStr));
        items.append(new QStandardItem(frameIdStr));
        items.append(new QStandardItem(frameTypeStr));
        items.append(new QStandardItem(frameFormatStr));
        items.append(new QStandardItem(frameLengthStr));
        items.append(new QStandardItem(frameDataStr));
        items.append(new QStandardItem("Success"));
        this->model->appendRow(items);
        txNum++;
        // qDebug("TX:%d",txNum);
    }

    /// 如果被动接收,则直接滚动到底部
    if(delaySender == NULL ) {
        if( this->ui->comboBox_baudrate->currentIndex() >= 12) {
            if(scrollToBottomCounter++ > 10) {
                this->ui->tableView->scrollToBottom();
                scrollToBottomCounter = 0;
            }
        } else {
            this->ui->tableView->scrollToBottom();
        }
        return;
    }

    /// 如果是主动发送，则如果波特率过高而且间隔时间很短，则做一个计数器刷新，防止UI线程卡死
    int interval = this->ui->lineEdit_sendInterval->text().toInt();
    int frameToSend = this->ui->lineEdit_numToSend->text().toInt();
    if(interval <= 1 && this->ui->comboBox_baudrate->currentIndex() >= 12) {
        if(scrollToBottomCounter++ > 10) {
            this->ui->tableView->scrollToBottom();
            scrollToBottomCounter = 0;
        }
    } else {
        this->ui->tableView->scrollToBottom();
    }

}


InnoMakerUsb2CanLib::Innomaker_device_bittming MainWindow::getBittming(int index) {

    InnoMakerUsb2CanLib::Innomaker_device_bittming bittming;
    switch (index)
    {
    case 0: // 20K
        bittming.prop_seg = 6;
        bittming.phase_seg1 = 7;
        bittming.phase_seg2 = 2;
        bittming.sjw = 1;
        bittming.brp = 150;
        break;
    case 1: // 33.33K
        bittming.prop_seg = 3;
        bittming.phase_seg1 = 3;
        bittming.phase_seg2 = 1;
        bittming.sjw = 1;
        bittming.brp = 180;
        break;
    case 2: // 40K
        bittming.prop_seg = 6;
        bittming.phase_seg1 = 7;
        bittming.phase_seg2 = 2;
        bittming.sjw = 1;
        bittming.brp = 75;
        break;
    case 3: // 50K
        bittming.prop_seg = 6;
        bittming.phase_seg1 = 7;
        bittming.phase_seg2 = 2;
        bittming.sjw = 1;
        bittming.brp = 60;
        break;
    case 4: // 66.66K
        bittming.prop_seg = 3;
        bittming.phase_seg1 = 3;
        bittming.phase_seg2 = 1;
        bittming.sjw = 1;
        bittming.brp = 90;
        break;
    case 5: // 80K
        bittming.prop_seg = 3;
        bittming.phase_seg1 = 3;
        bittming.phase_seg2 = 1;
        bittming.sjw = 1;
        bittming.brp = 75;
        break;
    case 6: // 83.33K
        bittming.prop_seg = 3;
        bittming.phase_seg1 = 3;
        bittming.phase_seg2 = 1;
        bittming.sjw = 1;
        bittming.brp = 72;
        break;


    case 7: // 100K
        bittming.prop_seg = 6;
        bittming.phase_seg1 = 7;
        bittming.phase_seg2 = 2;
        bittming.sjw = 1;
        bittming.brp = 30;
        break;
    case 8: // 125K
        bittming.prop_seg = 6;
        bittming.phase_seg1 = 7;
        bittming.phase_seg2 = 2;
        bittming.sjw = 1;
        bittming.brp = 24;
        break;
    case 9: // 200K
        bittming.prop_seg = 6;
        bittming.phase_seg1 = 7;
        bittming.phase_seg2 = 2;
        bittming.sjw = 1;
        bittming.brp = 15;
        break;
    case 10: // 250K
        bittming.prop_seg = 6;
        bittming.phase_seg1 = 7;
        bittming.phase_seg2 = 2;
        bittming.sjw = 1;
        bittming.brp = 12;
        break;
    case 11: // 400K
        bittming.prop_seg = 3;
        bittming.phase_seg1 = 3;
        bittming.phase_seg2 = 1;
        bittming.sjw = 1;
        bittming.brp = 15;
        break;
    case 12: // 500K
        bittming.prop_seg = 6;
        bittming.phase_seg1 = 7;
        bittming.phase_seg2 = 2;
        bittming.sjw = 1;
        bittming.brp = 6;
        break;
    case 13: // 666K
        bittming.prop_seg = 3;
        bittming.phase_seg1 = 3;
        bittming.phase_seg2 = 2;
        bittming.sjw = 1;
        bittming.brp = 8;
        break;
    case 14: /// 800K
        bittming.prop_seg = 7;
        bittming.phase_seg1 = 8;
        bittming.phase_seg2 = 4;
        bittming.sjw = 1;
        bittming.brp = 3;
        break;
    case 15: /// 1000K
        bittming.prop_seg = 5;
        bittming.phase_seg1 = 6;
        bittming.phase_seg2 = 4;
        bittming.sjw = 1;
        bittming.brp = 3;
        break;
    default: /// 1000K
        bittming.prop_seg = 5;
        bittming.phase_seg1 = 6;
        bittming.phase_seg2 = 4;
        bittming.sjw = 1;
        bittming.brp = 3;
        break;
    }
    return bittming;
}

void MainWindow::errorString(QString str) {

}


void MainWindow::on_lineEdit_frameId_textEdited(const QString &arg1)
{
    formatter->adjustTextFieldToHex(ui->lineEdit_frameId,8,true);
}


void MainWindow::on_lineEdit_data_textEdited(const QString &arg1)
{
    formatter->adjustTextFieldToHex(ui->lineEdit_data,16,true);
}

void MainWindow::saveToCSV()
{

    QString textData;
    int rows = model->rowCount();
    int columns = model->columnCount();

    textData = "SeqID, TimeStamp, Channel, Direction,FrameId,FrameType,FrameFormat,Length,FrameData,Message\n";

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            textData += model->data(model->index(i,j)).toString();
            textData += ", ";  // for .csv file format
        }
        textData += "\n";
    }

    QString fileName = QFileDialog::getSaveFileName(this,QString::fromLocal8Bit("Export CSV File"),"",tr("CSV File(*.csv)"));
    QFile csvFile(fileName);

    if(csvFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QTextStream out(&csvFile);
        out << textData;
        csvFile.close();
    }
}

void MainWindow::delaySenderFinish() {
    delaySender = NULL;
    sendThread = NULL;
    scrollToBottomCounter = 0;
    this->ui->pushButton_delaysend->setText("Delay Send");
    this->ui->tableView->scrollToBottom();
}




bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    Q_UNUSED(eventType);
    MSG* msg = reinterpret_cast<MSG*>(message);
    int msgType = msg->message;
    if(msgType == WM_DEVICECHANGE)
    {

        if(recvThread != NULL) {
            worker->exit = true;
            recvThread->quit();
            recvThread->wait();
            worker = NULL;
            recvThread = NULL;
        }
        if(sendThread != NULL) {
            delaySender->exit();
            sendThread->quit();
            sendThread->wait();
            delaySender = NULL;
            sendThread = NULL;
            this->ui->pushButton_delaysend->setText("Delay Send");
        }

        if(curDevice != NULL) {
            usbCanLib->urbResetDevice(curDevice);
            usbCanLib->closeInnoMakerDevice(curDevice);
            curDevice = NULL;
            ui->pushButton_openDevice->setText("Open Device");
            ui->comboBox_deviceIndex->setEnabled(true);
            ui->comboBox_baudrate->setEnabled(true);
            ui->comboBox_mode->setEnabled(true);
        }

    }

    return false;
}

