VERSION = 2.0.1

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20

include(./QXlsx/QXlsx.pri)

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    addnewconsumption.cpp \
    addnewcustomer.cpp \
    databasemanager.cpp \
    logmanager.cpp \
    main.cpp \
    mainwindow.cpp \
    searchcomsumption.cpp \
    searchcustomer.cpp \
    updateconsumption.cpp \
    updatecustomer.cpp \
    utils.cpp

HEADERS += \
    addnewconsumption.h \
    addnewcustomer.h \
    databasemanager.h \
    logmanager.h \
    mainwindow.h \
    namespace.h \
    searchcomsumption.h \
    searchcustomer.h \
    updateconsumption.h \
    updatecustomer.h \
    utils.h

FORMS += \
    addnewconsumption.ui \
    addnewcustomer.ui \
    mainwindow.ui \
    searchcomsumption.ui \
    searchcustomer.ui \
    updateconsumption.ui \
    updatecustomer.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
