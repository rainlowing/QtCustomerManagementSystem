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
    w.show();
    return a.exec();
}
