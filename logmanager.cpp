#include "logmanager.h"

LogManager::LogManager(QObject *parent)
    : QObject(parent) {
    m_logRoot = QCoreApplication::applicationDirPath() +
                QDir::separator() + "logs";
    m_level = Log::Level::DEBUG;
}

bool LogManager::initializeLogs() {
    if (!createLogRoot()) {
        qCritical() << "Failed to initialize LogManager!";
        return false;
    }

    for (int i = 0; i < static_cast<int>(QMetaEnum::fromType<Log::Category>().keyCount()); ++i) {
        auto category = static_cast<Log::Category>(i);
        logFiles[category] = nullptr;
        logStreams[category] = nullptr;
    }

    return true;
}

// void LogManager::setLogCategory(Log::Category category) {
//     m_category = category;
// }

void LogManager::setLogLevel(Log::Level level) {
    m_level = level;
}

void LogManager::log(Log::Category category, Log::Level level, const QString &message) {
    if (level < m_level) {
        return;
    }

    QMutexLocker locker(&m_mutex);

    updateLogFile(category);

    QString format = LogUtils::formatLogMessage(level, message);
    if (logStreams.contains(category)) {
        *logStreams[category] << format << "\n";
        logStreams[category]->flush();
    }
}

void LogManager::updateLogFile(Log::Category category) {
    QFile *file = logFiles[category];
    QString currentDate = QDateTime::currentDateTime().toString("yyyy-MM-dd");

    // 如果日志文件已打开且当前日期相同，则直接返回
    if (file && file->fileName().contains(currentDate))
        return;

    // 如果日志文件已打开，但日期不同，则关闭当前文件
    if (file) {
        file->close();
        delete file;
        file = nullptr;
    }

    delete logStreams[category];
    logStreams[category] = nullptr;

    file = new QFile(m_logRoot +
                     "/" + LogUtils::categoryToString(category) +
                     "/" + LogUtils::generateLogFileName(category));
    if (file->open(QIODevice::Append | QIODevice::Text)) {
        logFiles[category] = file;
        logStreams[category] = new QTextStream(file);
    } else {
        qDebug() << "Failed to open log file for category: " << LogUtils::categoryToString(category);
        delete file;
        file = nullptr;
        logStreams[category] = new QTextStream(stdout);
    }
}

bool LogManager::createLogRoot() {
    QDir logRoot(m_logRoot);
    bool success = true;

    for (int i = 0; success && i < QMetaEnum::fromType<Log::Category>().keyCount(); ++i) {
        success = success && logRoot.mkpath(LogUtils::categoryToString(static_cast<Log::Category>(i)));
    }

    return success;
}

LogManager::~LogManager() {
    for (auto stream : logStreams) {
        if (stream) {
            delete stream;
        }
    }
    for (auto file : logFiles) {
        if (file) {
            file->close();
            delete file;
        }
    }
}
