#include "utils.h"

// 函数：将日志类型转换为字符串（全小写）
QString LogUtils::categoryToString(Log::Category category) {
    const char *key = QMetaEnum::fromType<Log::Category>().valueToKey(static_cast<int>(category));

    if (!key) {
        qWarning("Invalid Log::Category value: %d", static_cast<int>(category));
        return "unknown";
    }

    QString strKey = QString::fromLatin1(key).toLower();
    return strKey;
}

// 函数：将日志级别转换为字符串（全大写）
QString LogUtils::levelToString(Log::Level level) {
    const char *key = QMetaEnum::fromType<Log::Level>().valueToKey(static_cast<int>(level));

    if (!key) {
        qWarning("Invalid Log::Level value: %d", static_cast<int>(level));
        return "unknown";
    }

    QString strKey = QString::fromLatin1(key);

    return strKey;
}

// 函数：根据日志类型和当前时间生成日志文件名
QString LogUtils::generateLogFileName(Log::Category category) {
    QString categoryStr = categoryToString(category);
    QString dateStr = QDateTime::currentDateTime().toString("yyyy-MM-dd");

    return QString("%1_%2.log").arg(categoryStr).arg(dateStr);
}

// 函数：日志消息格式化
QString LogUtils::formatLogMessage(Log::Level level, const QString& message) {
    QString levelStr = levelToString(level);
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");

    return QString("[ %1 | %2 ]: %4")
        .arg(timestamp)
        .arg(levelStr)
        .arg(message);
}




