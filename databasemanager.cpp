#include "databasemanager.h"

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent) {
    m_dbPath = QCoreApplication::applicationDirPath() +
               QDir::separator() + "database" +
               QDir::separator() + "database.db";
}

bool DatabaseManager::initializeDataBase() {
    if (!createDatabaseDirectory()) {
        qCritical() << "Failed";
    }

    return true;
}

bool DatabaseManager::createDatabaseDirectory() {
    QDir dbDir(QCoreApplication::applicationDirPath());
    return dbDir.mkpath("database");
}

bool DatabaseManager::createTables() {
    return true;
}


DatabaseManager::~DatabaseManager() {

}

