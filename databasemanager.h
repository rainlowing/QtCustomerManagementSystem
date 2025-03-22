#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QDir>
#include <QtSql>
#include <QCoreApplication>
#include <QDebug>

class DatabaseManager : public QObject {
    Q_OBJECT

public:
    explicit DatabaseManager(QObject *parent = nullptr);
    DatabaseManager(const DatabaseManager &) = delete;
    DatabaseManager &operator=(const DatabaseManager &) = delete;
    DatabaseManager(DatabaseManager &&) = delete;
    DatabaseManager &operator=(DatabaseManager &&) = delete;
    ~DatabaseManager();

    bool initializeDataBase();
    bool isReady() const { return m_db.isOpen(); }

private:
    QSqlDatabase                                m_db;
    QString                                     m_dbPath;

    bool createDatabaseDirectory();
    bool createTables();
};

#endif // DATABASEMANAGER_H
