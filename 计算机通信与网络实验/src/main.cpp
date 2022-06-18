#include "mainwindow.h"
extern FILE* logFp;
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    w.setWindowTitle("TFTP");
    return a.exec();
}
