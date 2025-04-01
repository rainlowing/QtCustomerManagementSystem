#ifndef LOGMANAGER_H
#define LOGMANAGER_H

#include <QObject>
#include <QCoreApplication>
#include <QDir>
#include <QDebug>
#include <QMap>
#include <QFile>
#include <QTextStream>
#include <QMutex>
#include <QMutexLocker>
#include <QMetaEnum>
#include <QDate>

#include "namespace.h"

class LogManager : public QObject {
    Q_OBJECT

private:
    explicit LogManager();
    bool init();
    void updateLogFile(Log::Category);

public:
    LogManager(const LogManager &) = delete;
    LogManager &operator=(const LogManager &) = delete;
    LogManager(LogManager &&) = delete;
    LogManager &operator=(LogManager &&) = delete;
    ~LogManager();

    static LogManager &getInstance();

    void setLogLevel(Log::Level l);
    void log(Log::Category c, Log::Level l, const QString &m);

private:
    QString                                             m_logRoot;
    Log::Category                                       m_category;
    Log::Level                                          m_level;
    QMap<Log::Category, QFile*>                         logFiles;
    QMap<Log::Category, QTextStream*>                   logStreams;
    QMutex                                              m_mutex;

};

#endif // LOGMANAGER_H
