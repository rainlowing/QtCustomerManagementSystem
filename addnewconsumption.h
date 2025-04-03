#ifndef ADDNEWCONSUMPTION_H
#define ADDNEWCONSUMPTION_H

#include <QDialog>
#include <QPushButton>
#include <QMessageBox>
#include <QVariantMap>
#include <QDoubleValidator>

#include "databasemanager.h"

namespace Ui {
class AddNewConsumption;
}

class AddNewConsumption : public QDialog
{
    Q_OBJECT

public:
    explicit AddNewConsumption(DatabaseManager *dbManager, QWidget *parent = nullptr);
    ~AddNewConsumption();

    void setBaseInfo();

private:
    bool isValid();

private slots:
    void handleDialogAccepted();
    void handleDialogRejected();

signals:
    void returnCode(int code);

private:
    Ui::AddNewConsumption   *ui;
    DatabaseManager         *m_dbManager;
    QVariantMap             m_data;
};

#endif // ADDNEWCONSUMPTION_H
