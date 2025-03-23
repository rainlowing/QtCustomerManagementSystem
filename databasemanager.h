#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QDir>
#include <QtSql>
#include <QCoreApplication>
#include <QDebug>

#include "logmanager.h"

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

    bool initializeDataBase(LogManager& lm);
    bool isReady() const { return m_db.isOpen(); }
    QSqlDatabase& getDB();

private:
    QString                                     message;
    QSqlDatabase                                m_db;
    QString                                     m_dbPath;

    bool createDatabaseDirectory();
    bool createTables(LogManager& lm);

    const QVector<TableDefinition>              m_tableDefinitions = {
        {
            1,
            "Customers Table",
            R"(
            CREATE TABLE IF NOT EXISTS customers (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                customer_id TEXT NOT NULL UNIQUE,
                name TEXT NOT NULL,
                gender TEXT NOT NULL CHECK(gender IN ('男', '女', '未知')),
                birthday DATE,
                phone TEXT,
                first_consumption_id TEXT,
                last_consumption_id TEXT,
                total_amount DECIMAL(10,2) CHECK(total_amount >= 0) DEFAULT 0.00,
                FOREIGN KEY (first_consumption_id) REFERENCES consumption_records(consumption_id),
                FOREIGN KEY (last_consumption_id) REFERENCES consumption_records(consumption_id)
            );)"},
        {
            1,
            "Service Record Table",
            R"(
            CREATE TABLE IF NOT EXISTS consumption_records (
                consumption_id TEXT PRIMARY KEY,
                customer_id TEXT NOT NULL,
                service TEXT,
                amount DECIMAL(10,2) NOT NULL CHECK(amount > 0) default 0.00,
                time DATETIME DEFAULT CURRENT_TIMESTAMP,
                FOREIGN KEY (customer_id) REFERENCES customers(customer_id)
            );)"},
    };
    const QStringList                           m_triggerSqls = {
        {
            R"(
            CREATE TRIGGER IF NOT EXISTS update_customer_consumption
            AFTER INSERT ON consumption_records
            FOR EACH ROW
            BEGIN
                -- 更新首次消费记录（仅当首次为空时）
                UPDATE customers
                SET first_consumption_id = NEW.consumption_id
                WHERE customer_id = NEW.customer_id
                AND first_consumption_id IS NULL;

                -- 更新最后一次消费记录
                UPDATE customers
                SET last_consumption_id = NEW.consumption_id
                WHERE customer_id = NEW.customer_id;

                -- 累加总消费金额
                UPDATE customers
                SET total_amount = total_amount + NEW.amount
                WHERE customer_id = NEW.customer_id;
            END;
            )"},
    };
};

#endif // DATABASEMANAGER_H
