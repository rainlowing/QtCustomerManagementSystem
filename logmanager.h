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

#include "utils.h"

class LogManager : public QObject {
    Q_OBJECT

public:
    explicit LogManager(QObject *parent = nullptr);
    LogManager(const LogManager &) = delete;
    LogManager &operator=(const LogManager &) = delete;
    LogManager(LogManager &&) = delete;
    LogManager &operator=(LogManager &&) = delete;
    ~LogManager();

    bool initializeLogs();

    // void setLogCategory(Log::Category c);
    void setLogLevel(Log::Level l);
    void log(Log::Category c, Log::Level l, const QString &m);

private:
    QString                                             m_logRoot;
    Log::Category                                       m_category;
    Log::Level                                          m_level;
    QMap<Log::Category, QFile*>                         logFiles;
    QMap<Log::Category, QTextStream*>                   logStreams;
    // QMap<Log::Category, QString>                        subLogPaths;
    QMutex                                              m_mutex;

    bool createLogRoot();
    void updateLogFile(Log::Category);
};

#endif // LOGMANAGER_H
