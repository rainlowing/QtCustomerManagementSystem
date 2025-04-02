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


//
bool DatabaseManager::selectAllName(QComboBox *combobox) {
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

bool DatabaseManager::selectAllService(QComboBox *combobox) {
    QSqlQuery query;
    QString sql = "SELECT service FROM consumptions";

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
            insertCT(customerID, name);
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

    emit dataChanged();
    return true;
}

bool DatabaseManager::updateCP(QGroupBox *fromGroup) {
    QLineEdit *idEdit = fromGroup->findChild<QLineEdit*>("idEdit");
    QComboBox *nameCombo = fromGroup->findChild<QComboBox*>("nameCombo");
    QComboBox *serviceCombo = fromGroup->findChild<QComboBox*>("serviceCombo");
    QLineEdit *amountEdit = fromGroup->findChild<QLineEdit*>("amountEdit");

    QString customerID;

    m_db.transaction();
    QSqlQuery selectSql;
    selectSql.prepare("SELECT customer_id, service, amount FROM consumptions WHERE consumption_id = ?");
    selectSql.addBindValue(idEdit->text());

    if (!selectSql.exec() || !selectSql.next()){
        message = QString("记录不存在 ID: %1").arg(idEdit->text());
        LogManager::getInstance().log(Log::DATABASE, Log::WARNING, message);
        m_db.rollback();
        return false;
    }

    const QString origCustomerID = selectSql.value(0).toString();
    const QString origService = selectSql.value(1).toString();
    const double origAmount = selectSql.value(2).toDouble();

    QStringList updateFields;
    QVariantList bindValues;

    // 检测字段变化
    if(customerID != origCustomerID){
        updateFields << "customer_id = ?";
        bindValues << customerID;
    }

    const QString newService = serviceCombo->currentText().trimmed();
    if(newService != origService){
        updateFields << "service = ?";
        bindValues << newService;
    }

    bool ok;
    const double newAmount = amountEdit->text().toDouble(&ok);
    if (!ok){
        message = "金额格式错误";
        LogManager::getInstance().log(Log::DATABASE, Log::WARNING, message);
        m_db.rollback();
        return false;
    }
    if(!qFuzzyCompare(newAmount, origAmount)){
        updateFields << "amount = ?";
        bindValues << newAmount;
    }

    // 没有需要更新的字段
    if(updateFields.isEmpty()){
        m_db.commit();
        message = "数据未发生变化";
        LogManager::getInstance().log(Log::DATABASE, Log::INFO, message);
        return true;
    }

    // 构建动态SQL
    QString updateSql = QString("UPDATE consumption_records SET %1 WHERE consumption_id = ?")
                            .arg(updateFields.join(", "));
    bindValues << idEdit->text();

    // 执行更新
    QSqlQuery updateQuery;
    updateQuery.prepare(updateSql);
    for (const QVariant &value : bindValues){
        updateQuery.addBindValue(value);
    }

    if (!updateQuery.exec()){
        message = QString("更新失败: %1").arg(updateQuery.lastError().text());
        LogManager::getInstance().log(Log::DATABASE, Log::ERROR, message);
        m_db.rollback();
        return false;
    }

    // 提交事务
    if (!m_db.commit()){
        message = "事务提交失败";
        LogManager::getInstance().log(Log::DATABASE, Log::ERROR, message);
        m_db.rollback();
        return false;
    }

    // 记录变更日志
    QStringList changes;
    if (customerID != origCustomerID) changes << QString("顾客 ID: %1 → %2").arg(origCustomerID).arg(customerID);
    if (newService != origService) changes << QString("服务: %1 → %2").arg(origService).arg(newService);
    if (!qFuzzyCompare(newAmount, origAmount)) changes << QString("金额: %1 → %2").arg(origAmount).arg(newAmount);

    LogManager::getInstance().log(Log::DATABASE, Log::INFO,
                   QString("记录更新成功 ID: %1\n变更明细:\n%2")
                       .arg(idEdit->text())
                       .arg(changes.join("\n")));

    return true;
}

bool DatabaseManager::insertCT(const QString &customerID, const QString& name) {
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

