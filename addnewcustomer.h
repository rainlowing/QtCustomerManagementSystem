#ifndef ADDNEWCUSTOMER_H
#define ADDNEWCUSTOMER_H

#include <QDialog>
#include <QVariantMap>
#include <QPushButton>

#include "databasemanager.h"

namespace Ui {
class AddNewCustomer;
}

class AddNewCustomer : public QDialog
{
    Q_OBJECT

public:
    explicit AddNewCustomer(DatabaseManager *dbManager, QWidget *parent = nullptr);
    ~AddNewCustomer();

    void setBaseInfo();

private:
    bool isValid();

private slots:
    void handleDialogAccepted();
    void handleDialogRejected();

signals:
    void returnCode(int code);

private:
    Ui::AddNewCustomer      *ui;
    DatabaseManager         *m_dbManager;
    QVariantMap             m_data;
};

#endif // ADDNEWCUSTOMER_H
