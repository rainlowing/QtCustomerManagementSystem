#include "databasemanager.h"
#include "qlineedit.h"
#include "logmanager.h"

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent) {
    m_dbPath = QCoreApplication::applicationDirPath() +
               QDir::separator() + "database" +
               QDir::separator() + "database.db";
}


// 初始化数据库：1.创建数据库目录；2.连接数据库；3.启用外键约束；4.建表
bool DatabaseManager::initDB() {
    QDir dbDir(QCoreApplication::applicationDirPath());
    if (!dbDir.mkpath("database")) {
        message = "创建数据库目录失败（Failed to create database directory!）";
        LogManager::getInstance().log(Log::DATABASE, Log::CRITICAL, message);
        return false;
    }

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(m_dbPath);
    if (!m_db.open()) {
        message = "数据库连接失败（Database connection error: " + m_db.lastError().text() + "）";
        LogManager::getInstance().log(Log::DATABASE, Log::CRITICAL, message);
        return false;
    }

    QSqlQuery pragmaQuery("PRAGMA foreign_keys = ON;");
    if (!pragmaQuery.exec()) {
        message = "启用外键失败（Failed to enable foreign keys: " + pragmaQuery.lastError().text() + "）";
        LogManager::getInstance().log(Log::DATABASE, Log::CRITICAL, message);
        return false;
    }

    return createTables(":/schema.json");
}


// 获取数据库对象
QSqlDatabase &DatabaseManager::getDB() {
    return m_db;
}


// 获得所有已记录的顾客姓名，并注入到 ComboBox 控件
bool DatabaseManager::selectAllName(QComboBox *combobox) {
    combobox->clear();
    QSqlQuery query;
    QString sql = "SELECT name FROM customers";

    if (query.exec(sql)) {
        while (query.next()) {
            QString value = query.value(0).toString();
            combobox->addItem(value);
        }
    } else {
        return false;
    }
    return true;
}


// 获得所有已记录的服务，并注入到 ComboBox 控件
bool DatabaseManager::selectAllService(QComboBox *combobox) {
    combobox->clear();
    QSqlQuery query;
    QString sql = "SELECT DISTINCT service FROM consumptions";

    if (query.exec(sql)) {
        while (query.next()) {
            QString value = query.value(0).toString();
            combobox->addItem(value);
        }
    } else {
        return false;
    }
    return true;
}


// 检查顾客名是已经存在
bool DatabaseManager::isCustomerExists(const QString &name) {
    QSqlQuery query;
    query.prepare("SELECT * FROM customers WHERE name = :name");
    query.bindValue(":name", name);
    query.exec();
    if (!query.next()) {
        return false;
    }
    return true;
}


// 获取下一条新顾客信息的 ID，但是不会进行插入
QString DatabaseManager::getNewCustomerID() {
    QString newID;

    QSqlQuery query;
    query.prepare("SELECT MAX(customer_id) FROM customers");

    if (query.exec()) {
        if (query.next()) {
            int idCounter = query.value(0).toString().mid(1).toInt();
            newID = QString("U%1").arg(idCounter + 1, 6, 10, QLatin1Char('0'));
        } else {
            newID = "U000001";
        }
    } else {
        message = "SQL 查询语句失败（Failed to exec select query: " +
                  query.lastError().text() + "）" + query.lastQuery();
        LogManager::getInstance().log(Log::DATABASE, Log::ERROR, message);
    }

    return newID;
}


// 根据姓名查询是否为新顾客，返回新顾客的 ID，但是不会进行插入
QString DatabaseManager::isNewCustomer(const QString &name) {
    QString customerID;

    QSqlQuery queryB;
    queryB.prepare("SELECT customer_id from customers WHERE name = :name");
    queryB.bindValue(":name", name);
    if (queryB.exec()) {
        if (queryB.next()) {
            customerID = queryB.value("customer_id").toString();
            return customerID;
        }
    }

    QSqlQuery queryA;
    if (queryA.exec("SELECT max(customer_id) FROM consumption_records")) {
        if (queryA.next()) {
            int maxID = queryA.value(0).toInt();
            int newID = (maxID > 0) ? maxID + 1 : 1;
            customerID = QString("%1").arg(newID, 6, 10, QLatin1Char('0'));
            insertCTByIDAndName(customerID, name);
        } else {
            customerID = "000001";
        }
    }

    return customerID;
}


