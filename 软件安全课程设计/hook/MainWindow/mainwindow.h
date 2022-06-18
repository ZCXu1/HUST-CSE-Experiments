#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "windows.h"
#include <qtreewidget.h>
#include <QTreeWidgetItem>
#include <qlist.h>
#include <qmap.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QString fileName;
    bool buttonClicked = false;
    QList<QTreeWidgetItem *> topItems;
    QVector<QString> type2Func = {"MessageBoxA","MessageBoxW","CreateFile","WriteFile","ReadFile","HeapCreate",
                                 "HeapDestroy","HeapFree","RegCreateKeyEx","RegSetValueEx","RegCloseKey",
                                 "RegOpenKeyEx","RegDeleteValue","socket","bind","send","recv","connect","CopyFileA","CopyFile"};
    QMap<QString,int> qmap;
    char priorityStr[8][20] = { "NORMAL", "IDLE" , "REALTIME", "HIGH", "NULL", "ABOVENORMAL", "BELOWNORMAL" };
    int GetProcessPriority(HANDLE hProcess);

private slots:


    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
