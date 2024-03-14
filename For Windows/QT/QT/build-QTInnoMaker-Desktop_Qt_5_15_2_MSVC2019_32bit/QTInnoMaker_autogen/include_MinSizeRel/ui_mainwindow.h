/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_5;
    QVBoxLayout *verticalLayout_3;
    QSpacerItem *verticalSpacer_2;
    QHBoxLayout *horizontalLayout_11;
    QSpacerItem *horizontalSpacer;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QComboBox *comboBox_deviceIndex;
    QPushButton *pushButton_scanDevice;
    QPushButton *pushButton_openDevice;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QComboBox *comboBox_format;
    QLabel *label_3;
    QComboBox *comboBox_type;
    QLabel *label_4;
    QComboBox *comboBox_mode;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_5;
    QComboBox *comboBox_baudrate;
    QLabel *label_6;
    QLabel *label_8;
    QLineEdit *lineEdit_frameId;
    QLabel *label_7;
    QLineEdit *lineEdit_data;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_9;
    QLineEdit *lineEdit_numToSend;
    QLabel *label_10;
    QLineEdit *lineEdit_sendInterval;
    QCheckBox *checkBox_IDAutoInc;
    QCheckBox *checkBox_DataAutoInc;
    QCheckBox *checkBox_hideErrorFrame;
    QHBoxLayout *horizontalLayout_5;
    QPushButton *pushButton_delaysend;
    QPushButton *pushButton_send;
    QPushButton *pushButton_clear;
    QPushButton *pushButton_export;
    QSpacerItem *horizontalSpacer_2;
    QSpacerItem *verticalSpacer;
    QVBoxLayout *verticalLayout_4;
    QSpacerItem *verticalSpacer_4;
    QHBoxLayout *horizontalLayout_12;
    QSpacerItem *horizontalSpacer_3;
    QTableView *tableView;
    QSpacerItem *horizontalSpacer_4;
    QSpacerItem *verticalSpacer_3;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1059, 569);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout_5 = new QVBoxLayout(centralwidget);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalSpacer_2 = new QSpacerItem(12, 12, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout_3->addItem(verticalSpacer_2);

        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setObjectName(QString::fromUtf8("horizontalLayout_11"));
        horizontalSpacer = new QSpacerItem(12, 18, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_11->addItem(horizontalSpacer);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(label);

        comboBox_deviceIndex = new QComboBox(centralwidget);
        comboBox_deviceIndex->setObjectName(QString::fromUtf8("comboBox_deviceIndex"));

        horizontalLayout->addWidget(comboBox_deviceIndex);

        pushButton_scanDevice = new QPushButton(centralwidget);
        pushButton_scanDevice->setObjectName(QString::fromUtf8("pushButton_scanDevice"));

        horizontalLayout->addWidget(pushButton_scanDevice);

        pushButton_openDevice = new QPushButton(centralwidget);
        pushButton_openDevice->setObjectName(QString::fromUtf8("pushButton_openDevice"));

        horizontalLayout->addWidget(pushButton_openDevice);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        sizePolicy.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
        label_2->setSizePolicy(sizePolicy);

        horizontalLayout_2->addWidget(label_2);

        comboBox_format = new QComboBox(centralwidget);
        comboBox_format->addItem(QString());
        comboBox_format->addItem(QString());
        comboBox_format->setObjectName(QString::fromUtf8("comboBox_format"));

        horizontalLayout_2->addWidget(comboBox_format);

        label_3 = new QLabel(centralwidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        sizePolicy.setHeightForWidth(label_3->sizePolicy().hasHeightForWidth());
        label_3->setSizePolicy(sizePolicy);

        horizontalLayout_2->addWidget(label_3);

        comboBox_type = new QComboBox(centralwidget);
        comboBox_type->addItem(QString());
        comboBox_type->addItem(QString());
        comboBox_type->setObjectName(QString::fromUtf8("comboBox_type"));

        horizontalLayout_2->addWidget(comboBox_type);

        label_4 = new QLabel(centralwidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        sizePolicy.setHeightForWidth(label_4->sizePolicy().hasHeightForWidth());
        label_4->setSizePolicy(sizePolicy);

        horizontalLayout_2->addWidget(label_4);

        comboBox_mode = new QComboBox(centralwidget);
        comboBox_mode->addItem(QString());
        comboBox_mode->addItem(QString());
        comboBox_mode->addItem(QString());
        comboBox_mode->setObjectName(QString::fromUtf8("comboBox_mode"));

        horizontalLayout_2->addWidget(comboBox_mode);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_5 = new QLabel(centralwidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        sizePolicy.setHeightForWidth(label_5->sizePolicy().hasHeightForWidth());
        label_5->setSizePolicy(sizePolicy);

        horizontalLayout_3->addWidget(label_5);

        comboBox_baudrate = new QComboBox(centralwidget);
        comboBox_baudrate->addItem(QString());
        comboBox_baudrate->addItem(QString());
        comboBox_baudrate->addItem(QString());
        comboBox_baudrate->addItem(QString());
        comboBox_baudrate->addItem(QString());
        comboBox_baudrate->addItem(QString());
        comboBox_baudrate->addItem(QString());
        comboBox_baudrate->addItem(QString());
        comboBox_baudrate->addItem(QString());
        comboBox_baudrate->addItem(QString());
        comboBox_baudrate->addItem(QString());
        comboBox_baudrate->addItem(QString());
        comboBox_baudrate->addItem(QString());
        comboBox_baudrate->addItem(QString());
        comboBox_baudrate->addItem(QString());
        comboBox_baudrate->addItem(QString());
        comboBox_baudrate->setObjectName(QString::fromUtf8("comboBox_baudrate"));

        horizontalLayout_3->addWidget(comboBox_baudrate);

        label_6 = new QLabel(centralwidget);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        sizePolicy.setHeightForWidth(label_6->sizePolicy().hasHeightForWidth());
        label_6->setSizePolicy(sizePolicy);

        horizontalLayout_3->addWidget(label_6);

        label_8 = new QLabel(centralwidget);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        sizePolicy.setHeightForWidth(label_8->sizePolicy().hasHeightForWidth());
        label_8->setSizePolicy(sizePolicy);

        horizontalLayout_3->addWidget(label_8);

        lineEdit_frameId = new QLineEdit(centralwidget);
        lineEdit_frameId->setObjectName(QString::fromUtf8("lineEdit_frameId"));

        horizontalLayout_3->addWidget(lineEdit_frameId);

        label_7 = new QLabel(centralwidget);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        sizePolicy.setHeightForWidth(label_7->sizePolicy().hasHeightForWidth());
        label_7->setSizePolicy(sizePolicy);

        horizontalLayout_3->addWidget(label_7);

        lineEdit_data = new QLineEdit(centralwidget);
        lineEdit_data->setObjectName(QString::fromUtf8("lineEdit_data"));

        horizontalLayout_3->addWidget(lineEdit_data);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label_9 = new QLabel(centralwidget);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        sizePolicy.setHeightForWidth(label_9->sizePolicy().hasHeightForWidth());
        label_9->setSizePolicy(sizePolicy);

        horizontalLayout_4->addWidget(label_9);

        lineEdit_numToSend = new QLineEdit(centralwidget);
        lineEdit_numToSend->setObjectName(QString::fromUtf8("lineEdit_numToSend"));

        horizontalLayout_4->addWidget(lineEdit_numToSend);

        label_10 = new QLabel(centralwidget);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        sizePolicy.setHeightForWidth(label_10->sizePolicy().hasHeightForWidth());
        label_10->setSizePolicy(sizePolicy);

        horizontalLayout_4->addWidget(label_10);

        lineEdit_sendInterval = new QLineEdit(centralwidget);
        lineEdit_sendInterval->setObjectName(QString::fromUtf8("lineEdit_sendInterval"));

        horizontalLayout_4->addWidget(lineEdit_sendInterval);

        checkBox_IDAutoInc = new QCheckBox(centralwidget);
        checkBox_IDAutoInc->setObjectName(QString::fromUtf8("checkBox_IDAutoInc"));

        horizontalLayout_4->addWidget(checkBox_IDAutoInc);

        checkBox_DataAutoInc = new QCheckBox(centralwidget);
        checkBox_DataAutoInc->setObjectName(QString::fromUtf8("checkBox_DataAutoInc"));

        horizontalLayout_4->addWidget(checkBox_DataAutoInc);

        checkBox_hideErrorFrame = new QCheckBox(centralwidget);
        checkBox_hideErrorFrame->setObjectName(QString::fromUtf8("checkBox_hideErrorFrame"));

        horizontalLayout_4->addWidget(checkBox_hideErrorFrame);


        verticalLayout->addLayout(horizontalLayout_4);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        pushButton_delaysend = new QPushButton(centralwidget);
        pushButton_delaysend->setObjectName(QString::fromUtf8("pushButton_delaysend"));

        horizontalLayout_5->addWidget(pushButton_delaysend);

        pushButton_send = new QPushButton(centralwidget);
        pushButton_send->setObjectName(QString::fromUtf8("pushButton_send"));

        horizontalLayout_5->addWidget(pushButton_send);

        pushButton_clear = new QPushButton(centralwidget);
        pushButton_clear->setObjectName(QString::fromUtf8("pushButton_clear"));

        horizontalLayout_5->addWidget(pushButton_clear);

        pushButton_export = new QPushButton(centralwidget);
        pushButton_export->setObjectName(QString::fromUtf8("pushButton_export"));

        horizontalLayout_5->addWidget(pushButton_export);


        verticalLayout->addLayout(horizontalLayout_5);


        horizontalLayout_11->addLayout(verticalLayout);

        horizontalSpacer_2 = new QSpacerItem(12, 18, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_11->addItem(horizontalSpacer_2);


        verticalLayout_3->addLayout(horizontalLayout_11);

        verticalSpacer = new QSpacerItem(12, 12, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout_3->addItem(verticalSpacer);


        verticalLayout_5->addLayout(verticalLayout_3);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        verticalSpacer_4 = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout_4->addItem(verticalSpacer_4);

        horizontalLayout_12 = new QHBoxLayout();
        horizontalLayout_12->setObjectName(QString::fromUtf8("horizontalLayout_12"));
        horizontalSpacer_3 = new QSpacerItem(12, 12, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_12->addItem(horizontalSpacer_3);

        tableView = new QTableView(centralwidget);
        tableView->setObjectName(QString::fromUtf8("tableView"));

        horizontalLayout_12->addWidget(tableView);

        horizontalSpacer_4 = new QSpacerItem(12, 12, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_12->addItem(horizontalSpacer_4);


        verticalLayout_4->addLayout(horizontalLayout_12);

        verticalSpacer_3 = new QSpacerItem(12, 0, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout_4->addItem(verticalSpacer_3);


        verticalLayout_5->addLayout(verticalLayout_4);

        MainWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        statusbar->setMinimumSize(QSize(0, 20));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Device Index:", nullptr));
        pushButton_scanDevice->setText(QCoreApplication::translate("MainWindow", "Scan Device", nullptr));
        pushButton_openDevice->setText(QCoreApplication::translate("MainWindow", "Open Device", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "Format:", nullptr));
        comboBox_format->setItemText(0, QCoreApplication::translate("MainWindow", "standard", nullptr));
        comboBox_format->setItemText(1, QCoreApplication::translate("MainWindow", "extended", nullptr));

        label_3->setText(QCoreApplication::translate("MainWindow", "Type:", nullptr));
        comboBox_type->setItemText(0, QCoreApplication::translate("MainWindow", "data frame", nullptr));
        comboBox_type->setItemText(1, QCoreApplication::translate("MainWindow", "remote frame", nullptr));

        label_4->setText(QCoreApplication::translate("MainWindow", "Mode:", nullptr));
        comboBox_mode->setItemText(0, QCoreApplication::translate("MainWindow", "normal", nullptr));
        comboBox_mode->setItemText(1, QCoreApplication::translate("MainWindow", "loopback", nullptr));
        comboBox_mode->setItemText(2, QCoreApplication::translate("MainWindow", "listen-only", nullptr));

        label_5->setText(QCoreApplication::translate("MainWindow", "BaudRate:", nullptr));
        comboBox_baudrate->setItemText(0, QCoreApplication::translate("MainWindow", "20K", nullptr));
        comboBox_baudrate->setItemText(1, QCoreApplication::translate("MainWindow", "33.33K", nullptr));
        comboBox_baudrate->setItemText(2, QCoreApplication::translate("MainWindow", "40K", nullptr));
        comboBox_baudrate->setItemText(3, QCoreApplication::translate("MainWindow", "50K", nullptr));
        comboBox_baudrate->setItemText(4, QCoreApplication::translate("MainWindow", "66.66K", nullptr));
        comboBox_baudrate->setItemText(5, QCoreApplication::translate("MainWindow", "80K", nullptr));
        comboBox_baudrate->setItemText(6, QCoreApplication::translate("MainWindow", "83.33K", nullptr));
        comboBox_baudrate->setItemText(7, QCoreApplication::translate("MainWindow", "100K", nullptr));
        comboBox_baudrate->setItemText(8, QCoreApplication::translate("MainWindow", "125K", nullptr));
        comboBox_baudrate->setItemText(9, QCoreApplication::translate("MainWindow", "200K", nullptr));
        comboBox_baudrate->setItemText(10, QCoreApplication::translate("MainWindow", "250K", nullptr));
        comboBox_baudrate->setItemText(11, QCoreApplication::translate("MainWindow", "400K", nullptr));
        comboBox_baudrate->setItemText(12, QCoreApplication::translate("MainWindow", "500K", nullptr));
        comboBox_baudrate->setItemText(13, QCoreApplication::translate("MainWindow", "666K", nullptr));
        comboBox_baudrate->setItemText(14, QCoreApplication::translate("MainWindow", "800K", nullptr));
        comboBox_baudrate->setItemText(15, QCoreApplication::translate("MainWindow", "1000K", nullptr));

        label_6->setText(QCoreApplication::translate("MainWindow", "Sample Point:87%", nullptr));
        label_8->setText(QCoreApplication::translate("MainWindow", "Frame ID:", nullptr));
        lineEdit_frameId->setText(QString());
        label_7->setText(QCoreApplication::translate("MainWindow", "Data:", nullptr));
        label_9->setText(QCoreApplication::translate("MainWindow", "NumberToSend(1-5000k):", nullptr));
        lineEdit_numToSend->setText(QString());
        label_10->setText(QCoreApplication::translate("MainWindow", "Send Interval(0ms-5000ms):", nullptr));
        lineEdit_sendInterval->setText(QString());
        checkBox_IDAutoInc->setText(QCoreApplication::translate("MainWindow", "ID Auto Inc.", nullptr));
        checkBox_DataAutoInc->setText(QCoreApplication::translate("MainWindow", "Data Auto Inc.", nullptr));
        checkBox_hideErrorFrame->setText(QCoreApplication::translate("MainWindow", "Hide Error Frame:", nullptr));
        pushButton_delaysend->setText(QCoreApplication::translate("MainWindow", "Delayed Send", nullptr));
        pushButton_send->setText(QCoreApplication::translate("MainWindow", "Send", nullptr));
        pushButton_clear->setText(QCoreApplication::translate("MainWindow", "Clear", nullptr));
        pushButton_export->setText(QCoreApplication::translate("MainWindow", "Export", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
