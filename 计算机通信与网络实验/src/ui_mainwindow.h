/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QWidget *verticalLayoutWidget_2;
    QVBoxLayout *verticalLayout_3;
    QTabWidget *tabWidget;
    QWidget *tab;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_6;
    QPushButton *PathChoose;
    QLineEdit *PathShow;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label;
    QLineEdit *uploadLocalIP;
    QLabel *label_2;
    QLineEdit *uploadServerIP;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_3;
    QComboBox *uploadMode;
    QPushButton *uploadButton;
    QSpacerItem *horizontalSpacer_2;
    QWidget *tab_2;
    QWidget *verticalLayoutWidget_3;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_6;
    QLineEdit *downloadServerIP;
    QLabel *label_4;
    QLineEdit *downloadServerFilename;
    QHBoxLayout *horizontalLayout;
    QLabel *label_7;
    QLineEdit *downloadLocalIP;
    QLabel *label_5;
    QLineEdit *downloadLocalFilename;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_8;
    QComboBox *downloadMode;
    QPushButton *downloadButton;
    QSpacerItem *horizontalSpacer;
    QTextBrowser *output;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(594, 401);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        QFont font;
        font.setFamily(QString::fromUtf8("\351\273\221\344\275\223"));
        font.setPointSize(12);
        centralwidget->setFont(font);
        verticalLayoutWidget_2 = new QWidget(centralwidget);
        verticalLayoutWidget_2->setObjectName(QString::fromUtf8("verticalLayoutWidget_2"));
        verticalLayoutWidget_2->setGeometry(QRect(0, 0, 591, 351));
        verticalLayout_3 = new QVBoxLayout(verticalLayoutWidget_2);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        tabWidget = new QTabWidget(verticalLayoutWidget_2);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setFont(font);
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        verticalLayoutWidget = new QWidget(tab);
        verticalLayoutWidget->setObjectName(QString::fromUtf8("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(0, 0, 571, 114));
        verticalLayout_2 = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        PathChoose = new QPushButton(verticalLayoutWidget);
        PathChoose->setObjectName(QString::fromUtf8("PathChoose"));
        PathChoose->setFont(font);

        horizontalLayout_6->addWidget(PathChoose);

        PathShow = new QLineEdit(verticalLayoutWidget);
        PathShow->setObjectName(QString::fromUtf8("PathShow"));

        horizontalLayout_6->addWidget(PathShow);


        verticalLayout_2->addLayout(horizontalLayout_6);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        label = new QLabel(verticalLayoutWidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setFont(font);

        horizontalLayout_5->addWidget(label);

        uploadLocalIP = new QLineEdit(verticalLayoutWidget);
        uploadLocalIP->setObjectName(QString::fromUtf8("uploadLocalIP"));

        horizontalLayout_5->addWidget(uploadLocalIP);

        label_2 = new QLabel(verticalLayoutWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setFont(font);

        horizontalLayout_5->addWidget(label_2);

        uploadServerIP = new QLineEdit(verticalLayoutWidget);
        uploadServerIP->setObjectName(QString::fromUtf8("uploadServerIP"));

        horizontalLayout_5->addWidget(uploadServerIP);


        verticalLayout_2->addLayout(horizontalLayout_5);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_3 = new QLabel(verticalLayoutWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setFont(font);

        horizontalLayout_3->addWidget(label_3);

        uploadMode = new QComboBox(verticalLayoutWidget);
        uploadMode->addItem(QString());
        uploadMode->addItem(QString());
        uploadMode->setObjectName(QString::fromUtf8("uploadMode"));
        uploadMode->setFont(font);

        horizontalLayout_3->addWidget(uploadMode);

        uploadButton = new QPushButton(verticalLayoutWidget);
        uploadButton->setObjectName(QString::fromUtf8("uploadButton"));
        uploadButton->setFont(font);

        horizontalLayout_3->addWidget(uploadButton);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);


        verticalLayout_2->addLayout(horizontalLayout_3);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        verticalLayoutWidget_3 = new QWidget(tab_2);
        verticalLayoutWidget_3->setObjectName(QString::fromUtf8("verticalLayoutWidget_3"));
        verticalLayoutWidget_3->setGeometry(QRect(0, 0, 581, 111));
        verticalLayout_4 = new QVBoxLayout(verticalLayoutWidget_3);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label_6 = new QLabel(verticalLayoutWidget_3);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        horizontalLayout_4->addWidget(label_6);

        downloadServerIP = new QLineEdit(verticalLayoutWidget_3);
        downloadServerIP->setObjectName(QString::fromUtf8("downloadServerIP"));

        horizontalLayout_4->addWidget(downloadServerIP);

        label_4 = new QLabel(verticalLayoutWidget_3);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        horizontalLayout_4->addWidget(label_4);

        downloadServerFilename = new QLineEdit(verticalLayoutWidget_3);
        downloadServerFilename->setObjectName(QString::fromUtf8("downloadServerFilename"));

        horizontalLayout_4->addWidget(downloadServerFilename);


        verticalLayout_4->addLayout(horizontalLayout_4);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label_7 = new QLabel(verticalLayoutWidget_3);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        horizontalLayout->addWidget(label_7);

        downloadLocalIP = new QLineEdit(verticalLayoutWidget_3);
        downloadLocalIP->setObjectName(QString::fromUtf8("downloadLocalIP"));

        horizontalLayout->addWidget(downloadLocalIP);

        label_5 = new QLabel(verticalLayoutWidget_3);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        horizontalLayout->addWidget(label_5);

        downloadLocalFilename = new QLineEdit(verticalLayoutWidget_3);
        downloadLocalFilename->setObjectName(QString::fromUtf8("downloadLocalFilename"));

        horizontalLayout->addWidget(downloadLocalFilename);


        verticalLayout_4->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_8 = new QLabel(verticalLayoutWidget_3);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        horizontalLayout_2->addWidget(label_8);

        downloadMode = new QComboBox(verticalLayoutWidget_3);
        downloadMode->addItem(QString());
        downloadMode->addItem(QString());
        downloadMode->setObjectName(QString::fromUtf8("downloadMode"));

        horizontalLayout_2->addWidget(downloadMode);

        downloadButton = new QPushButton(verticalLayoutWidget_3);
        downloadButton->setObjectName(QString::fromUtf8("downloadButton"));

        horizontalLayout_2->addWidget(downloadButton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);


        verticalLayout_4->addLayout(horizontalLayout_2);

        tabWidget->addTab(tab_2, QString());

        verticalLayout_3->addWidget(tabWidget);

        output = new QTextBrowser(verticalLayoutWidget_2);
        output->setObjectName(QString::fromUtf8("output"));
        output->setFont(font);

        verticalLayout_3->addWidget(output);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 594, 23));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        PathChoose->setText(QCoreApplication::translate("MainWindow", "Choose File", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Local IP", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "Server IP", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "Mode", nullptr));
        uploadMode->setItemText(0, QCoreApplication::translate("MainWindow", "ascii", nullptr));
        uploadMode->setItemText(1, QCoreApplication::translate("MainWindow", "bin", nullptr));

        uploadButton->setText(QCoreApplication::translate("MainWindow", "Upload", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab), QCoreApplication::translate("MainWindow", "UPLOAD", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "Server IP", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "Server Filename", nullptr));
        label_7->setText(QCoreApplication::translate("MainWindow", "Local IP", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "Local Filename", nullptr));
        label_8->setText(QCoreApplication::translate("MainWindow", "Mode", nullptr));
        downloadMode->setItemText(0, QCoreApplication::translate("MainWindow", "ascii", nullptr));
        downloadMode->setItemText(1, QCoreApplication::translate("MainWindow", "bin", nullptr));

        downloadButton->setText(QCoreApplication::translate("MainWindow", "Download", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QCoreApplication::translate("MainWindow", "DOWNLOAD", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
