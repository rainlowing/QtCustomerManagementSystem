#include "databasemanager.h"

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent) {
    m_dbPath = QCoreApplication::applicationDirPath() +
               QDir::separator() + "database" +
               QDir::separator() + "database.db";
}

bool DatabaseManager::initializeDataBase(LogManager& logManager) {
    if (!createDatabaseDirectory()) {
        message = "创建数据库目录失败（Failed to create database directory!）";
        logManager.log(Log::LOG_DATABASE, Log::CRITICAL, message);
        return false;
    }

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(m_dbPath);
    if (!m_db.open()) {
        message = "数据库连接失败（Database connection error: " + m_db.lastError().text() + "）";
        logManager.log(Log::LOG_DATABASE, Log::CRITICAL, message);
        return false;
    }

    // 启用外键约束
    QSqlQuery pragmaQuery("PRAGMA foreign_keys = ON;");
    if (!pragmaQuery.exec()) {
        message = "启用外键失败（Failed to enable foreign keys: " + pragmaQuery.lastError().text() + "）";
        logManager.log(Log::LOG_DATABASE, Log::CRITICAL, message);
        return false;
    }

    logManager.log(Log::LOG_DATABASE, Log::INFO, "数据库初始化成功。");
    return createTables(logManager);
}

QSqlDatabase &DatabaseManager::getDB() {
    return m_db;
}

bool DatabaseManager::createDatabaseDirectory() {
    QDir dbDir(QCoreApplication::applicationDirPath());
    return dbDir.mkpath("database");
}

bool DatabaseManager::createTables(LogManager& logManager) {
    QSqlQuery query;
    bool success = true;

    for (const auto& tD : m_tableDefinitions) {
        if (!query.exec(tD.sql)) {
            message = "数据库语句执行失败（Failed to execute SQL: " + tD.sql +
                              "\nError:" + query.lastError().text() + "）";
            logManager.log(Log::LOG_DATABASE, Log::CRITICAL, message);
            success = false;
            break;
        }
    }

    for (const QString& tS : m_triggerSqls) {
        if (!query.exec(tS)) {
            message = "数据库语句执行失败（Failed to execute SQL: " + tS +
                              "\nError:" + query.lastError().text() + "）";
            logManager.log(Log::LOG_DATABASE, Log::CRITICAL, message);
            success =  false;
            break;
        }
    }

    return success;
}


DatabaseManager::~DatabaseManager() {

}

