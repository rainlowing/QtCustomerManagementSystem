#include "logmanager.h"
#include "utils.h"

// 确定日志文件根目录和日志管理器级别，并初始化
LogManager::LogManager() {
    m_logRoot = QCoreApplication::applicationDirPath() +
                QDir::separator() + "logs";
    m_level = Log::DEBUG;
    if (!init()) {
        qCritical() << "Failed to initialize log manager!";
    }
}


// 创建单例
LogManager &LogManager::getInstance() {
    static LogManager instance;
    return instance;
}


// 初始化日志管理器实例：1.创建日志目录；2.创建日志文件指针和流指针
bool LogManager::init() {
    QDir logRoot(m_logRoot);
    bool success = true;
    for (int i = 0; success && i < QMetaEnum::fromType<Log::Category>().keyCount(); ++i) {
        success = success && logRoot.mkpath(LogUtils::categoryToString(static_cast<Log::Category>(i)));
    }

    if (!success) {
        qCritical() << "Failed to create log directory!";
        return false;
    }

    for (int i = 0; i < static_cast<int>(QMetaEnum::fromType<Log::Category>().keyCount()); ++i) {
        auto category = static_cast<Log::Category>(i);
        logFiles[category] = nullptr;
        logStreams[category] = nullptr;
    }

    return true;
}


// 设置日志管理器级别
void LogManager::setLogLevel(Log::Level level) {
    m_level = level;
}


// 实现日志记录：1.低于管理器级别的不记录；2.确保日志文件以天为单位生成；3.日志内容格式化
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


// 检查是否需要更新日志文件
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
                     QDir::separator() + LogUtils::categoryToString(category) +
                     QDir::separator() + LogUtils::generateLogFileName(category));
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
