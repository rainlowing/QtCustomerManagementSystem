#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QDir>
#include <QtSql>
#include <QCoreApplication>
#include <QDebug>
#include <QHash>
#include <QGroupBox>
#include <QLineEdit>
#include <QComboBox>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

struct TableDefinition {
    int version;
    QString description;
    QString sql;
};

class DatabaseManager : public QObject {
    Q_OBJECT

public:
    explicit DatabaseManager(QObject *parent = nullptr);
    DatabaseManager(const DatabaseManager &) = delete;
    DatabaseManager &operator=(const DatabaseManager &) = delete;
    DatabaseManager(DatabaseManager &&) = delete;
    DatabaseManager &operator=(DatabaseManager &&) = delete;
    ~DatabaseManager();

    bool initDB();
    bool isReady() const { return m_db.isOpen(); }
    QSqlDatabase& getDB();
    bool selectAllName(QComboBox *);
    bool selectAllService(QComboBox *);
    QString nameToID(const QString &);
    bool insertCP(QGroupBox *);
    bool updateCP(QGroupBox *);
    bool insertCT(const QString &, const QString &);

private:
    QString                                     message;
    QSqlDatabase                                m_db;
    QString                                     m_dbPath;

    bool createTables(const QString &jsonFilePath);
};

#endif // DATABASEMANAGER_H
