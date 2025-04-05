#include "mainwindow.h"

#include <QApplication>
#include <QSqlDatabase>
#include <QDebug>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QIcon icon(":/Iconshock-Vista-General-House.ico");
    a.setWindowIcon(icon);
    MainWindow w;
    w.setWindowIcon(icon);
    w.setWindowTitle("管理系统");
    w.show();
    return a.exec();
}