// 插入一条新消费记录，如果是新顾客则先插入一条简易的新顾客信息
bool DatabaseManager::insertCP(QVariantMap &data) {
    QSqlQuery query;
    query.prepare("SELECT customer_id FROM customers WHERE name = :name");
    query.bindValue(":name", data["name"]);
    query.exec();
    if (query.next()) {
        data["customer_id"] = query.value("customer_id").toString();
    } else {
        query.prepare("INSERT INTO customers (name) VALUES (:name)");
        query.bindValue(":name", data["name"]);
        query.exec();
        query.finish();

        query.prepare("SELECT customer_id FROM customers WHERE name = :name");
        query.bindValue(":name", data["name"]);
        query.exec();
        if (query.next()) {
            data["customer_id"] = query.value("customer_id").toString();
        }
    }
    query.finish();

    query.prepare("INSERT INTO consumptions (customer_id, service, amount, note) VALUES (?, ?, ?, ?)");
    query.addBindValue(data["customer_id"]);
    query.addBindValue(data["service"]);
    query.addBindValue(data["amount"]);
    query.addBindValue(data["note"]);

    if (!query.exec()) {
        message = "插入新消费记录失败（Failed to insert a new consumption record: " +
                  query.lastError().text() + "）" +
                  query.lastQuery();
        LogManager::getInstance().log(Log::DATABASE, Log::WARNING, message);
        return false;
    }

    emit consumptionDataChanged();
    emit customerDataChanged();
    return true;
}


// 更新一条消费记录：仅更新有变化的字段
bool DatabaseManager::updateCP(QVariantMap &data) {
    QSqlQuery query;
    query.prepare("SELECT customer_id FROM customers WHERE name = :name");
    query.bindValue(":name", data["name"]);
    query.exec();
    if (query.next()) {
        data["customer_id"] = query.value("customer_id").toString();
    }
    query.finish();
    data.remove("name");

    QVariantMap originData;
    query.prepare("SELECT consumption_id, customer_id, service, amount, note FROM consumptions WHERE consumption_id = :consumption_id");
    query.bindValue(":consumption_id", data["consumption_id"]);
    query.exec();
    if (query.next()) {
        originData["consumption_id"] = query.value("consumption_id").toString();
        originData["customer_id"] = query.value("customer_id").toString();
        originData["service"] = query.value("service").toString();
        originData["amount"] = query.value("amount").toString();
        originData["note"] = query.value("note").toString();
    }
    query.finish();

    QStringList updateFields;
    QVariantMap updateValues;
    for(auto key : originData.keys()) {
        if (originData[key] != data[key]) {
            updateFields.append(QString("%1 = :%1").arg(key));
            updateValues[key] = data[key];
        }
    }

    query.prepare("UPDATE consumptions SET " + updateFields.join(", ") + " WHERE consumption_id = :consumption_id");
    query.bindValue(":consumption_id", data["consumption_id"]);
    for (auto key : updateValues.keys()) {
        query.bindValue(":" + key, updateValues[key]);
    }

    if (!query.exec()) {
        message = "更新消费记录失败（Failed to update consumption record: " +
                  query.lastError().text() + "）" +
                  query.lastQuery();
        LogManager::getInstance().log(Log::DATABASE, Log::WARNING, message);
        return false;
    }

    emit consumptionDataChanged();
    emit customerDataChanged();
    return true;
}


bool DatabaseManager::insertCT(QVariantMap &data) {
    QSqlQuery query;
    query.prepare("SELECT * FROM customers WHERE name = :name");
    query.bindValue(":name", data["name"]);
    query.exec();
    if (query.next()) {
        return false;
    }
    query.finish();

    query.prepare("INSERT INTO customers (name, gender, birthday, phone, note) VALUES (?, ?, ?, ?, ?)");
    query.addBindValue(data["name"]);
    query.addBindValue(data["gender"]);
    query.addBindValue(data["birthday"]);
    query.addBindValue(data["phone"]);
    query.addBindValue(data["note"]);

    if (!query.exec()) {
        message = "插入新顾客记录失败（Failed to insert a new consumption record: " +
                  query.lastError().text() + "）" +
                  query.lastQuery();
        LogManager::getInstance().log(Log::DATABASE, Log::WARNING, message);
        return false;
    }

    emit customerDataChanged();
    return true;
}

