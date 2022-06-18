#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Windows.h"
#include <string>
#include <QDir>
#include <qfile.h>
#include <qfiledialog.h>
#include <QScrollBar>
#include <qmessagebox.h>
#include <cstring>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <qwidget.h>
#include <qtreewidget.h>
#include <qicon.h>
#include <QIcon>
#include <stdlib.h>
#define LOGPATH "D:\\hook\\log.txt"
using namespace std;




MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags()& ~Qt::WindowMaximizeButtonHint);
    setFixedSize(this->width(), this->height());
    this->setWindowIcon(QIcon(":/img/icons8-reddit-480.png"));
    //ui->treeWidget->setColumnWidth(0,500);
    ui->treeWidget->setHeaderLabels(QStringList()<<"data");
    for(int i = 0; i < type2Func.size();i++){
        topItems.push_back(new QTreeWidgetItem(QStringList()<<type2Func[i]));
        qmap.insert(type2Func[i],i);
        //countmap.insert(type2Func[i],0);
    }
    ui->treeWidget->addTopLevelItems(topItems);


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString directory =
             QDir::toNativeSeparators(QFileDialog::getOpenFileName(this,tr("选择"),"..\\InjectDll\\Debug","可执行程序(*.exe)"));
    if(!directory.isEmpty()){
         if(ui->comboBox->findText(directory) == -1){
             ui->comboBox->addItem(directory);
         }
         ui->comboBox->setCurrentIndex(ui->comboBox->findText(directory));
         fileName = directory;
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    if(fileName != NULL){
        STARTUPINFOA startupInfo;
        PROCESS_INFORMATION  processInfo;
        ZeroMemory(&startupInfo,sizeof(startupInfo));
        ZeroMemory(&processInfo,sizeof(processInfo));
        QByteArray qba = fileName.toLatin1();
        char *str = qba.data();
        CreateProcessA("D:\\hook\\InjectDll\\Debug\\Console.exe",str, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &startupInfo, &processInfo);

        ui->textBrowser->setText(QString::number(processInfo.dwProcessId));
        ui->textBrowser_2->setText(QString(QLatin1String(priorityStr[GetProcessPriority(processInfo.hProcess)])));
        ui->textBrowser_3->setText(fileName.mid(fileName.lastIndexOf('\\')+1));
        buttonClicked = true;


    }else{
        QMessageBox::information(this,"提示","请先选择文件！");
    }
}

void MainWindow::on_pushButton_3_clicked()
{
    if(buttonClicked){

        ifstream i(LOGPATH);
        string s;
        //当前状态
        //QString curtype;
        //QString qs;
        int count = 0;
        //类型紧接着为时间 单独列一个child
        bool next = false;
        QTreeWidgetItem * topItem;
        QTreeWidgetItem * time;
        while (getline(i,s)&&count<100000){
            QString qs = QString::fromStdString(s);
            if(type2Func.contains(qs)){
                topItem = topItems[qmap[qs]];
                next = true;
                continue;
            }
                if(next){
                    time =new QTreeWidgetItem(QStringList()<<qs);
                    topItem->addChild(time);
                    next = false;
                }else{
                    time->addChild(new QTreeWidgetItem(QStringList()<<qs));
                }
            count++;
        }
        buttonClicked = false;

    }else{
        QMessageBox::information(this,"提示","请先点击测试");
    }
}

void MainWindow::on_pushButton_4_clicked()
{
    for(int i = 0;i < topItems.size();i++){
        int count = topItems[i]->childCount();
        for (int j = 0;j < count; j++) {
            topItems[i]->removeChild(topItems[i]->child(j));
        }
    }
}
int MainWindow::GetProcessPriority(HANDLE hProcess)
{
    switch (GetPriorityClass(hProcess))
    {
    case NORMAL_PRIORITY_CLASS:return 0;
    case  IDLE_PRIORITY_CLASS:return 1;
    case REALTIME_PRIORITY_CLASS:return 2;
    case HIGH_PRIORITY_CLASS:return 3;
    case ABOVE_NORMAL_PRIORITY_CLASS:return 5;
    case BELOW_NORMAL_PRIORITY_CLASS:return 6;
    default:return 4;
    }
}

void MainWindow::on_pushButton_5_clicked()
{
    system("D:\\hook\\log.txt");
}
