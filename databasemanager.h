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
#include <QVariantMap>
#include <QSqlRecord>

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

    QString getNewCustomerID();
    bool insertCT(QVariantMap &data);
    bool insertCTByIDAndName(const QString &customerID, const QString& name);

    QString getNewConsumptionID();
    bool insertCP(QVariantMap &data);
    QString isNewCustomer(const QString &name);
    bool updateCP(QVariantMap &data);

signals:
    void consumptionDataChanged();
    void customerDataChanged();

private:
    QString                                     message;
    QSqlDatabase                                m_db;
    QString                                     m_dbPath;

    bool createTables(const QString &jsonFilePath);
};

#endif // DATABASEMANAGER_H
