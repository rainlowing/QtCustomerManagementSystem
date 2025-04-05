#ifndef UPDATECUSTOMER_H
#define UPDATECUSTOMER_H

#include <QDialog>
#include <QVariantMap>
#include <QDate>

#include "databasemanager.h"

namespace Ui {
class UpdateCustomer;
}

class UpdateCustomer : public QDialog
{
    Q_OBJECT

public:
    explicit UpdateCustomer(DatabaseManager *dbManager, QWidget *parent = nullptr);
    ~UpdateCustomer();

    void setBaseInfo(const QVariantMap &data);

private:
    bool isValid();

private slots:
    void handleDialogAccepted();
    void handleDialogRejected();

signals:
    void returnCode(int code);

private:
    Ui::UpdateCustomer      *ui;
    DatabaseManager         *m_dbManager;
    QVariantMap             m_data;
};

#endif // UPDATECUSTOMER_H
