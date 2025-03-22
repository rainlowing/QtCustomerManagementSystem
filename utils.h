#ifndef UTILS_H
#define UTILS_H

#include "namespace.h"
#include <QDir>
#include <QDate>

namespace LogUtils {
    QString categoryToString(Log::Category c);
    QString levelToString(Log::Level l);
    QString generateLogFileName(Log::Category c);
    QString formatLogMessage(Log::Level l, const QString& m);
    // QString getLogFilePath(Log::Category c, const QString &logRoot);
}

#endif // UTILS_H