bool DatabaseManager::insertCTByIDAndName(const QString &customerID, const QString &name) {
    m_db.transaction();
    QSqlQuery insertSql;
    insertSql.prepare("INSERT INTO customers "
                      "(customer_id, name, gender) "
                      "VALUES (?, ?, ?)");
    insertSql.addBindValue(customerID);
    insertSql.addBindValue(name);
    insertSql.addBindValue("未知");

    if (!insertSql.exec()) {
        m_db.rollback();
        message = "插入新顾客记录失败（Failed to insert a new consumption record: " +
                  insertSql.lastError().text() + "）" +
                  insertSql.lastQuery();
        LogManager::getInstance().log(Log::DATABASE, Log::WARNING, message);
        return false;
    }
    m_db.commit();
    return true;
}


bool DatabaseManager::updateCT(QVariantMap &data) {
    QSqlQuery query;
    QVariantMap originData;
    query.prepare("SELECT customer_id, name, gender, birthday, phone, note FROM customers WHERE customer_id = :customer_id");
    query.bindValue(":customer_id", data["customer_id"]);
    query.exec();
    if (query.next()) {
        originData["customer_id"] = query.value("customer_id").toString();
        originData["name"] = query.value("name").toString();
        originData["gender"] = query.value("gender").toString();
        originData["birthday"] = query.value("birthday").toString();
        originData["phone"] = query.value("phone").toString();
        originData["note"] = query.value("note").toString();
    }
    query.finish();

    QStringList updateFields;
    QVariantMap updateValues;
    for(auto key : originData.keys()) {
        if (originData[key] != data[key]) {
            updateFields.append(QString("%1 = :%1").arg(key));
            updateValues[key] = data[key];
        }
    }

    query.prepare("UPDATE customers SET " + updateFields.join(", ") + " WHERE customer_id = :customer_id");
    query.bindValue(":customer_id", data["customer_id"]);
    for (auto key : updateValues.keys()) {
        query.bindValue(":" + key, updateValues[key]);
    }

    if (!query.exec()) {
        message = "更新顾客信息失败（Failed to update consumption record: " +
                  query.lastError().text() + "）" +
                  query.lastQuery();
        LogManager::getInstance().log(Log::DATABASE, Log::WARNING, message);
        return false;
    }

    emit consumptionDataChanged();
    emit customerDataChanged();
    return true;
}


// 获取下一条新消费记录的 ID，但是不会进行插入
QString DatabaseManager::getNewConsumptionID() {
    QString newID;

    QString today = QDate::currentDate().toString("yyyyMMdd");
    QSqlQuery query;
    query.prepare("SELECT day, counter FROM consumption_counter WHERE day = :day");
    query.bindValue(":day", today);

    if (query.exec()) {
        if (query.next()) {
            QString counter = QString("%1").arg(query.value("counter").toInt(), 3, 10, QLatin1Char('0'));
            newID = "OD" + today + counter;
        } else {
            newID = "OD" + today + "001";
        }
    } else {
        message = "SQL 查询语句失败（Failed to exec select query: " +
                  query.lastError().text() + "）" + query.lastQuery();
        LogManager::getInstance().log(Log::DATABASE, Log::ERROR, message);
    }

    return newID;
}


// 建表：从 json 文件中读取建表语句，建立事务逐条执行
bool DatabaseManager::createTables(const QString &jsonFilePath) {
    QFile file(jsonFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        message = "无法打开 JSON 文件（Failed to open json file: " + jsonFilePath + "）";
        LogManager::getInstance().log(Log::DATABASE, Log::CRITICAL, message);
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        message = "JSON 解析错误（Failed to parse json: " +
                  parseError.errorString() + "）";
        LogManager::getInstance().log(Log::DATABASE, Log::CRITICAL, message);
    }

    QJsonObject obj = doc.object();
    QJsonArray statements = obj.value("statements").toArray();
    QSqlQuery query;
    m_db.transaction();
    for (int i = 0; i < statements.size(); ++i) {
        QString sql = statements.at(i).toString();
        if (!query.exec(sql)) {
            message = "执行 SQL 语句出错（Failed to exec SQL statements: " +
                      query.lastError().text() + "）" + query.lastQuery();
            LogManager::getInstance().log(Log::DATABASE, Log::CRITICAL, message);
            m_db.rollback();
            return false;
        }
    }
    m_db.commit();
    LogManager::getInstance().log(Log::DATABASE, Log::INFO, "完成建表");

    return true;
}


DatabaseManager::~DatabaseManager() {

}

